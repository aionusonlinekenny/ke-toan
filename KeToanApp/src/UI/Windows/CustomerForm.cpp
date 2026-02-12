#include "CustomerForm.h"
#include <windowsx.h>
#include <commctrl.h>

namespace KeToanApp {

    CustomerForm::CustomerForm(HWND parent)
        : parent_(parent)
        , hwnd_(nullptr)
        , listView_(nullptr)
        , btnAdd_(nullptr)
        , btnEdit_(nullptr)
        , btnDelete_(nullptr)
        , btnClose_(nullptr) {
    }

    CustomerForm::~CustomerForm() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    bool CustomerForm::Create() {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"KeToanCustomerForm";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassExW(&wc);

        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            L"KeToanCustomerForm",
            L"Danh mục Khách hàng",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
            parent_,
            nullptr,
            GetModuleHandle(nullptr),
            this
        );

        return hwnd_ != nullptr;
    }

    void CustomerForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    LRESULT CALLBACK CustomerForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        CustomerForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            form = static_cast<CustomerForm*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
        } else {
            form = reinterpret_cast<CustomerForm*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (form) {
            switch (msg) {
                case WM_CREATE:
                    form->OnCreate();
                    return 0;
                case WM_DESTROY:
                    form->OnDestroy();
                    return 0;
                case WM_SIZE:
                    form->OnSize(LOWORD(lParam), HIWORD(lParam));
                    return 0;
                case WM_COMMAND:
                    form->OnCommand(wParam, lParam);
                    return 0;
                case WM_NOTIFY:
                    form->OnNotify(reinterpret_cast<NMHDR*>(lParam));
                    return 0;
            }
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    void CustomerForm::OnCreate() {
        InitializeControls();
        LoadCustomers();
    }

    void CustomerForm::OnDestroy() {
        hwnd_ = nullptr;
    }

    void CustomerForm::OnSize(int width, int height) {
        if (listView_) {
            SetWindowPos(listView_, nullptr, 10, 10, width - 20, height - 60, SWP_NOZORDER);
        }

        int btnWidth = 80;
        int btnHeight = 30;
        int btnY = height - 45;
        int spacing = 10;

        if (btnClose_) {
            SetWindowPos(btnClose_, nullptr, width - btnWidth - 10, btnY, btnWidth, btnHeight, SWP_NOZORDER);
        }
        if (btnDelete_) {
            SetWindowPos(btnDelete_, nullptr, width - (btnWidth * 2) - spacing - 10, btnY, btnWidth, btnHeight, SWP_NOZORDER);
        }
        if (btnEdit_) {
            SetWindowPos(btnEdit_, nullptr, width - (btnWidth * 3) - (spacing * 2) - 10, btnY, btnWidth, btnHeight, SWP_NOZORDER);
        }
        if (btnAdd_) {
            SetWindowPos(btnAdd_, nullptr, width - (btnWidth * 4) - (spacing * 3) - 10, btnY, btnWidth, btnHeight, SWP_NOZORDER);
        }
    }

    void CustomerForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        switch (LOWORD(wParam)) {
            case ID_BTN_ADD:
                MessageBoxW(hwnd_, L"Chức năng thêm khách hàng đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;
            case ID_BTN_EDIT:
                MessageBoxW(hwnd_, L"Chức năng sửa khách hàng đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;
            case ID_BTN_DELETE:
                if (MessageBoxW(hwnd_, L"Bạn có chắc muốn xóa khách hàng này?", L"Xác nhận", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    MessageBoxW(hwnd_, L"Đã xóa khách hàng", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                }
                break;
            case ID_BTN_CLOSE:
                DestroyWindow(hwnd_);
                break;
        }
    }

    void CustomerForm::OnNotify(NMHDR* nmhdr) {
        KETOAN_UNUSED(nmhdr);
    }

    void CustomerForm::InitializeControls() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);

        // ListView
        listView_ = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            WC_LISTVIEW,
            L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            10, 10, 760, 380,
            hwnd_,
            reinterpret_cast<HMENU>(ID_LISTVIEW),
            hInstance,
            nullptr
        );

        ListView_SetExtendedListViewStyle(listView_, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        InitializeListView();

        // Buttons
        btnAdd_ = CreateWindowW(
            L"BUTTON", L"Thêm",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, 400, 80, 30,
            hwnd_, reinterpret_cast<HMENU>(ID_BTN_ADD), hInstance, nullptr
        );

        btnEdit_ = CreateWindowW(
            L"BUTTON", L"Sửa",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            100, 400, 80, 30,
            hwnd_, reinterpret_cast<HMENU>(ID_BTN_EDIT), hInstance, nullptr
        );

        btnDelete_ = CreateWindowW(
            L"BUTTON", L"Xóa",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            190, 400, 80, 30,
            hwnd_, reinterpret_cast<HMENU>(ID_BTN_DELETE), hInstance, nullptr
        );

        btnClose_ = CreateWindowW(
            L"BUTTON", L"Đóng",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            280, 400, 80, 30,
            hwnd_, reinterpret_cast<HMENU>(ID_BTN_CLOSE), hInstance, nullptr
        );
    }

    void CustomerForm::InitializeListView() {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        col.fmt = LVCFMT_LEFT;

        col.pszText = const_cast<LPWSTR>(L"Mã KH");
        col.cx = 100;
        ListView_InsertColumn(listView_, 0, &col);

        col.pszText = const_cast<LPWSTR>(L"Tên khách hàng");
        col.cx = 250;
        ListView_InsertColumn(listView_, 1, &col);

        col.pszText = const_cast<LPWSTR>(L"Số điện thoại");
        col.cx = 150;
        ListView_InsertColumn(listView_, 2, &col);

        col.pszText = const_cast<LPWSTR>(L"Địa chỉ");
        col.cx = 240;
        ListView_InsertColumn(listView_, 3, &col);
    }

    void CustomerForm::LoadCustomers() {
        // Sample data
        AddCustomerToList(0, L"KH001", L"Công ty TNHH ABC", L"0901234567", L"123 Nguyễn Huệ, Q.1, TP.HCM");
        AddCustomerToList(1, L"KH002", L"Công ty CP XYZ", L"0912345678", L"456 Lê Lợi, Q.3, TP.HCM");
        AddCustomerToList(2, L"KH003", L"Cửa hàng Minh Anh", L"0923456789", L"789 Trần Hưng Đạo, Q.5, TP.HCM");
    }

    void CustomerForm::AddCustomerToList(int index, const std::wstring& code, const std::wstring& name,
                                         const std::wstring& phone, const std::wstring& address) {
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = index;

        item.iSubItem = 0;
        item.pszText = const_cast<LPWSTR>(code.c_str());
        ListView_InsertItem(listView_, &item);

        ListView_SetItemText(listView_, index, 1, const_cast<LPWSTR>(name.c_str()));
        ListView_SetItemText(listView_, index, 2, const_cast<LPWSTR>(phone.c_str()));
        ListView_SetItemText(listView_, index, 3, const_cast<LPWSTR>(address.c_str()));
    }

} // namespace KeToanApp
