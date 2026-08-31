/*
 * DatabaseManager.hpp
 *
 *  Created on: 24 jul 2026
 *      Author: usuario001
 */

/**
 * @file DatabaseManager.hpp
 * @brief Gestor de base de datos para almacenar playlists y canciones
 * @author usuario001
 * @date 24 de julio de 2026
 * @version 1.0
 */

#ifndef DATABASEMANAGER_HPP_
#define DATABASEMANAGER_HPP_

#include <string>
#include <vector>
#include <tinyxml2.h>
#include <filesystem>
#include <mutex>

/**
 * @struct Song
 * @brief Estructura que representa una canción en la base de datos
 */
struct Song {
	/**
	 * @brief Ruta completa del archivo de música
	 */
	std::string filepath;

	/**
	 * @brief Nombre del archivo sin la ruta
	 */
	std::string filename;

	/**
	 * @brief Indica si la canción está marcada como favorita
	 */
	bool favorite;
};

/**
 * @struct Playlist
 * @brief Estructura que representa una lista de reproducción
 */
struct Playlist {
	/**
	 * @brief Nombre de la lista de reproducción
	 */
	std::string name;

	/**
	 * @brief Calificación de la lista (0-5 estrellas)
	 */
	int rating;

	/**
	 * @brief Vector de canciones en la lista de reproducción
	 */
	std::vector<Song> songs;
};

/**
 * @class DatabaseManager
 * @brief Gestor de base de datos que maneja playlists y canciones
 *
 * Proporciona funcionalidad para cargar, guardar, crear y modificar playlists
 * utilizando archivos XML. Thread-safe mediante mutex.
 */
class DatabaseManager {
public:
	/**
	 * @brief Constructor del gestor de base de datos
	 */
	DatabaseManager();

	/**
	 * @brief Destructor del gestor de base de datos
	 */
	~DatabaseManager();

	/**
	 * @brief Carga la base de datos desde el archivo XML
	 */
	void load();

	/**
	 * @brief Guarda la base de datos en el archivo XML
	 */
	void save();
	void saveLocked();

	/**
	 * @brief Obtiene todas las playlists
	 * @note Devuelve por valor para evitar data races
	 * @return Vector de playlists actuales
	 */
	std::vector<Playlist> getPlaylists();

	/**
	 * @brief Añade una nueva playlist a la base de datos
	 * @param name Nombre de la nueva playlist
	 */
	void addPlaylist(const std::string &name);

	/**
	 * @brief Añade canciones a una playlist específica
	 * @param playlistIndex Índice de la playlist
	 * @param filePaths Vector de rutas de archivos a añadir
	 */
	void addSongsToPlaylist(int playlistIndex, const std::vector<std::string> &filePaths);

	/**
	 * @brief Establece la calificación de una playlist
	 * @param playlistIndex Índice de la playlist
	 * @param rating Nueva calificación (típicamente 0-5)
	 */
	void setPlaylistRating(int playlistIndex, int rating);

	/**
	 * @brief Alterna el estado de favorita de una canción
	 * @param playlistIndex Índice de la playlist
	 * @param songIndex Índice de la canción
	 */
	void toggleFavorite(int playlistIndex, int songIndex);

	/**
	 * @brief Elimina una playlist de la base de datos
	 * @param playlistIndex Índice de la playlist a eliminar
	 * @return true si se eliminó correctamente, false si el índice es inválido
	 */
	bool removePlaylist(int playlistIndex);

	/**
	 * @brief Elimina una canción de una playlist
	 * @param playlistIndex Índice de la playlist
	 * @param songIndex Índice de la canción a eliminar
	 * @return true si se eliminó correctamente, false si los índices son inválidos
	 */
	bool removeSong(int playlistIndex, int songIndex);


	size_t getPlaylistCount();
	bool getPlaylist(size_t index, Playlist &out);

private:
	/**
	 * @brief Ruta al archivo de base de datos XML
	 */
	std::filesystem::path dbPath_;

	/**
	 * @brief Vector de playlists cargadas en memoria
	 */
	std::vector<Playlist> playlists_;

	/**
	 * @brief Mutex para garantizar acceso thread-safe
	 */
	std::mutex mutex_;
};

#endif /* DATABASEMANAGER_HPP_ */
