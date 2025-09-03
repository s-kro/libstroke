/*
   libstroke - an X11 stroke interface library
   Copyright (c) 1996,1997,1998,1999,2001  Mark F. Willey, ETLA Technical

   See the files COPYRIGHT and LICENSE for distribution information.

*/

/*
   This test program was modified from the xev program.
   The following is a copy of the copyright for the xev program:
*/

/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

* xev author:  Jim Fulton, MIT X Consortium
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <ctype.h>

#include "stroke.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

void set_sizehints (XSizeHints *hintp,
                    int min_width, int min_height, int defwidth,
                    int defheight, int defx, int defy, char *geom);
void do_ButtonPress (XEvent *eventp);
void do_ButtonPress (XEvent *eventp);
void do_ButtonRelease (XEvent *eventp);
void do_MotionNotify (XEvent *eventp);



// send motion events to the stroek_record function?
int send_motion;

#define WINDOW_MIN_WIDTH 500
#define WINDOW_MIN_HEIGHT 500
#define WINDOW_DEF_WIDTH (WINDOW_MIN_WIDTH + 100)
#define WINDOW_DEF_HEIGHT (WINDOW_MIN_HEIGHT + 100)
#define WINDOW_DEF_X 100
#define WINDOW_DEF_Y 100


typedef unsigned long Pixel;

char *Yes = "YES";
char *No = "NO";
char *Unknown = "unknown";

char *ProgramName;
Display *dpy;
int screen;

int
usage (void)
{
    static char *msg[] = {
"    -display displayname                X server to contact",
"    -geometry geom                      size and location of window",
"    -bw pixels                          border width in pixels",
"    -bs {NotUseful,WhenMapped,Always}   backingstore attribute",
"    -id windowid                        use existing window",
"    -s                                  set save-unders attribute",
"    -name string                        window name",
"    -rv                                 reverse video",
"",
NULL};
    char **cpp;

    fprintf (stderr, "usage:  %s [-options ...]\n", ProgramName);
    fprintf (stderr, "where options include:\n");

    for (cpp = msg; *cpp; cpp++) {
	fprintf (stderr, "%s\n", *cpp);
    }

    exit (1);
}

static int
parse_backing_store (char *s)
{
    int len = strlen (s);
    char *cp;

    for (cp = s; *cp; cp++) {
	if (isascii (*cp) && isupper (*cp)) *cp = tolower (*cp);
    }

    if (strncmp (s, "notuseful", len) == 0) return (NotUseful);
    if (strncmp (s, "whenmapped", len) == 0) return (WhenMapped);
    if (strncmp (s, "always", len) == 0) return (Always);

    return usage ();
}

char sequence[STROKE_MAX_SEQUENCE+1];

int
main (int argc, char **argv)
{
  char *displayname = NULL;
  char *geom = NULL;
  int i;
  XSizeHints hints;
  int borderwidth = 2;
  Window w;
  XSetWindowAttributes attr;
  XWindowAttributes wattr;
  unsigned long mask = 0L;
  int done;
  char *name = "Stroke Tester";
  Bool reverse = False;
  unsigned long back, fore;
  send_motion = FALSE;
  w = 0;
  ProgramName = argv[0];
  for (i = 1; i < argc; i++) {
    char *arg = argv[i];

	if (arg[0] == '-') {
	    switch (arg[1]) {
	      case 'd':			/* -display host:dpy */
		if (++i >= argc) usage ();
		displayname = argv[i];
		continue;
	      case 'g':			/* -geometry geom */
		if (++i >= argc) usage ();
		geom = argv[i];
		continue;
	      case 'b':
		switch (arg[2]) {
		  case 'w':		/* -bw pixels */
		    if (++i >= argc) usage ();
		    borderwidth = atoi (argv[i]);
		    continue;
		  case 's':		/* -bs type */
		    if (++i >= argc) usage ();
		    attr.backing_store = parse_backing_store (argv[i]);
		    mask |= CWBackingStore;
		    continue;
		  default:
		    usage ();
		}
	      case 'i':			/* -id */
		if (++i >= argc) usage ();
		sscanf(argv[i], "0x%lx", &w);
		if (!w)
		    sscanf(argv[i], "%ld", &w);
		if (!w)
		    usage ();
		continue;
	      case 'n':			/* -name */
		if (++i >= argc) usage ();
		name = argv[i];
		continue;
	      case 'r':			/* -rv */
		reverse = True;
		continue;
	      case 's':			/* -s */
		attr.save_under = True;
		mask |= CWSaveUnder;
		continue;
	      default:
		usage ();
	    }				/* end switch on - */
	} else
	  usage ();
    }					/* end for over argc */

    dpy = XOpenDisplay (displayname);
    if (!dpy) {
	fprintf (stderr, "%s:  unable to open display '%s'\n",
		 ProgramName, XDisplayName (displayname));
	exit (1);
    }

    screen = DefaultScreen (dpy);

    /* select for all events */
    attr.event_mask =      ButtonPressMask |
			   ButtonReleaseMask |
			   PointerMotionMask |
			   Button1MotionMask |
			   Button2MotionMask |
			   Button3MotionMask |
			   Button4MotionMask |
			   Button5MotionMask |
			   ButtonMotionMask |
			   OwnerGrabButtonMask;

    if (w) {
	XGetWindowAttributes(dpy, w, &wattr);
	if (wattr.all_event_masks & ButtonPressMask)
	    attr.event_mask &= ~ButtonPressMask;
	attr.event_mask &= ~SubstructureRedirectMask;
	XSelectInput(dpy, w, attr.event_mask);
    } else {
	set_sizehints (&hints, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT,
		       WINDOW_DEF_WIDTH, WINDOW_DEF_HEIGHT,
		       WINDOW_DEF_X, WINDOW_DEF_Y, geom);

	if (reverse) {
	    back = BlackPixel(dpy,screen);
	    fore = WhitePixel(dpy,screen);
	} else {
	    back = WhitePixel(dpy,screen);
	    fore = BlackPixel(dpy,screen);
	}

	attr.background_pixel = back;
	attr.border_pixel = fore;
	mask |= (CWBackPixel | CWBorderPixel | CWEventMask);

	w = XCreateWindow (dpy, RootWindow (dpy, screen), hints.x, hints.y,
			   hints.width, hints.height, borderwidth, 0,
			   InputOutput, (Visual *)CopyFromParent,
			   mask, &attr);

	XSetStandardProperties (dpy, w, name, NULL, (Pixmap) 0,
				argv, argc, &hints);

	XMapWindow (dpy, w);

	// initialize stroke library routines
#ifdef STROKE_MOUSE_FOOTPRINTS
        stroke_init_with_mouse_footprints (dpy, w);
#else
	stroke_init ();
#endif

    }

    for (done = 0; !done; ) {
	XEvent event;

	XNextEvent (dpy, &event);

	switch (event.type) {
	  case ButtonPress:
	    do_ButtonPress (&event);
	    break;
	  case ButtonRelease:
	    do_ButtonRelease (&event);
	    break;
	  case MotionNotify:
	    do_MotionNotify (&event);
	    break;
	  default:
	    printf ("Unknown event type %d\n", event.type);
	    break;
	}
    }

    XCloseDisplay (dpy);
    exit (0);
}

