#include "Localization.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace i18n {

// Local helper function to parse nested JSON
static void ParseNestedJsonHelper(const std::string& prefix, const nlohmann::json& obj,
                                   std::unordered_map<std::string, std::string>& strings) {
    for (auto& [key, value] : obj.items()) {
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;

        if (value.is_string()) {
            strings[fullKey] = value.get<std::string>();
        }
        else if (value.is_object()) {
            ParseNestedJsonHelper(fullKey, value, strings);
        }
    }
}

Localization& Localization::Instance() {
    static Localization instance;
    return instance;
}

Localization::Localization() {
    m_availableLanguages = {
        {"zh-CN", "Chinese (Simplified)", "\xe7\xae\x80\xe4\xbd\x93\xe4\xb8\xad\xe6\x96\x87"},
        {"zh-TW", "Chinese (Traditional)", "\xe7\xb9\x81\xe9\xab\x94\xe4\xb8\xad\xe6\x96\x87"},
        {"en-US", "English", "English"}
    };
}

bool Localization::LoadLanguage(const std::string& langCode) {
    std::string filePath = m_basePath + "/" + langCode + ".json";

    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;

        m_strings.clear();
        ParseNestedJsonHelper("", j, m_strings);

        m_currentLanguage = langCode;
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

const char* Localization::Get(const std::string& key) const {
    auto it = m_strings.find(key);
    if (it != m_strings.end()) {
        return it->second.c_str();
    }

    m_fallback = "[" + key + "]";
    return m_fallback.c_str();
}

void Localization::ScanAvailableLanguages() {
    m_availableLanguages.clear();

    if (!fs::exists(m_basePath)) {
        return;
    }

    for (const auto& entry : fs::directory_iterator(m_basePath)) {
        if (entry.path().extension() == ".json") {
            std::string code = entry.path().stem().string();

            std::ifstream file(entry.path());
            if (file.is_open()) {
                try {
                    nlohmann::json j;
                    file >> j;

                    LanguageInfo info;
                    info.code = code;
                    info.name = j.value("_language_name", code);
                    info.nativeName = j.value("_language_native", code);
                    m_availableLanguages.push_back(info);
                }
                catch (...) {
                    m_availableLanguages.push_back({code, code, code});
                }
            }
        }
    }
}

} // namespace i18n
