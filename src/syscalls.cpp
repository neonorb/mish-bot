/*
 * syscalls.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: chris13524
 */

#include <new>
#include <mish.h>
#include <iostream>
#include <feta.h>
#include <tgbot/tgbot.h>

using namespace mish;
using namespace TgBot;

extern Bot* bot;
extern Message::Ptr currentMessage;

Value* sendFunction(List<Value*>* arguments) {
	String message = ((StringValue*) arguments->get(0))->value;
	bot->getApi().sendMessage(currentMessage->chat->id, std::string(message));

	return NULL;
}

Value* sendFormattedFunction(List<Value*>* arguments) {
	String message = ((StringValue*) arguments->get(0))->value;
	bot->getApi().sendMessage(currentMessage->chat->id, std::string(message),
			false, 0, GenericReply::Ptr(), "Markdown");

	return NULL;
}

List<Function*> syscalls;

void registerSyscalls() {
	// send
	List<ValueType>* sendParameterTypes = new List<ValueType>();
	sendParameterTypes->add(ValueType::STRING);
	Function* send = new Function("__send"_H, sendParameterTypes,
			ValueType::VOID, BIND_FREE_CB(sendFunction));
	syscalls.add(send);
	mish_syscalls.add(send);

	// sendFormatted
	List<ValueType>* sendFormattedParameterTypes = new List<ValueType>();
	sendFormattedParameterTypes->add(ValueType::STRING);
	Function* sendFormatted = new Function("__sendFormatted"_H, sendFormattedParameterTypes,
			ValueType::VOID, BIND_FREE_CB(sendFormattedFunction));
	syscalls.add(sendFormatted);
	mish_syscalls.add(sendFormatted);
}

void unregisterSyscalls() {
	Iterator<Function*> iterator = syscalls.iterator();
	while (iterator.hasNext()) {
		Function* function = iterator.next();
		mish_syscalls.remove(function);
		delete function;
	}

	syscalls.clear();
}
