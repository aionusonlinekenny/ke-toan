#include "Config.h"
#include "../Utils/Logger.h"
#include <fstream>
#include <sstream>

namespace KeToanApp {

    Config::Config()
        : settings_()
        , configPath_("config.ini")
    {
    }

    bool Config::Load(const std::string& filename) {
        configPath_ = filename;

        std::ifstream file(filename);
        if (!file.is_open()) {
            Logger::Warning("Config file not found: %s, using defaults", filename.c_str());
            return false;
        }

        // Simple INI parser
        std::string line;
        std::string currentSection;

        while (std::getline(file, line)) {
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Skip empty lines and comments
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }

            // Section header
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                currentSection = line.substr(1, line.length() - 2);
                continue;
            }

            // Key-value pair
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                // Trim key and value
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                // Parse based on section
                if (currentSection == "Database") {
                    if (key == "Type") {
                        if (value == "SQLite") {
                            settings_.dbType = DatabaseType::SQLite;
                        } else if (value == "SQLServer") {
                            settings_.dbType = DatabaseType::SQLServer;
                        }
                    } else if (key == "Path") {
                        settings_.databasePath = value;
                    }
                } else if (currentSection == "Application") {
                    if (key == "Language") {
                        settings_.language = value;
                    } else if (key == "DateFormat") {
                        settings_.dateFormat = value;
                    } else if (key == "NumberPrecision") {
                        settings_.numberPrecision = std::stoi(value);
                    }
                }
            }
        }

        file.close();
        Logger::Info("Configuration loaded from: %s", filename.c_str());
        return true;
    }

    bool Config::Save(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::Error("Failed to save config file: %s", filename.c_str());
            return false;
        }

        // Write Database section
        file << "[Database]\n";
        file << "Type=" << (settings_.dbType == DatabaseType::SQLite ? "SQLite" : "SQLServer") << "\n";
        file << "Path=" << settings_.databasePath << "\n";
        file << "\n";

        // Write Application section
        file << "[Application]\n";
        file << "Language=" << settings_.language << "\n";
        file << "DateFormat=" << settings_.dateFormat << "\n";
        file << "NumberPrecision=" << settings_.numberPrecision << "\n";

        file.close();
        Logger::Info("Configuration saved to: %s", filename.c_str());
        return true;
    }

} // namespace KeToanApp
