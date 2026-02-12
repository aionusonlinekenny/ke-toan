// Force UTF-8 encoding for Vietnamese text
#pragma execution_character_set("utf-8")

#include "SupplierForm.h"
#include "../../Utils/Logger.h"
#include <windowsx.h>
#include <commctrl.h>

namespace KeToanApp {

    const wchar_t* SupplierForm::CLASS_NAME = L"KeToanAppSupplierForm";

    SupplierForm::SupplierForm(HWND hParent)
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

    SupplierForm::~SupplierForm() {
        Close();
        if (hFont_) {
            DeleteObject(hFont_);
            hFont_ = nullptr;
        }
    }

    bool SupplierForm::Create() {
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
                Logger::Error("Failed to register SupplierForm window class, error: %d", err);
                return false;
            }
        }

        // Create window
        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            CLASS_NAME,
            L"Danh mục Nhà cung cấp",
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
            Logger::Error("Failed to create SupplierForm window, error: %d", err);
            return false;
        }

        return true;
    }

    void SupplierForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    void SupplierForm::Close() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    LRESULT CALLBACK SupplierForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        SupplierForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            form = reinterpret_cast<SupplierForm*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
            form->hwnd_ = hwnd;
        } else {
            form = reinterpret_cast<SupplierForm*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (form) {
            return form->HandleMessage(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT SupplierForm::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

    void SupplierForm::OnCreate() {
        Logger::Info("SupplierForm initializing...");

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
        LoadSuppliers();
        RefreshListView();

        // Center window on parent
        RECT rcParent, rcWindow;
        GetWindowRect(hParent_, &rcParent);
        GetWindowRect(hwnd_, &rcWindow);

        int x = rcParent.left + (rcParent.right - rcParent.left - (rcWindow.right - rcWindow.left)) / 2;
        int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcWindow.bottom - rcWindow.top)) / 2;
        SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        Logger::Info("SupplierForm initialized successfully");
    }

    void SupplierForm::CreateControls() {
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

    void SupplierForm::CreateListView() {
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
        CreateWindowW(L"STATIC", L"Tổng số: 0 nhà cung cấp",
            WS_CHILD | WS_VISIBLE,
            10, 540, 300, 20, hwnd_, nullptr, hInst, nullptr);
    }

    void SupplierForm::SetupListViewColumns() {
        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

        // Mã NCC
        lvc.fmt = LVCFMT_LEFT;
        lvc.pszText = const_cast<LPWSTR>(L"Mã NCC");
        lvc.cx = 100;
        ListView_InsertColumn(hListView_, 0, &lvc);

        // Tên nhà cung cấp
        lvc.pszText = const_cast<LPWSTR>(L"Tên nhà cung cấp");
        lvc.cx = 300;
        ListView_InsertColumn(hListView_, 1, &lvc);

        // Số điện thoại
        lvc.pszText = const_cast<LPWSTR>(L"Số điện thoại");
        lvc.cx = 150;
        ListView_InsertColumn(hListView_, 2, &lvc);

        // Địa chỉ
        lvc.pszText = const_cast<LPWSTR>(L"Địa chỉ");
        lvc.cx = 300;
        ListView_InsertColumn(hListView_, 3, &lvc);
    }

    void SupplierForm::LoadSuppliers() {
        // Sample data
        suppliers_.clear();

        Supplier s1;
        s1.MaNCC = L"NCC001";
        s1.TenNCC = L"Công ty Vật tư Xây dựng";
        s1.SoDT = L"0281234567";
        s1.DiaChi = L"12 Lý Thái Tổ, Q.10, TP.HCM";
        suppliers_.push_back(s1);

        Supplier s2;
        s2.MaNCC = L"NCC002";
        s2.TenNCC = L"Nhà phân phối Điện máy";
        s2.SoDT = L"0282345678";
        s2.DiaChi = L"34 Cách Mạng Tháng 8, Q.3, TP.HCM";
        suppliers_.push_back(s2);

        Supplier s3;
        s3.MaNCC = L"NCC003";
        s3.TenNCC = L"Tập đoàn Thương mại";
        s3.SoDT = L"0283456789";
        s3.DiaChi = L"56 Võ Văn Tần, Q.3, TP.HCM";
        suppliers_.push_back(s3);

        Logger::Info("Loaded %zu suppliers", suppliers_.size());
    }

    void SupplierForm::RefreshListView() {
        ListView_DeleteAllItems(hListView_);

        for (size_t i = 0; i < suppliers_.size(); i++) {
            const Supplier& s = suppliers_[i];

            LVITEMW lvi = { 0 };
            lvi.mask = LVIF_TEXT;
            lvi.iItem = static_cast<int>(i);

            // Mã NCC
            lvi.iSubItem = 0;
            lvi.pszText = const_cast<LPWSTR>(s.MaNCC.c_str());
            ListView_InsertItem(hListView_, &lvi);

            // Tên nhà cung cấp
            lvi.iSubItem = 1;
            lvi.pszText = const_cast<LPWSTR>(s.TenNCC.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Số điện thoại
            lvi.iSubItem = 2;
            lvi.pszText = const_cast<LPWSTR>(s.SoDT.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Địa chỉ
            lvi.iSubItem = 3;
            lvi.pszText = const_cast<LPWSTR>(s.DiaChi.c_str());
            ListView_SetItem(hListView_, &lvi);
        }
    }

    void SupplierForm::ClearListView() {
        ListView_DeleteAllItems(hListView_);
    }

    void SupplierForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        int wmId = LOWORD(wParam);

        switch (wmId) {
            case IDC_BTN_THEM:
                MessageBoxW(hwnd_, L"Chức năng thêm nhà cung cấp đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_SUA:
                MessageBoxW(hwnd_, L"Chức năng sửa nhà cung cấp đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_XOA:
                if (selectedIndex_ >= 0) {
                    const Supplier& s = suppliers_[selectedIndex_];
                    std::wstring msg = L"Bạn có chắc muốn xóa nhà cung cấp \"" + s.TenNCC + L"\"?";

                    int result = MessageBoxW(hwnd_, msg.c_str(), L"Xác nhận xóa", MB_YESNO | MB_ICONQUESTION);
                    if (result == IDYES) {
                        suppliers_.erase(suppliers_.begin() + selectedIndex_);
                        selectedIndex_ = -1;
                        RefreshListView();
                        MessageBoxW(hwnd_, L"Đã xóa nhà cung cấp thành công!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                    }
                } else {
                    MessageBoxW(hwnd_, L"Vui lòng chọn nhà cung cấp cần xóa!", L"Thông báo", MB_OK | MB_ICONWARNING);
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

    void SupplierForm::OnNotify(LPARAM lParam) {
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

    void SupplierForm::OnSize(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(wParam);
        KETOAN_UNUSED(lParam);
        // Size handling if needed in future
    }

    void SupplierForm::OnClose() {
        DestroyWindow(hwnd_);
    }

} // namespace KeToanApp
