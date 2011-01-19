# #############################################################################
# CONFIG
#

DESTDIR=
PREFIX=/usr/local
PPPD_PATH="\"/usr/sbin/pppd\""
PPPD_IPS="\"10.0.0.1:10.0.0.2\""

#
# #############################################################################

all: pppoj

D=$(shell basename $(shell pwd))

pppoj: pppoj.cpp
	g++ pppoj.cpp -o pppoj -lgloox -lpthread -DPPPD_PATH=$(PPPD_PATH) -DPPPD_IPS=$(PPPD_IPS)

clean:
	rm pppoj

install: pppoj
	install -D -m0755 pppoj $(DESTDIR)/$(PREFIX)/bin/pppoj

dist:
	cd .. && tar czf pppoj-`date +%Y%m%d`.tar.gz $(D)/Makefile $(D)/pppoj.cpp
