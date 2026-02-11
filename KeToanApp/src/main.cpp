#include "KeToanApp/Common.h"
#include "Core/Application.h"
#include "Utils/Logger.h"
#include <exception>
#include <system_error>

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
        // Initialize logger
        KeToanApp::Logger::Initialize("ketoan.log");
        KeToanApp::Logger::Info("========================================");
        KeToanApp::Logger::Info("KeToan Application Starting...");
        KeToanApp::Logger::Info("Version: %d.%d.%d",
            KETOANAPP_VERSION_MAJOR,
            KETOANAPP_VERSION_MINOR,
            KETOANAPP_VERSION_PATCH);

        // Create and run application
        KeToanApp::Application app(hInstance);

        if (!app.Initialize()) {
            KeToanApp::Logger::Error("Failed to initialize application");
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
        KeToanApp::Logger::Shutdown();

        return exitCode;
    }
    catch (const std::system_error& e) {
        KeToanApp::Logger::Error("System error [%d]: %s", e.code().value(), e.what());

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
        KeToanApp::Logger::Error("Unhandled exception: %s", e.what());

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
        KeToanApp::Logger::Error("Unknown exception occurred");
        MessageBoxW(nullptr,
            L"Đã xảy ra lỗi không xác định.",
            L"Lỗi",
            MB_ICONERROR);
        return 1;
    }
}
