TARGET=cgcef_verify
MAN=$(TARGET).1.gz
INSTALLDIR=$(DESTDIR)/usr

all: $(TARGET) $(MAN)

install: $(TARGET) $(MAN)
	install -d $(INSTALLDIR)/bin
	install -c -m 755 $(TARGET) $(INSTALLDIR)/bin/
	install -d $(INSTALLDIR)/share/man/man1
	install $(MAN) $(INSTALLDIR)/share/man/man1

%.1.gz: %.md
	pandoc -s -t man $< -o $<.tmp
	gzip -9 < $<.tmp > $@


clean:
	rm -f $(MAN) $(TARGET).md.tmp
