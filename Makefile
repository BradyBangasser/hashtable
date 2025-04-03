PWD:=$(shell pwd)
INSTALL?=/usr/local

ifdef DEBUG
	FLAGS:=$(FLAGS) -g -Og
else
	FLAGS:=$(FLAGS) -O2
endif

all: $(PWD)/libhashtable.a

test: $(PWD)/hashtable.c
	$(CC) -DINTERNAL_TEST -Og -g $< -o $@

install: all
	mkdir -p $(INSTALL)/lib
	mkdir -p $(INSTALL)/include
	cp $(PWD)/libhashtable.a $(INSTALL)/lib/libhashtable.a
	cp $(PWD)/hashtable.h $(INSTALL)/include/hashtable.h

clean: 
	rm -f $(PWD)/libhashtable.a $(PWD)/hashtable.c.o test

$(PWD)/libhashtable.a: $(PWD)/hashtable.c.o
	ar r $@ $^ > /dev/null

$(PWD)/%.c.o: $(PWD)/%.c
	$(CC) -c -o $@ $^ $(FLAGS)
