# Makefile for Mish Bot

NAME=mish-bot

CPPSOURCES=main syscalls

LIBS=feta mish danbo

# ----

-include ../make-base/make-base.mk

ARCHS=x86_64  # we only support x86_64 atm

LIB_FLAGS=-lboost_system -pthread lib/libTgBot.a -lcrypto -lssl

-include ../make-base/make-bin.mk