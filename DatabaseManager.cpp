/*
 * DatabaseManager.cpp
 *
 *  Created on: 24 jul 2026
 *      Author: usuario001
 */

#include "DatabaseManager.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

DatabaseManager::DatabaseManager() {
	dbPath_ = fs::current_path() / "music_db.xml";
	load();
}

DatabaseManager::~DatabaseManager() {
	save();
}

void DatabaseManager::load() {
	std::lock_guard<std::mutex> lock(mutex_);
	playlists_.clear();
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(dbPath_.c_str()) != tinyxml2::XML_SUCCESS) {
		return;
	}

	tinyxml2::XMLElement *root = doc.FirstChildElement("MusicPlayerDB");
	if (!root) {
		return;
	}

	tinyxml2::XMLElement *plNode = root->FirstChildElement("Playlist");
	while (plNode) {
		Playlist pl;
		pl.name = plNode->Attribute("name") ? plNode->Attribute("name") : "Unknown";
		pl.rating = plNode->IntAttribute("rating", 3);

		tinyxml2::XMLElement *songNode = plNode->FirstChildElement("Song");
		while (songNode) {
			Song s;
			s.filepath = songNode->Attribute("path") ? songNode->Attribute("path") : "";
			s.filename = fs::path(s.filepath).filename().string();
			s.favorite = songNode->BoolAttribute("favorite", false);
			pl.songs.push_back(s);
			songNode = songNode->NextSiblingElement("Song");
		}
		playlists_.push_back(pl);
		plNode = plNode->NextSiblingElement("Playlist");
	}
}

//void DatabaseManager::save() {
//	std::lock_guard<std::mutex> lock(mutex_);
//	tinyxml2::XMLDocument doc;
//	auto *decl = doc.NewDeclaration();
//	doc.InsertFirstChild(decl);
//
//	auto *root = doc.NewElement("MusicPlayerDB");
//	doc.InsertEndChild(root);
//
//	for (const auto &pl : playlists_) {
//		auto *plNode = doc.NewElement("Playlist");
//		plNode->SetAttribute("name", pl.name.c_str());
//		plNode->SetAttribute("rating", pl.rating);
//
//		for (const auto &s : pl.songs) {
//			auto *sNode = doc.NewElement("Song");
//			sNode->SetAttribute("path", s.filepath.c_str());
//			sNode->SetAttribute("favorite", s.favorite);
//			plNode->InsertEndChild(sNode);
//		}
//		root->InsertEndChild(plNode);
//	}
//	doc.SaveFile(dbPath_.c_str());
//}

void DatabaseManager::save() {
	std::lock_guard<std::mutex> lock(mutex_);
	saveLocked();
}
void DatabaseManager::saveLocked() {
	tinyxml2::XMLDocument doc;
	auto *decl = doc.NewDeclaration();
	doc.InsertFirstChild(decl);

	auto *root = doc.NewElement("MusicPlayerDB");
	doc.InsertEndChild(root);

	for (const auto &pl : playlists_) {
		auto *plNode = doc.NewElement("Playlist");
		plNode->SetAttribute("name", pl.name.c_str());
		plNode->SetAttribute("rating", pl.rating);
		for (const auto &s : pl.songs) {
			auto *sNode = doc.NewElement("Song");
			sNode->SetAttribute("path", s.filepath.c_str());
			sNode->SetAttribute("favorite", s.favorite);
			plNode->InsertEndChild(sNode);
		}
		root->InsertEndChild(plNode);
	}
	doc.SaveFile(dbPath_.c_str());
}

std::vector<Playlist> DatabaseManager::getPlaylists() {
	std::lock_guard<std::mutex> lock(mutex_);
	return playlists_; // Devuelve una copia segura
}

// obtiene de forma segura la playlist en 'index' y la copia a 'out'
bool DatabaseManager::getPlaylist(size_t index, Playlist &out) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (index < playlists_.size()) {
		out = playlists_[index];
		return true;
	}
	return false;
}

size_t DatabaseManager::getPlaylistCount() {
	std::lock_guard<std::mutex> lock(mutex_);
	return playlists_.size();
}

//void DatabaseManager::addPlaylist(const std::string &name) {
//	std::lock_guard<std::mutex> lock(mutex_);
//	playlists_.push_back( { name, 3, { } });
//	save();
//}

void DatabaseManager::addPlaylist(const std::string &name) {
	std::lock_guard<std::mutex> lock(mutex_);
	playlists_.push_back( { name, 3, { } });
	saveLocked();   // <- ya no vuelve a bloquear
}