void
do_ButtonPress (eventp)
    XEvent *eventp;
{
    XButtonEvent *e = (XButtonEvent *) eventp;

    // begin recording points in stroke
    if (e->button == 2)
      send_motion = TRUE;

    return;
}

void
do_ButtonRelease (eventp)
    XEvent *eventp;
{
    XButtonEvent *e = (XButtonEvent *) eventp;

    // if it's button 2, translate the stroke
    if (e->button == 2) {
      send_motion = FALSE;

      if (stroke_trans (sequence) == TRUE)
	printf ("Translation succeeded: ");
      else
	printf ("Translation failed: ");
      printf ("Sequence=\"%s\"\n",sequence);
    }

    return;
}

void
do_MotionNotify (eventp)
    XEvent *eventp;
{
    XMotionEvent *e = (XMotionEvent *) eventp;

    // if button 2 is down, record the point in the stroke
    if (send_motion == TRUE) {
      stroke_record (e->x,e->y);
    }

    return;
}

void
set_sizehints (hintp, min_width, min_height,
	       defwidth, defheight, defx, defy, geom)
    XSizeHints *hintp;
    int min_width, min_height, defwidth, defheight, defx, defy;
    char *geom;
{
    int geom_result;

    /* set the size hints, algorithm from xlib xbiff */

    hintp->width = hintp->min_width = min_width;
    hintp->height = hintp->min_height = min_height;
    hintp->flags = PMinSize;
    hintp->x = hintp->y = 0;
    geom_result = NoValue;
    if (geom != NULL) {
	geom_result = XParseGeometry (geom, &hintp->x, &hintp->y,
				      (unsigned int *)&hintp->width,
				      (unsigned int *)&hintp->height);
	if ((geom_result & WidthValue) && (geom_result & HeightValue)) {
#define max(a,b) ((a) > (b) ? (a) : (b))
	    hintp->width = max (hintp->width, hintp->min_width);
	    hintp->height = max (hintp->height, hintp->min_height);
	    hintp->flags |= USSize;
	}
	if ((geom_result & XValue) && (geom_result & YValue)) {
	    hintp->flags += USPosition;
	}
    }
    if (!(hintp->flags & USSize)) {
	hintp->width = defwidth;
	hintp->height = defheight;
	hintp->flags |= PSize;
    }
/*
    if (!(hintp->flags & USPosition)) {
	hintp->x = defx;
	hintp->y = defy;
	hintp->flags |= PPosition;
    }
 */
    if (geom_result & XNegative) {
	hintp->x = DisplayWidth (dpy, DefaultScreen (dpy)) + hintp->x -
		    hintp->width;
    }
    if (geom_result & YNegative) {
	hintp->y = DisplayHeight (dpy, DefaultScreen (dpy)) + hintp->y -
		    hintp->height;
    }
    return;
}
