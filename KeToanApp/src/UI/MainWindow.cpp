#include "MainWindow.h"
#include "../Utils/Logger.h"

namespace KeToanApp {

    const wchar_t* MainWindow::CLASS_NAME = L"KeToanAppMainWindow";

    MainWindow::MainWindow(HINSTANCE hInstance)
        : hInstance_(hInstance)
        , hwnd_(nullptr)
        , hMenu_(nullptr)
        , hStatusBar_(nullptr)
    {
    }

    MainWindow::~MainWindow() {
        Close();
    }

    bool MainWindow::Create() {
        // Register window class
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance_;
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = CLASS_NAME;
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        if (!RegisterClassExW(&wc)) {
            Logger::Error("Failed to register window class");
            return false;
        }

        // Create window
        hwnd_ = CreateWindowExW(
            0,                              // Extended style
            CLASS_NAME,                     // Class name
            L"Phần Mềm Kế Toán",           // Window title
            WS_OVERLAPPEDWINDOW,           // Window style
            CW_USEDEFAULT, CW_USEDEFAULT,  // Position
            1024, 768,                      // Size
            nullptr,                        // Parent window
            nullptr,                        // Menu
            hInstance_,                     // Instance handle
            this                            // Additional application data
        );

        if (!hwnd_) {
            Logger::Error("Failed to create window");
            return false;
        }

        // Create UI components
        if (!CreateMenuBar()) {
            Logger::Error("Failed to create menu bar");
            return false;
        }

        if (!CreateStatusBar()) {
            Logger::Error("Failed to create status bar");
            return false;
        }

        Logger::Info("Main window created successfully");
        return true;
    }

    void MainWindow::Show(int nCmdShow) {
        if (hwnd_) {
            ShowWindow(hwnd_, nCmdShow);
            UpdateWindow(hwnd_);
        }
    }

