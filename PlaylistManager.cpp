/*
 * PlaylistManager.cpp
 *
 *  Created on: 25 jul 2026
 *      Author: DjSteker
 */

#include "PlaylistManager.hpp"
#include "AudioEngine.hpp"
#include "DatabaseManager.hpp"
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

PlaylistManager::PlaylistManager(DatabaseManager *db, GtkWidget *list_playlists, GtkWidget *list_songs, GtkWidget *spin_rating, GtkWidget *lbl_now_playing) :
		db_(db), list_playlists_(list_playlists), list_songs_(list_songs), spin_rating_(spin_rating), lbl_now_playing_(lbl_now_playing), current_playlist_index_(-1), current_song_index_(-1) {
}

PlaylistManager::~PlaylistManager() {
}

void PlaylistManager::refreshPlaylists() {
	while (GtkWidget *child = gtk_widget_get_first_child(list_playlists_)) {
		gtk_list_box_remove(GTK_LIST_BOX(list_playlists_), child);
	}

	// db_->getPlaylists() devuelve por valor: aquí es seguro porque el bucle range-for
	// une una referencia directamente al temporal (extensión de vida garantizada).
	for (const auto &pl : db_->getPlaylists()) {
		std::string text = pl.name + " (⭐ " + std::to_string(pl.rating) + ")";
		GtkWidget *label = gtk_label_new(text.c_str());
		gtk_widget_set_margin_start(label, 10);
		gtk_widget_set_margin_top(label, 5);
		gtk_widget_set_margin_bottom(label, 5);
		gtk_label_set_xalign(GTK_LABEL(label), 0.0);
		gtk_list_box_append(GTK_LIST_BOX(list_playlists_), label);
	}
}

void PlaylistManager::refreshSongs(int current_playlist_index) {
	while (GtkWidget *child = gtk_widget_get_first_child(list_songs_)) {
		gtk_list_box_remove(GTK_LIST_BOX(list_songs_), child);
	}

	if (current_playlist_index >= 0) {
		// FIX: db_->getPlaylists() devuelve un vector<Playlist> temporal. Indexar con
		// operator[] y tomar una referencia a .songs directamente sobre el resultado
		// de esa llamada NO extiende la vida del temporal (solo se extiende cuando la
		// referencia se une directamente al temporal, no a través de una llamada a
		// función como operator[]). Guardamos la copia en una variable nombrada para
		// que viva durante todo el bloque.
		std::vector<Playlist> playlists = db_->getPlaylists();
		const auto &songs = playlists[current_playlist_index].songs;
		for (size_t i = 0; i < songs.size(); ++i) {
			GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
			GtkWidget *label = gtk_label_new(songs[i].filename.c_str());
			gtk_widget_set_hexpand(label, TRUE);
			gtk_label_set_xalign(GTK_LABEL(label), 0.0);
			gtk_box_append(GTK_BOX(box), label);
			gtk_widget_set_margin_start(box, 5);
			gtk_list_box_append(GTK_LIST_BOX(list_songs_), box);
		}
	}
}

void PlaylistManager::setCurrentPlaylist(int index) {
	current_playlist_index_ = index;
	if (index >= 0) {
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_rating_), db_->getPlaylists()[index].rating);
	}
	current_song_index_ = -1;
	refreshSongs(index);
}

void PlaylistManager::setCurrentSong(int index) {
	current_song_index_ = index;
}

int PlaylistManager::getCurrentPlaylistIndex() const {
	return current_playlist_index_;
}

int PlaylistManager::getCurrentSongIndex() const {
	return current_song_index_;
}

void PlaylistManager::addPlaylist(const std::string &name) {
	db_->addPlaylist(name);
	refreshPlaylists();
}

void PlaylistManager::removePlaylist(int index) {
	if (index < 0) {
		return;
	}
	db_->removePlaylist(index);
	current_playlist_index_ = -1;
	current_song_index_ = -1;
	refreshPlaylists();
	refreshSongs(-1);
	gtk_label_set_text(GTK_LABEL(lbl_now_playing_), "Lista eliminada.");
}

void PlaylistManager::removeSong(int playlist_index, int song_index) {
	if (song_index < 0) {
		gtk_label_set_text(GTK_LABEL(lbl_now_playing_), "Selecciona una canción primero.");
		return;
	}
	if (playlist_index >= 0) {
		db_->removeSong(playlist_index, song_index);
		refreshSongs(playlist_index);
	}
	current_song_index_ = -1;
	gtk_label_set_text(GTK_LABEL(lbl_now_playing_), "Canción eliminada de la lista.");
}

void PlaylistManager::setPlaylistRating(int playlist_index, int rating) {
	if (playlist_index >= 0) {
		db_->setPlaylistRating(playlist_index, rating);
		refreshPlaylists();
	}
}

void PlaylistManager::playSong(int song_index, DatabaseManager *db, std::unique_ptr<AudioEngine> &audio_engine, int current_playlist_index, std::vector<std::string> &direct_files) {
	if (current_playlist_index >= 0) {
		// FIX: misma corrección que en refreshSongs — copia nombrada para evitar
		// la referencia colgante sobre el temporal devuelto por getPlaylists().
		std::vector<Playlist> playlists = db->getPlaylists();
		const auto &songs = playlists[current_playlist_index].songs;
		if (song_index >= 0 && song_index < (int) songs.size()) {
			current_song_index_ = song_index;
			if (audio_engine->loadWav(songs[song_index].filepath)) {
				std::string text = "Reproduciendo: " + songs[song_index].filename;
				gtk_label_set_text(GTK_LABEL(lbl_now_playing_), text.c_str());
				audio_engine->play();
				GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(list_songs_), song_index);
				if (row) {
					gtk_list_box_select_row(GTK_LIST_BOX(list_songs_), row);
				}
			}
		}
		return;
	}
	if (!direct_files.empty() && song_index >= 0 && song_index < (int) direct_files.size()) {
		current_song_index_ = song_index;
		if (audio_engine->loadWav(direct_files[song_index])) {
			std::string filename = fs::path(direct_files[song_index]).filename().string();
			std::string text = "Reproduciendo: " + filename;
			gtk_label_set_text(GTK_LABEL(lbl_now_playing_), text.c_str());
			audio_engine->play();
			GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(list_songs_), song_index);
			if (row) {
				gtk_list_box_select_row(GTK_LIST_BOX(list_songs_), row);
			}
		}
	}
}
