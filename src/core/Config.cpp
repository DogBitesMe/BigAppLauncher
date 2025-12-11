#include "Config.h"
#include <fstream>

Config& Config::Instance() {
    static Config instance;
    return instance;
}

bool Config::Load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;

        if (j.contains("api") && j["api"].contains("baseUrl")) {
            m_apiBaseUrl = j["api"]["baseUrl"].get<std::string>();
        }

        if (j.contains("ui")) {
            if (j["ui"].contains("language")) {
                m_language = j["ui"]["language"].get<std::string>();
            }
            if (j["ui"].contains("scale")) {
                m_uiScale = j["ui"]["scale"].get<float>();
            }
        }

        if (j.contains("video") && j["video"].contains("loginBackground")) {
            m_loginVideoPath = j["video"]["loginBackground"].get<std::string>();
        }

        if (j.contains("updates") && j["updates"].contains("checkOnStartup")) {
            m_checkUpdatesOnStartup = j["updates"]["checkOnStartup"].get<bool>();
        }

        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool Config::Save(const std::string& path) {
    nlohmann::json j;

    j["api"]["baseUrl"] = m_apiBaseUrl;

    j["ui"]["language"] = m_language;
    j["ui"]["scale"] = m_uiScale;

    j["video"]["loginBackground"] = m_loginVideoPath;

    j["updates"]["checkOnStartup"] = m_checkUpdatesOnStartup;

    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    file << j.dump(2);
    return true;
}
