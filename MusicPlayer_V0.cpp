//============================================================================
// Name        : MusicPlayer_V0.cpp
// Author      :  DjSteker
// Version     : 1.0
// Copyright   :
// Description : Reproductor GTK4 + ALSA + TinyXML2 (Single File)
//
//$(shell pkg-config --cflags gtk4 alsa tinyxml2)
//$(shell pkg-config --libs gtk4 alsa tinyxml2)
//============================================================================

#include <gtk/gtk.h>
#include "MainWindow.hpp"
#include <string>
#include <iostream>

static void activate(GtkApplication *app, gpointer user_data) {
	GtkWidget *win = create_app_window(app);
	gtk_window_present(GTK_WINDOW(win));
}

int main(int argc, char **argv) {
	std::cerr << "\n iniciando. \n";
	int status = 1;
	try {
		GtkApplication *app = gtk_application_new("org.djsteker.musicplayer", G_APPLICATION_DEFAULT_FLAGS);
		g_signal_connect(app, "activate", G_CALLBACK(activate), nullptr);

		status = g_application_run(G_APPLICATION(app), argc, argv);
		g_object_unref(app);
	} catch (const std::exception &e) {
		std::cerr << "Excepción capturada: " << e.what() << "\n";
	}
	std::cerr << "\nterminado. Result: " << status << "\n";
	return status;
}
