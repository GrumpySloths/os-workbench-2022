NAME := $(shell basename $(PWD))
export MODULE := M5
all: $(NAME)-64 $(NAME)-32
LDFLAGS += -lm

include ../Makefile

frecov:frecov.c
	@make
	@./frecov-64 frecov.img
