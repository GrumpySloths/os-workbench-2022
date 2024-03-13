NAME :=thread_os
SRCS := $(shell find -L ./src/ -name "*.c")
export MODULE  := L0
export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH :=x86_64-qemu
endif

include $(AM_HOME)/Makefile
include ../Makefile.lab 

QEMUGDB= -nographic -S -s 

image: git
run-gdb:build-arg
	@qemu-system-x86_64 $(QEMU_FLAGS) $(QEMUGDB)

### 清理abstract-machine中所有文件
CLEAN_AM := $(shell find ../abstract-machine -mindepth 2 -name build) 
clean_am: $(CLEAN_AM)
	@echo "clean success"
$(CLEAN_AM):
	@rm -rf $@
.PHONY: clean_am $(CLEAN_AM)

debug:
	qemu-system-x86_64 -nographic -s -S build/thread_os-x86_64-qemu 