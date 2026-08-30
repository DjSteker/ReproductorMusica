/*
 * MainWindow.cpp
 *
 *  Created on: 24 jul 2026
 *      Author: DjSteker
 */

#include "MainWindow.hpp"
#include "DeviceUtils.hpp"
#include <glib/gstdio.h>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <vector>

namespace fs = std::filesystem;

G_DEFINE_TYPE(AppWindow, app_window, GTK_TYPE_APPLICATION_WINDOW)

static void play_song(AppWindow *win, int song_index) {
	if (win->shuffle_mode && !win->direct_files.empty()) {
		std::vector<int> indices;
		if (win->playlist_manager->getCurrentPlaylistIndex() >= 0) {
			// FIX: copia nombrada del vector devuelto por getPlaylists() para evitar
			// unir "songs" a un temporal que se destruye al final de la sentencia
			// (operator[] es una llamada a función: no extiende la vida del temporal).
			std::vector<Playlist> playlists = win->db->getPlaylists();
			const auto &songs = playlists[win->playlist_manager->getCurrentPlaylistIndex()].songs;
			for (size_t i = 0; i < songs.size(); ++i) {
				indices.push_back(i);
			}
		} else if (!win->direct_files.empty()) {
			for (size_t i = 0; i < win->direct_files.size(); ++i) {
				indices.push_back(i);
			}
		}
		if (!indices.empty()) {
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(indices.begin(), indices.end(), g);
			song_index = indices[0];
		}
	}

	win->playlist_manager->playSong(
			song_index,
			win->db.get(),
			win->audio_engine,
			win->playlist_manager->getCurrentPlaylistIndex(),
			win->direct_files
	);
}

static void on_playlist_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data) {
	if (!row) {
		return;
	}
	AppWindow *win = APP_WINDOW(user_data);
	int index = gtk_list_box_row_get_index(row);
	win->playlist_manager->setCurrentPlaylist(index);
}

static void on_song_activated(GtkListBox *box, GtkListBoxRow *row, gpointer user_data) {
	if (!row) {
		return;
	}
	int index = gtk_list_box_row_get_index(row);
	play_song(APP_WINDOW(user_data), index);
}

static void on_play_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	if (!win->audio_engine->isPlaying() && win->playlist_manager->getCurrentSongIndex() >= 0) {
		win->audio_engine->play();
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("paused").c_str());
	} else if (win->playlist_manager->getCurrentSongIndex() < 0 && !win->direct_files.empty()) {
		play_song(win, 0);
	} else if (win->playlist_manager->getCurrentSongIndex() < 0 && win->playlist_manager->getCurrentPlaylistIndex() >= 0) {
		// FIX: copia nombrada, misma razón que en play_song().
		std::vector<Playlist> playlists = win->db->getPlaylists();
		const auto &songs = playlists[win->playlist_manager->getCurrentPlaylistIndex()].songs;
		if (!songs.empty()) {
			play_song(win, 0);
		}
	}
}

static void on_pause_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	win->audio_engine->pause();
	if (win->audio_engine->isPaused()) {
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("paused").c_str());
	} else {
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("now_playing").c_str());
	}
}

static void on_stop_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	win->audio_engine->stop();
	gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("stopped").c_str());
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(win->progress_bar), 0.0);
}

static void on_remove_playlist_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	int current_playlist = win->playlist_manager->getCurrentPlaylistIndex();
	if (current_playlist < 0) {
		return;
	}
	win->audio_engine->stop();
	win->playlist_manager->removePlaylist(current_playlist);
}

static void on_remove_song_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	int current_playlist = win->playlist_manager->getCurrentPlaylistIndex();
	int current_song = win->playlist_manager->getCurrentSongIndex();
	if (current_song < 0) {
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("select_song_first").c_str());
		return;
	}
	if (win->audio_engine->isPlaying()) {
		win->audio_engine->stop();
	}
	win->playlist_manager->removeSong(current_playlist, current_song);
}

