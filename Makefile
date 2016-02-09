OUT = nooballoc_test nooballoc_test2
HEADERS = $(wildcard *.h)

all: $(OUT) $(HEADERS)

clean:
	rm -f $(OUT)
