// Force UTF-8 encoding for Vietnamese text
#pragma execution_character_set("utf-8")

#include "CustomerForm.h"
#include "../../Utils/Logger.h"
#include <windowsx.h>
#include <commctrl.h>

namespace KeToanApp {

    const wchar_t* CustomerForm::CLASS_NAME = L"KeToanAppCustomerForm";

    CustomerForm::CustomerForm(HWND hParent)
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

    CustomerForm::~CustomerForm() {
        Close();
        if (hFont_) {
            DeleteObject(hFont_);
            hFont_ = nullptr;
        }
    }

    bool CustomerForm::Create() {
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
                Logger::Error("Failed to register CustomerForm window class, error: %d", err);
                return false;
            }
        }

        // Create window
        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            CLASS_NAME,
            L"Danh mục Khách hàng",
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
            Logger::Error("Failed to create CustomerForm window, error: %d", err);
            return false;
        }

        return true;
    }

    void CustomerForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    void CustomerForm::Close() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    LRESULT CALLBACK CustomerForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        CustomerForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            form = reinterpret_cast<CustomerForm*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
            form->hwnd_ = hwnd;
        } else {
            form = reinterpret_cast<CustomerForm*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (form) {
            return form->HandleMessage(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT CustomerForm::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

    void CustomerForm::OnCreate() {
        Logger::Info("CustomerForm initializing...");

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
        LoadCustomers();
        RefreshListView();

        // Center window on parent
        RECT rcParent, rcWindow;
        GetWindowRect(hParent_, &rcParent);
        GetWindowRect(hwnd_, &rcWindow);

        int x = rcParent.left + (rcParent.right - rcParent.left - (rcWindow.right - rcWindow.left)) / 2;
        int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcWindow.bottom - rcWindow.top)) / 2;
        SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        Logger::Info("CustomerForm initialized successfully");
    }

    void CustomerForm::CreateControls() {
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

    void CustomerForm::CreateListView() {
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
        CreateWindowW(L"STATIC", L"Tổng số: 0 khách hàng",
            WS_CHILD | WS_VISIBLE,
            10, 540, 300, 20, hwnd_, nullptr, hInst, nullptr);
    }

    void CustomerForm::SetupListViewColumns() {
        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

        // Mã KH
        lvc.fmt = LVCFMT_LEFT;
        lvc.pszText = const_cast<LPWSTR>(L"Mã KH");
        lvc.cx = 100;
        ListView_InsertColumn(hListView_, 0, &lvc);

        // Tên khách hàng
        lvc.pszText = const_cast<LPWSTR>(L"Tên khách hàng");
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

    void CustomerForm::LoadCustomers() {
        // Sample data
        customers_.clear();

        Customer c1;
        c1.MaKH = L"KH001";
        c1.TenKH = L"Công ty TNHH ABC";
        c1.SoDT = L"0901234567";
        c1.DiaChi = L"123 Nguyễn Huệ, Q.1, TP.HCM";
        customers_.push_back(c1);

        Customer c2;
        c2.MaKH = L"KH002";
        c2.TenKH = L"Công ty CP XYZ";
        c2.SoDT = L"0912345678";
        c2.DiaChi = L"456 Lê Lợi, Q.3, TP.HCM";
        customers_.push_back(c2);

        Customer c3;
        c3.MaKH = L"KH003";
        c3.TenKH = L"Cửa hàng Minh Anh";
        c3.SoDT = L"0923456789";
        c3.DiaChi = L"789 Trần Hưng Đạo, Q.5, TP.HCM";
        customers_.push_back(c3);

        Logger::Info("Loaded %zu customers", customers_.size());
    }

    void CustomerForm::RefreshListView() {
        ListView_DeleteAllItems(hListView_);

        for (size_t i = 0; i < customers_.size(); i++) {
            const Customer& c = customers_[i];

            LVITEMW lvi = { 0 };
            lvi.mask = LVIF_TEXT;
            lvi.iItem = static_cast<int>(i);

            // Mã KH
            lvi.iSubItem = 0;
            lvi.pszText = const_cast<LPWSTR>(c.MaKH.c_str());
            ListView_InsertItem(hListView_, &lvi);

            // Tên khách hàng
            lvi.iSubItem = 1;
            lvi.pszText = const_cast<LPWSTR>(c.TenKH.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Số điện thoại
            lvi.iSubItem = 2;
            lvi.pszText = const_cast<LPWSTR>(c.SoDT.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Địa chỉ
            lvi.iSubItem = 3;
            lvi.pszText = const_cast<LPWSTR>(c.DiaChi.c_str());
            ListView_SetItem(hListView_, &lvi);
        }
    }

    void CustomerForm::ClearListView() {
        ListView_DeleteAllItems(hListView_);
    }

    void CustomerForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        int wmId = LOWORD(wParam);

        switch (wmId) {
            case IDC_BTN_THEM:
                MessageBoxW(hwnd_, L"Chức năng thêm khách hàng đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_SUA:
                MessageBoxW(hwnd_, L"Chức năng sửa khách hàng đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;

            case IDC_BTN_XOA:
                if (selectedIndex_ >= 0) {
                    const Customer& c = customers_[selectedIndex_];
                    std::wstring msg = L"Bạn có chắc muốn xóa khách hàng \"" + c.TenKH + L"\"?";

                    int result = MessageBoxW(hwnd_, msg.c_str(), L"Xác nhận xóa", MB_YESNO | MB_ICONQUESTION);
                    if (result == IDYES) {
                        customers_.erase(customers_.begin() + selectedIndex_);
                        selectedIndex_ = -1;
                        RefreshListView();
                        MessageBoxW(hwnd_, L"Đã xóa khách hàng thành công!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                    }
                } else {
                    MessageBoxW(hwnd_, L"Vui lòng chọn khách hàng cần xóa!", L"Thông báo", MB_OK | MB_ICONWARNING);
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

    void CustomerForm::OnNotify(LPARAM lParam) {
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

    void CustomerForm::OnSize(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(wParam);
        KETOAN_UNUSED(lParam);
        // Size handling if needed in future
    }

    void CustomerForm::OnClose() {
        DestroyWindow(hwnd_);
    }

} // namespace KeToanApp
