#ifndef MAIN_H
#define MAIN_H

static GtkWidget *window, *scrollwin, *textview1;

static GtkSourceBuffer *sBuf;
 
static gboolean open_file (const gchar*);
 
static void dialog_result(GtkWidget*, gint , gpointer);
static void open_dialog(GtkMenuItem*, gpointer*);
void about_dialog(gpointer*);


#endif
