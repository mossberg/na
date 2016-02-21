OUT = nooballoc_test nooballoc_test2 linkedlist freelist_test
CFLAGS += -Wno-gnu-zero-variadic-macro-arguments
HEADERS = $(wildcard *.h)

all: $(OUT) $(HEADERS)

nooballoc_test2: nooballoc_test2.c nooballoc2.h
	gcc -o $@ nooballoc_test2.c -Wall -pedantic -Wno-gnu-zero-variadic-macro-arguments

freelist_test: freelist_test.c freelist.h
	gcc -o $@ freelist_test.c -Wall -pedantic -Wno-gnu-zero-variadic-macro-arguments

clean:
	rm -f $(OUT)
