DESTDIR=
PREFIX=/usr

all: pppoj

pppoj: pppoj.cpp
	g++ pppoj.cpp -o pppoj -lgloox -lpthread

clean:
	rm pppoj

install: pppoj
	install -D -m0755 pppoj $(DESTDIR)/$(PREFIX)/bin/pppoj
