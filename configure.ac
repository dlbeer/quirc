AC_INIT([quirc], [1.0], [https://github.com/dlbeer/quirc/issues], [quirc], [https://github.com/dlbeer/quirc])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h:config.in)
AM_INIT_AUTOMAKE
dnl AM_SILENT_RULES req. automake 1.11.  [no] defaults V=1
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

PKG_PROG_PKG_CONFIG
AC_EXEEXT
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_SED
AC_PROG_INSTALL
AC_PROG_LN_S

AC_HEADER_STDC

AC_ARG_ENABLE([quirc-examples],
    AS_HELP_STRING([--disable-quirc-examples  disable compilation of examples]),
    [], [enable_compile_examples=yes])

AC_MSG_CHECKING(whether to disable building of examples)
if test "$enable_compile_examples" = "yes"; then
  AC_MSG_RESULT(no)

  PKG_CHECK_MODULES(sdl, sdl)
  AC_SUBST(sdl_CFLAGS)
  AC_SUBST(sdl_LIBS)

  AC_CHECK_HEADERS(setjmp.h jpeglib.h)
  AC_CHECK_HEADERS(linux/videodev2.h linux/videodev.h)
else
  AC_MSG_RESULT(yes)
fi

AM_CONDITIONAL([COMPILE_EXAMPLES], [test "$enable_compile_examples" = "yes"])

AC_SEARCH_LIBS([rint], [m])

AC_ARG_ENABLE([large-images],
    AS_HELP_STRING([--enable-large-images  enable decoding of large images]),
    [large_images=true],
    [large_images=false])

AC_MSG_CHECKING(whether to support decoding lage images)
if test "x$large_images" = "xtrue"; then
  AC_DEFINE(DECODE_LARGE_IMAGES, 1, [Define if decoding of large images is enabled])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi 

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
