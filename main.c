//compile like this
//gcc `pkg-config gtk+-3.0 gtksourceview-3.0 --cflags` main.c -o lemon.exe `pkg-config gtk+-3.0 gtksourceview-3.0 --libs`

 #include <gtk/gtk.h>
 #include <gtksourceview/gtksourceview.h>
 #include <gtksourceview/gtksourcebuffer.h>
 #include <gtksourceview/gtksourcelanguage.h>
 #include <gtksourceview/gtksourcelanguagemanager.h>
 #include <gtksourceview/gtksourcelanguage.h>


 int main( int argc, char *argv[] )
 {
   static GtkWidget *window, *scrollwin, *textview1;
   GtkWidget *vbox;

   GtkWidget *menubar;
   GtkWidget *fileMenu;
   GtkWidget *fileMi, *quitMi, *openMi, *saveMi;
   
   GtkSourceLanguageManager *lm;
   GtkSourceBuffer *sBuf;
   GtkSourceLanguage *language = NULL;

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
    language = gtk_source_language_manager_get_language (lm,"c");

    if (language == NULL)
    {
		g_print ("No language found for mime type `%s'\n", "c");
		g_object_set (G_OBJECT (sBuf), "highlight", FALSE, NULL);
    }
    else
    {
		 gtk_source_buffer_set_language (sBuf, language);
		 g_object_set (G_OBJECT (sBuf), "highlight", TRUE, NULL);
    }

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	menubar = gtk_menu_bar_new();
    fileMenu = gtk_menu_new();

	fileMi = gtk_menu_item_new_with_label("파일");
	openMi = gtk_menu_item_new_with_label("열기");
	saveMi = gtk_menu_item_new_with_label("저장");
	quitMi = gtk_menu_item_new_with_label("닫기");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
	  
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), saveMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), scrollwin, TRUE, TRUE, 0);
	
	g_signal_connect(G_OBJECT(window), "destroy",
    G_CALLBACK(gtk_main_quit), NULL);


    g_signal_connect(G_OBJECT(quitMi), "activate",
    G_CALLBACK(gtk_main_quit), NULL);


    gtk_widget_show_all (window);
    gtk_main ();
	
    return 0;
 }