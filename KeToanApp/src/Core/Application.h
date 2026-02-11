#pragma once

#include "KeToanApp/Common.h"
#include "KeToanApp/Types.h"
#include "Config.h"
#include "../Database/DatabaseManager.h"
#include "../UI/MainWindow.h"

namespace KeToanApp {

    class Application {
    public:
        explicit Application(HINSTANCE hInstance);
        ~Application();

        // Non-copyable
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        // Initialize application
        bool Initialize();

        // Run message loop
        int Run(int nCmdShow);

        // Shutdown application
        void Shutdown();

        // Getters
        HINSTANCE GetHInstance() const { return hInstance_; }
        Config& GetConfig() { return config_; }
        DatabaseManager& GetDatabase() { return *database_; }
        MainWindow* GetMainWindow() { return mainWindow_.get(); }

        // Singleton access
        static Application* GetInstance() { return instance_; }

    private:
        static Application* instance_;

        HINSTANCE hInstance_;
        Config config_;
        std::unique_ptr<DatabaseManager> database_;
        std::unique_ptr<MainWindow> mainWindow_;
        bool initialized_;

        // Initialize components
        bool InitializeCommonControls();
        bool InitializeDatabase();
        bool InitializeMainWindow();
    };

} // namespace KeToanApp
