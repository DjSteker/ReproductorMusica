/*
 * Localization.hpp
 *
 *  Created on: 25 jul 2026
 *      Author: usuario001
 */

#ifndef LOCALIZATION_HPP_
#define LOCALIZATION_HPP_

#include <string>
#include <unordered_map>
#include <tinyxml2.h>

class Localization {
public:
    Localization();
    bool load(const std::string &language = "es");

    // CAMBIADO: Ahora devuelve por valor (std::string) en lugar de referencia (const std::string&)
    // Esto evita el crash por devolver una referencia a una variable temporal destruida.
    std::string getText(const std::string &key) const;

private:
    std::unordered_map<std::string, std::string> translations_;
    std::string current_language_;
};

#endif /* LOCALIZATION_HPP_ */
