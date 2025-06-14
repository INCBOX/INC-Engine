#include "KeyValues.h"
#include <fstream>
#include <sstream>

KeyValues::KeyValues(const std::string& name) : m_name(name) {}

KeyValues::~KeyValues() {}

void KeyValues::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) {
        auto firstQuote = line.find('"');
        auto secondQuote = line.find('"', firstQuote + 1);
        auto thirdQuote = line.find('"', secondQuote + 1);
        auto fourthQuote = line.find('"', thirdQuote + 1);

        if (firstQuote != std::string::npos && fourthQuote != std::string::npos) {
            std::string key = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
            std::string value = line.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1);
            m_values[key] = value;
        }
    }
}

const std::string& KeyValues::GetString(const std::string& key, const std::string& defaultValue) const {
    auto it = m_values.find(key);
    if (it != m_values.end()) return it->second;
    return defaultValue;
}