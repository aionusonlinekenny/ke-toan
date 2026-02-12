#pragma once

#include "KeToanApp/Common.h"
#include <windows.h>
#include <string>
#include <vector>

namespace KeToanApp {

    /**
     * Form quản lý danh mục tài khoản kế toán
     */
    class AccountForm {
    public:
        explicit AccountForm(HWND parent);
        ~AccountForm();

        bool Create();
        void Show();

        // Prevent copying
        AccountForm(const AccountForm&) = delete;
        AccountForm& operator=(const AccountForm&) = delete;

    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void OnCreate();
        void OnDestroy();
        void OnSize(int width, int height);
        void OnCommand(WPARAM wParam, LPARAM lParam);
        void OnNotify(NMHDR* nmhdr);

        void InitializeControls();
        void InitializeListView();
        void LoadAccounts();
        void AddAccountToList(int index, const std::wstring& code, const std::wstring& name,
                             const std::wstring& type, const std::wstring& description);

        HWND parent_;
        HWND hwnd_;
        HWND listView_;
        HWND btnAdd_;
        HWND btnEdit_;
        HWND btnDelete_;
        HWND btnClose_;

        static constexpr int ID_LISTVIEW = 1001;
        static constexpr int ID_BTN_ADD = 1002;
        static constexpr int ID_BTN_EDIT = 1003;
        static constexpr int ID_BTN_DELETE = 1004;
        static constexpr int ID_BTN_CLOSE = 1005;
    };

} // namespace KeToanApp
