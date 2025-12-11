#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace i18n {

struct LanguageInfo {
    std::string code;
    std::string name;
    std::string nativeName;
};

class Localization {
public:
    static Localization& Instance();

    bool LoadLanguage(const std::string& langCode);

    const std::string& GetCurrentLanguage() const { return m_currentLanguage; }

    const std::vector<LanguageInfo>& GetAvailableLanguages() const { return m_availableLanguages; }

    const char* Get(const std::string& key) const;

    void SetBasePath(const std::string& path) { m_basePath = path; }

private:
    Localization();
    ~Localization() = default;

    void ScanAvailableLanguages();

    std::string m_basePath = "assets/lang";
    std::string m_currentLanguage;
    std::unordered_map<std::string, std::string> m_strings;
    std::vector<LanguageInfo> m_availableLanguages;

    mutable std::string m_fallback;
};

// Convenience function
inline const char* T(const std::string& key) {
    return Localization::Instance().Get(key);
}

// Set language
inline bool SetLanguage(const std::string& langCode) {
    return Localization::Instance().LoadLanguage(langCode);
}

// Get current language
inline const std::string& GetLanguage() {
    return Localization::Instance().GetCurrentLanguage();
}

} // namespace i18n
