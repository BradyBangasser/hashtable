PWD:=$(shell pwd)

ifdef DEBUG
	FLAGS:=$(FLAGS) -g -Og
else
	FLAGS:=$(FLAGS) -O2
endif

all: $(PWD)/libhashtable.a

test: $(PWD)/hashtable.c
	$(CC) -DINTERNAL_TEST -Og -g $< -o $@

install: all
	cp $(PWD)/libhashtable.a /usr/local/lib/libhashtable.a
	cp $(PWD)/hashtable.h /usr/local/include/hashtable.h

clean: 
	rm -f $(PWD)/libhashtable.a $(PWD)/hashtable.c.o

$(PWD)/libhashtable.a: $(PWD)/hashtable.c.o
	ar r $@ $^ > /dev/null

$(PWD)/%.c.o: $(PWD)/%.c
	$(CC) -c -o $@ $^ $(FLAGS)
