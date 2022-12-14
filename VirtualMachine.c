#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

GtkWidget *win;

GtkWidget *scr2;
GtkWidget *scr3;

GtkEntryBuffer* eb;
GtkWidget* entry;

GtkTextBuffer *tb;
GtkTextBuffer *tb2;
GtkTextBuffer *tb3;
GtkTextBuffer *tb4;
GtkTextBuffer *tb5;
GtkTextBuffer *tb6;
GtkTextBuffer *tb7;


GtkWidget *btno;
GtkWidget *btnr;
GtkWidget *btnrs;

bool typing = false;
bool started = false;
bool step = false;

GFile *file;
char filename[100];
GtkWidget *isnew;

int Memoria[50]; //memoria
int l = 0; //linha
int p; //flag
FILE *arquivo;
char f[200][26] = {0};

static void buttonState();
static void on_entry_activate(GtkEntry *entry,gpointer output);
static void click_run(GtkButton *btn, gpointer output);
static void click_run_step(GtkButton *btn, gpointer output);
static void on_response (GtkNativeDialog *native, int response);
static void click_open (GtkButton *btn, gpointer used_data);
static void app_activate (GApplication *app, gpointer user_data);
static void on_entry_activate(GtkEntry *entry,gpointer output);
void VM(bool step, gpointer output);
int openFILE();
int search(int label);
int getCommand(char *line, gpointer output);

static void on_entry_activate(GtkEntry *entry,gpointer output);

static void buttonState() {
    if(started){
        gtk_widget_set_sensitive(GTK_WIDGET(btno),FALSE);
        if(typing){
            gtk_widget_set_sensitive(GTK_WIDGET(btnr),FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(btnrs),FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(entry),TRUE);
        }
        else {
            gtk_widget_set_sensitive(GTK_WIDGET(btnr),TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(btnrs),TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(entry),FALSE);
        }
    }
    else {
        gtk_widget_set_sensitive(GTK_WIDGET(btno),TRUE);
    }
}

static void click_run(GtkButton *btn, gpointer output) {
    if(!started){
    	gtk_text_buffer_set_text (tb3, "\n", -1);
    	gtk_text_buffer_set_text (tb2, "\n", -1);
        gtk_widget_queue_draw(scr2);
        gtk_widget_queue_draw(scr3);
        while (g_main_context_pending(NULL)) {
    		g_main_context_iteration(NULL,FALSE);
	    }
	    
        started = true;
    }
    step = false;
    VM(step, output);
}

static void click_run_step(GtkButton *btn, gpointer output) {
    if(!started) {
    	gtk_text_buffer_set_text (tb3, "\n", -1);
    	gtk_text_buffer_set_text (tb2, "\n", -1);
        gtk_widget_queue_draw(scr2);
        gtk_widget_queue_draw(scr3);
        while (g_main_context_pending(NULL)) {
    		g_main_context_iteration(NULL,FALSE);
	    }
	    
        started = true;
    }
    step = true;
    VM(step, output);
}

static void on_response(GtkNativeDialog *native, int response) {
    char *contents;
    gsize length;
    if (response == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (native);
        file= gtk_file_chooser_get_file (chooser);
        if(g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)) {
            gtk_text_buffer_set_text (tb, contents, length);
            g_free (contents);
            strcpy(filename, g_file_get_path (file));
        }
    }
    g_object_unref (native);
}

static void click_open (GtkButton *btn, gpointer used_data) {
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    native = gtk_file_chooser_native_new ("Open File",win,action,"_Open","_Cancel");

    g_signal_connect (native, "response", G_CALLBACK (on_response), NULL);
    gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
}

