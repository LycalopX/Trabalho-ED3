# Makefile para compilação e execução do trabalho

SRCS = $(shell find src -name "*.c")

all:
	gcc -o programaTrab $(SRCS) -lmd

run:
	./programaTrab

clean:
	rm -f programaTrab

