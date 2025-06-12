#pragma once
#include <string>
#include <unordered_map>
#include <vector>

class KeyValues {
public:
    KeyValues(const std::string& name);
    ~KeyValues();

    void LoadFromFile(const std::string& filepath);
    const std::string& GetString(const std::string& key, const std::string& defaultValue = "") const;

private:
    std::string m_name;
    std::unordered_map<std::string, std::string> m_values;
};