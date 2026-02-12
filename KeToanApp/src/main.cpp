#include "KeToanApp/Common.h"
#include "Core/Application.h"
#include "Utils/Logger.h"
#include <exception>
#include <system_error>
#include <iostream>

// Windows entry point
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    KETOAN_UNUSED(hPrevInstance);
    KETOAN_UNUSED(lpCmdLine);

    try {
        // Allocate console for debugging
        #ifdef _DEBUG
        if (AllocConsole()) {
            FILE* pConsole = nullptr;
            freopen_s(&pConsole, "CONOUT$", "w", stdout);
            freopen_s(&pConsole, "CONOUT$", "w", stderr);
            SetConsoleTitleW(L"KeToan Debug Console");
            std::cout << "Debug console initialized" << std::endl;
        }
        #endif

        // Initialize logger
        KeToanApp::Logger::Initialize("ketoan.log");
        KeToanApp::Logger::Info("========================================");
        KeToanApp::Logger::Info("KeToan Application Starting...");
        KeToanApp::Logger::Info("Version: %d.%d.%d",
            KETOANAPP_VERSION_MAJOR,
            KETOANAPP_VERSION_MINOR,
            KETOANAPP_VERSION_PATCH);

        #ifdef _DEBUG
        std::cout << "Creating application..." << std::endl;
        #endif

        // Create and run application
        KeToanApp::Application app(hInstance);

        #ifdef _DEBUG
        std::cout << "Initializing application..." << std::endl;
        #endif

        if (!app.Initialize()) {
            KeToanApp::Logger::Error("Failed to initialize application");

            #ifdef _DEBUG
            std::cerr << "ERROR: Application initialization failed!" << std::endl;
            std::cout << "Press any key to exit..." << std::endl;
            std::cin.get();
            #endif

            MessageBoxW(nullptr,
                L"Không thể khởi động ứng dụng. Vui lòng kiểm tra log file.",
                L"Lỗi Khởi Động",
                MB_ICONERROR);
            return 1;
        }

        KeToanApp::Logger::Info("Application initialized successfully");

        // Run message loop
        int exitCode = app.Run(nCmdShow);

        KeToanApp::Logger::Info("Application exiting with code: %d", exitCode);

        #ifdef _DEBUG
        std::cout << "About to shutdown logger..." << std::endl;
        FreeConsole();
        #endif

        // Shutdown logger last, after all other cleanup
        KeToanApp::Logger::Shutdown();

        #ifdef _DEBUG
        std::cout << "Logger shutdown complete, exiting normally" << std::endl;
        #endif

        return exitCode;
    }
    catch (const std::system_error& e) {
        // Don't log here if logger might be shut down already
        #ifdef _DEBUG
        std::cerr << "SYSTEM ERROR [" << e.code().value() << "]: " << e.what() << std::endl;
        #endif

        std::wstring errorMsg = L"Lỗi hệ thống:\n";
        int wideSize = MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, nullptr, 0);
        if (wideSize > 0) {
            std::wstring wideStr(wideSize, 0);
            MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, &wideStr[0], wideSize);
            errorMsg += wideStr;
        }

        MessageBoxW(nullptr, errorMsg.c_str(), L"Lỗi Hệ Thống", MB_ICONERROR);
        return 1;
    }
    catch (const std::exception& e) {
        // Don't log here if logger might be shut down already
        #ifdef _DEBUG
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        #endif

        std::wstring errorMsg = L"Lỗi nghiêm trọng:\n";
        int wideSize = MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, nullptr, 0);
        if (wideSize > 0) {
            std::wstring wideStr(wideSize, 0);
            MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, &wideStr[0], wideSize);
            errorMsg += wideStr;
        }

        MessageBoxW(nullptr, errorMsg.c_str(), L"Lỗi", MB_ICONERROR);
        return 1;
    }
    catch (...) {
        // Don't log here if logger might be shut down already
        #ifdef _DEBUG
        std::cerr << "UNKNOWN EXCEPTION!" << std::endl;
        #endif

        MessageBoxW(nullptr,
            L"Đã xảy ra lỗi không xác định.",
            L"Lỗi",
            MB_ICONERROR);
        return 1;
    }
}
