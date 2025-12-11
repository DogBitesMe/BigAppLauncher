#pragma once

#include <string>
#include <nlohmann/json.hpp>

class Config {
public:
    static Config& Instance();

    bool Load(const std::string& path = "config.json");
    bool Save(const std::string& path = "config.json");

    // API settings
    std::string GetApiBaseUrl() const { return m_apiBaseUrl; }
    void SetApiBaseUrl(const std::string& url) { m_apiBaseUrl = url; }

    // UI settings
    std::string GetLanguage() const { return m_language; }
    void SetLanguage(const std::string& lang) { m_language = lang; }

    float GetUiScale() const { return m_uiScale; }
    void SetUiScale(float scale) { m_uiScale = scale; }

    // Video settings
    std::string GetLoginVideoPath() const { return m_loginVideoPath; }
    void SetLoginVideoPath(const std::string& path) { m_loginVideoPath = path; }

    // Update settings
    bool GetCheckUpdatesOnStartup() const { return m_checkUpdatesOnStartup; }
    void SetCheckUpdatesOnStartup(bool check) { m_checkUpdatesOnStartup = check; }

private:
    Config() = default;
    ~Config() = default;

    std::string m_apiBaseUrl = "https://api.example.com/v1";
    std::string m_language = "zh-CN";
    float m_uiScale = 1.0f;
    std::string m_loginVideoPath = "assets/videos/login_bg.mp4";
    bool m_checkUpdatesOnStartup = true;
};