static void on_rating_changed(GtkSpinButton *spin, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	int current_playlist = win->playlist_manager->getCurrentPlaylistIndex();
	if (current_playlist >= 0) {
		win->playlist_manager->setPlaylistRating(current_playlist, gtk_spin_button_get_value_as_int(spin));
	}
}

static void on_file_selected(GObject *source, GAsyncResult *result, gpointer user_data) {
	GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
	AppWindow *win = APP_WINDOW(user_data);
	GError *error = nullptr;
	GListModel *list = gtk_file_dialog_open_multiple_finish(dialog, result, &error);
	if (error) {
		g_error_free(error);
		return;
	}
	if (!list) {
		return;
	}

	std::vector<std::string> wav_files;
	guint count = g_list_model_get_n_items(list);
	for (guint i = 0; i < count; i++) {
		GFile *file = G_FILE(g_list_model_get_item(list, i));
		char *path = g_file_get_path(file);
		if (path) {
			if (fs::path(path).extension() == ".wav") {
				wav_files.push_back(path);
			}
			g_free(path);
		}
		g_object_unref(file);
	}
	g_object_unref(list);

	if (!wav_files.empty()) {
		int current_playlist = win->playlist_manager->getCurrentPlaylistIndex();
		if (current_playlist >= 0) {
			win->db->addSongsToPlaylist(current_playlist, wav_files);
			win->playlist_manager->refreshSongs(current_playlist);
		} else {
			win->direct_files.insert(win->direct_files.end(), wav_files.begin(), wav_files.end());
			win->playlist_manager->refreshSongs(-1);
		}
		char msg[100];
		snprintf(msg, sizeof(msg), win->localization->getText("songs_added").c_str(), (int) wav_files.size());
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), msg);
	}
}

static void on_load_file_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	GtkFileDialog *dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_title(dialog, win->localization->getText("load_files_button").c_str());
	gtk_file_dialog_open_multiple(dialog, GTK_WINDOW(win), nullptr, on_file_selected, user_data);
}

static void on_folder_selected(GObject *source, GAsyncResult *result, gpointer user_data) {
	GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
	AppWindow *win = APP_WINDOW(user_data);
	GError *error = nullptr;
	GFile *file = gtk_file_dialog_select_folder_finish(dialog, result, &error);
	if (error) {
		g_error_free(error);
		return;
	}
	if (!file) {
		return;
	}
	char *path = g_file_get_path(file);
	g_object_unref(file);
	if (path) {
		std::vector<std::string> wav_files;
		try {
			for (const auto &entry : fs::recursive_directory_iterator(path)) {
				if (entry.is_regular_file() && entry.path().extension() == ".wav") {
					wav_files.push_back(entry.path().string());
				}
			}
		} catch (const fs::filesystem_error &e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
		if (!wav_files.empty()) {
			int current_playlist = win->playlist_manager->getCurrentPlaylistIndex();
			if (current_playlist >= 0) {
				win->db->addSongsToPlaylist(current_playlist, wav_files);
				win->playlist_manager->refreshSongs(current_playlist);
			} else {
				win->direct_files.insert(win->direct_files.end(), wav_files.begin(), wav_files.end());
				win->playlist_manager->refreshSongs(-1);
			}
			char msg[100];
			snprintf(msg, sizeof(msg), win->localization->getText("songs_added").c_str(), (int) wav_files.size());
			gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), msg);
		}
		g_free(path);
	}
}

static void on_load_folder_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	GtkFileDialog *dialog = gtk_file_dialog_new();
	gtk_file_dialog_set_title(dialog, win->localization->getText("load_folder_button").c_str());
	// NOTA: se sustituye la lambda C++ por una función estática con nombre
	// (on_folder_selected) para seguir tu convención de no usar lambdas en
	// callbacks de señales/CB de GTK/GLib.
	gtk_file_dialog_select_folder(dialog, GTK_WINDOW(win), nullptr, on_folder_selected, user_data);
}

