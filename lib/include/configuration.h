#pragma once

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

std::map<std::string, std::string> readConfiguration(const std::string& filename);
void saveConfiguration(const std::map<std::string, std::string>& keyValuePairs, const std::string& filename);
