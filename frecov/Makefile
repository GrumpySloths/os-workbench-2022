NAME := $(shell basename $(PWD))
export MODULE := M5
all: $(NAME)-64 $(NAME)-32
LDFLAGS += -lm
# CFLAGS+= -DDEBUG_LONGNAME
# CFLAGS+= -DDEBUG

include ../Makefile

frecov:frecov.c
	@make
	@./frecov-64 M5-frecov.img
