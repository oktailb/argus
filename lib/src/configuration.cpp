#include "configuration.h"

std::map<std::string, std::string> readConfiguration(const std::string& filename)
{
    std::map<std::string, std::string> keyValuePairs;
    std::ifstream file(filename);

    if (!file.is_open()) {
        // Gérer l'erreur si le fichier ne peut pas être ouvert
        return keyValuePairs;
    }

    std::string line;
    std::string currentSection = "";

    while (std::getline(file, line)) {
        // Si la ligne est vide ou commence par ';', c'est un commentaire, on la ignore
        if (line.empty() || line[0] == ';') {
            continue;
        }

        // Vérifier si la ligne est une section
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
        } else {
            // Sinon, c'est une clé/valeur
            std::size_t separatorPos = line.find('=');
            if (separatorPos != std::string::npos) {
                std::string key = line.substr(0, separatorPos);
                std::string value = line.substr(separatorPos + 1);
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
