// Force UTF-8 encoding for Vietnamese text
#pragma execution_character_set("utf-8")

#include "WarehouseForm.h"
#include "../../Utils/Logger.h"
#include <windowsx.h>
#include <commctrl.h>

namespace KeToanApp {

    const wchar_t* WarehouseForm::CLASS_NAME = L"KeToanAppWarehouseForm";

    WarehouseForm::WarehouseForm(HWND hParent)
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

    WarehouseForm::~WarehouseForm() {
        Close();
        if (hFont_) {
            DeleteObject(hFont_);
            hFont_ = nullptr;
        }
    }

    bool WarehouseForm::Create() {
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
                Logger::Error("Failed to register WarehouseForm window class, error: %d", err);
                return false;
            }
        }

        // Create window
        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            CLASS_NAME,
            L"Danh mục Kho",
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
            Logger::Error("Failed to create WarehouseForm window, error: %d", err);
            return false;
        }

        return true;
    }

    void WarehouseForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    void WarehouseForm::Close() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    LRESULT CALLBACK WarehouseForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        WarehouseForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            form = reinterpret_cast<WarehouseForm*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
            form->hwnd_ = hwnd;
        } else {
            form = reinterpret_cast<WarehouseForm*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (form) {
            return form->HandleMessage(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT WarehouseForm::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

    void WarehouseForm::OnCreate() {
        Logger::Info("WarehouseForm initializing...");

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
        LoadWarehouses();
        RefreshListView();

        // Center window on parent
        RECT rcParent, rcWindow;
        GetWindowRect(hParent_, &rcParent);
        GetWindowRect(hwnd_, &rcWindow);

        int x = rcParent.left + (rcParent.right - rcParent.left - (rcWindow.right - rcWindow.left)) / 2;
        int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcWindow.bottom - rcWindow.top)) / 2;
        SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        Logger::Info("WarehouseForm initialized successfully");
    }

    void WarehouseForm::CreateControls() {
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

    void WarehouseForm::CreateListView() {
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
        CreateWindowW(L"STATIC", L"Tổng số: 0 kho",
            WS_CHILD | WS_VISIBLE,
            10, 540, 300, 20, hwnd_, nullptr, hInst, nullptr);
    }

    void WarehouseForm::SetupListViewColumns() {
        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

        // Mã kho
        lvc.fmt = LVCFMT_LEFT;
        lvc.pszText = const_cast<LPWSTR>(L"Mã kho");
        lvc.cx = 100;
        ListView_InsertColumn(hListView_, 0, &lvc);

        // Tên kho
        lvc.pszText = const_cast<LPWSTR>(L"Tên kho");
        lvc.cx = 200;
        ListView_InsertColumn(hListView_, 1, &lvc);

        // Địa chỉ
        lvc.pszText = const_cast<LPWSTR>(L"Địa chỉ");
        lvc.cx = 300;
        ListView_InsertColumn(hListView_, 2, &lvc);

        // Quản lý
        lvc.pszText = const_cast<LPWSTR>(L"Quản lý");
        lvc.cx = 250;
        ListView_InsertColumn(hListView_, 3, &lvc);
    }

    void WarehouseForm::LoadWarehouses() {
        // Sample data
        warehouses_.clear();

        Warehouse w1;
        w1.MaKho = L"KHO01";
        w1.TenKho = L"Kho trung tâm";
        w1.DiaChi = L"12 Nguyễn Trãi, Q.1, TP.HCM";
        w1.QuanLy = L"Nguyễn Văn A";
        warehouses_.push_back(w1);

        Warehouse w2;
        w2.MaKho = L"KHO02";
        w2.TenKho = L"Kho chi nhánh 1";
        w2.DiaChi = L"34 Lê Văn Việt, Q.9, TP.HCM";
        w2.QuanLy = L"Trần Thị B";
        warehouses_.push_back(w2);

        Warehouse w3;
        w3.MaKho = L"KHO03";
        w3.TenKho = L"Kho chi nhánh 2";
        w3.DiaChi = L"56 Quốc lộ 1A, Q.Bình Tân, TP.HCM";
        w3.QuanLy = L"Lê Văn C";
        warehouses_.push_back(w3);

        Logger::Info("Loaded %zu warehouses", warehouses_.size());
    }

    void WarehouseForm::RefreshListView() {
        ListView_DeleteAllItems(hListView_);

        for (size_t i = 0; i < warehouses_.size(); i++) {
            const Warehouse& w = warehouses_[i];

            LVITEMW lvi = { 0 };
            lvi.mask = LVIF_TEXT;
            lvi.iItem = static_cast<int>(i);

            // Mã kho
            lvi.iSubItem = 0;
            lvi.pszText = const_cast<LPWSTR>(w.MaKho.c_str());
            ListView_InsertItem(hListView_, &lvi);

            // Tên kho
            lvi.iSubItem = 1;
            lvi.pszText = const_cast<LPWSTR>(w.TenKho.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Địa chỉ
            lvi.iSubItem = 2;
            lvi.pszText = const_cast<LPWSTR>(w.DiaChi.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Quản lý
            lvi.iSubItem = 3;
            lvi.pszText = const_cast<LPWSTR>(w.QuanLy.c_str());
            ListView_SetItem(hListView_, &lvi);
        }
    }

    void WarehouseForm::ClearListView() {
        ListView_DeleteAllItems(hListView_);
    }

    void WarehouseForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        int wmId = LOWORD(wParam);

        switch (wmId) {
            case IDC_BTN_THEM:
                MessageBoxW(hwnd_, L"Chức năng thêm kho đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_SUA:
                MessageBoxW(hwnd_, L"Chức năng sửa kho đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_XOA:
                if (selectedIndex_ >= 0) {
                    const Warehouse& w = warehouses_[selectedIndex_];
                    std::wstring msg = L"Bạn có chắc muốn xóa kho \"" + w.TenKho + L"\"?";

                    int result = MessageBoxW(hwnd_, msg.c_str(), L"Xác nhận xóa", MB_YESNO | MB_ICONQUESTION);
                    if (result == IDYES) {
                        warehouses_.erase(warehouses_.begin() + selectedIndex_);
                        selectedIndex_ = -1;
                        RefreshListView();
                        MessageBoxW(hwnd_, L"Đã xóa kho thành công!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                    }
                } else {
                    MessageBoxW(hwnd_, L"Vui lòng chọn kho cần xóa!", L"Thông báo", MB_OK | MB_ICONWARNING);
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

    void WarehouseForm::OnNotify(LPARAM lParam) {
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

    void WarehouseForm::OnSize(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(wParam);
        KETOAN_UNUSED(lParam);
        // Size handling if needed in future
    }

    void WarehouseForm::OnClose() {
        DestroyWindow(hwnd_);
    }

} // namespace KeToanApp