static void on_shuffle_toggled(GtkToggleButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	win->shuffle_mode = gtk_toggle_button_get_active(button);
}

static void on_repeat_toggled(GtkToggleButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	win->repeat_mode = gtk_toggle_button_get_active(button);
}

static void on_volume_changed(GtkRange *range, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	float volume = gtk_range_get_value(range);
	win->audio_engine->setVolume(volume);
}

[[maybe_unused]] static void on_progress_callback(double progress, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(win->progress_bar), progress);
}

static void on_finished_callback(gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	int current_playlist = win->playlist_manager->getCurrentPlaylistIndex();
	int current_song = win->playlist_manager->getCurrentSongIndex();

	if (win->repeat_mode) {
		play_song(win, current_song);
		return;
	}

	if (current_playlist >= 0) {
		// FIX: copia nombrada, misma razón que en play_song().
		std::vector<Playlist> playlists = win->db->getPlaylists();
		const auto &songs = playlists[current_playlist].songs;
		if (current_song + 1 < (int) songs.size()) {
			play_song(win, current_song + 1);
		} else if (!win->direct_files.empty()) {
			win->playlist_manager->setCurrentPlaylist(-1);
			play_song(win, 0);
		} else {
			gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("end_of_list").c_str());
			win->playlist_manager->setCurrentSong(-1);
		}
	} else if (!win->direct_files.empty()) {
		if (current_song + 1 < (int) win->direct_files.size()) {
			play_song(win, current_song + 1);
		} else {
			gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("end_of_list").c_str());
			win->playlist_manager->setCurrentSong(-1);
		}
	} else {
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), win->localization->getText("end_of_list").c_str());
		win->playlist_manager->setCurrentSong(-1);
	}
}

static void on_add_playlist_ok_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(g_object_get_data(G_OBJECT(button), "win_ptr"));
	GtkEntry *entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "entry_ptr"));
	GtkWindow *dialog = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "dialog_ptr"));

	const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
	if (text && text[0] != '\0') {
		win->playlist_manager->addPlaylist(text);
	}
	gtk_window_destroy(dialog);
}

static void on_add_playlist_clicked(GtkButton *button, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	GtkWidget *dialog = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(dialog), win->localization->getText("add_playlist_title").c_str());
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(win));

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_margin_start(vbox, 10);
	gtk_widget_set_margin_end(vbox, 10);
	gtk_widget_set_margin_top(vbox, 10);
	gtk_widget_set_margin_bottom(vbox, 10);

	gtk_box_append(GTK_BOX(vbox), gtk_label_new(win->localization->getText("playlist_name_label").c_str()));
	GtkWidget *entry = gtk_entry_new();
	// FIX: antes se precargaba el campo con el texto del botón "Crear" (bug de
	// copia/pega). Se usa un placeholder en vez de texto real precargado.
	gtk_entry_set_placeholder_text(GTK_ENTRY(entry), win->localization->getText("playlist_name_label").c_str());
	gtk_box_append(GTK_BOX(vbox), entry);

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_halign(hbox, GTK_ALIGN_END);
	GtkWidget *btn_cancel = gtk_button_new_with_label(win->localization->getText("cancel_button").c_str());
	GtkWidget *btn_ok = gtk_button_new_with_label(win->localization->getText("create_button").c_str());
	gtk_box_append(GTK_BOX(hbox), btn_cancel);
	gtk_box_append(GTK_BOX(hbox), btn_ok);
	gtk_box_append(GTK_BOX(vbox), hbox);

	gtk_window_set_child(GTK_WINDOW(dialog), vbox);

	g_object_set_data(G_OBJECT(btn_ok), "win_ptr", win);
	g_object_set_data(G_OBJECT(btn_ok), "entry_ptr", entry);
	g_object_set_data(G_OBJECT(btn_ok), "dialog_ptr", dialog);
	g_signal_connect(btn_cancel, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
	g_signal_connect(btn_ok, "clicked", G_CALLBACK(on_add_playlist_ok_clicked), win);

	gtk_window_present(GTK_WINDOW(dialog));
}

