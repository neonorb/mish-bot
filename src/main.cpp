#include <new>
#include <mish.h>
#include <signal.h>
#include <stdio.h>
#include <exception>
#include <iostream>

#include <tgbot/tgbot.h>
#include <syscalls.h>
#include <optionparser.h>

using namespace std;
using namespace TgBot;
using namespace mish;

bool sigintGot = false;

namespace feta {

void crash(String message) {
	std::cerr << "[CRASH] " << message << std::endl;
	unregisterSyscalls();
	exit(1);
}

void log(String message) {
	std::cout << "[NORMAL] " << message << std::endl;
}

void fault(String message) {
	std::cerr << "[FAULT] " << message << std::endl;
}

void debugPrint(String message) {
	std::cout << message;
}

}

namespace fetaimpl {

void* malloc(feta::size size) {
	return ::malloc(size);
}

void free(void* thing) {
	::free(thing);
}

}

static option::ArgStatus MandatoryOption(const option::Option& option, bool) {
	if (option.arg) {
		return option::ARG_OK;
	} else {
		return option::ARG_IGNORE;
	}
}

enum optionIndex {
	UNKNOWN, HELP, TOKEN, TOKEN_ENV
#ifdef ALLOW_TEST
	,TEST
#endif
}	;
const option::Descriptor usage[] =
		{

				{ UNKNOWN, 0, "", "", option::Arg::None,
						"USAGE: mishbot --token=<token>|--token-env [options]\n\n" "Options:" },

				{ HELP, 0, "h", "help", option::Arg::None,
						"  --help, -h  \tPrint usage and exit." },

				{ TOKEN, 0, "t", "token", MandatoryOption,
						"  --token, -c  \tSpecify the bot token." },

				{ TOKEN_ENV, 0, "e", "token-env", option::Arg::None,
						"  --token-env, -e  \tUse the TOKEN environment variable as the token." },

#ifdef ALLOW_TEST
				{	TEST, 0, "t", "test", option::Arg::None, " --test, -c  \tRun tests."},
#endif

				{ 0, 0, 0, 0, 0, 0 }

		};

Bot* bot;
Message::Ptr currentMessage;

int main(int argc, char* argv[]) {
	// parse options
	argc -= (argc > 0);
	argv += (argc > 0); // skip program name argv[0] if present
	option::Stats stats(usage, argc, argv);
	option::Option options[stats.options_max], buffer[stats.buffer_max];
	option::Parser parse(usage, argc, argv, options, buffer);

	if (parse.error()) {
		return 1;
	}

	// print help if necessary
	if (options[HELP]) {
		option::printUsage(std::cout, usage);
		return 0;
	}

	String token;
	if (options[TOKEN]) {
		token = options[TOKEN].arg;
		if (token == NULL) {
			cerr << "Missing token." << endl;
			exit(1);
		}
	} else if (options[TOKEN_ENV]) {
		token = getenv("TOKEN");
		if (token == NULL) {
			cerr << "Missing TOKEN environement variable." << endl;
			exit(1);
		}
	} else {
		cerr << "Missing either --token or --token-env option." << endl;
		exit(1);
	}

	cout << "Using token: " << token << endl;
	bot = new Bot(token);

	registerSyscalls();
	signal(SIGINT, [](int s) {
		UNUSED(s);
		if(sigintGot) {
			unregisterSyscalls();
			exit(1);
		}
		cout << "stopping..." << endl;
		cout << "do it again to force stop" << endl;
		sigintGot = true;
	});

	bot->getEvents().onCommand("start",
			[](Message::Ptr message) {
				bot->getApi().sendMessage(message->chat->id, "Hello! You can run Mish code like so:\n`/mish <code>`");
			});
	bot->getEvents().onCommand("mish",
			[](Message::Ptr message) {
				uinteger pos = message->text.find(" ");
				if(pos == string::npos) {
					bot->getApi().sendMessage(message->chat->id, "You didn't send any code!");
					return;
				}

				// FIXME joining these lines will cause problems
				string messageCode = message->text.substr(pos);
				String sourceCode = messageCode.c_str();

				Code* code = compile::compile(sourceCode);
				if(code != NULL) {
					currentMessage = message;
					execute::execute(code);
					delete code;
				}
			});

	try {
		cout << "Started bot with username: "
				<< bot->getApi().getMe()->username.c_str() << endl;

		TgLongPoll longPoll(*bot);
		while (!sigintGot) {
			longPoll.start();
		}
	} catch (exception& e) {
		if (strequ(e.what(), "Not Found")) {
			cerr << "Invalid token." << endl;
			exit(1);
		} else {
			cerr << "error: " << e.what() << endl;
			exit(1);
		}
	}

	unregisterSyscalls();
	return 0;
}
