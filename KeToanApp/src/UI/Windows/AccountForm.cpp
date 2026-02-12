// Force UTF-8 encoding for Vietnamese text
#pragma execution_character_set("utf-8")

#include "AccountForm.h"
#include "../../Utils/Logger.h"
#include <windowsx.h>
#include <commctrl.h>

namespace KeToanApp {

    const wchar_t* AccountForm::CLASS_NAME = L"KeToanAppAccountForm";

    AccountForm::AccountForm(HWND hParent)
        : hInstance_(GetModuleHandle(nullptr))
        , hParent_(hParent)
        , hwnd_(nullptr)
        , hFont_(nullptr)
        , selectedIndex_(-1)
    {
        hListView_ = nullptr;
        hBtnThem_ = nullptr;
        hBtnSua_ = nullptr;
        hBtnXoa_ = nullptr;
        hBtnDong_ = nullptr;
    }

    AccountForm::~AccountForm() {
        Close();
        if (hFont_) {
            DeleteObject(hFont_);
            hFont_ = nullptr;
        }
    }

    bool AccountForm::Create() {
        // Register window class
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance_;
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = CLASS_NAME;
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        if (!RegisterClassExW(&wc)) {
            DWORD err = GetLastError();
            if (err != ERROR_CLASS_ALREADY_EXISTS) {
                Logger::Error("Failed to register AccountForm window class, error: %d", err);
                return false;
            }
        }

        // Create window
        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            CLASS_NAME,
            L"Danh mục Tài khoản kế toán",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            900, 600,
            hParent_,
            nullptr,
            hInstance_,
            this
        );

        if (!hwnd_) {
            DWORD err = GetLastError();
            Logger::Error("Failed to create AccountForm window, error: %d", err);
            return false;
        }

