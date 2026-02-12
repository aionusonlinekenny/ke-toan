#include "WarehouseForm.h"
#include <windowsx.h>
#include <commctrl.h>

namespace KeToanApp {

    WarehouseForm::WarehouseForm(HWND parent)
        : parent_(parent)
        , hwnd_(nullptr)
        , listView_(nullptr)
        , btnAdd_(nullptr)
        , btnEdit_(nullptr)
        , btnDelete_(nullptr)
        , btnClose_(nullptr) {
    }

    WarehouseForm::~WarehouseForm() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    bool WarehouseForm::Create() {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"KeToanWarehouseForm";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassExW(&wc);

        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            L"KeToanWarehouseForm",
            L"Danh mục Kho",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
            parent_,
            nullptr,
            GetModuleHandle(nullptr),
            this
        );

        return hwnd_ != nullptr;
    }

    void WarehouseForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    LRESULT CALLBACK WarehouseForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        WarehouseForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            form = static_cast<WarehouseForm*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
        } else {
            form = reinterpret_cast<WarehouseForm*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
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

    void WarehouseForm::OnCreate() {
        InitializeControls();
        LoadWarehouses();
    }

    void WarehouseForm::OnDestroy() {
        hwnd_ = nullptr;
    }

    void WarehouseForm::OnSize(int width, int height) {
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

    void WarehouseForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        switch (LOWORD(wParam)) {
            case ID_BTN_ADD:
                MessageBoxW(hwnd_, L"Chức năng thêm kho đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;
            case ID_BTN_EDIT:
                MessageBoxW(hwnd_, L"Chức năng sửa kho đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;
            case ID_BTN_DELETE:
                if (MessageBoxW(hwnd_, L"Bạn có chắc muốn xóa kho này?", L"Xác nhận", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    MessageBoxW(hwnd_, L"Đã xóa kho", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                }
                break;
            case ID_BTN_CLOSE:
                DestroyWindow(hwnd_);
                break;
        }
    }

    void WarehouseForm::OnNotify(NMHDR* nmhdr) {
        KETOAN_UNUSED(nmhdr);
    }

    void WarehouseForm::InitializeControls() {
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

    void WarehouseForm::InitializeListView() {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        col.fmt = LVCFMT_LEFT;

        col.pszText = const_cast<LPWSTR>(L"Mã kho");
        col.cx = 100;
        ListView_InsertColumn(listView_, 0, &col);

        col.pszText = const_cast<LPWSTR>(L"Tên kho");
        col.cx = 200;
        ListView_InsertColumn(listView_, 1, &col);

        col.pszText = const_cast<LPWSTR>(L"Địa chỉ");
        col.cx = 300;
        ListView_InsertColumn(listView_, 2, &col);

        col.pszText = const_cast<LPWSTR>(L"Quản lý");
        col.cx = 140;
        ListView_InsertColumn(listView_, 3, &col);
    }

    void WarehouseForm::LoadWarehouses() {
        // Sample data
        AddWarehouseToList(0, L"KHO01", L"Kho trung tâm", L"12 Nguyễn Trãi, Q.1, TP.HCM", L"Nguyễn Văn A");
        AddWarehouseToList(1, L"KHO02", L"Kho chi nhánh 1", L"34 Lê Văn Việt, Q.9, TP.HCM", L"Trần Thị B");
        AddWarehouseToList(2, L"KHO03", L"Kho chi nhánh 2", L"56 Quốc lộ 1A, Q.Bình Tân, TP.HCM", L"Lê Văn C");
    }

    void WarehouseForm::AddWarehouseToList(int index, const std::wstring& code, const std::wstring& name,
                                           const std::wstring& address, const std::wstring& manager) {
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = index;

        item.iSubItem = 0;
        item.pszText = const_cast<LPWSTR>(code.c_str());
        ListView_InsertItem(listView_, &item);

        ListView_SetItemText(listView_, index, 1, const_cast<LPWSTR>(name.c_str()));
        ListView_SetItemText(listView_, index, 2, const_cast<LPWSTR>(address.c_str()));
        ListView_SetItemText(listView_, index, 3, const_cast<LPWSTR>(manager.c_str()));
    }

} // namespace KeToanApp
