#pragma once
/**
 * @file configuration.h
 * @brief Very simple methods to read/write INI file format
 *
 */
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/**
 * @brief readConfiguration
 * @param filename
 * @return map<string, string>
 * @brief produces a map associating each key/value pairs. In case of groups [Groupname] each key of the group is prefixed by the group name and a /
 */
std::map<std::string, std::string> readConfiguration(const std::string& filename);

/**
 * @brief saveConfiguration
 * @param keyValuePairs
 * @param filename
 */
void saveConfiguration(const std::map<std::string, std::string>& keyValuePairs, const std::string& filename);

static inline int my_stoi(const std::string& str) {
    try {
        return stoi(str);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << str << std::endl;
        throw;
    }
}