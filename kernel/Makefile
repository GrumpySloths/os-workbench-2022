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

test_kmt:git
	@gcc $(shell find src/ -maxdepth 1 -name "*.c")  \
			test/test_kmt.c \
			$(AM_HOME)/klib/src/stdlib.c\
			-Iframework -Itest -I$(AM_HOME)/klib/include -DTEST  -g -lpthread \
			-o build/test_kmt
testall:test
	@build/test 1
	@build/test 0

test2: git
	@gcc $(shell find src/ -name "*.c")  \
			$(shell find test/ -name "*.c") \
			-Iframework -Itest -I$(AM_HOME)/klib/include -DTEST -DNDEBUG -g  \
			-o build/test
	@build/test

### 清理abstract-machine中所有文件
CLEAN_AM := $(shell find ../abstract-machine -mindepth 2 -name build) 
clean_am: $(CLEAN_AM)
	@echo "clean success"
$(CLEAN_AM):
	@rm -rf $@
.PHONY: clean_am $(CLEAN_AM)

### qemu-debug
QEMUGDB= -nographic -S -s 
run-gdb:build-arg
	@qemu-system-x86_64 $(QEMU_FLAGS) $(QEMUGDB)

debug:
	qemu-system-x86_64 -nographic -s -S build/kernel-x86_64-qemu