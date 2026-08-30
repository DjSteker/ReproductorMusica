/*
 * DatabaseManager.hpp
 *
 *  Created on: 24 jul 2026
 *      Author: usuario001
 */

#ifndef DATABASEMANAGER_HPP_
#define DATABASEMANAGER_HPP_

#include <string>
#include <vector>
#include <tinyxml2.h>
#include <filesystem>
#include <mutex>

struct Song {
	std::string filepath;
	std::string filename;
	bool favorite;
};

struct Playlist {
	std::string name;
	int rating;
	std::vector<Song> songs;
};

class DatabaseManager {
public:
	DatabaseManager();
	~DatabaseManager();
	void load();
	void save();

	// CAMBIADO: Devuelve por valor en vez de por referencia para evitar Data Races
	std::vector<Playlist> getPlaylists();

	void addPlaylist(const std::string &name);
	void addSongsToPlaylist(int playlistIndex, const std::vector<std::string> &filePaths);
	void setPlaylistRating(int playlistIndex, int rating);
	void toggleFavorite(int playlistIndex, int songIndex);
	bool removePlaylist(int playlistIndex);
	bool removeSong(int playlistIndex, int songIndex);

private:
	std::filesystem::path dbPath_;
	std::vector<Playlist> playlists_;
	std::mutex mutex_;
};

#endif /* DATABASEMANAGER_HPP_ */
