dnl
dnl Configure for Xnp2
dnl

dnl require autoconf 2.59
AC_PREREQ(2.59)
AC_REVISION($Revision: 1.5 $)

AC_INIT(Xnp2, 0.83, nonakap@gmail.com, xnp2)
AM_INIT_AUTOMAKE([no-define no-dist no-installinfo subdir-objects])
AC_CONFIG_SRCDIR([../np2ver.h])
AC_CONFIG_HEADERS(config.h)
AC_CANONICAL_HOST

dnl
dnl Checks for programs.
dnl
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_RANLIB
AC_PROG_INSTALL

dnl
dnl Checks for libraries.
dnl
AC_PATH_XTRA

dnl
dnl Checks for header files.
dnl
AC_STDC_HEADERS
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h malloc.h stddef.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h termios.h unistd.h])

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_C_BIGENDIAN
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

dnl
dnl Check GCC
dnl
if test x"$GCC" != "xyes"; then
    AC_MSG_ERROR(This package required GCC.)
fi

dnl
dnl Checks for library functions.
dnl
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gettimeofday memset mkdir strcasecmp strdup strerror strtol])

dnl
dnl Checks for math library functions.
dnl
AC_CHECK_FUNCS(floor,,AC_CHECK_LIB(m,floor))
AC_CHECK_FUNCS(pow,,AC_CHECK_LIB(m,pow))
AC_CHECK_FUNCS(sqrt,,AC_CHECK_LIB(m,sqrt))

dnl
dnl Xnp2 configure options.
dnl
AC_ARG_ENABLE(ia32,
	AC_HELP_STRING([--enable-ia32],[Use IA-32 emulation [[default=no]]]),,
	[enable_ia32="no"])
AC_ARG_ENABLE(xf86vidmode,
	AC_HELP_STRING([--enable-xf86vidmode],[Use XF86VidMode extension [[default=yes]]]),,
	[enable_xf86vidmode="yes"])
AC_ARG_ENABLE(sdl,
	AC_HELP_STRING([--enable-sdl],[Use SDL library [[default=yes]]]),,
	[enable_sdl="yes"])
AC_ARG_ENABLE(sdlmixer,
	AC_HELP_STRING([--enable-sdlmixer],[Use SDL_mixer library [[default=yes]]]),,
	[enable_sdlmixer="yes"])
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug],[Enable debugging [[default=no]]]),,
	[enable_debug="no"])

dnl
dnl Checks for IA-32 CPU emulation
dnl
AM_CONDITIONAL(CPUCORE_IA32, test x"$enable_ia32" = "xyes")

dnl
dnl Checks for XF86VidMode extension
dnl
if test x"$enable_xf86vidmode" = "xyes"; then
	AC_CHECK_LIB(Xxf86vm, XF86VidModeQueryExtension,
		X_PRE_LIBS="$X_PRE_LIBS -lXxf86vm"
		AC_DEFINE(HAVE_XF86VIDMODE,1,[Define to 1 if you have the libXxf86vm library.]),
		,
		$X_LIBS $X_PRE_LIBS -lXext $X_EXTRA_LIBS)
fi

dnl
dnl Checks for GTK+
dnl
AM_PATH_GTK_2_0(2.6.0,,
    AC_MSG_ERROR(Test for GTK failed. See the file 'x11/INSTALL.ja' for help))
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
if test x"$no_gtk" != "xyes"; then
	AC_DEFINE(USE_GTK2,1,[Define to 1 if you have the GTK+-2.6 (or later) library.])
fi

dnl
dnl Checks for SDL
dnl
if test x"$enable_sdlmixer" = "xyes"; then
	AM_PATH_SDL(1.2.0)
	if test x"$no_sdl" != "xyes"; then
		AC_DEFINE(USE_SDLAUDIO,,[Define if you have the SDL library.])
	fi
fi

dnl
dnl Checks for SDL_mixer
dnl
if test x"$enable_sdlmixer" = "xyes"; then
	if test x"$no_sdl" != "xyes"; then
		AC_CHECK_LIB(SDL_mixer,Mix_OpenAudio,
			AC_DEFINE(USE_SDLMIXER,,[Define if you have the SDL_mixer library.])
			SDL_LIBS="$SDL_LIBS -lSDL_mixer",
			$SDL_CFLAGS, $SDL_LIBS)
	fi
fi

dnl
dnl Checks for debug mode
dnl
AC_MSG_CHECKING([whether to enable debugging])
if test x"$enable_debug" = "xyes"; then
	CFLAGS="$CFLAGS -DDEBUG -g -O0"
	CFLAGS="$CFLAGS -Wall -Wcast-align"
	CFLAGS="$CFLAGS -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations"
	CFLAGS="$CFLAGS -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-missing-field-initializers"
	CFLAGS="$CFLAGS -Wstack-protector -fstack-protector --param ssp-buffer-size=1"
else
	CFLAGS="$CFLAGS -DNDEBUG"
fi
AC_MSG_RESULT($enable_debug)

dnl
dnl Output Makefiles
dnl
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
