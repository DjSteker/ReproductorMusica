/*
 * MainWindow.hpp
 *
 *  Created on: 24 jul 2026
 *      Author: usuario001
 */

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <gtk/gtk.h>
#include "AudioEngine.hpp"
#include "DatabaseManager.hpp"
#include "PlaylistManager.hpp"
#include "Localization.hpp"
//#include "LocalizationManager.hpp"
#include <memory>
#include <vector>

typedef struct _AppWindow AppWindow;

#define APP_TYPE_WINDOW (app_window_get_type())
G_DECLARE_FINAL_TYPE(AppWindow, app_window, APP, WINDOW, GtkApplicationWindow)

struct _AppWindow {
	GtkApplicationWindow parent_instance;

	// Widgets
	GtkWidget *list_playlists;
	GtkWidget *list_songs;
	GtkWidget *lbl_now_playing;
	GtkWidget *spin_rating;
	GtkWidget *lbl_device;
	GtkWidget *device_selector;
	GtkWidget *btn_play;
	GtkWidget *btn_pause;
	GtkWidget *btn_stop;
	GtkWidget *btn_add_playlist;
	GtkWidget *btn_remove_playlist;
	GtkWidget *btn_load_file;
	GtkWidget *btn_load_folder;
	GtkWidget *btn_remove_song;
	GtkWidget *btn_shuffle;
	GtkWidget *btn_repeat;
	GtkWidget *progress_bar;
	GtkWidget *volume_scale;

	// Objetos
	std::unique_ptr<AudioEngine> audio_engine;
	std::unique_ptr<DatabaseManager> db;
	std::unique_ptr<PlaylistManager> playlist_manager;
	std::unique_ptr<Localization> localization;
	//std::unique_ptr<LocalizationManager> localization;
	std::vector<std::string> direct_files;
	bool shuffle_mode;
	bool repeat_mode;
};

GtkWidget* create_app_window(GtkApplication *app);
GtkWidget* create_device_selector(AppWindow *win);

// Callbacks
static void on_playlist_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data);
static void on_song_activated(GtkListBox *box, GtkListBoxRow *row, gpointer user_data);
static void on_play_clicked(GtkButton *button, gpointer user_data);
static void on_pause_clicked(GtkButton *button, gpointer user_data);
static void on_stop_clicked(GtkButton *button, gpointer user_data);
static void on_add_playlist_clicked(GtkButton *button, gpointer user_data);
static void on_remove_playlist_clicked(GtkButton *button, gpointer user_data);
static void on_remove_song_clicked(GtkButton *button, gpointer user_data);
static void on_rating_changed(GtkSpinButton *spin, gpointer user_data);
static void on_file_selected(GObject *source, GAsyncResult *result, gpointer user_data);
static void on_load_file_clicked(GtkButton *button, gpointer user_data);
static void on_load_folder_clicked(GtkButton *button, gpointer user_data);
static void on_shuffle_toggled(GtkToggleButton *button, gpointer user_data);
static void on_repeat_toggled(GtkToggleButton *button, gpointer user_data);
static void on_volume_changed(GtkRange *range, gpointer user_data);
static void on_progress_callback(double progress, gpointer user_data);

#endif /* MAINWINDOW_HPP_ */
