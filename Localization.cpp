/*
 * Localization.cpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

#include "Localization.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Localization::Localization() {
    current_language_ = "es";

    // CARGA POR DEFECTO: Evita el crash si no existe el XML
    translations_ = {
        {"app_title", "DJ Steker Music Player V1 - GTK4 Puro"},
        {"now_playing", "Reproduciendo: "},
        {"paused", "Pausado."},
        {"stopped", "Detenido."},
        {"playlist_removed", "Lista eliminada."},
        {"song_removed", "Canción eliminada de la lista."},
        {"select_song_first", "Selecciona una canción primero."},
        {"end_of_list", "Fin de la lista."},
        {"add_playlist_title", "Nueva Lista"},
        {"playlist_name_label", "Nombre de la lista:"},
        {"cancel_button", "Cancelar"},
        {"create_button", "Crear"},
        {"load_files_button", "Cargar Archivos"},
        {"load_folder_button", "Cargar Carpeta"},
        {"remove_song_button", "Eliminar Canción"},
        {"remove_playlist", "Eliminar Lista"},
        {"play_button", "▶ Play"},
        {"pause_button", "⏸ Pause"},
        {"stop_button", "⏹ Stop"},
        {"rating_label", "Puntuación:"},
        {"device_label", "Salida:"},
        {"no_songs_loaded", "Sin canciones cargadas"},
        {"songs_added", "Añadidas %d canciones."}
    };

    load("es"); // Intenta sobreescribir con el XML si existe
}

bool Localization::load(const std::string &language) {
    current_language_ = language;
    fs::path localization_path = fs::current_path() / "localization.xml";

    if (!fs::exists(localization_path)) {
        return false;
    }

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(localization_path.c_str()) != tinyxml2::XML_SUCCESS) {
        return false;
    }

    tinyxml2::XMLElement *root = doc.FirstChildElement("Localization");
    if (!root) {
        return false;
    }

    for (tinyxml2::XMLElement *lang = root->FirstChildElement("Language"); lang; lang = lang->NextSiblingElement("Language")) {
        const char *lang_name = lang->Attribute("name");
        if (!lang_name || std::string(lang_name) != language) {
            continue;
        }

        for (tinyxml2::XMLElement *text = lang->FirstChildElement("Text"); text; text = text->NextSiblingElement("Text")) {
            const char *key = text->Attribute("key");
            const char *value = text->GetText();
            if (key && value) {
                translations_[key] = value;
            }
        }
        return true;
    }
    return false;
}

std::string Localization::getText(const std::string &key) const {
    auto it = translations_.find(key);
    if (it != translations_.end()) {
        return it->second;
    }
    return key; // Seguro ahora: devuelve una copia del string
}
