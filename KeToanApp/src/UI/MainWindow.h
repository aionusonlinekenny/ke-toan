#pragma once

#include "KeToanApp/Common.h"

namespace KeToanApp {

    class MainWindow {
    public:
        explicit MainWindow(HINSTANCE hInstance);
        ~MainWindow();

        // Window management
        bool Create();
        void Show(int nCmdShow);
        void Close();

        // Getters
        HWND GetHandle() const { return hwnd_; }

    private:
        HINSTANCE hInstance_;
        HWND hwnd_;
        HMENU hMenu_;
        HWND hStatusBar_;
        HFONT hFont_;

        static const wchar_t* CLASS_NAME;

        // Window procedure
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Message handlers
        void OnCreate();
        void OnDestroy();
        void OnSize(WPARAM wParam, LPARAM lParam);
        void OnPaint();
        void OnCommand(WPARAM wParam, LPARAM lParam);

        // UI creation
        bool CreateMenuBar();
        bool CreateStatusBar();
        bool CreateToolBar();

        // Menu command IDs
        enum MenuCommands {
            ID_FILE_NEW = 1001,
            ID_FILE_OPEN = 1002,
            ID_FILE_SAVE = 1003,
            ID_FILE_EXIT = 1004,

            ID_KHO_SANPHAM = 2001,
            ID_KHO_NHAP = 2002,
            ID_KHO_XUAT = 2003,
            ID_KHO_TONKHO = 2004,

            ID_KETOAN_TAIKHOAN = 3001,
            ID_KETOAN_CHUNGTU = 3002,
            ID_KETOAN_SOCAI = 3003,
            ID_KETOAN_CONGNO = 3004,

            ID_REPORT_TONKHO = 4001,
            ID_REPORT_XUATNHAPTON = 4002,
            ID_REPORT_DOANHTHU = 4003,
            ID_REPORT_CONGNO = 4004,

            ID_HELP_ABOUT = 9001
        };
    };

} // namespace KeToanApp
