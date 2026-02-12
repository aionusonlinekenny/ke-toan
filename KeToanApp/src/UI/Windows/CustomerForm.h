#pragma once

#include "KeToanApp/Common.h"
#include <vector>
#include <string>

namespace KeToanApp {

    // Customer data structure
    struct Customer {
        std::wstring MaKH;      // Customer code
        std::wstring TenKH;     // Customer name
        std::wstring SoDT;      // Phone number
        std::wstring DiaChi;    // Address

        Customer() = default;
    };

    class CustomerForm {
    public:
        explicit CustomerForm(HWND hParent);
        ~CustomerForm();

        bool Create();
        void Show();
        void Close();

    private:
        HINSTANCE hInstance_;
        HWND hParent_;
        HWND hwnd_;
        HFONT hFont_;

        static const wchar_t* CLASS_NAME;

        // Controls
        HWND hListView_;
        HWND hBtnThem_;
        HWND hBtnSua_;
        HWND hBtnXoa_;
        HWND hBtnDong_;

        // Data
        std::vector<Customer> customers_;
        int selectedIndex_;

        // Window procedure
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Message handlers
        void OnCreate();
        void OnCommand(WPARAM wParam, LPARAM lParam);
        void OnNotify(LPARAM lParam);
        void OnSize(WPARAM wParam, LPARAM lParam);
        void OnClose();

        // UI creation
        void CreateControls();
        void CreateListView();

        // Data management
        void LoadCustomers();
        void RefreshListView();
        void ClearListView();

        // Control IDs
        enum {
            IDC_LISTVIEW = 1001,
            IDC_BTN_THEM = 1002,
            IDC_BTN_SUA = 1003,
            IDC_BTN_XOA = 1004,
            IDC_BTN_DONG = 1005
        };
    };

} // namespace KeToanApp
