/* GTK+  stroke test program
   Copyright (c) 2000, 2001 Dan Nicolaescu
   See the file COPYING for distribution information.
*/

#include "config.h"

#include <math.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include "gstroke.h"

static gint
delete_event (GtkWidget *widget, GdkEvent *event  G_GNUC_UNUSED, gpointer data G_GNUC_UNUSED)
{
  gtk_widget_destroy (widget);
  gtk_main_quit ();
  return TRUE;
}

static void
clear_clicked (GtkWidget *widget G_GNUC_UNUSED, gpointer data)
{
  gdk_window_clear (GTK_WIDGET (data)->window);
}

static void
exit_clicked (GtkWidget *widget G_GNUC_UNUSED, gpointer data)
{
  gtk_widget_destroy (GTK_WIDGET (data));
  gtk_main_quit ();
}

static void
gstroke_exit (GtkWidget *widget G_GNUC_UNUSED)
{
  gtk_main_quit ();
}

static void
gstroke_clear (GtkWidget *widget)
{
  gdk_window_clear (widget->window);
}

static void
gstroke_help (GtkWidget *widget G_GNUC_UNUSED)
{
  printf ("-----\nStroke 456 clears the screen\n");
  printf ("Stroke 654 exits the program\n");
  printf ("Stroke 4123658 displays the help message\n------\n");
}


int
main (int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *da;
  GtkWidget *hbox;
  GtkWidget *button;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "GNOME-stroke test program");
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
		      (GtkSignalFunc) delete_event,
		      NULL);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  da = gtk_drawing_area_new ();
  gtk_widget_set_events (GTK_WIDGET (da), GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK);
  gtk_drawing_area_size (GTK_DRAWING_AREA (da), 600, 600);


  /* The sequence below shows how to use libgstroke */
  gstroke_enable (da);
  gstroke_signal_connect (da, "654", (GtkSignalFunc)gstroke_exit, NULL);
  gstroke_signal_connect (da, "456", (GtkSignalFunc)gstroke_clear, NULL);
  gstroke_signal_connect (da, "4123658", (GtkSignalFunc)gstroke_help, NULL);

  gtk_box_pack_start (GTK_BOX (vbox), da, TRUE, TRUE, 0);
  gtk_widget_show (da);

  hbox = gtk_hbox_new (TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  button = gtk_button_new_with_label ("Clear stroke test area");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      (GtkSignalFunc) clear_clicked,
		      da);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_with_label ("Exit");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      (GtkSignalFunc) exit_clicked,
		      window);
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  gtk_widget_show (window);

  /*hack: call the help function to display the help message*/

  gstroke_help (window);

  gtk_main ();
  return 0;
}
