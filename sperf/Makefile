NAME := $(shell basename $(PWD))
export MODULE := M3
all: $(NAME)-64 $(NAME)-32

include ../Makefile

sperf:sperf.c
	@gcc sperf.c -o sperf-64
	@./sperf-64
