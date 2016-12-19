# Makefile for Mish Bot

NAME=mish-bot

CSOURCES=main

LIBS=feta mish

# ----

-include ../make-base/make-base.mk

all: build/mishbot

build/mishbot: $(OBJECTS)
	@ld $(OBJECTS)           \
		$(LIBS_FLAGS)    \
		-o build/mishbot \
