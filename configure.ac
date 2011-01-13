#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#####################################################################
# Cadence Configure.in
# 13/9/2010
# Nicolas Pope
#####################################################################

#AC_PREREQ(2.59)
AC_INIT(Cadence, 1.1)
AC_CONFIG_SRCDIR(loader/linux.cpp)
AC_CANONICAL_TARGET
AC_CONFIG_HEADER(config.h)

dnl #####################################################################
dnl Functions for use later on
dnl #####################################################################

AC_DEFUN([CF_CHECK_CACHE],
[
#
# Check if we're accidentally using a cache from a different machine.
# Derive the system name, as a check for reusing the autoconf cache.
#
# If we've packaged config.guess and config.sub, run that (since it does a
# better job than uname).
#
if test -f $srcdir/config.guess ; then
        AC_CANONICAL_HOST
        system_name="$host_os"
else
        system_name="`(uname -s -r) 2>/dev/null`"
        if test -z "$system_name" ; then
                system_name="`(hostname) 2>/dev/null`"
        fi
fi
test -n "$system_name" && AC_DEFINE_UNQUOTED(SYSTEM_NAME,"$system_name")
AC_CACHE_VAL(cf_cv_system_name,[cf_cv_system_name="$system_name"])

test -z "$system_name" && system_name="$cf_cv_system_name"
test -n "$cf_cv_system_name" && AC_MSG_RESULT("Configuring for $cf_cv_system_name")

if test ".$system_name" != ".$cf_cv_system_name" ; then
        AC_MSG_RESULT("Cached system name does not agree with actual")
        AC_ERROR("Please remove config.cache and try again.")
fi
])dnl


AC_DEFUN([CF_ANSI_CC_CHECK],
[
AC_MSG_CHECKING(for ${CC-cc} option to accept ANSI C)
AC_CACHE_VAL(cf_cv_ansi_cc,[
cf_cv_ansi_cc=no
cf_save_CFLAGS="$CFLAGS"
# Don't try gcc -ansi; that turns off useful extensions and
# breaks some systems' header files.
# AIX                   -qlanglvl=ansi
# Ultrix and OSF/1      -std1
# HP-UX                 -Aa -D_HPUX_SOURCE
# SVR4                  -Xc
# UnixWare 1.2          (cannot use -Xc, since ANSI/POSIX clashes)
for cf_arg in "-DCC_HAS_PROTOS" "" -qlanglvl=ansi -std1 "-Aa -D_HPUX_SOURCE" -Xc
do
        CFLAGS="$cf_save_CFLAGS $cf_arg"
        AC_TRY_COMPILE(
[
#ifndef CC_HAS_PROTOS
#if !defined(__STDC__) || __STDC__ != 1
choke me
#endif
#endif
],[
        int test (int i, double x);
        struct s1 {int (*f) (int a);};
        struct s2 {int (*f) (double a);};],
        [cf_cv_ansi_cc="$cf_arg"; break])
done
CFLAGS="$cf_save_CFLAGS"
])
AC_MSG_RESULT($cf_cv_ansi_cc)

if test "$cf_cv_ansi_cc" != "no"; then
if test ".$cf_cv_ansi_cc" != ".-DCC_HAS_PROTOS"; then
        CFLAGS="$CFLAGS $cf_cv_ansi_cc"
else
        AC_DEFINE(CC_HAS_PROTOS)
fi
fi
])dnl


dnl #####################################################################
dnl End functions - carry on with the work
dnl #####################################################################

# Checks for programs.
AC_PROG_RANLIB
AC_PROG_CXX
AC_PROG_CC
AC_LANG(C++)
AC_PROG_LEX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h stddef.h stdlib.h string.h sys/time.h unistd.h])
AC_CHECK_HEADERS([string vector list])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

# Might define TM_IN_SYS_TIME, "which means that including sys/time.h
# had better define struct tm." (from Autoconf manual - huh?).
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([memset sqrt strstr strdup])

# Tcl on Linux on PC seems to require -ldl
AC_CHECK_LIB(dl, dlerror)

#########################################################################
# X / GUI libraries
#########################################################################
dnl Sets $X_CFLAGS, $X_LIBS, $X_EXTRA_LIBS, $X_PRE_LIBS
AC_PATH_XTRA

AC_MSG_CHECKING(system type to determine GUI libraries)

dnl The following isn't a great way to do this -- it is borrowed from
dnl the run time linker config test below.  Hopefully it'll do for
dnl our common cases though.

