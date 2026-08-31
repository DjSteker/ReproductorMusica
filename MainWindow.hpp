/*
 * MainWindow.hpp
 *
 *  Created on: 24 jul 2026
 *      Author: usuario001
 */

/**
 * @file MainWindow.hpp
 * @brief Definición de la ventana principal de la aplicación
 * @author usuario001
 * @date 24 de julio de 2026
 * @version 1.0
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

/**
 * @typedef AppWindow
 * @brief Estructura opaca que representa la ventana principal de la aplicación
 */
typedef struct _AppWindow AppWindow;

/**
 * @def APP_TYPE_WINDOW
 * @brief Tipo de GObject para AppWindow
 */
#define APP_TYPE_WINDOW (app_window_get_type())
G_DECLARE_FINAL_TYPE(AppWindow, app_window, APP, WINDOW, GtkApplicationWindow)

/**
 * @struct _AppWindow
 * @brief Estructura que contiene todos los elementos de la ventana principal
 *
 * Contiene referencias a widgets, motores de reproducción y gestores de datos.
 */
struct _AppWindow {
	/**
	 * @brief Instancia padre de GtkApplicationWindow
	 */
	GtkApplicationWindow parent_instance;

	/* ==================== WIDGETS DE INTERFAZ ==================== */

	/**
	 * @brief Caja de lista para mostrar playlists
	 */
	GtkWidget *list_playlists;

	/**
	 * @brief Caja de lista para mostrar canciones
	 */
	GtkWidget *list_songs;

	/**
	 * @brief Etiqueta que muestra la canción actual en reproducción
	 */
	GtkWidget *lbl_now_playing;

	/**
	 * @brief Control deslizante para la calificación de playlist
	 */
	GtkWidget *spin_rating;

	/**
	 * @brief Etiqueta que muestra el dispositivo de audio actual
	 */
	GtkWidget *lbl_device;

	/**
	 * @brief Selector desplegable para elegir dispositivo de audio
	 */
	GtkWidget *device_selector;

	/**
	 * @brief Botón de reproducción
	 */
	GtkWidget *btn_play;

	/**
	 * @brief Botón de pausa
	 */
	GtkWidget *btn_pause;

	/**
	 * @brief Botón de detención
	 */
	GtkWidget *btn_stop;

	/**
	 * @brief Botón para añadir nueva playlist
	 */
	GtkWidget *btn_add_playlist;

	/**
	 * @brief Botón para eliminar playlist
	 */
	GtkWidget *btn_remove_playlist;

	/**
	 * @brief Botón para cargar archivo de música
	 */
	GtkWidget *btn_load_file;

	/**
	 * @brief Botón para cargar carpeta de música
	 */
	GtkWidget *btn_load_folder;

	/**
	 * @brief Botón para eliminar canción
	 */
	GtkWidget *btn_remove_song;

	/**
	 * @brief Botón para activar modo aleatorio
	 */
	GtkWidget *btn_shuffle;

	/**
	 * @brief Botón para activar repetición
	 */
	GtkWidget *btn_repeat;

	/**
	 * @brief Barra de progreso de reproducción
	 */
	GtkWidget *progress_bar;

	/**
	 * @brief Control deslizante de volumen
	 */
	GtkWidget *volume_scale;

	/* ==================== OBJETOS DE MOTOR Y DATOS ==================== */

	/**
	 * @brief Motor de audio para reproducción
	 */
	std::unique_ptr<AudioEngine> audio_engine;

	/**
	 * @brief Gestor de base de datos de playlists
	 */
	std::unique_ptr<DatabaseManager> db;

	/**
	 * @brief Gestor de playlists
	 */
	std::unique_ptr<PlaylistManager> playlist_manager;

	/**
	 * @brief Gestor de localización
	 */
	std::unique_ptr<Localization> localization;
	//std::unique_ptr<LocalizationManager> localization;

	/**
	 * @brief Vector de archivos cargados directamente (sin playlist)
	 */
	std::vector<std::string> direct_files;

	/**
	 * @brief Indica si el modo aleatorio está activo
	 */
	bool shuffle_mode;

	/**
	 * @brief Indica si la repetición está activa
	 */
	bool repeat_mode;
};

/**
 * @brief Crea y retorna la ventana principal de la aplicación
 * @param app Aplicación GTK a la que pertenece la ventana
 * @return Widget de la ventana principal
 */
GtkWidget* create_app_window(GtkApplication *app);

/**
 * @brief Crea el selector de dispositivos de audio
 * @param win Ventana principal
 * @return Widget del selector de dispositivos
 */
GtkWidget* create_device_selector(AppWindow *win);

/* ==================== CALLBACKS DE EVENTOS ==================== */

/**
 * @brief Callback cuando se selecciona una playlist
 * @param box Caja de lista de playlists
 * @param row Fila seleccionada
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_playlist_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data);

/**
 * @brief Callback cuando se activa una canción
 * @param box Caja de lista de canciones
 * @param row Fila activada
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_song_activated(GtkListBox *box, GtkListBoxRow *row, gpointer user_data);

/**
 * @brief Callback del botón de reproducción
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_play_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback del botón de pausa
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_pause_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback del botón de detención
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_stop_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback del botón de añadir playlist
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_add_playlist_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback del botón de eliminar playlist
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_remove_playlist_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback del botón de eliminar canción
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_remove_song_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback cuando cambia la calificación
 * @param spin Control de números
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_rating_changed(GtkSpinButton *spin, gpointer user_data);

/**
 * @brief Callback cuando se selecciona un archivo
 * @param source Objeto origen
 * @param result Resultado de la operación asincrónica
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_file_selected(GObject *source, GAsyncResult *result, gpointer user_data);

/**
 * @brief Callback del botón de cargar archivo
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_load_file_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback del botón de cargar carpeta
 * @param button Botón presionado
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_load_folder_clicked(GtkButton *button, gpointer user_data);

/**
 * @brief Callback cuando se activa/desactiva el modo aleatorio
 * @param button Botón de alternancia
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_shuffle_toggled(GtkToggleButton *button, gpointer user_data);

/**
 * @brief Callback cuando se activa/desactiva la repetición
 * @param button Botón de alternancia
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_repeat_toggled(GtkToggleButton *button, gpointer user_data);

/**
 * @brief Callback cuando cambia el volumen
 * @param range Control de rango
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_volume_changed(GtkRange *range, gpointer user_data);

/**
 * @brief Callback de progreso de reproducción
 * @param progress Valor de progreso (0.0 a 1.0)
 * @param user_data Datos del usuario (AppWindow*)
 */
static void on_progress_callback(double progress, gpointer user_data);

#endif /* MAINWINDOW_HPP_ */
