AUTOMAKE_OPTIONS = foreign subdir-objects

ACLOCAL_AMFLAGS = -I m4

VERSION_INFO = 1:0:0

lib_LTLIBRARIES = libquirc.la
BUILT_SOURCES = lib/quirc-config.h
lib/quirc-config.h: config.h make-quirc-config.sed
	$(AM_V_at)$(SED) -f $(srcdir)/make-quirc-config.sed < config.h > $@T
	$(AM_V_at)mv -f $@T $@

include_HEADERS = lib/quirc.h \
	lib/quirc-config.h

libquirc_la_SOURCES = lib/decode.c \
	lib/identify.c \
	lib/quirc.c \
	lib/version_db.c
libquirc_la_LIBADD = $(SYS_LIBS)
libquirc_la_LDFLAGS = -version-info $(VERSION_INFO)

bin_PROGRAMS = quirc-demo \
	quirc-scanner

noinst_PROGRAMS = qrtest \
	inspect

DEMO_SOURCES = \
    demo/camera.c \
    demo/mjpeg.c \
    demo/convert.c \
    demo/dthash.c \
    demo/demoutil.c

qrtest_SOURCES = tests/dbgutil.c tests/qrtest.c
qrtest_LDADD = libquirc.a -ljpeg

inspect_SOURCES = tests/dbgutil.c tests/inspect.c
inspect_CFLAGS = $(sdl_CFLAGS)
inspect_LDADD = libquirc.a -ljpeg $(sdl_LIBS) -lSDL_gfx

quirc_demo_SOURCES = $(DEMO_SOURCES) tests/dbgutil.c demo/demo.c
quirc_demo_CFLAGS = $(sdl_CFLAGS)
quirc_demo_LDADD = libquirc.a -ljpeg $(sdl_LIBS) -lSDL_gfx

quirc_scanner_SOURCES = $(DEMO_SOURCES) tests/dbgutil.c demo/scanner.c
quirc_scanner_LDADD = libquirc.a -ljpeg

AM_CPPFLAGS = -I$(srcdir)/lib $(SYS_INCLUDES)

DISTCLEANFILES = ./lib/quirc-config.h
