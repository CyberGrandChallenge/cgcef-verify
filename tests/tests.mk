CGC_LDFLAGS += -lcgc -nostdlib -L/usr/lib 
CGC_CFLAGS  += -fno-builtin -I/usr/include

all: $(ELF_BINS) $(CGC_BINS) $(CGC_BAD_BINS)
	@echo ELF binaries: $(ELF_BINS)
	@echo CGC binaries: $(CGC_BINS) $(CGC_BAD_BINS)

$(CGC_BINS):
	PATH=/usr/i386-linux-cgc/bin:$(PATH) $(CC) $(CGC_CFLAGS) -c $@.c -o $@.o
	PATH=/usr/i386-linux-cgc/bin:$(PATH) $(LD) $(CGC_LDFLAGS) $@.o -o $@

$(CGC_BAD_BINS):
	PATH=/usr/i386-linux-cgc/bin:$(PATH) $(CC) $(CGC_CFLAGS) -c $@.c -o $@.o
	PATH=/usr/i386-linux-cgc/bin:$(PATH) $(LD) $(CGC_LDFLAGS) $@.o -o $@
	echo -n "AAAA" > $@.tmp
	dd if=$@.tmp of=$@ conv=notrunc count=4 bs=1 seek=52

$(ELF_BINS):
	$(CC) $(CFLAGS) $@.c -o $@

.PHONY: test

test: $(CGC_TESTS)

$(CGC_TESTS):
	python $@.py

clean:
	rm -f $(ELF_BINS) $(CGC_BINS) $(CGC_BAD_BINS) *.o


