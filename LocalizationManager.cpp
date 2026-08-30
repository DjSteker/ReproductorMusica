/*
 * LocalizationManager.cpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

#include "LocalizationManager.hpp"
#include <tinyxml2.h>
#include <iostream>
#include <unistd.h> // Para readlink
#include <limits.h> // Para PATH_MAX

namespace fs = std::filesystem;

LocalizationManager::LocalizationManager() :
		current_language_("es") {
	localization_path_ = getExecutableDirectory() / "localization.xml";
	if (!load(current_language_)) {
		std::cerr << "Advertencia: No se pudo cargar localization.xml. Usando traducciones por defecto." << std::endl;
		loadDefaultTranslations();
	}
}

LocalizationManager::~LocalizationManager() {
}

fs::path LocalizationManager::getExecutableDirectory() const {
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	if (count != -1) {
		return fs::path(std::string(result, count)).parent_path();
	}
	return fs::current_path(); // Fallback: directorio actual
}

fs::path LocalizationManager::getLocalizationFilePath() const {
	return localization_path_;
}

bool LocalizationManager::load(const std::string &language) {
	try {
		if (!fs::exists(localization_path_)) {
			std::cerr << "Error: Archivo de localización no encontrado en: " << localization_path_ << std::endl;
			return false;
		}

		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(localization_path_.c_str()) != tinyxml2::XML_SUCCESS) {
			std::cerr << "Error al parsear localization.xml" << std::endl;
			return false;
		}

		translations_.clear();
		current_language_ = language;

		tinyxml2::XMLElement *root = doc.FirstChildElement("Localization");
		if (!root) {
			std::cerr << "Error: No se encontró el elemento raíz 'Localization' en localization.xml" << std::endl;
			return false;
		}

		for (tinyxml2::XMLElement *lang = root->FirstChildElement("Language"); lang; lang = lang->NextSiblingElement("Language")) {
			const char *lang_name = lang->Attribute("name");
			if (!lang_name || std::string(lang_name) != language)
				continue;

			for (tinyxml2::XMLElement *text = lang->FirstChildElement("Text"); text; text = text->NextSiblingElement("Text")) {
				const char *key = text->Attribute("key");
				const char *value = text->GetText();
				if (key && value) {
					translations_[key] = value;
				}
			}
			return true;
		}

		std::cerr << "Error: Idioma '" << language << "' no encontrado en localization.xml" << std::endl;
		return false;
	} catch (const std::exception &e) {
		std::cerr << "Excepción al cargar localization.xml: " << e.what() << std::endl;
		return false;
	}
}

void LocalizationManager::loadDefaultTranslations() {
	// Traducciones por defecto en español (fallback)
	translations_ = { { "app_title", "DJ Steker Music Player V1 - GTK4 Puro" }, { "now_playing", "Reproduciendo: " }, { "paused", "Pausado." }, { "stopped", "Detenido." }, { "playlist_removed",
			"Lista eliminada." }, { "song_removed", "Canción eliminada de la lista." }, { "select_song_first", "Selecciona una canción primero." }, { "end_of_list", "Fin de la lista." }, {
			"add_playlist_title", "Nueva Lista" }, { "playlist_name_label", "Nombre de la lista:" }, { "cancel_button", "Cancelar" }, { "create_button", "Crear" },
			{ "load_files_button", "Cargar Archivos" }, { "load_folder_button", "Cargar Carpeta" }, { "remove_song_button", "Eliminar Canción" }, { "play_button", "▶ Play" }, { "pause_button", "⏸ Pause" },
			{ "stop_button", "⏹ Stop" }, { "rating_label", "Puntuación:" }, { "device_label", "Salida:" }, { "no_songs_loaded", "Sin canciones cargadas" }, { "songs_added", "Añadidas %d canciones." } };
}

const std::string& LocalizationManager::getText(const std::string &key) const {
	auto it = translations_.find(key);
	if (it != translations_.end()) {
		return it->second;
	}
	// Si no se encuentra la clave, devolver la clave misma (para depuración)
	return key;
}
