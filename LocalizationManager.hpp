/*
 * LocalizationManager.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

#ifndef LOCALIZATIONMANAGER_HPP_
#define LOCALIZATIONMANAGER_HPP_

#include <string>
#include <unordered_map>
#include <filesystem>

class LocalizationManager {
public:
    LocalizationManager();
    ~LocalizationManager();

    bool load(const std::string& language = "es");
    const std::string& getText(const std::string& key) const;
    std::filesystem::path getLocalizationFilePath() const;

private:
    std::unordered_map<std::string, std::string> translations_;
    std::string current_language_;
    std::filesystem::path localization_path_;

    std::filesystem::path getExecutableDirectory() const;
    void loadDefaultTranslations(); // Carga traducciones por defecto si falla el XML
};

#endif /* LOCALIZATION_HPP_ */
