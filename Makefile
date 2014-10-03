TARGET=cgcef_verify
OBJS=main.o
LIBS=-lcgcef
MAN=$(TARGET).1.gz


INCLUDES+=-I/usr/include -I/usr/include/libcgcef
DEBUG+=-g
WARNS+=-Wall -W -Werror -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
CFLAGS+=$(WARNS) -O $(DEBUG) $(INCLUDES)
LDFLAGS+=-O -g

all: $(TARGET) $(MAN)

install: $(TARGET) $(MAN)
	install -d $(INSTALLDIR)/bin
	install -c -m 755 $(TARGET) $(INSTALLDIR)/bin/
	install -d $(INSTALLDIR)/share/man/man1
	install $(MAN) $(INSTALLDIR)/share/man/man1

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(DEFINES) $*.c -o $*.o

%.1.gz: %.md
	pandoc -s -t man $< -o $<.tmp
	gzip -9 < $<.tmp > $@


clean:
	rm -f $(TARGET) $(OBJS) $(MAN) $(TARGET).md.tmp