case "`(uname -s) 2>/dev/null`" in
CYGWIN*)
	# Probably Windows
	AW_X_LIBS="-lGDI32 -lUSER32 -lCOMDLG32 -lKERNEL32 -Wl,--subsystem,windows"
	ICON_OBJS="eden-res.o"
	;;
Darwin*)
	# Probably Mac
	AW_X_LIBS="-framework CoreFoundation -framework Carbon"
	ICON_OBJS=""
	;;
*)
	# Probably some flavour of X11
	AW_X_LIBS="-lXt -lX11"
	if test "$aw_want_sasami" = "yes"; then
		AW_X_LIBS="$AW_X_LIBS -lXmu -lXext"
	fi
	ICON_OBJS=""
	;;
esac

AC_MSG_RESULT(think we need $AW_X_LIBS)


#########################################################################
# Memory alignment
#########################################################################

# I could use the gcc __alignof__ function, but then this would
# restrict us to using gcc only. So we attempt to detect the alignment
# required instead.

AC_MSG_CHECKING(how doubles should be aligned)
AC_TRY_RUN(
	[
#include <stdio.h>

struct foo {
  union align {
    double d;
    long l;
    void *p;
  } u;
  char c;
};

int main() {
  struct foo f[2];
  FILE *fp;
  int n;

  if ((fp = fopen("conftestalignoutput", "w")) == NULL) exit(1);

  if( 2*sizeof(union align) != sizeof(f) ) {
    n = fprintf(fp, "%ld\n", (long)(sizeof(struct foo) - sizeof(union align)));
  }
  else {
    n = fprintf(fp, "%ld\n", (long)sizeof(union align));
  }

  /* If fprintf didn't complete successfully, exit code 1 */
  if (n < 0) exit(1);

  if (fclose(fp) != 0) exit(1);

  /* All OK - exit code 0 */
  exit(0);
}
	],
	[
		# The code compiled, linked, executed and had exit code 0,
		# so we can read the alignment value written to the temporary
		# file.
		aw_alignment=`cat conftestalignoutput`
		AC_DEFINE_UNQUOTED([DOUBLE_ALIGNMENT], [$aw_alignment], [Check double alignment])
		AC_MSG_RESULT($aw_alignment byte boundaries)
	],
	[
		# The test failed in some way, so assume the worst.
		AC_DEFINE([DOUBLE_ALIGNMENT], [sizeof(double)], [Check double alignment])
		AC_MSG_RESULT([sizeof(double) byte boundaries])
	],
	[
		# We are cross compiling so can't run the test, so we
		# assume the worst.
		AC_DEFINE([DOUBLE_ALIGNMENT], [sizeof(double)], [Check double alignment])
		AC_MSG_RESULT([sizeof(double) byte boundaries])
	]
)


#########################################################################
# dmalloc (tool for checking for memory leaks etc)
#########################################################################
# I'm trusting the user to get this right, so no checks are done to see
# if the arguments provided are correct.  If they want to run it with a
# memory debugger, they should know what they are doing... I hope.

DMALLOC_INCLUDE=
DMALLOC_LIB=
AC_ARG_WITH(dmalloc-include,
	[  --with-dmalloc-include=DIR
                          dmalloc include file is in DIR],
	[
	AC_DEFINE([HAVE_DMALLOC], 1, [Memory leak check])
	DMALLOC_INCLUDE=-I$withval
	]
)
AC_ARG_WITH(dmalloc-lib,
	[  --with-dmalloc-lib=DIR  dmalloc lib file is in DIR],
	[
	AC_DEFINE([HAVE_DMALLOC], 1, [Memory leak check])
	DMALLOC_LIB="-L$withval -ldmalloc"
	]
)


#########################################################################
# Pascal strings
#########################################################################

# Need to try with -fpascal-strings first as \p only results in a
# warning if the option is not given
aw_save_cflags=$CFLAGS
CFLAGS="$CFLAGS -fpascal-strings"

AC_TRY_COMPILE(
[],
[
char *pascal = "\pHello world";
],
[],
[
	# Had a problem: all we can do is to omit the option again
	CFLAGS="$aw_save_cflags"
]
)


#########################################################################
# Create the Makefiles
#########################################################################

# Substitute the following variables in the Makefiles
AC_SUBST(AW_X_LIBS)

AC_SUBST(DEPEND_MAKEDEFS)

AC_SUBST(ICON_OBJS)


#################

CXXFLAGS="$CXXFLAGS -g -Wall -DLINUX "
CFLAGS="-DLINUX -O3"
LDFLAGS="$LDFLAGS -lpthread -ldl"

