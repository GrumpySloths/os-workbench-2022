CC = gcc
CFLAGS = -Wall -g  -I.
export AM_HOME := $(PWD)/../abstract-machine

# test:test.c
# 	@gcc test.c $(AM_HOME)/klib/src/stdlib.c\
# 				-I$(AM_HOME)/klib/include -I../kernel/test\
# 				-o test
test: src/test.c src/malloc.h
	@$(CC) $(CFLAGS)  $< -o build/$@
	@build/$@

test2: src/test2.c 
	@$(CC) $(CFLAGS)  $< -o build/$@ 
	@build/$@

clean:
	rm -f *.o executable