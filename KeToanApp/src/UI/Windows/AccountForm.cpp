#include "AccountForm.h"
#include <windowsx.h>
#include <commctrl.h>

namespace KeToanApp {

    AccountForm::AccountForm(HWND parent)
        : parent_(parent)
        , hwnd_(nullptr)
        , listView_(nullptr)
        , btnAdd_(nullptr)
        , btnEdit_(nullptr)
        , btnDelete_(nullptr)
        , btnClose_(nullptr) {
    }

    AccountForm::~AccountForm() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
    }

    bool AccountForm::Create() {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"KeToanAccountForm";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClassExW(&wc);

        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            L"KeToanAccountForm",
            L"Danh mục Tài khoản Kế toán",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 900, 500,
            parent_,
            nullptr,
            GetModuleHandle(nullptr),
            this
        );

        return hwnd_ != nullptr;
    }

    void AccountForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    LRESULT CALLBACK AccountForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        AccountForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            form = static_cast<AccountForm*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
        } else {
            form = reinterpret_cast<AccountForm*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
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

    void AccountForm::OnCreate() {
        InitializeControls();
        LoadAccounts();
    }

    void AccountForm::OnDestroy() {
        hwnd_ = nullptr;
    }

    void AccountForm::OnSize(int width, int height) {
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

    void AccountForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        switch (LOWORD(wParam)) {
            case ID_BTN_ADD:
                MessageBoxW(hwnd_, L"Chức năng thêm tài khoản đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;
            case ID_BTN_EDIT:
                MessageBoxW(hwnd_, L"Chức năng sửa tài khoản đang phát triển", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                break;
            case ID_BTN_DELETE:
                if (MessageBoxW(hwnd_, L"Bạn có chắc muốn xóa tài khoản này?", L"Xác nhận", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    MessageBoxW(hwnd_, L"Đã xóa tài khoản", L"Thông báo", MB_OK | MB_ICONINFORMATION);
                }
                break;
            case ID_BTN_CLOSE:
                DestroyWindow(hwnd_);
                break;
        }
    }

    void AccountForm::OnNotify(NMHDR* nmhdr) {
        KETOAN_UNUSED(nmhdr);
    }

    void AccountForm::InitializeControls() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);

        // ListView
        listView_ = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            WC_LISTVIEW,
            L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            10, 10, 860, 380,
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

    void AccountForm::InitializeListView() {
        LVCOLUMNW col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
        col.fmt = LVCFMT_LEFT;

        col.pszText = const_cast<LPWSTR>(L"Số TK");
        col.cx = 100;
        ListView_InsertColumn(listView_, 0, &col);

        col.pszText = const_cast<LPWSTR>(L"Tên tài khoản");
        col.cx = 250;
        ListView_InsertColumn(listView_, 1, &col);

        col.pszText = const_cast<LPWSTR>(L"Loại TK");
        col.cx = 150;
        ListView_InsertColumn(listView_, 2, &col);

        col.pszText = const_cast<LPWSTR>(L"Diễn giải");
        col.cx = 340;
        ListView_InsertColumn(listView_, 3, &col);
    }

    void AccountForm::LoadAccounts() {
        // Sample data - Danh mục tài khoản kế toán theo hệ thống Việt Nam
        AddAccountToList(0, L"111", L"Tiền mặt", L"Tài sản", L"Tiền Việt Nam, ngoại tệ");
        AddAccountToList(1, L"112", L"Tiền gửi ngân hàng", L"Tài sản", L"Tiền gửi các tài khoản ngân hàng");
        AddAccountToList(2, L"131", L"Phải thu khách hàng", L"Tài sản", L"Các khoản phải thu của khách hàng");
        AddAccountToList(3, L"152", L"Nguyên liệu, vật liệu", L"Tài sản", L"Nguyên vật liệu tồn kho");
        AddAccountToList(4, L"156", L"Hàng hóa", L"Tài sản", L"Hàng hóa tồn kho");
        AddAccountToList(5, L"331", L"Phải trả người bán", L"Nợ phải trả", L"Các khoản phải trả nhà cung cấp");
        AddAccountToList(6, L"511", L"Doanh thu bán hàng", L"Doanh thu", L"Doanh thu từ bán hàng hóa");
        AddAccountToList(7, L"632", L"Giá vốn hàng bán", L"Chi phí", L"Giá vốn của hàng hóa đã bán");
    }

    void AccountForm::AddAccountToList(int index, const std::wstring& code, const std::wstring& name,
                                       const std::wstring& type, const std::wstring& description) {
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = index;

        item.iSubItem = 0;
        item.pszText = const_cast<LPWSTR>(code.c_str());
        ListView_InsertItem(listView_, &item);

        ListView_SetItemText(listView_, index, 1, const_cast<LPWSTR>(name.c_str()));
        ListView_SetItemText(listView_, index, 2, const_cast<LPWSTR>(type.c_str()));
        ListView_SetItemText(listView_, index, 3, const_cast<LPWSTR>(description.c_str()));
    }

} // namespace KeToanApp
