# autoconf macros for detecting libstroke support 
# Use AC_CHECK_LIBSTROKE in your configure.in.  

# the smr_* functions have been taken from fvwm-2.4.0
dnl Allow argument for optional libraries; wraps AC_ARG_WITH, to
dnl provide a "--with-foo-library" option in the configure script, where foo
dnl is presumed to be a library name.  The argument given by the user
dnl (i.e. "bar" in ./configure --with-foo-library=bar) may be one of three
dnl things:
dnl     * boolean (no, yes or blank): whether to use library or not
dnl     * file: assumed to be the name of the library
dnl     * directory: assumed to *contain* the library
dnl
dnl The argument is sanity-checked.  If all is well, two variables are
dnl set: "with_foo" (value is yes, no, or maybe), and "foo_LIBS" (value
dnl is either blank, a file, -lfoo, or '-L/some/dir -lfoo').  The idea
dnl is: the first tells you whether the library is to be used or not
dnl (or the user didn't specify one way or the other) and the second
dnl to put on the command line for linking with the library.
dnl
dnl Usage:
dnl smr_ARG_WITHLIB(name, libname, description)
dnl
dnl name                name for --with argument ("foo" for libfoo)
dnl libname             (optional) actual name of library,
dnl                     if different from name
dnl description         (optional) used to construct help string
dnl
AC_DEFUN(smr_ARG_WITHLIB, [

ifelse($2, , smr_lib=[$1], smr_lib=[$2])

AC_ARG_WITH([$1]-library,
ifelse($3, ,
[  --with-$1-library[=PATH]  use $1 library],
[  --with-$1-library[=PATH]  use $1 library ($3)]),
[
    if test "$withval" = yes; then
        with_[$1]=yes
        [$1]_LIBS="-l${smr_lib}"
    elif test "$withval" = no; then
        with_[$1]=no
        [$1]_LIBS=
    else
        with_[$1]=yes
        if test -f "$withval"; then
            [$1]_LIBS=$withval
        elif test -d "$withval"; then
            [$1]_LIBS="-L$withval -l${smr_lib}"
        else
            AC_MSG_ERROR(argument must be boolean, file, or directory)
        fi
    fi
], [
    with_[$1]=maybe
    [$1]_LIBS="-l${smr_lib}"
])])



dnl Check if the include files for a library are accessible, and
dnl define the variable "name_CFLAGS" with the proper "-I" flag for
dnl the compiler.  The user has a chance to specify the includes
dnl location, using "--with-foo-includes".
dnl
dnl This should be used *after* smr_ARG_WITHLIB *and* AC_CHECK_LIB are
dnl successful.
dnl
dnl Usage:
dnl smr_ARG_WITHINCLUDES(name, header, extra-flags)
dnl
dnl name                library name, MUST same as used with smr_ARG_WITHLIB
dnl header              a header file required for using the lib
dnl extra-flags         (optional) flags required when compiling the
dnl                     header, typically more includes; for ex. X_CFLAGS
dnl
AC_DEFUN(smr_ARG_WITHINCLUDES, [

AC_ARG_WITH([$1]-includes,
[  --with-$1-includes=DIR  set directory for $1 headers],
[
    if test -d "$withval"; then
        [$1]_CFLAGS="-I${withval}"
    else
        AC_MSG_ERROR(argument must be a directory)
    fi])

    dnl We need to put the given include directory into CPPFLAGS temporarily, but
    dnl then restore CPPFLAGS to its old value.
    dnl
    smr_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS ${[$1]_CFLAGS}"
    ifelse($3, , , CPPFLAGS="$CPPFLAGS [$3]")

    AC_CHECK_HEADERS($2)

    CPPFLAGS=$smr_save_CPPFLAGS
])



dnl Probe for an optional library.  This macro creates both
dnl --with-foo-library and --with-foo-includes options for the configure
dnl script.  If --with-foo-library is *not* specified, the default is to
dnl probe for the library, and use it if found.
dnl
dnl Usage:
dnl smr_CHECK_LIB(name, libname, desc, func, header, x-libs, x-flags)
dnl
dnl name        name for --with options
dnl libname     (optional) real name of library, if different from
dnl             above
dnl desc        (optional) short descr. of library, for help string
dnl func        function of library, to probe for
dnl header      (optional) header required for using library
dnl x-libs      (optional) extra libraries, if needed to link with lib
dnl x-flags     (optional) extra flags, if needed to include header files
dnl
AC_DEFUN(smr_CHECK_LIB,
[
ifelse($2, , smr_lib=[$1], smr_lib=[$2])
ifelse($5, , , smr_header=[$5])
smr_ARG_WITHLIB($1,$2,$3)
if test "$with_$1" != no; then
    AC_CHECK_LIB($smr_lib, $4,
        smr_havelib=yes,
        smr_havelib=no; problem_$1=": Cannot find required lib$smr_lib",
        ifelse($6, , ${$1_LIBS}, [${$1_LIBS} $6]))
    if test "$smr_havelib" = yes -a "$smr_header" != ""; then
        smr_ARG_WITHINCLUDES($1, $smr_header, $7)
        smr_safe=`echo "$smr_header" | sed 'y%./+-%__p_%'`
        if eval "test \"`echo '$ac_cv_header_'$smr_safe`\" != yes"; then
            smr_havelib=no
            problem_$1=": Can't find required $smr_header"
        fi
    fi
    if test "$smr_havelib" = yes; then
        with_$1=yes
        problem_$1=
    else
        $1_LIBS=
        $1_CFLAGS=
        with_$1=no
    fi
else
    problem_$1=": Explicitly disabled"
fi])

dnl AC_CHECK_LIBSTROKE
dnl  The HAVE_LIBSTROKE macro will be defined (to be used in
dnl  config.h.in) and stroke_CFLAGS stroke_LIBS will be set to proper
dnl  values (to be used for compiling and linking)
dnl  --with-stroke-library and --with-stroke-includes can be used to
dnl  tell configure where to find the stroke.h header file and the
dnl  libstroke library
AC_DEFUN([AC_CHECK_LIBSTROKE],
[
	smr_CHECK_LIB(stroke, , mouse strokes recognition, stroke_init, stroke.h,  $GTK_LIBS, $GTK_CFLAGS)
	AC_SUBST(gstroke_LIBS)
	AC_SUBST(gstroke_CFLAGS)
	if test "x$with_stroke" = xyes; then
		AC_DEFINE(HAVE_STROKE)
	fi
])