if test -f /local/java/qt-x11-opensource/bin/moc ; then
	QTMOC="/local/java/qt-x11-opensource/bin/moc"
	QTLIBS="-L/local/java/qt-x11-opensource/lib"
	QTINCLUDES="-I/local/java/qt-x11-opensource/include/Qt  -I/local/java/qt-x11-opensource/include/QtCore  -I/local/java/qt-x11-opensource/include/QtGui -I/local/java/qt-x11-opensource/include" 
else
	if test -f /usr/include/qt4/Qt/qlist.h; then
		QTMOC="moc"
		QTINCLUDES="-I/usr/include/qt4 -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui"
		QTLIBS=""
	else
		QTMOC="moc"
		QTINCLUDES="-I/usr/include/QtCore -I/usr/include/QtGui"
		QTLIBS=""
	fi
fi

if test "$target_cpu" == "x86_64"; then
	CXXFLAGS="$CXXFLAGS -DX86_64"
	LDFLAGS="$LDFLAGS -L/usr/X11R6/lib64"
else
	CXXFLAGS="$CXXFLAGS -march=pentium3 -mfpmath=sse"
	LDFLAGS="$LDFLAGS -L/usr/X11R6/lib"
fi


# Checks for libraries.
WGDLIBS="-lGL -lGLU -lX11 -lXxf86vm"

##############################################################################
# Select which modules to build
##############################################################################
MODULES="ui basic_agents xnet wgd_base wgd_resources_base wgd_base_2d wgd_base_3d wgd_heightmap wgd_input wgd_models wgd_widgets"


AC_ARG_ENABLE(debug,
	[  --enable-debug    Enable all debug output],
	[
	CXXFLAGS="$CXXFLAGS -DDEBUG"
	CFLAGS="$CFLAGS -DDEBUG"
	]
)

# CXXFLAGS="$CXXFLAGS -I/usr/include/libxml++-2.6 -I/usr/include/glibmm-2.4"


AC_SUBST(QTMOC)
AC_SUBST(QTLIBS)
AC_SUBST(WGDLIBS)
AC_SUBST(QTINCLUDES)
AC_SUBST(MODULES)

AC_OUTPUT(Makefile libcadence/Makefile libcadence/src/Makefile libcadence/src/doste/Makefile libcadence/src/library/Makefile libcadence/scripts/Makefile loader/Makefile loader/cadence modules/Makefile modules/module.mk modules/module-src.mk modules/module-scripts.mk modules/module-data.mk modules/module-scripts-sub.mk modules/module-data-sub.mk modules/ui/Makefile modules/ui/src/Makefile modules/ui/data/Makefile modules/ui/data/ui/Makefile modules/ui/scripts/Makefile modules/ui/scripts/ui/Makefile modules/xnet/Makefile modules/xnet/src/Makefile modules/xnet/scripts/Makefile modules/xnet/scripts/xnet/Makefile modules/basic_agents/Makefile modules/basic_agents/src/Makefile modules/basic_agents/scripts/Makefile modules/basic_agents/scripts/agents/Makefile modules/wgd_base/Makefile modules/wgd_base/src/Makefile modules/wgd_base/scripts/Makefile modules/wgd_base/scripts/wgd/Makefile modules/wgd_base_2d/Makefile modules/wgd_base_2d/src/Makefile modules/wgd_base_2d/scripts/Makefile modules/wgd_base_2d/scripts/wgd/Makefile  modules/wgd_base_3d/Makefile modules/wgd_base_3d/src/Makefile modules/wgd_base_3d/scripts/Makefile modules/wgd_base_3d/scripts/wgd/Makefile modules/wgd_heightmap/Makefile modules/wgd_heightmap/src/Makefile modules/wgd_heightmap/scripts/Makefile modules/wgd_heightmap/scripts/wgd/Makefile modules/wgd_input/Makefile modules/wgd_input/src/Makefile modules/wgd_input/scripts/Makefile modules/wgd_input/scripts/wgd/Makefile modules/wgd_models/Makefile modules/wgd_models/src/Makefile modules/wgd_models/scripts/Makefile modules/wgd_models/scripts/wgd/Makefile modules/wgd_resources_base/Makefile modules/wgd_resources_base/src/Makefile modules/wgd_resources_base/scripts/Makefile modules/wgd_resources_base/scripts/wgd/Makefile modules/wgd_widgets/Makefile modules/wgd_widgets/src/Makefile modules/wgd_widgets/scripts/Makefile modules/wgd_widgets/scripts/wgd/Makefile modules/wgd_widgets/scripts/wgd/themes/Makefile modules/wgd_widgets/scripts/wgd/themes/simple_dark/Makefile modules/wgd_widgets/data/Makefile modules/wgd_widgets/data/wgd/Makefile )

####################
