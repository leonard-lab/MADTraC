CC = gcc
AR = ar

#CFLAGS = -g -Wall -I.
CFLAGS = -O3 -Wall -I.
LDFLAGS = -L. -lhungarian

HUNGLIB = libhungarian.a
VERSION = 0.3

# place to put Python modules (they'll go in PREFIX/lib/python2.2/site-packages)
PREFIX=/usr/local

all: $(HUNGLIB) test timetest

test: test.c $(HUNGLIB)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

timetest: timetest.c $(HUNGLIB)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lm

OBJS = hungarian.o makeprob.o

$(HUNGLIB): $(OBJS)
	$(AR) cr $@ $(OBJS)
	
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

python: $(HUNGLIB)
	cd python && \
	python ./setup.py build && \
	cd ..

pyinstall: $(HUNGLIB)
	cd python && \
	python ./setup.py install --prefix=$(PREFIX) && \
	cd ..

dist: clean
	cd .. && rm -f libhungarian-$(VERSION).tar.gz && ln -sf hungarian libhungarian-$(VERSION) && tar -hcvzf libhungarian-$(VERSION).tar.gz libhungarian-$(VERSION) --exclude libhungarian-$(VERSION).tar.gz --exclude CVS && rm -f libhungarian-$(VERSION)
	mv ../libhungarian-$(VERSION).tar.gz .

clean:
	rm -f *.o *.a test timetest
	# Clean the python build directory
	rm -rf python/build
