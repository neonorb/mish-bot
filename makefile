# Makefile for Mish Bot

NAME=mish-bot

CSOURCES=main syscalls

LIBS=feta mish

# ----

-include ../make-base/make-base.mk

LIBS_FLAGS+=-lboost_system -pthread -lssl -lcrypto

all: build/mishbot

build/mishbot: $(OBJECTS)
	@g++ $(OBJECTS)      \
		lib/libTgBot.a   \
		$(LIBS_FLAGS)    \
		-o build/mishbot \
	
	@chmod +x build/mishbot