//void DatabaseManager::addSongsToPlaylist(int playlistIndex, const std::vector<std::string> &filePaths) {
//	std::lock_guard<std::mutex> lock(mutex_);
//	if (playlistIndex < 0 || playlistIndex >= (int) playlists_.size() || filePaths.empty()) {
//		return;
//	}
//
//	for (const auto &path : filePaths) {
//		bool exists = false;
//		for (const auto &existing_song : playlists_[playlistIndex].songs) {
//			if (existing_song.filepath == path) {
//				exists = true;
//				break;
//			}
//		}
//		if (!exists) {
//			Song new_song;
//			new_song.filepath = path;
//			new_song.filename = fs::path(path).filename().string();
//			new_song.favorite = false;
//			playlists_[playlistIndex].songs.push_back(new_song);
//		}
//	}
//	save();
//}

void DatabaseManager::addSongsToPlaylist(int playlistIndex, const std::vector<std::string> &filePaths) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (playlistIndex < 0 || playlistIndex >= (int) playlists_.size() || filePaths.empty()) {
		return;
	}
	for (const auto &path : filePaths) {
		bool exists = std::any_of(playlists_[playlistIndex].songs.begin(), playlists_[playlistIndex].songs.end(), [&](const Song &s) {
			return s.filepath == path;
		});
		if (!exists) {
			Song new_song { path, std::filesystem::path(path).filename().string(), false };
			playlists_[playlistIndex].songs.push_back(new_song);
		}
	}
	saveLocked();
}

//void DatabaseManager::setPlaylistRating(int playlistIndex, int rating) {
//	std::lock_guard<std::mutex> lock(mutex_);
//	if (playlistIndex >= 0 && playlistIndex < (int) playlists_.size()) {
//		playlists_[playlistIndex].rating = rating;
//		save();
//	}
//}

void DatabaseManager::setPlaylistRating(int playlistIndex, int rating) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (playlistIndex < 0 || playlistIndex >= (int) playlists_.size()) {
		return;
	}
	playlists_[playlistIndex].rating = rating;
	saveLocked();
}

//void DatabaseManager::toggleFavorite(int playlistIndex, int songIndex) {
//	std::lock_guard<std::mutex> lock(mutex_);
//	if (playlistIndex >= 0 && playlistIndex < (int) playlists_.size() && songIndex >= 0 && songIndex < (int) playlists_[playlistIndex].songs.size()) {
//		playlists_[playlistIndex].songs[songIndex].favorite = !playlists_[playlistIndex].songs[songIndex].favorite;
//		save();
//	}
//}

void DatabaseManager::toggleFavorite(int playlistIndex, int songIndex) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (playlistIndex < 0 || playlistIndex >= (int) playlists_.size()) {
		return;
	}
	auto &songs = playlists_[playlistIndex].songs;
	if (songIndex < 0 || songIndex >= (int) songs.size()) {
		return;
	}
	songs[songIndex].favorite = !songs[songIndex].favorite;
	saveLocked();
}

//bool DatabaseManager::removePlaylist(int playlistIndex) {
//	std::lock_guard<std::mutex> lock(mutex_);
//	if (playlistIndex >= 0 && playlistIndex < (int) playlists_.size()) {
//		playlists_.erase(playlists_.begin() + playlistIndex);
//		save();
//		return true;
//	}
//	return false;
//}
bool DatabaseManager::removePlaylist(int playlistIndex) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (playlistIndex < 0 || playlistIndex >= (int) playlists_.size()) {
		return false;
	}
	playlists_.erase(playlists_.begin() + playlistIndex);
	saveLocked();
	return true;
}

//bool DatabaseManager::removeSong(int playlistIndex, int songIndex) {
//	std::lock_guard<std::mutex> lock(mutex_);
//	if (playlistIndex >= 0 && playlistIndex < (int) playlists_.size() && songIndex >= 0 && songIndex < (int) playlists_[playlistIndex].songs.size()) {
//		playlists_[playlistIndex].songs.erase(playlists_[playlistIndex].songs.begin() + songIndex);
//		save();
//		return true;
//	}
//	return false;
//}

bool DatabaseManager::removeSong(int playlistIndex, int songIndex) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (playlistIndex < 0 || playlistIndex >= (int) playlists_.size()) {
		return false;}
	auto &songs = playlists_[playlistIndex].songs;
	if (songIndex < 0 || songIndex >= (int) songs.size()) {
		return false;}
	songs.erase(songs.begin() + songIndex);
	saveLocked();
	return true;
}

