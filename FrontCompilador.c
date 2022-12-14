#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


GtkWidget *win;
GtkTextBuffer *tb;
GFile *file;
char filename[100];
GtkWidget *isnew;

static void on_response (GtkNativeDialog *native, int response);
static void click_open (GtkButton *btn, gpointer used_data);
static void click_run (GtkButton *btn, gpointer used_data);
static void app_activate (GApplication *app, gpointer user_data);

static void on_response (GtkNativeDialog *native, int response){
	char *contents;
	gsize length;
	if (response == GTK_RESPONSE_ACCEPT){
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
		file= gtk_file_chooser_get_file (chooser);
		if (g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)){
			gtk_text_buffer_set_text (tb, contents, length);
			g_free (contents);
			
			strcpy(filename, g_file_get_path(file));
		}
	}

	g_object_unref (native);
}

static void click_open (GtkButton *btn, gpointer used_data){
	GtkFileChooserNative *native;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

	native = gtk_file_chooser_native_new ("Open File", win, action, "_Open", "_Cancel");

	g_signal_connect (native, "response", G_CALLBACK (on_response), NULL);
	gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
}

static void click_run (GtkButton *btn, gpointer used_data){

	if(isnew){
        gtk_text_buffer_set_text (used_data, "Salve antes de compilar", -1);
    } else {
	
		pid_t cpid;
		int stat;
		
		if(fork() == 0) {
			char* args[] = {"./Compilador", filename, NULL};
			g_print("%s", filename);
			execv("./Compilador", args);
		} else {
            cpid = wait(&stat);
        }
			
		if(WIFEXITED(stat)) {
			FILE* compLog;
			if((compLog = fopen("errorlog.txt", "r")) == NULL){
				printf("ERRO");
				exit(1);
			}

			char s[100];
			while(fgets(s, 100, compLog)) {
				GtkTextIter end;
				gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER (used_data),&end);
				gtk_text_buffer_insert(GTK_TEXT_BUFFER (used_data),&end,s,-1);
			}
			
		} else if(WIFSIGNALED(stat)){
			psignal(WTERMSIG(stat), "Exit signal");
        }
	
	}
    
}

static void app_activate (GApplication *app, gpointer user_data) {
	GtkWidget *scr;
	GtkWidget *scr2;
	gchar *text;
	GtkWidget *tv;
	GtkWidget *tv2;
	GtkWidget *dmy10;
	GtkWidget *dmy15;
	GtkTextBuffer *tb2;

	GtkWidget *dmy20;
	GtkWidget *dmy30;
	GtkWidget *dmy35;
	GtkWidget *boxv;
	GtkWidget *boxh;
	GtkWidget *btno;
	GtkWidget *btnc;

	text = "";

	win = gtk_application_window_new (GTK_APPLICATION (app));
	gtk_window_set_title (GTK_WINDOW (win), "Compilador");
	gtk_window_maximize(GTK_WINDOW (win));

	boxv = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_window_set_child (GTK_WINDOW (win), boxv);

	boxh = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_append (GTK_BOX (boxv), boxh);
	dmy10 = gtk_label_new(NULL);
	gtk_label_set_width_chars (GTK_LABEL (dmy10), 5);
	dmy15 = gtk_label_new(NULL);
	gtk_label_set_width_chars (GTK_LABEL (dmy15), 2);
	dmy20 = gtk_label_new(NULL);
	gtk_widget_set_hexpand (dmy20, TRUE);
	dmy30 = gtk_label_new(NULL);
	gtk_label_set_width_chars (GTK_LABEL (dmy30), 2);
	dmy35 = gtk_label_new(NULL);
	gtk_label_set_width_chars (GTK_LABEL (dmy35), 5);
	
	btno = gtk_button_new_with_label ("Open");
	btnc = gtk_button_new_with_label ("Run");
	
	gtk_box_append (GTK_BOX (boxh), dmy10);
	gtk_box_append (GTK_BOX (boxh), dmy15);
	gtk_box_append (GTK_BOX (boxh), btno);
	gtk_box_append (GTK_BOX (boxh), dmy20);
	gtk_box_append (GTK_BOX (boxh), dmy30);
	gtk_box_append (GTK_BOX (boxh), btnc);
	gtk_box_append (GTK_BOX (boxh), dmy35);

	
	scr = gtk_scrolled_window_new ();
	tv = gtk_text_view_new ();
	tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
	gtk_text_buffer_set_text (tb, text, -1);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
	gtk_box_append(GTK_BOX (boxv),scr);
	gtk_widget_set_hexpand(scr, TRUE);
	gtk_widget_set_vexpand(scr, TRUE);

	scr2 = gtk_scrolled_window_new ();
	tv2 = gtk_text_view_new ();
	tb2 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv2));
	gtk_text_buffer_set_text (tb2, text, -1);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv2), GTK_WRAP_WORD_CHAR);
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr2), tv2);
	gtk_box_append(GTK_BOX (boxv),scr2);
	gtk_text_view_set_editable(tv2, FALSE);
	gtk_scrolled_window_set_has_frame(scr2, TRUE);
	gtk_scrolled_window_get_min_content_height(scr2);
	gtk_widget_set_hexpand(scr2, TRUE);
	
	g_signal_connect(btno, "clicked", G_CALLBACK(click_open),NULL);
	g_signal_connect(btnc, "clicked", G_CALLBACK(click_run), tb2);

	gtk_widget_show (win);
}

int main (int argc, char **argv) {
	GtkApplication *app;
	int stat;

	app = gtk_application_new ("c.c", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
	stat =g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);
	
	return stat;
 }
