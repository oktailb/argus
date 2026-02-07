#include "configuration.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace {

/** Enlève \r et \n en fin de ligne (fichiers Windows / anciens Mac). */
void trimLineEnd(std::string& s) {
    s.erase(std::find(s.begin(), s.end(), '\0'), s.end());
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n'))
        s.pop_back();
}

/** Enlève espaces et \r\n en début et fin. */
void trimKeyValue(std::string& s) {
    trimLineEnd(s);
    while (!s.empty() && (std::isspace(static_cast<unsigned char>(s.back())) || s.back() == '\r'))
        s.pop_back();
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
        ++start;
    if (start > 0)
        s = s.substr(start);
}

} // namespace

std::map<std::string, std::string> readConfiguration(const std::string& filename)
{
    std::map<std::string, std::string> keyValuePairs;
    std::ifstream file(filename);

    if (!file.is_open()) {
        return keyValuePairs;
    }

    std::string line;
    std::string currentSection = "";

    while (std::getline(file, line)) {
        // Sanitize: Remove null bytes if any
        line.erase(std::remove(line.begin(), line.end(), '\0'), line.end());
        trimLineEnd(line);
        if (line.empty() || line[0] == ';') {
            continue;
        }

        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            trimKeyValue(currentSection);
        } else {
            std::size_t separatorPos = line.find('=');
            if (separatorPos != std::string::npos) {
                std::string key = line.substr(0, separatorPos);
                std::string value = line.substr(separatorPos + 1);
                trimKeyValue(key);
                trimKeyValue(value);
                keyValuePairs[currentSection + "/" + key] = value;
            }
        }
    }

    file.close();
    return keyValuePairs;
}

void saveConfiguration(const std::map<std::string, std::string>& keyValuePairs, const std::string& filename)
{
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << "Erreur : Impossible d'ouvrir le fichier " << filename << " pour sauvegarder la configuration." << std::endl;
        return;
    }

    std::string currentSection;

    for (const auto& entry : keyValuePairs)
    {
        std::size_t slashPos = entry.first.find('/');
        if (slashPos != std::string::npos)
        {
            // Si c'est une clé de section, mettre à jour la section actuelle
            std::string sectionName = entry.first.substr(0, slashPos);
            if (sectionName != currentSection)
            {
                currentSection = sectionName;
                file << "[" << currentSection << "]" << std::endl;
            }
            file << entry.first.substr(slashPos + 1) << "=" << entry.second << std::endl;
        }
        else
            file << entry.first << "=" << entry.second << std::endl;
    }

    file.close();
}
