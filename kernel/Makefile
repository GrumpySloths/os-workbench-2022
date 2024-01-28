NAME           := kernel
SRCS           := framework/main.c $(shell find -L ./src/ -name "*.c")
INC_PATH       := include/ framework/

export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH    := x86_64-qemu
endif

include $(AM_HOME)/Makefile
include ../Makefile.lab
image: git
# ifdef DEBUG
# CFLAGS += -DDEBUG
# endif


test: git
	@gcc $(shell find src/ -name "*.c")  \
			$(shell find test/ -name "*.c") \
			$(AM_HOME)/klib/src/stdlib.c\
			-Iframework -Itest -I$(AM_HOME)/klib/include -DTEST  -g -lpthread \
			-o build/test
	@build/test

test2: git
	@gcc $(shell find src/ -name "*.c")  \
			$(shell find test/ -name "*.c") \
			-Iframework -Itest -I$(AM_HOME)/klib/include -DTEST -DNDEBUG -g  \
			-o build/test
	@build/test