    void MainWindow::Close() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        MainWindow* window = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = reinterpret_cast<MainWindow*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        } else {
            window = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (window) {
            return window->HandleMessage(msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT MainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case WM_CREATE:
                OnCreate();
                return 0;

            case WM_DESTROY:
                OnDestroy();
                return 0;

            case WM_SIZE:
                OnSize(wParam, lParam);
                return 0;

            case WM_PAINT:
                OnPaint();
                return 0;

            case WM_COMMAND:
                OnCommand(wParam, lParam);
                return 0;

            default:
                return DefWindowProc(hwnd_, msg, wParam, lParam);
        }
    }

    void MainWindow::OnCreate() {
        Logger::Info("Main window WM_CREATE");
    }

    void MainWindow::OnDestroy() {
        Logger::Info("Main window WM_DESTROY");
        PostQuitMessage(0);
    }

    void MainWindow::OnSize(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(wParam);

        // Resize status bar
        if (hStatusBar_) {
            SendMessage(hStatusBar_, WM_SIZE, 0, 0);
        }

        // TODO: Resize other child controls
        KETOAN_UNUSED(lParam);
    }

    void MainWindow::OnPaint() {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd_, &ps);

        // TODO: Custom painting

        EndPaint(hwnd_, &ps);
    }

    void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        int wmId = LOWORD(wParam);

        switch (wmId) {
            case ID_FILE_EXIT:
                Close();
                break;

            case ID_KHO_SANPHAM:
                MessageBoxW(hwnd_, L"Quản lý sản phẩm", L"Thông báo", MB_OK);
                break;

            case ID_KHO_NHAP:
                MessageBoxW(hwnd_, L"Nhập kho", L"Thông báo", MB_OK);
                break;

            case ID_KHO_XUAT:
                MessageBoxW(hwnd_, L"Xuất kho", L"Thông báo", MB_OK);
                break;

            case ID_KETOAN_TAIKHOAN:
                MessageBoxW(hwnd_, L"Quản lý tài khoản", L"Thông báo", MB_OK);
                break;

            case ID_KETOAN_CHUNGTU:
                MessageBoxW(hwnd_, L"Chứng từ kế toán", L"Thông báo", MB_OK);
                break;

            case ID_HELP_ABOUT:
                MessageBoxW(hwnd_,
                    L"Phần Mềm Kế Toán v1.0\n"
                    L"Migrated from Microsoft Access\n"
                    L"Built with C++ and Win32 API",
                    L"Giới thiệu",
                    MB_OK | MB_ICONINFORMATION);
                break;

            default:
                break;
        }
    }

    bool MainWindow::CreateMenuBar() {
        hMenu_ = CreateMenu();

        // File menu
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenuW(hFileMenu, MF_STRING, ID_FILE_NEW, L"&Mới\tCtrl+N");
        AppendMenuW(hFileMenu, MF_STRING, ID_FILE_OPEN, L"&Mở\tCtrl+O");
        AppendMenuW(hFileMenu, MF_STRING, ID_FILE_SAVE, L"&Lưu\tCtrl+S");
        AppendMenuW(hFileMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"&Thoát\tAlt+F4");
        AppendMenuW(hMenu_, MF_POPUP, (UINT_PTR)hFileMenu, L"&Tập tin");

        // Kho menu
        HMENU hKhoMenu = CreatePopupMenu();
        AppendMenuW(hKhoMenu, MF_STRING, ID_KHO_SANPHAM, L"Danh mục &Sản phẩm");
        AppendMenuW(hKhoMenu, MF_STRING, ID_KHO_NHAP, L"&Nhập kho");
        AppendMenuW(hKhoMenu, MF_STRING, ID_KHO_XUAT, L"&Xuất kho");
        AppendMenuW(hKhoMenu, MF_STRING, ID_KHO_TONKHO, L"&Tồn kho");
        AppendMenuW(hMenu_, MF_POPUP, (UINT_PTR)hKhoMenu, L"&Kho");

        // KeToan menu
        HMENU hKeToanMenu = CreatePopupMenu();
        AppendMenuW(hKeToanMenu, MF_STRING, ID_KETOAN_TAIKHOAN, L"&Tài khoản");
        AppendMenuW(hKeToanMenu, MF_STRING, ID_KETOAN_CHUNGTU, L"&Chứng từ");
        AppendMenuW(hKeToanMenu, MF_STRING, ID_KETOAN_SOCAI, L"&Sổ cái");
        AppendMenuW(hKeToanMenu, MF_STRING, ID_KETOAN_CONGNO, L"&Công nợ");
        AppendMenuW(hMenu_, MF_POPUP, (UINT_PTR)hKeToanMenu, L"&Kế toán");

        // Report menu
        HMENU hReportMenu = CreatePopupMenu();
        AppendMenuW(hReportMenu, MF_STRING, ID_REPORT_TONKHO, L"Báo cáo &Tồn kho");
        AppendMenuW(hReportMenu, MF_STRING, ID_REPORT_XUATNHAPTON, L"Báo cáo &Xuất nhập tồn");
        AppendMenuW(hReportMenu, MF_STRING, ID_REPORT_DOANHTHU, L"Báo cáo &Doanh thu");
        AppendMenuW(hReportMenu, MF_STRING, ID_REPORT_CONGNO, L"Báo cáo &Công nợ");
        AppendMenuW(hMenu_, MF_POPUP, (UINT_PTR)hReportMenu, L"&Báo cáo");

        // Help menu
        HMENU hHelpMenu = CreatePopupMenu();
        AppendMenuW(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"&Giới thiệu");
        AppendMenuW(hMenu_, MF_POPUP, (UINT_PTR)hHelpMenu, L"&Trợ giúp");

        SetMenu(hwnd_, hMenu_);
        return true;
    }

    bool MainWindow::CreateStatusBar() {
        hStatusBar_ = CreateWindowExW(
            0,
            STATUSCLASSNAMEW,
            nullptr,
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
            0, 0, 0, 0,
            hwnd_,
            nullptr,
            hInstance_,
            nullptr
        );

        if (!hStatusBar_) {
            return false;
        }

        // Set status bar text
        SendMessageW(hStatusBar_, SB_SETTEXTW, 0, (LPARAM)L"Sẵn sàng");

        return true;
    }

    bool MainWindow::CreateToolBar() {
        // TODO: Implement toolbar
        return true;
    }

} // namespace KeToanApp