        return true;
    }

    void AccountForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    void AccountForm::Close() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    LRESULT CALLBACK AccountForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        AccountForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            form = reinterpret_cast<AccountForm*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
            form->hwnd_ = hwnd;
        } else {
            form = reinterpret_cast<AccountForm*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (form) {
            return form->HandleMessage(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT AccountForm::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case WM_CREATE:
                OnCreate();
                return 0;

            case WM_COMMAND:
                OnCommand(wParam, lParam);
                return 0;

            case WM_NOTIFY:
                OnNotify(lParam);
                return 0;

            case WM_SIZE:
                OnSize(wParam, lParam);
                return 0;

            case WM_DESTROY:
                hwnd_ = nullptr;
                return 0;

            case WM_CLOSE:
                OnClose();
                return 0;

            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }

    void AccountForm::OnCreate() {
        Logger::Info("AccountForm initializing...");

        // Create font for Vietnamese text
        hFont_ = CreateFontW(
            -16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );

        // Create all controls
        CreateControls();
        CreateListView();

        // Load sample data
        LoadAccounts();
        RefreshListView();

        // Center window on parent
        RECT rcParent, rcWindow;
        GetWindowRect(hParent_, &rcParent);
        GetWindowRect(hwnd_, &rcWindow);

        int x = rcParent.left + (rcParent.right - rcParent.left - (rcWindow.right - rcWindow.left)) / 2;
        int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcWindow.bottom - rcWindow.top)) / 2;
        SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        Logger::Info("AccountForm initialized successfully");
    }

    void AccountForm::CreateControls() {
        HINSTANCE hInst = GetModuleHandle(nullptr);
        int yPos = 10;

        // === TOOLBAR BUTTONS ===
        hBtnThem_ = CreateWindowW(L"BUTTON", L"Thêm",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            10, yPos, 80, 30, hwnd_, (HMENU)IDC_BTN_THEM, hInst, nullptr);

        hBtnSua_ = CreateWindowW(L"BUTTON", L"Sửa",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            100, yPos, 80, 30, hwnd_, (HMENU)IDC_BTN_SUA, hInst, nullptr);

        hBtnXoa_ = CreateWindowW(L"BUTTON", L"Xóa",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            190, yPos, 80, 30, hwnd_, (HMENU)IDC_BTN_XOA, hInst, nullptr);

        hBtnDong_ = CreateWindowW(L"BUTTON", L"Đóng",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            280, yPos, 80, 30, hwnd_, (HMENU)IDC_BTN_DONG, hInst, nullptr);

        // Apply font to all controls
        if (hFont_) {
            EnumChildWindows(hwnd_, [](HWND hwnd, LPARAM lParam) -> BOOL {
                SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
                return TRUE;
            }, (LPARAM)hFont_);
        }
    }

    void AccountForm::CreateListView() {
        HINSTANCE hInst = GetModuleHandle(nullptr);

        // Create ListView below buttons
        hListView_ = CreateWindowExW(
            0,
            WC_LISTVIEWW,
            L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL |
            LVS_SHOWSELALWAYS | WS_BORDER,
            10, 50, 865, 480,
            hwnd_,
            (HMENU)IDC_LISTVIEW,
            hInst,
            nullptr
        );

        // Set extended styles
        ListView_SetExtendedListViewStyle(hListView_,
            LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

        // Apply font
        if (hFont_) {
            SendMessage(hListView_, WM_SETFONT, (WPARAM)hFont_, TRUE);
        }

        SetupListViewColumns();

        // Add status label at bottom
        CreateWindowW(L"STATIC", L"Tổng số: 0 tài khoản",
            WS_CHILD | WS_VISIBLE,
            10, 540, 300, 20, hwnd_, nullptr, hInst, nullptr);
    }

    void AccountForm::SetupListViewColumns() {
        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

        // Mã TK
        lvc.fmt = LVCFMT_LEFT;
        lvc.pszText = const_cast<LPWSTR>(L"Mã TK");
        lvc.cx = 120;
        ListView_InsertColumn(hListView_, 0, &lvc);

        // Tên tài khoản
        lvc.pszText = const_cast<LPWSTR>(L"Tên tài khoản");
        lvc.cx = 300;
        ListView_InsertColumn(hListView_, 1, &lvc);

        // Loại TK
        lvc.pszText = const_cast<LPWSTR>(L"Loại TK");
        lvc.cx = 150;
        ListView_InsertColumn(hListView_, 2, &lvc);

        // Mô tả
        lvc.pszText = const_cast<LPWSTR>(L"Mô tả");
        lvc.cx = 280;
        ListView_InsertColumn(hListView_, 3, &lvc);
    }

    void AccountForm::LoadAccounts() {
        // Sample data
        accounts_.clear();

        Account a1;
        a1.MaTK = L"111";
        a1.TenTK = L"Tiền mặt";
        a1.LoaiTK = L"Tài sản";
        a1.MoTa = L"Tiền mặt tại quỹ";
        accounts_.push_back(a1);

        Account a2;
        a2.MaTK = L"112";
        a2.TenTK = L"Tiền gửi ngân hàng";
        a2.LoaiTK = L"Tài sản";
        a2.MoTa = L"Tiền gửi tại các ngân hàng";
        accounts_.push_back(a2);

        Account a3;
        a3.MaTK = L"331";
        a3.TenTK = L"Phải trả người bán";
        a3.LoaiTK = L"Nợ phải trả";
        a3.MoTa = L"Công nợ phải trả nhà cung cấp";
        accounts_.push_back(a3);

        Account a4;
        a4.MaTK = L"511";
        a4.TenTK = L"Doanh thu bán hàng";
        a4.LoaiTK = L"Doanh thu";
        a4.MoTa = L"Doanh thu từ bán hàng hóa, dịch vụ";
        accounts_.push_back(a4);

        Logger::Info("Loaded %zu accounts", accounts_.size());
    }

    void AccountForm::RefreshListView() {
        ListView_DeleteAllItems(hListView_);

        for (size_t i = 0; i < accounts_.size(); i++) {
            const Account& a = accounts_[i];

            LVITEMW lvi = { 0 };
            lvi.mask = LVIF_TEXT;
            lvi.iItem = static_cast<int>(i);

            // Mã TK
            lvi.iSubItem = 0;
            lvi.pszText = const_cast<LPWSTR>(a.MaTK.c_str());
            ListView_InsertItem(hListView_, &lvi);

            // Tên tài khoản
            lvi.iSubItem = 1;
            lvi.pszText = const_cast<LPWSTR>(a.TenTK.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Loại TK
            lvi.iSubItem = 2;
            lvi.pszText = const_cast<LPWSTR>(a.LoaiTK.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Mô tả
            lvi.iSubItem = 3;
            lvi.pszText = const_cast<LPWSTR>(a.MoTa.c_str());
            ListView_SetItem(hListView_, &lvi);
        }
    }

    void AccountForm::ClearListView() {
        ListView_DeleteAllItems(hListView_);
    }

    void AccountForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        int wmId = LOWORD(wParam);

        switch (wmId) {
            case IDC_BTN_THEM:
                MessageBoxW(hwnd_, L"Chức năng thêm tài khoản đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_SUA:
                MessageBoxW(hwnd_, L"Chức năng sửa tài khoản đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_XOA:
                if (selectedIndex_ >= 0) {
                    const Account& a = accounts_[selectedIndex_];
                    std::wstring msg = L"Bạn có chắc muốn xóa tài khoản \"" + a.TenTK + L"\"?";

                    int result = MessageBoxW(hwnd_, msg.c_str(), L"Xác nhận xóa", MB_YESNO | MB_ICONQUESTION);
                    if (result == IDYES) {
                        accounts_.erase(accounts_.begin() + selectedIndex_);
                        selectedIndex_ = -1;
                        RefreshListView();
                        MessageBoxW(hwnd_, L"Đã xóa tài khoản thành công!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                    }
                } else {
                    MessageBoxW(hwnd_, L"Vui lòng chọn tài khoản cần xóa!", L"Thông báo", MB_OK | MB_ICONWARNING);
                }
                break;

            case IDC_BTN_DONG:
                DestroyWindow(hwnd_);
                break;

            case IDCANCEL:
                OnClose();
                break;
        }
    }

    void AccountForm::OnNotify(LPARAM lParam) {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);

        if (nmhdr->hwndFrom == hListView_) {
            if (nmhdr->code == LVN_ITEMCHANGED) {
                LPNMLISTVIEW pnmv = reinterpret_cast<LPNMLISTVIEW>(lParam);
                if (pnmv->uNewState & LVIS_SELECTED) {
                    selectedIndex_ = pnmv->iItem;
                }
            }
        }
    }

    void AccountForm::OnSize(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(wParam);
        KETOAN_UNUSED(lParam);
        // Size handling if needed in future
    }

    void AccountForm::OnClose() {
        DestroyWindow(hwnd_);
    }

} // namespace KeToanApp
