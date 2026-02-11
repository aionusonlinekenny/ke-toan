#pragma once

#include "KeToanApp/Common.h"
#include "KeToanApp/Types.h"

namespace KeToanApp {

    class Config {
    public:
        Config();
        ~Config() = default;

        // Load/Save configuration
        bool Load(const std::string& filename = "config.ini");
        bool Save(const std::string& filename = "config.ini");

        // Getters
        const AppSettings& GetSettings() const { return settings_; }
        AppSettings& GetSettings() { return settings_; }

        // Specific getters
        std::string GetDatabasePath() const { return settings_.databasePath; }
        DatabaseType GetDatabaseType() const { return settings_.dbType; }
        std::string GetLanguage() const { return settings_.language; }

        // Setters
        void SetDatabasePath(const std::string& path) { settings_.databasePath = path; }
        void SetDatabaseType(DatabaseType type) { settings_.dbType = type; }
        void SetLanguage(const std::string& lang) { settings_.language = lang; }

    private:
        AppSettings settings_;
        std::string configPath_;

        // Helper methods
        std::string ReadValue(const std::string& section,
                             const std::string& key,
                             const std::string& defaultValue);
        void WriteValue(const std::string& section,
                       const std::string& key,
                       const std::string& value);
    };

} // namespace KeToanApp
