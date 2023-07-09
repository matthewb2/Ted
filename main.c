//compile like this
//gcc `pkg-config gtk+-3.0 gtksourceview-3.0 --cflags` main.c -o lemon.exe `pkg-config gtk+-3.0 gtksourceview-3.0 --libs`

 #include <gtk/gtk.h>
 #include <gtksourceview/gtksource.h>
 #include "main.h"
 
  
 static gboolean open_file (const gchar *filename)
{
  GtkSourceLanguageManager *lm;
  GtkSourceLanguage *language = NULL;
  GError *err = NULL;
  gboolean reading;
  GtkTextIter iter;
  GIOChannel *io;
  gchar *buffer;

  g_return_val_if_fail (sBuf != NULL, FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);
  g_return_val_if_fail (GTK_SOURCE_BUFFER (sBuf), FALSE);

  /* get the Language for C source mimetype */
  lm = g_object_get_data (G_OBJECT (sBuf), "languages-manager");

  language = gtk_source_language_manager_get_language (lm,"c");
  g_print("Language: [%s]\n", gtk_source_language_get_name(language));

  if (language != NULL)
  {
   gtk_source_buffer_set_language (sBuf, language);
   
  }

  /* Now load the file from Disk */
  io = g_io_channel_new_file (filename, "r", &err);
  if (!io)
  {
    g_print("error: %s %s\n", (err)->message, filename);
    return FALSE;
  }

  if (g_io_channel_set_encoding (io, "utf-8", &err) != G_IO_STATUS_NORMAL)
  {
   g_print("err: Failed to set encoding:\n%s\n%s", filename, (err)->message);
   return FALSE;
  }

  gtk_source_buffer_begin_not_undoable_action (sBuf);

  //clear buffer
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sBuf), "", 0);
  
  buffer = g_malloc (4096);
  reading = TRUE;
  while (reading)
  {
    gsize bytes_read;
    GIOStatus status;

    status = g_io_channel_read_chars (io, buffer, 4096, &bytes_read, &err);
    switch (status)
    {
      case G_IO_STATUS_EOF: reading = FALSE;

      case G_IO_STATUS_NORMAL:
        if (bytes_read == 0) continue;		
        gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (sBuf), &iter);
        gtk_text_buffer_insert (GTK_TEXT_BUFFER(sBuf),&iter,buffer,bytes_read);
        break;

      case G_IO_STATUS_AGAIN: continue;

      case G_IO_STATUS_ERROR:

      default:
        g_print("err (%s): %s", filename, (err)->message);
        /* because of error in input we clear already loaded text */
        gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sBuf), "", 0);
		reading = FALSE;
        break;
      }
  }
  g_free (buffer);
  gtk_source_buffer_end_not_undoable_action (sBuf);
  g_io_channel_unref (io);

  if (err)
  {
   g_error_free (err);
   return FALSE;
  }

  gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (sBuf), FALSE);

  /* move cursor to the beginning */
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (sBuf), &iter);
  gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (sBuf), &iter);

  g_object_set_data_full (G_OBJECT (sBuf),"filename", g_strdup (filename),
						(GDestroyNotify) g_free);

  return TRUE;
}

static void dialog_result(GtkWidget *dialog, gint resp, gpointer data)
{
    if (resp == GTK_RESPONSE_OK) {
        // do nothing
    } else {
        gtk_widget_destroy(dialog);
    }
}

static void open_dialog(GtkMenuItem *menuitem, gpointer *window)
{
    GtkWidget *dialog;
    GtkWidget *textview1;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

    GError *error;
    gboolean read_file_status;
    gchar *contents;
	

    dialog = gtk_file_chooser_dialog_new("Choose a file:", GTK_WINDOW(window), 
                        GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN,
                         GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
                         GTK_RESPONSE_CANCEL, NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_OK){
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
				read_file_status = g_file_get_contents(filename,&contents,NULL, &error);
				if (read_file_status == FALSE) {
					g_error("error opening file: %s\n",error && error->message ? error->message : "No Detail");
					return;
				}
		open_file(filename);
		g_free (filename);
    }
	gtk_widget_destroy(dialog);
}
	
 int main( int argc, char *argv[] )
 {
   
   GtkWidget *vbox;

   GtkWidget *menubar;
   GtkWidget *fileMenu, *helpMenu;
   GtkWidget *fileMi, *quitMi, *openMi, *saveMi;
   GtkWidget *helpMi, *aboutMi;
   
   GtkSourceLanguageManager *lm;
   
   GtkSourceLanguage *lang = NULL;

   /* Create window */
   gtk_init (&argc, &argv);
   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_window_set_default_size (GTK_WINDOW(window), 640, 480);
   gtk_window_set_title (GTK_WINDOW(window),"Lemon Editor");
   g_signal_connect (G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);

   /* Add GTKSourceView widget */
   scrollwin = gtk_scrolled_window_new (NULL, NULL);
   gtk_widget_show (scrollwin);

   lm = gtk_source_language_manager_new();
   sBuf = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
   g_object_ref (lm);
   g_object_set_data_full ( G_OBJECT (sBuf), "languages-manager",
                            lm, (GDestroyNotify) g_object_unref);
							
    textview1 = gtk_source_view_new_with_buffer (sBuf);

    gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW(textview1), TRUE);

    /* Add widgets to window */
    gtk_widget_show (textview1);
    gtk_container_add (GTK_CONTAINER (scrollwin), textview1);

    lm = g_object_get_data (G_OBJECT (sBuf), "languages-manager");

    /* Open C language, and check if its available */
    lang = gtk_source_language_manager_get_language (lm,"c");

    if (lang != NULL)
    {
		 gtk_source_buffer_set_language (sBuf, lang);
		 gtk_source_buffer_set_highlight_syntax(G_OBJECT (sBuf), TRUE);
    }
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	menubar = gtk_menu_bar_new();
    fileMenu = gtk_menu_new();
	helpMenu = gtk_menu_new();

	fileMi = gtk_menu_item_new_with_label("파일");
	//
	openMi = gtk_menu_item_new_with_label("열기");
	saveMi = gtk_menu_item_new_with_label("저장");
	quitMi = gtk_menu_item_new_with_label("닫기");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
	  
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), saveMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi);
	
	
	helpMi = gtk_menu_item_new_with_label("도움말");
	aboutMi = gtk_menu_item_new_with_label("정보");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMi), helpMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), aboutMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), helpMi);
	
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), scrollwin, TRUE, TRUE, 0);
	
	g_signal_connect(G_OBJECT(window), "destroy",
    G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(G_OBJECT(quitMi), "activate",
    G_CALLBACK(gtk_main_quit), NULL);
	
	g_signal_connect(G_OBJECT(openMi), "activate", G_CALLBACK(open_dialog), window);
	g_signal_connect(G_OBJECT(aboutMi), "activate", G_CALLBACK(about_dialog), window);
	
	//about_dialog(window);
	
    gtk_widget_show_all (window);
    gtk_main ();
	
	
	
    return 0;
 }
 
 void about_dialog(gpointer* window)
 {
	GdkPixbuf *example_logo = gdk_pixbuf_new_from_file ("./lemon-png-38649.png", NULL);
		
		gtk_show_about_dialog (NULL,
		"program-name", "레몬텍스트에디터",
		"logo", example_logo,
		"title", "정보",
		"version", "0.1",
		"license-type", GTK_LICENSE_LGPL_2_1,
		 "copyright", "엠케이솔루션 제공",
		NULL);
		
 }