static void app_activate (GApplication *app, gpointer user_data) {
    GtkWidget *scr;
    GtkWidget *scr4;

    GtkWidget *tv;
    GtkWidget *tv2;
    GtkWidget *tv3;
    GtkWidget *tv4;
    GtkWidget *tv5;
    GtkWidget *tv6;
    GtkWidget *tv7;

    GtkWidget *boxv;
    GtkWidget *boxv2;
    GtkWidget *boxv3;
    GtkWidget *boxv4;
    GtkWidget *boxv5;

    GtkWidget *boxh;
    GtkWidget *boxh2;
    GtkWidget *boxh3;

    GtkWidget *dmy10;
    GtkWidget *dmy15;
    GtkWidget *dmy20;
    GtkWidget *dmy30;
    GtkWidget *dmy35;

    gchar *text;
    gchar *text2;
    gchar *text3;
    gchar *text4;

    text = "\n";
    text2= "\n\t\tCodigo de Maquina\n";
    text3= "\n\tMemoria (Stack)\n";
    text4= "\n\t\tSaida de Dados\n";

    win = gtk_application_window_new (GTK_APPLICATION (app));
    gtk_window_set_title (GTK_WINDOW (win), "Maquina Virtual");
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
	gtk_widget_set_vexpand (dmy20, TRUE);
	dmy30 = gtk_label_new(NULL);
	gtk_label_set_width_chars (GTK_LABEL (dmy30), 2);
	dmy35 = gtk_label_new(NULL);
	gtk_label_set_width_chars (GTK_LABEL (dmy35), 5);

    btno = gtk_button_new_with_label ("Open");
    btnr = gtk_button_new_with_label ("Run");
    btnrs = gtk_button_new_with_label ("Step by Step");

	gtk_box_append (GTK_BOX (boxh), dmy15);
	gtk_box_append (GTK_BOX (boxh), btno);

	gtk_box_append (GTK_BOX (boxh), dmy35);

    boxh2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append (GTK_BOX (boxv), boxh2);

    boxv2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append (GTK_BOX (boxh2), boxv2);

    tv4 = gtk_text_view_new ();
    tb4 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv4));
    gtk_text_buffer_set_text (tb4, text2, -1);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv4), GTK_WRAP_WORD_CHAR);
    gtk_box_append(GTK_BOX (boxv2),tv4);
    gtk_text_view_set_editable(tv4, FALSE);
    gtk_widget_set_hexpand(tv4, TRUE);
    gtk_widget_set_vexpand(tv4, FALSE);

    scr = gtk_scrolled_window_new ();
    tv = gtk_text_view_new ();
    tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
    gtk_text_buffer_set_text (tb, text, -1);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr), tv);
    gtk_box_append(GTK_BOX (boxv2),scr);
    gtk_scrolled_window_set_has_frame(scr, TRUE);
    gtk_widget_set_size_request (scr, 720, 350);


    boxv3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append (GTK_BOX (boxh2), boxv3);

    tv5 = gtk_text_view_new ();
    tb5 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv5));
    gtk_text_buffer_set_text (tb5, text3, -1);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv5), GTK_WRAP_WORD_CHAR);
    gtk_box_append(GTK_BOX (boxv3),tv5);
    gtk_text_view_set_editable(tv5, FALSE);
    gtk_widget_set_hexpand(tv5, TRUE);
    gtk_widget_set_vexpand(tv5, FALSE);

    scr2 = gtk_scrolled_window_new ();
    tv2 = gtk_text_view_new ();
    tb2 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv2));
    gtk_text_buffer_set_text (tb2, text, -1);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv2), GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr2), tv2);
    gtk_box_append(GTK_BOX (boxv3),scr2);
    gtk_text_view_set_editable(tv2, FALSE);
    gtk_scrolled_window_set_has_frame(scr2, TRUE);
    gtk_scrolled_window_get_max_content_height(scr2);
    gtk_widget_set_hexpand(scr2, TRUE);
    gtk_widget_set_size_request (scr2, 0,350);


    boxh3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append (GTK_BOX (boxv), boxh3);

    boxv4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append (GTK_BOX (boxh3), boxv4);

    tv6 = gtk_text_view_new ();
    tb6 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv6));
    gtk_text_buffer_set_text (tb6, text4, -1);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv6), GTK_WRAP_WORD_CHAR);
    gtk_box_append(GTK_BOX (boxv4),tv6);
    gtk_text_view_set_editable(tv6, FALSE);
    gtk_widget_set_size_request (tv6, 720,0);

    scr3 = gtk_scrolled_window_new ();
    tv3 = gtk_text_view_new ();
    tb3 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv3));
    gtk_text_buffer_set_text (tb3, text, -1);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv3), GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scr3), tv3);
    gtk_box_append(GTK_BOX (boxv4),scr3);
    gtk_text_view_set_editable(tv3, FALSE);
    gtk_scrolled_window_set_has_frame(scr3, TRUE);
    gtk_scrolled_window_get_min_content_height(scr3);
    gtk_widget_set_vexpand(scr3, TRUE);
    gtk_widget_set_hexpand(scr3, TRUE);
    //gtk_widget_set_size_request (scr3, 720,0);


    entry= gtk_entry_new();
    eb = gtk_entry_get_buffer(entry);
    gtk_box_append(GTK_BOX (boxv4),entry);
    gtk_entry_set_has_frame(entry,TRUE);
    gtk_widget_set_vexpand(entry, FALSE);
    gtk_widget_set_size_request (entry, 720,0);



    boxv5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append (GTK_BOX (boxh3), boxv5);

    gtk_box_append (GTK_BOX (boxv5), dmy20);
    gtk_box_append (GTK_BOX (boxv5), btnr);
    gtk_box_append (GTK_BOX (boxh), dmy30);
    gtk_widget_set_size_request(btnr,10,5);
    gtk_box_append (GTK_BOX (boxv5), btnrs);
    gtk_widget_set_size_request(btnrs,10,5);

    g_signal_connect(btno, "clicked", G_CALLBACK(click_open),NULL);
    g_signal_connect(btnr, "clicked", G_CALLBACK(click_run),tb3);
    g_signal_connect(btnrs, "clicked", G_CALLBACK(click_run_step),tb3);
    g_signal_connect(G_OBJECT(entry),"activate", G_CALLBACK(on_entry_activate),tb3);


    gtk_widget_show (win);
}
 