// --- Creación del selector de dispositivo (usa play_song) ---

static void on_device_selected(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data) {
	AppWindow *win = APP_WINDOW(user_data);
	guint index = gtk_drop_down_get_selected(dropdown);
	GPtrArray *names = g_ptr_array_ref((GPtrArray*) g_object_get_data(G_OBJECT(dropdown), "real_device_names"));
	const char *device_name = (const char*) g_ptr_array_index(names, index);
	if (!device_name) {
		g_ptr_array_unref(names);
		return;
	}
	win->audio_engine->stop();
	std::string dev_str(device_name);
	if (win->audio_engine->init(dev_str)) {
		int current_playlist = win->playlist_manager->getCurrentPlaylistIndex();
		int current_song = win->playlist_manager->getCurrentSongIndex();
		if (current_playlist >= 0 && current_song >= 0) {
			// FIX: copia nombrada, misma razón que en play_song().
			std::vector<Playlist> playlists = win->db->getPlaylists();
			const auto &songs = playlists[current_playlist].songs;
			if (current_song < (int) songs.size()) {
				play_song(win, current_song);
			}
		}
		std::string msg = "Dispositivo: " + std::string(device_name);
		if (msg.length() > 40) {
			msg = msg.substr(0, 37) + "...";
		}
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), msg.c_str());
	} else {
		gtk_label_set_text(GTK_LABEL(win->lbl_now_playing), "Error al cambiar dispositivo.");
		win->audio_engine->init("default");
	}
	g_ptr_array_unref(names);
}

GtkWidget* create_device_selector(AppWindow *win) {
	std::vector<std::string> devices = DeviceUtils::getInstance().getPlaybackDevices();
	bool has_default = false;
	for (const auto &dev : devices) {
		if (dev.find("default") != std::string::npos) {
			has_default = true;
			break;
		}
	}
	if (!has_default) {
		devices.insert(devices.begin(), "default");
	}

	GPtrArray *str_array = g_ptr_array_new();
	for (const auto &dev : devices) {
		std::string name = dev;
		if (name.length() > 30) {
			name = name.substr(0, 27) + "...";
		}
		g_ptr_array_add(str_array, g_strdup(name.c_str()));
	}
	// FIX CRÍTICO: gtk_string_list_new() exige un array const char* const*
	// terminado en NULL. g_ptr_array_new() NO añade terminador NULL de forma
	// automática (documentado en GLib: los arrays creados con los constructores
	// normales no son NULL-terminated). Sin este terminador, gtk_string_list_new
	// lee memoria fuera de los límites del array hasta encontrar por azar un
	// puntero NULL: comportamiento indefinido que casi con toda seguridad
	// provocaba el crash al arrancar, ya que create_device_selector() se llama
	// durante la inicialización de la ventana.
	g_ptr_array_add(str_array, nullptr);

	GListModel *model = G_LIST_MODEL(gtk_string_list_new((const char* const*) str_array->pdata));

	// gtk_string_list_new() copia internamente cada string, así que liberamos
	// aquí los g_strdup (incluido el terminador NULL, sobre el que g_free es
	// seguro) y el propio GPtrArray.
	for (guint i = 0; i < str_array->len; i++) {
		g_free(g_ptr_array_index(str_array, i));
	}
	g_ptr_array_free(str_array, TRUE);

	GtkWidget *dropdown = gtk_drop_down_new(nullptr, nullptr);
	gtk_drop_down_set_model(GTK_DROP_DOWN(dropdown), model);
	g_object_unref(model);

	GPtrArray *real_names = g_ptr_array_new();
	for (const auto &dev : devices) {
		g_ptr_array_add(real_names, g_strdup(dev.c_str()));
	}
	g_object_set_data_full(G_OBJECT(dropdown), "real_device_names", real_names, (GDestroyNotify) g_ptr_array_free);

	// NOTA: se sustituye la lambda C++ (+[](...)) por una función estática con
	// nombre (on_device_selected) para seguir tu convención de no usar lambdas
	// en callbacks de señales de GTK/GLib.
	g_signal_connect(dropdown, "notify::selected-item", G_CALLBACK(on_device_selected), win);
	return dropdown;
}

