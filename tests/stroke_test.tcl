#!/usr/bin/wish
# stroke_test.tcl
# A clone of stroke_test using Tcl/Tk, which is far shorter than the
# plain C version.  Long live Tcl/Tk!
# By Chris Laas, chrisl@cybercom.net.
# This file falls under the copyright of libstroke.

load ../libstroke/.libs/libstroke_tcl.so.0.2
bind . <2>         { stroke record %x %y }
bind . <B2-Motion> { stroke record %x %y }
bind . <ButtonRelease-2> { puts [stroke translate] }
