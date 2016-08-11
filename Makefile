# quirc -- QR-code recognition library
# Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

CC ?= gcc
PREFIX ?= /usr/local
SDL_CFLAGS != pkg-config --cflags sdl
SDL_LIBS != pkg-config --libs sdl

LIB_VERSION = 1.0

CFLAGS ?= -O3 -Wall -fPIC
QUIRC_CFLAGS = -Ilib $(CFLAGS) $(SDL_CFLAGS)
LIB_OBJ = \
    lib/decode.o \
    lib/identify.o \
    lib/quirc.o \
    lib/version_db.o
DEMO_OBJ = \
    demo/camera.o \
    demo/mjpeg.o \
    demo/convert.o \
    demo/dthash.o \
    demo/demoutil.o

all: libquirc.so qrtest inspect quirc-demo quirc-scanner

qrtest: tests/dbgutil.o tests/qrtest.o libquirc.a
	$(CC) -o $@ tests/dbgutil.o tests/qrtest.o libquirc.a $(LDFLAGS) -lm -ljpeg -lpng

inspect: tests/dbgutil.o tests/inspect.o libquirc.a
	$(CC) -o $@ tests/dbgutil.o tests/inspect.o libquirc.a $(LDFLAGS) -lm -ljpeg -lpng $(SDL_LIBS) -lSDL_gfx

quirc-demo: $(DEMO_OBJ) demo/demo.o libquirc.a
	$(CC) -o $@ $(DEMO_OBJ) demo/demo.o libquirc.a $(LDFLAGS) -lm -ljpeg $(SDL_LIBS) -lSDL_gfx

quirc-scanner: $(DEMO_OBJ) demo/scanner.o libquirc.a
	$(CC) -o $@ $(DEMO_OBJ) demo/scanner.o libquirc.a $(LDFLAGS) -lm -ljpeg

libquirc.a: $(LIB_OBJ)
	rm -f $@
	ar cru $@ $(LIB_OBJ)
	ranlib $@

.PHONY: libquirc.so
libquirc.so: libquirc.so.$(LIB_VERSION)

libquirc.so.$(LIB_VERSION): $(LIB_OBJ)
	$(CC) -shared -o $@ $(LIB_OBJ) $(LDFLAGS) -lm

.c.o:
	$(CC) $(QUIRC_CFLAGS) -o $@ -c $<

install: libquirc.a libquirc.so.$(LIB_VERSION) quirc-demo quirc-scanner
	install -o root -g root -m 0644 lib/quirc.h $(DESTDIR)$(PREFIX)/include
	install -o root -g root -m 0644 libquirc.a $(DESTDIR)$(PREFIX)/lib
	install -o root -g root -m 0755 libquirc.so.$(LIB_VERSION) \
		$(DESTDIR)$(PREFIX)/lib
	install -o root -g root -m 0755 quirc-demo $(DESTDIR)$(PREFIX)/bin
	install -o root -g root -m 0755 quirc-scanner $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/include/quirc.h
	rm -f $(DESTDIR)$(PREFIX)/lib/libquirc.so.$(LIB_VERSION)
	rm -f $(DESTDIR)$(PREFIX)/lib/libquirc.a
	rm -f $(DESTDIR)$(PREFIX)/bin/quirc-demo
	rm -f $(DESTDIR)$(PREFIX)/bin/quirc-scanner

clean:
	rm -f */*.o
	rm -f */*.lo
	rm -f libquirc.a
	rm -f libquirc.so.$(LIB_VERSION)
	rm -f qrtest
	rm -f inspect
	rm -f quirc-demo
	rm -f quirc-scanner