// --- Inicialización GObject ---

static void app_window_class_init(AppWindowClass *klass) {
}

// ---------------------------------------------------------------------
// Macro de depuración: define NDEBUG_STARTUP para desactivar todos los
// mensajes de un plumazo (p.ej. en el Makefile: -DNDEBUG_STARTUP), sin
// tener que borrar las líneas.
// ---------------------------------------------------------------------
#ifdef NDEBUG_STARTUP
	#define DBG_LOG(msg) do {} while (0)
#else
	#define DBG_LOG(msg) do { std::cerr << "[startup] " << msg << std::endl; } while (0)
#endif

static void app_window_init(AppWindow *self) {
	DBG_LOG("app_window_init: inicio");

	self->audio_engine = std::make_unique<AudioEngine>();
	DBG_LOG("app_window_init: AudioEngine creado");

	self->db = std::make_unique<DatabaseManager>();
	DBG_LOG("app_window_init: DatabaseManager creado/cargado (music_db.xml)");

	self->localization = std::make_unique<Localization>();
	DBG_LOG("app_window_init: Localization creado");

	self->shuffle_mode = false;
	self->repeat_mode = false;

	DBG_LOG("app_window_init: antes de audio_engine->init(\"default\") -- SOSPECHOSO #1 (snd_pcm_open)");
	if (!self->audio_engine->init("default")) {
		std::cerr << "Fallo ALSA" << std::endl;
	}
	DBG_LOG("app_window_init: después de audio_engine->init");

	gtk_window_set_title(GTK_WINDOW(self), self->localization->getText("app_title").c_str());
	gtk_window_set_default_size(GTK_WINDOW(self), 900, 600);
	DBG_LOG("app_window_init: título y tamaño puestos");

	self->audio_engine->on_finished_callback = [self]() {
		g_idle_add([](gpointer data) -> gboolean {
			on_finished_callback(data);
			return G_SOURCE_REMOVE;
		}, self);
	};
	DBG_LOG("app_window_init: on_finished_callback asignado");

	GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	GtkWidget *top_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

	GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_margin_start(left_box, 5);
	gtk_widget_set_margin_end(left_box, 5);
	gtk_widget_set_margin_top(left_box, 5);
	gtk_widget_set_margin_bottom(left_box, 5);

	GtkWidget *top_left_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	self->btn_add_playlist = gtk_button_new_with_label(self->localization->getText("add_playlist_title").c_str());
	self->btn_remove_playlist = gtk_button_new_with_label(self->localization->getText("remove_playlist").c_str());
	g_signal_connect(self->btn_add_playlist, "clicked", G_CALLBACK(on_add_playlist_clicked), self);
	g_signal_connect(self->btn_remove_playlist, "clicked", G_CALLBACK(on_remove_playlist_clicked), self);
	gtk_box_append(GTK_BOX(top_left_box), self->btn_add_playlist);
	gtk_box_append(GTK_BOX(top_left_box), self->btn_remove_playlist);

	GtkWidget *rating_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_append(GTK_BOX(rating_box), gtk_label_new(self->localization->getText("rating_label").c_str()));
	self->spin_rating = gtk_spin_button_new_with_range(1, 5, 1);
	g_signal_connect(self->spin_rating, "value-changed", G_CALLBACK(on_rating_changed), self);
	gtk_box_append(GTK_BOX(rating_box), self->spin_rating);

	gtk_box_append(GTK_BOX(left_box), top_left_box);
	gtk_box_append(GTK_BOX(left_box), rating_box);

	GtkWidget *scroll_pl = gtk_scrolled_window_new();
	gtk_widget_set_vexpand(scroll_pl, TRUE);
	self->list_playlists = gtk_list_box_new();
	g_signal_connect(self->list_playlists, "row-activated", G_CALLBACK(on_playlist_selected), self);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_pl), self->list_playlists);
	gtk_box_append(GTK_BOX(left_box), scroll_pl);

	GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_margin_start(right_box, 5);
	gtk_widget_set_margin_end(right_box, 5);
	gtk_widget_set_margin_top(right_box, 5);
	gtk_widget_set_margin_bottom(right_box, 5);

	GtkWidget *top_right_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	self->btn_load_file = gtk_button_new_with_label(self->localization->getText("load_files_button").c_str());
	self->btn_load_folder = gtk_button_new_with_label(self->localization->getText("load_folder_button").c_str());
	self->btn_remove_song = gtk_button_new_with_label(self->localization->getText("remove_song_button").c_str());
	g_signal_connect(self->btn_load_file, "clicked", G_CALLBACK(on_load_file_clicked), self);
	g_signal_connect(self->btn_load_folder, "clicked", G_CALLBACK(on_load_folder_clicked), self);
	g_signal_connect(self->btn_remove_song, "clicked", G_CALLBACK(on_remove_song_clicked), self);
	gtk_box_append(GTK_BOX(top_right_box), self->btn_load_file);
	gtk_box_append(GTK_BOX(top_right_box), self->btn_load_folder);
	gtk_box_append(GTK_BOX(top_right_box), self->btn_remove_song);

	gtk_box_append(GTK_BOX(right_box), top_right_box);

	GtkWidget *scroll_songs = gtk_scrolled_window_new();
	gtk_widget_set_vexpand(scroll_songs, TRUE);
	self->list_songs = gtk_list_box_new();
	g_signal_connect(self->list_songs, "row-activated", G_CALLBACK(on_song_activated), self);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_songs), self->list_songs);
	gtk_box_append(GTK_BOX(right_box), scroll_songs);

	GtkWidget *bottom_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_margin_start(bottom_box, 5);
	gtk_widget_set_margin_end(bottom_box, 5);
	gtk_widget_set_margin_top(bottom_box, 5);
	gtk_widget_set_margin_bottom(bottom_box, 5);

	GtkWidget *controls_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	self->btn_play = gtk_button_new_with_label(self->localization->getText("play_button").c_str());
	self->btn_pause = gtk_button_new_with_label(self->localization->getText("pause_button").c_str());
	self->btn_stop = gtk_button_new_with_label(self->localization->getText("stop_button").c_str());
	gtk_widget_set_hexpand(self->btn_play, TRUE);
	gtk_widget_set_hexpand(self->btn_pause, TRUE);
	gtk_widget_set_hexpand(self->btn_stop, TRUE);
	gtk_widget_set_size_request(self->btn_play, -1, 36);
	gtk_widget_set_size_request(self->btn_pause, -1, 36);
	gtk_widget_set_size_request(self->btn_stop, -1, 36);
	g_signal_connect(self->btn_play, "clicked", G_CALLBACK(on_play_clicked), self);
	g_signal_connect(self->btn_pause, "clicked", G_CALLBACK(on_pause_clicked), self);
	g_signal_connect(self->btn_stop, "clicked", G_CALLBACK(on_stop_clicked), self);
	gtk_box_append(GTK_BOX(controls_row), self->btn_play);
	gtk_box_append(GTK_BOX(controls_row), self->btn_pause);
	gtk_box_append(GTK_BOX(controls_row), self->btn_stop);

	self->btn_shuffle = gtk_toggle_button_new_with_label("🔀 Shuffle");
	self->btn_repeat = gtk_toggle_button_new_with_label("🔁 Repeat");
	g_signal_connect(self->btn_shuffle, "toggled", G_CALLBACK(on_shuffle_toggled), self);
	g_signal_connect(self->btn_repeat, "toggled", G_CALLBACK(on_repeat_toggled), self);
	gtk_box_append(GTK_BOX(controls_row), self->btn_shuffle);
	gtk_box_append(GTK_BOX(controls_row), self->btn_repeat);
	DBG_LOG("app_window_init: widgets de controles creados");

	GtkWidget *device_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	self->lbl_device = gtk_label_new(self->localization->getText("device_label").c_str());
	gtk_widget_set_halign(self->lbl_device, GTK_ALIGN_START);
	gtk_widget_set_margin_start(self->lbl_device, 5);

	DBG_LOG("app_window_init: antes de create_device_selector -- SOSPECHOSO #2 (snd_device_name_hint)");
	self->device_selector = create_device_selector(self);
	DBG_LOG("app_window_init: después de create_device_selector");

	gtk_widget_set_hexpand(self->device_selector, TRUE);
	gtk_box_append(GTK_BOX(device_row), self->lbl_device);
	gtk_box_append(GTK_BOX(device_row), self->device_selector);

	self->progress_bar = gtk_progress_bar_new();
	gtk_widget_set_hexpand(self->progress_bar, TRUE);

	GtkWidget *volume_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_append(GTK_BOX(volume_row), gtk_label_new("Volumen:"));
	self->volume_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 1.0, 0.01);
	gtk_range_set_value(GTK_RANGE(self->volume_scale), 1.0);
	gtk_widget_set_hexpand(self->volume_scale, TRUE);
	g_signal_connect(self->volume_scale, "value-changed", G_CALLBACK(on_volume_changed), self);
	gtk_box_append(GTK_BOX(volume_row), self->volume_scale);

	self->lbl_now_playing = gtk_label_new(self->localization->getText("no_songs_loaded").c_str());
	gtk_widget_set_hexpand(self->lbl_now_playing, TRUE);
	gtk_label_set_ellipsize(GTK_LABEL(self->lbl_now_playing), PANGO_ELLIPSIZE_END);
	gtk_widget_set_margin_top(self->lbl_now_playing, 5);
	gtk_widget_set_margin_bottom(self->lbl_now_playing, 5);

	gtk_box_append(GTK_BOX(bottom_box), controls_row);
	gtk_box_append(GTK_BOX(bottom_box), device_row);
	gtk_box_append(GTK_BOX(bottom_box), self->progress_bar);
	gtk_box_append(GTK_BOX(bottom_box), volume_row);
	gtk_box_append(GTK_BOX(bottom_box), self->lbl_now_playing);
	DBG_LOG("app_window_init: bottom_box completo");

	self->playlist_manager = std::make_unique<PlaylistManager>(self->db.get(), self->list_playlists, self->list_songs, self->spin_rating, self->lbl_now_playing);
	DBG_LOG("app_window_init: PlaylistManager creado");

	gtk_paned_set_start_child(GTK_PANED(top_paned), left_box);
	gtk_paned_set_end_child(GTK_PANED(top_paned), right_box);
	gtk_paned_set_shrink_start_child(GTK_PANED(top_paned), FALSE);
	gtk_paned_set_shrink_end_child(GTK_PANED(top_paned), FALSE);

	gtk_paned_set_start_child(GTK_PANED(paned), top_paned);
	gtk_paned_set_end_child(GTK_PANED(paned), bottom_box);
	gtk_paned_set_shrink_end_child(GTK_PANED(paned), FALSE);

	gtk_window_set_child(GTK_WINDOW(self), paned);
	DBG_LOG("app_window_init: jerarquía de widgets montada en la ventana");

	self->playlist_manager->refreshPlaylists();
	DBG_LOG("app_window_init: refreshPlaylists() hecho -- fin de app_window_init");
}

GtkWidget* create_app_window(GtkApplication *app) {
	return GTK_WIDGET(g_object_new(APP_TYPE_WINDOW, "application", app, nullptr));
}
