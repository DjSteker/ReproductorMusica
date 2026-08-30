/*
 * PlaylistManager.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

#ifndef PLAYLISTMANAGER_HPP_
#define PLAYLISTMANAGER_HPP_

#include <gtk/gtk.h>
#include "DatabaseManager.hpp"
#include "AudioEngine.hpp"

class PlaylistManager {
public:
	PlaylistManager(DatabaseManager *db, GtkWidget *list_playlists, GtkWidget *list_songs, GtkWidget *spin_rating, GtkWidget *lbl_now_playing);
	~PlaylistManager();

	void refreshPlaylists();
	void refreshSongs(int current_playlist_index);
	void setCurrentPlaylist(int index);
	void setCurrentSong(int index);
	void addPlaylist(const std::string &name);
	void removePlaylist(int index);
	void removeSong(int playlist_index, int song_index);
	void setPlaylistRating(int playlist_index, int rating);
	void playSong(int song_index, DatabaseManager *db, std::unique_ptr<AudioEngine> &audio_engine, int current_playlist_index, std::vector<std::string> &direct_files);

	int getCurrentPlaylistIndex() const;
	int getCurrentSongIndex() const;

private:
	DatabaseManager *db_;
	GtkWidget *list_playlists_;
	GtkWidget *list_songs_;
	GtkWidget *spin_rating_;
	GtkWidget *lbl_now_playing_;
	int current_playlist_index_;
	int current_song_index_;
};

#endif /* PLAYLISTMANAGER_HPP_ */

