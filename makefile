# Makefile for Mish Bot

NAME=mish-bot

CSOURCES=mishbot

LIBS=feta mish

# ----

-include ../make-base/make-base.mk

INCLUDE_FLAGS:=$(INCLUDE_FLAGS) -I /tmp/tgbot-cpp/include/

all: build/mishbot

build/mishbot: $(OBJECTS)
	@ld $(OBJECTS)           \
		$(LIBS_FLAGS)    \
		-l libTgBot.a    \
		-o build/mishbot \
