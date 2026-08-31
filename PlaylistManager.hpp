/*
 * PlaylistManager.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

/**
 * @file PlaylistManager.hpp
 * @brief Gestor de listas de reproducción para la interfaz GTK
 * @author usuario001
 * @date 31 de agosto de 2026
 * @version 1.1
 */

#ifndef PLAYLISTMANAGER_HPP_
#define PLAYLISTMANAGER_HPP_

#include <gtk/gtk.h>
#include "DatabaseManager.hpp"
#include "AudioEngine.hpp"
#include <vector>
#include <string>

/**
 * @class PlaylistManager
 * @brief Gestor que sincroniza la base de datos de playlists con la interfaz GTK
 *
 * Proporciona funcionalidad para actualizar listas de reproducción, reproducir
 * canciones y gestionar la interfaz de usuario. Actúa como controlador entre
 * la base de datos y la interfaz gráfica.
 *
 * También sabe representar el modo "sin playlist" (archivos cargados
 * directamente, sin asociar a ninguna lista), mostrando en ese caso los
 * nombres recibidos en el parámetro direct_files de refreshSongs()/removePlaylist().
 */
class PlaylistManager {
public:
	/**
	 * @brief Constructor del gestor de playlists
	 * @param db Puntero a DatabaseManager
	 * @param list_playlists Widget de lista de playlists GTK
	 * @param list_songs Widget de lista de canciones GTK
	 * @param spin_rating Control de calificación GTK
	 * @param lbl_now_playing Etiqueta de canción actual GTK
	 */
	PlaylistManager(DatabaseManager *db, GtkWidget *list_playlists, GtkWidget *list_songs, GtkWidget *spin_rating, GtkWidget *lbl_now_playing);

	/**
	 * @brief Destructor del gestor de playlists
	 */
	~PlaylistManager();

	/**
	 * @brief Recarga y actualiza la visualización de playlists en la interfaz
	 *
	 * Si había una playlist seleccionada (según el índice interno) antes de
	 * refrescar, se intenta reseleccionar la fila correspondiente para no
	 * perder la sincronía entre el índice interno y lo que ve el usuario.
	 */
	void refreshPlaylists();

	/**
	 * @brief Recarga y actualiza la visualización de canciones
	 *
	 * Si current_playlist_index es >= 0, se muestran las canciones de esa
	 * playlist (desde la base de datos). Si es -1, se muestra en su lugar
	 * el contenido de direct_files (archivos cargados sin playlist).
	 *
	 * @param current_playlist_index Índice de la playlist actual, o -1 para
	 * modo "archivos sueltos"
	 * @param direct_files Archivos sueltos a mostrar cuando
	 * current_playlist_index es -1. Se ignora si current_playlist_index >= 0.
	 */
	void refreshSongs(int current_playlist_index, const std::vector<std::string> &direct_files = { });

	/**
	 * @brief Establece la playlist actual
	 * @param index Índice de la playlist a establecer como actual
	 */
	void setCurrentPlaylist(int index);

	/**
	 * @brief Establece la canción actual
	 * @param index Índice de la canción a establecer como actual
	 */
	void setCurrentSong(int index);

	/**
	 * @brief Añade una nueva playlist
	 * @param name Nombre de la nueva playlist
	 */
	void addPlaylist(const std::string &name);

	/**
	 * @brief Elimina una playlist
	 *
	 * Tras eliminarla, deja el gestor en modo "sin playlist" y refresca la
	 * lista de canciones mostrando direct_files si se proporciona, en vez
	 * de dejar la lista de canciones simplemente vacía.
	 *
	 * @param index Índice de la playlist a eliminar
	 * @param direct_files Archivos sueltos a mostrar tras la eliminación
	 * (opcional; si se omite, la lista de canciones queda vacía)
	 */
	void removePlaylist(int index, const std::vector<std::string> &direct_files = { });

	/**
	 * @brief Elimina una canción de una playlist
	 * @param playlist_index Índice de la playlist
	 * @param song_index Índice de la canción a eliminar
	 */
	void removeSong(int playlist_index, int song_index);

	/**
	 * @brief Establece la calificación de una playlist
	 * @param playlist_index Índice de la playlist
	 * @param rating Nueva calificación
	 */
	void setPlaylistRating(int playlist_index, int rating);

	/**
	 * @brief Inicia la reproducción de una canción
	 * @param song_index Índice de la canción a reproducir
	 * @param db Puntero a DatabaseManager
	 * @param audio_engine Referencia al motor de audio
	 * @param current_playlist_index Índice de la playlist actual
	 * @param direct_files Vector de archivos directos (sin playlist)
	 */
	void playSong(int song_index, DatabaseManager *db, std::unique_ptr<AudioEngine> &audio_engine, int current_playlist_index, std::vector<std::string> &direct_files);

	/**
	 * @brief Obtiene el índice de la playlist actual
	 * @return Índice de la playlist actual
	 */
	int getCurrentPlaylistIndex() const;

	/**
	 * @brief Obtiene el índice de la canción actual
	 * @return Índice de la canción actual
	 */
	int getCurrentSongIndex() const;

private:
	/**
	 * @brief Puntero a la base de datos
	 */
	DatabaseManager *db_;

	/**
	 * @brief Widget de lista de playlists GTK
	 */
	GtkWidget *list_playlists_;

	/**
	 * @brief Widget de lista de canciones GTK
	 */
	GtkWidget *list_songs_;

	/**
	 * @brief Widget de control de calificación GTK
	 */
	GtkWidget *spin_rating_;

	/**
	 * @brief Widget de etiqueta de canción actual GTK
	 */
	GtkWidget *lbl_now_playing_;

	/**
	 * @brief Índice de la playlist actual
	 */
	int current_playlist_index_;

	/**
	 * @brief Índice de la canción actual
	 */
	int current_song_index_;
};

#endif /* PLAYLISTMANAGER_HPP_ */
