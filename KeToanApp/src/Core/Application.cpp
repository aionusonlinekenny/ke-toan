#include "Application.h"
#include "../Utils/Logger.h"

namespace KeToanApp {

    Application* Application::instance_ = nullptr;

    Application::Application(HINSTANCE hInstance)
        : hInstance_(hInstance)
        , config_()
        , database_(nullptr)
        , mainWindow_(nullptr)
        , initialized_(false)
    {
        instance_ = this;
    }

    Application::~Application() {
        Shutdown();
        instance_ = nullptr;
    }

    bool Application::Initialize() {
        if (initialized_) {
            return true;
        }

        Logger::Info("Initializing application components...");

        // Load configuration
        if (!config_.Load()) {
            Logger::Warning("Failed to load config, using defaults");
        }

        // Initialize common controls
        if (!InitializeCommonControls()) {
            Logger::Error("Failed to initialize common controls");
            return false;
        }

        // Initialize database
        if (!InitializeDatabase()) {
            Logger::Error("Failed to initialize database");
            return false;
        }

        // Initialize main window
        if (!InitializeMainWindow()) {
            Logger::Error("Failed to initialize main window");
            return false;
        }

        initialized_ = true;
        Logger::Info("Application initialized successfully");
        return true;
    }

    int Application::Run(int nCmdShow) {
        if (!initialized_) {
            Logger::Error("Cannot run uninitialized application");
            return -1;
        }

        // Show main window
        if (mainWindow_) {
            mainWindow_->Show(nCmdShow);
        }

        // Message loop
        MSG msg = { 0 };
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

    void Application::Shutdown() {
        if (!initialized_) {
            return;
        }

        Logger::Info("Shutting down application...");

        // Cleanup in reverse order
        mainWindow_.reset();
        database_.reset();

        // Save configuration
        config_.Save();

        initialized_ = false;
        Logger::Info("Application shutdown complete");
    }

    bool Application::InitializeCommonControls() {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_LISTVIEW_CLASSES;

        if (!InitCommonControlsEx(&icex)) {
            Logger::Error("InitCommonControlsEx failed");
            return false;
        }

        Logger::Info("Common controls initialized");
        return true;
    }

    bool Application::InitializeDatabase() {
        try {
            database_ = std::make_unique<DatabaseManager>(config_.GetSettings());

            if (!database_->Connect()) {
                Logger::Error("Failed to connect to database");
                return false;
            }

            Logger::Info("Database initialized: %s", config_.GetSettings().databasePath.c_str());
            return true;
        }
        catch (const DatabaseException& e) {
            Logger::Error("Database initialization error: %s", e.what());
            return false;
        }
    }

    bool Application::InitializeMainWindow() {
        try {
            mainWindow_ = std::make_unique<MainWindow>(hInstance_);

            if (!mainWindow_->Create()) {
                Logger::Error("Failed to create main window");
                return false;
            }

            Logger::Info("Main window created");
            return true;
        }
        catch (const std::exception& e) {
            Logger::Error("Main window creation error: %s", e.what());
            return false;
        }
    }

} // namespace KeToanApp
