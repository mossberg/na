OUT = nooballoc_test nooballoc_test2
HEADERS = $(wildcard *.h)

all: $(OUT) $(HEADERS)

nooballoc_test2: nooballoc_test.c nooballoc2.h
	gcc -o $@ nooballoc_test2.c

clean:
	rm -f $(OUT)
