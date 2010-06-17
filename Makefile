# #############################################################################
# CONFIG
#

DESTDIR=
PREFIX=/usr
PPPD_PATH="\"/usr/sbin/pppd\""

#
# #############################################################################

all: pppoj

D=$(shell basename $(shell pwd))

pppoj: pppoj.cpp
	g++ pppoj.cpp -o pppoj -lgloox -lpthread -DPPPD_PATH=$(PPPD_PATH)

clean:
	rm pppoj

install: pppoj
	install -D -m0755 pppoj $(DESTDIR)/$(PREFIX)/bin/pppoj

dist:
	cd .. && tar czf pppoj-`date +%Y%m%d`.tar.gz $(D)/Makefile $(D)/pppoj.cpp
