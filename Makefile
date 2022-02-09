# Copyright (c) 2022 Asaf Zebulon
# 
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT

CC = gcc

CFLAGS += -ggdb3 -std=gnu99
LDFLAGS += -lm

PROGS = main
TESTS = 

main: main.o model.o
	$(CC) $(CFLAGS) -Wall -Werror $^ -o $@ $(LDFLAGS)

.PHONY: tests
tests: $(TESTS)

.PHONY: clean
clean:
	rm -rf $(PROGS)
	rm -rf $(TESTS)
	rm -rf *.o
	rm -rf valgrind-out.txt