static void on_entry_activate(GtkEntry *entry,gpointer output) {
    Memoria[p] =atoi(gtk_entry_buffer_get_text(eb));
    typing = false;
    buttonState();
    gtk_text_buffer_set_text (eb, "", -1);
    gtk_widget_queue_draw(entry);
    while (g_main_context_pending(NULL)) {
    	g_main_context_iteration(NULL,FALSE);
    }
    VM(step, output);
}

int main (int argc, char **argv) {
    GtkApplication *app;
    int stat;

    app = gtk_application_new ("c.c", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    stat =g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return stat;
}


void VM(bool step, gpointer output) {
    int I =0;
    int s = -1;

    openFILE();
    while (I != -1) {
    	char str[42],str2[200] = {0};
        I = getCommand(f[l], output);
        s = p;
        while (s >= 0) {
            sprintf (str, " %d:\t\t%d\n", s, Memoria[s]);
            strcat(str2, str);
            s--;
        } 
        gtk_text_buffer_set_text (tb2, str2, -1);
        gtk_widget_queue_draw(scr2);
        gtk_widget_queue_draw(scr3);
        
        while (g_main_context_pending(NULL)) {
    		g_main_context_iteration(NULL,FALSE);
	    }
        
        if(step) {
            return;
        }
    }
}


int openFILE() {
    arquivo=fopen(filename,"r");
    if(arquivo == NULL) {
        printf("ERRO");
        exit(1);
    }

    char charA;
	charA = fgetc(arquivo);
	int r = 0;
	int c = 0;
	
    while(charA != EOF) {
		if(charA == '\n') {
			f[r][c] = '\0';
			r++;
			c=0;
		} else {
			f[r][c] = charA;
			c++;
		}
		charA = fgetc(arquivo);
	}
	return r;
}

int search(int label) {
	char rotulo [5] = {0};
	char comando [9] = {0};
	int j,k = 0, aux;
	int flag = 0;

	while(strcmp(comando,"HLT") && atoi(rotulo) != label) {
		k++;
		j = 0;
		aux = 0;

        while (f[k][j] != 9) {
            rotulo[j] = f[k][j];
            j++;
        }

        aux = j+1;
        rotulo[aux] = 0;
        j = 0;

        while (f[k][aux+j] != 9) {
            comando[j] = f[k][aux+j];
            j++;
        }

        comando[aux+j+1] = 0;

		if(!strcmp(comando,"HLT")) {
			flag = 1;
		}

	}

	if(flag) {
	    return -1;
	} else {
        printf ( " jump to: %d \n", k+1);
		return k;
	}
}

int getCommand(char *line, gpointer output) {

    buttonState();

	char rotulo [10] = {0};
	char comando [10] = {0};
	char param1 [10] = {0};
	char param2 [10] = {0};

	int I = 0, aux = 0;
	while (line[I] != 9) {
        rotulo[I] = line[I];
        I++;
    }

    aux = I + 1;
    I=0;

    while (line[aux+I] != 9) {
        comando[I] = line[aux+I];
        I++;
    }

    aux = aux+ I + 1;
    I = 0;

    while(line[aux+I] != 9) {
        param1[I] = line[aux+I];
        I++;
    }

    aux = aux+ I + 1;
    I = 0;

    while(line[aux+I] != 9) {
        param2[I] = line[aux+I];
        I++;
    }
    
    if(!strcmp(comando,"LDC")) {
        int k;
        k=atoi(param1);
        printf("constante %d\n",k);
        p++;
        Memoria[p]=k;
        l++;
    } else if(!strcmp(comando,"LDV")) {
		int n = atoi(param1);
		p++;
		Memoria[p] = Memoria[n];
		printf("variavel %d\n",Memoria[p]);
		l++;
	} else if (!strcmp(comando,"ADD")) {
		Memoria[p-1]= Memoria[p-1]+Memoria[p];
		p--;
		l++;
	} else if (!strcmp(comando,"SUB")) {
		Memoria[p-1]= Memoria[p-1]-Memoria[p];
		p--;
		l++;
	} else if (!strcmp(comando,"MULT")) {
		Memoria[p-1]= Memoria[p-1]*Memoria[p];
		p--;
		l++;
	} else if (!strcmp(comando,"DIV")) {
		Memoria[p-1]= Memoria[p-1]/Memoria[p];
		p--;
		l++;
	} else if (!strcmp(comando,"INV")) {
		Memoria[p]= -(Memoria[p]);
		l++;
	} else if(!strcmp(comando,"AND")) {
        if(Memoria[p-1] == 1 && Memoria[p] == 1) {
			Memoria[p-1] = 1;
        } else {
			Memoria[p-1] = 0;
        }
        p--;
        l++;
	} else if(!strcmp(comando,"OR")) {
		if(Memoria[p-1] == 1 || Memoria[p] == 1) {
			Memoria[p-1] = 1;
		} else {
		    Memoria[p-1] = 0;
		}
		p--;
		l++;
	} else if(!strcmp(comando,"NEG")) {
		Memoria[p] = 1-Memoria[p];
		l++;
	} else if(!strcmp(comando,"CME")) {
		if(Memoria[p-1] < Memoria[p]) {
			Memoria[p-1] = 1;
		} else {
		    Memoria[p-1] = 0;
		}
		p--;
		l++;
	} else if(!strcmp(comando,"CMEQ")){
	    if(Memoria[p-1] <= Memoria[p]){
			Memoria[p-1] = 1;
		} else {
			Memoria[p-1] = 0;
		}
		p--;
		l++;
	} else if(!strcmp(comando,"CMA")){
		if(Memoria[p-1] > Memoria[p]){
			Memoria[p-1] = 1;
		} else{
			Memoria[p-1] = 0;
		}
		p--;
		l++;
	} else if(!strcmp(comando,"CMAQ")){
		if(Memoria[p-1] >= Memoria[p]){
			Memoria[p-1] = 1;
		} else {
			Memoria[p-1] = 0;
		}
		p--;
		l++;
	} else if(!strcmp(comando,"CEQ")){
		if(Memoria[p-1] == Memoria[p]){
			Memoria[p-1] = 1;
		} else {
			Memoria[p-1] = 0;
		}
		p--;
		l++;
	} else if(!strcmp(comando,"CDIF")){
		if(Memoria[p-1] != Memoria[p]){
			Memoria[p-1] = 1;
		} else {
			Memoria[p-1] = 0;
		}
		p--;
		l++;
	} else if(!strcmp(comando,"STR")){
		int n;
		n = atoi(param1);
		Memoria[n] = Memoria[p];
		p--;
		l++;
	} else if(!strcmp(comando,"JMP")){
		int u;
		u = atoi(param1);
		l = search(u);
	} else if(!strcmp(comando,"JMPF")){
		if(Memoria[p] == 0) {
			int u = atoi(param1);
			l = search(u);
		} else {
			l++;
		}
		p--;
	} else if(!strcmp(comando,"RD")){
        printf("READ\n");
        typing = true;
        buttonState();
        l++;
		p++;
		return -1;

    } else if(!strcmp(comando,"PRN")){
        printf("PRINT\n%d\n",Memoria[p]);
        
        char s[42];
        
        sprintf(s, "%d\n", Memoria[p]);
        
        GtkTextIter end;
	    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER (output),&end);
	    gtk_text_buffer_insert(GTK_TEXT_BUFFER (output),&end, s,-1);
        
        p--;
        l++;
    } else if(!strcmp(comando,"START")) {
		printf("start\n");//start
		p=-1;
		l++;
	} else if(!strcmp(comando,"ALLOC")){
		int m,n,k;
		m=atoi(param1);
		n=atoi(param2);
		printf("alloc %d %d\n",m,n);
		for(k=0;k<n;k++) {
            p++;
			Memoria[p]=Memoria[m+k];
        }
		l++;
    } else if(!strcmp(comando,"DALLOC")){
		int m,n,k;
		m=atoi(param1);
		n=atoi(param2);
		printf("dalloc %d %d\n",m,n);
		for(k=n-1;k>=0;k--) {
			Memoria[m+k]= Memoria[p];
			p--;
		}
		l++;
	} else if(!strcmp(comando,"HLT")){
        printf("HALT");//halt
        l = 0;
        started = false;
        return -1;
    } else if(!strcmp(comando,"CALL")){
        int r;
        r=atoi(param1);
        p++;
        Memoria[p]=l+1;
        l = search(r);
        printf("call %d\n",Memoria[p]);
    } else if(!strcmp(comando,"RETURN")){
        printf("RETURN %d",Memoria[p]);
        l=Memoria[p];
        p--;
    } else if(strcmp(comando,"NULL")){
			return 1;
	} else {
		l++;
	}
}
