// Force UTF-8 encoding for Vietnamese text
#pragma execution_character_set("utf-8")

#include "ProductForm.h"
#include "../../Utils/Logger.h"
#include <windowsx.h>
#include <commctrl.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace KeToanApp {

    const wchar_t* ProductForm::CLASS_NAME = L"KeToanAppProductForm";

    ProductForm::ProductForm(HWND hParent)
        : hInstance_(GetModuleHandle(nullptr))
        , hParent_(hParent)
        , hwnd_(nullptr)
        , hFont_(nullptr)
        , selectedIndex_(-1)
        , isEditMode_(false)
    {
        // Initialize controls to nullptr
        hEditMaSP_ = nullptr;
        hEditTenSP_ = nullptr;
        hComboDVT_ = nullptr;
        hComboNhomHang_ = nullptr;
        hEditGiaMua_ = nullptr;
        hEditGiaBan_ = nullptr;
        hEditLaiLo_ = nullptr;
        hEditMoTa_ = nullptr;
        hRadioActive_ = nullptr;
        hRadioInactive_ = nullptr;
        hListView_ = nullptr;
        hEditSearch_ = nullptr;
        hBtnThem_ = nullptr;
        hBtnSua_ = nullptr;
        hBtnXoa_ = nullptr;
        hBtnLuu_ = nullptr;
        hBtnHuy_ = nullptr;
        hBtnTimKiem_ = nullptr;
    }

    ProductForm::~ProductForm() {
        Close();
        if (hFont_) {
            DeleteObject(hFont_);
            hFont_ = nullptr;
        }
    }

    bool ProductForm::Create() {
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
                Logger::Error("Failed to register ProductForm window class, error: %d", err);
                return false;
            }
        }

        // Create window
        hwnd_ = CreateWindowExW(
            WS_EX_DLGMODALFRAME,
            CLASS_NAME,
            L"Danh mục Sản phẩm",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            900, 700,
            hParent_,
            nullptr,
            hInstance_,
            this
        );

        if (!hwnd_) {
            DWORD err = GetLastError();
            Logger::Error("Failed to create ProductForm window, error: %d", err);
            return false;
        }

        return true;
    }

    void ProductForm::Show() {
        if (hwnd_) {
            ShowWindow(hwnd_, SW_SHOW);
            UpdateWindow(hwnd_);
        }
    }

    void ProductForm::Close() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    LRESULT CALLBACK ProductForm::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        ProductForm* form = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            form = reinterpret_cast<ProductForm*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(form));
            form->hwnd_ = hwnd;
        } else {
            form = reinterpret_cast<ProductForm*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (form) {
            return form->HandleMessage(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT ProductForm::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

    void ProductForm::OnCreate() {
        Logger::Info("ProductForm initializing...");

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
        LoadProducts();
        RefreshListView();

        // Center window on parent
        RECT rcParent, rcWindow;
        GetWindowRect(hParent_, &rcParent);
        GetWindowRect(hwnd_, &rcWindow);

        int x = rcParent.left + (rcParent.right - rcParent.left - (rcWindow.right - rcWindow.left)) / 2;
        int y = rcParent.top + (rcParent.bottom - rcParent.top - (rcWindow.bottom - rcWindow.top)) / 2;
        SetWindowPos(hwnd_, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        Logger::Info("ProductForm initialized successfully");
    }

    void ProductForm::CreateControls() {
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

        hBtnLuu_ = CreateWindowW(L"BUTTON", L"Lưu",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            280, yPos, 80, 30, hwnd_, (HMENU)IDC_BTN_LUU, hInst, nullptr);

        hBtnHuy_ = CreateWindowW(L"BUTTON", L"Hủy",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            370, yPos, 80, 30, hwnd_, (HMENU)IDC_BTN_HUY, hInst, nullptr);

        // Search box (right side)
        CreateWindowW(L"STATIC", L"Tìm kiếm:",
            WS_CHILD | WS_VISIBLE,
            550, yPos + 5, 80, 20, hwnd_, nullptr, hInst, nullptr);

        hEditSearch_ = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            635, yPos + 3, 150, 24, hwnd_, (HMENU)IDC_EDIT_SEARCH, hInst, nullptr);

        hBtnTimKiem_ = CreateWindowW(L"BUTTON", L"🔍",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            795, yPos, 80, 30, hwnd_, (HMENU)IDC_BTN_TIMKIEM, hInst, nullptr);

        yPos += 50;

        // === GROUPBOX: THÔNG TIN SẢN PHẨM ===
        HWND hGroupBox = CreateWindowW(L"BUTTON", L"Thông tin sản phẩm",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, yPos, 865, 280, hwnd_, nullptr, hInst, nullptr);

        yPos += 30;

        // Row 1: Mã sản phẩm
        CreateWindowW(L"STATIC", L"Mã sản phẩm (*):",
            WS_CHILD | WS_VISIBLE,
            25, yPos, 150, 20, hwnd_, nullptr, hInst, nullptr);

        hEditMaSP_ = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            180, yPos - 3, 250, 24, hwnd_, (HMENU)IDC_EDIT_MASP, hInst, nullptr);

        CreateWindowW(L"BUTTON", L"Tự động",
            WS_CHILD | WS_VISIBLE,
            440, yPos - 3, 100, 24, hwnd_, nullptr, hInst, nullptr);

        yPos += 35;

        // Row 2: Tên sản phẩm
        CreateWindowW(L"STATIC", L"Tên sản phẩm (*):",
            WS_CHILD | WS_VISIBLE,
            25, yPos, 150, 20, hwnd_, nullptr, hInst, nullptr);

        hEditTenSP_ = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            180, yPos - 3, 680, 24, hwnd_, (HMENU)IDC_EDIT_TENSP, hInst, nullptr);

        yPos += 35;

        // Row 3: Đơn vị tính
        CreateWindowW(L"STATIC", L"Đơn vị tính:",
            WS_CHILD | WS_VISIBLE,
            25, yPos, 150, 20, hwnd_, nullptr, hInst, nullptr);

        hComboDVT_ = CreateWindowW(L"COMBOBOX", L"",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            180, yPos - 3, 250, 200, hwnd_, (HMENU)IDC_COMBO_DVT, hInst, nullptr);

        // Add items to combo
        ComboBox_AddString(hComboDVT_, L"Cái");
        ComboBox_AddString(hComboDVT_, L"Hộp");
        ComboBox_AddString(hComboDVT_, L"Kg");
        ComboBox_AddString(hComboDVT_, L"Mét");
        ComboBox_AddString(hComboDVT_, L"Lít");
        ComboBox_AddString(hComboDVT_, L"Quyển");
        ComboBox_AddString(hComboDVT_, L"Thùng");
        ComboBox_SetCurSel(hComboDVT_, 0);

        yPos += 35;

        // Row 4: Nhóm hàng
        CreateWindowW(L"STATIC", L"Nhóm hàng:",
            WS_CHILD | WS_VISIBLE,
            25, yPos, 150, 20, hwnd_, nullptr, hInst, nullptr);

        hComboNhomHang_ = CreateWindowW(L"COMBOBOX", L"",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            180, yPos - 3, 250, 200, hwnd_, (HMENU)IDC_COMBO_NHOMHANG, hInst, nullptr);

        ComboBox_AddString(hComboNhomHang_, L"Văn phòng phẩm");
        ComboBox_AddString(hComboNhomHang_, L"Nguyên liệu");
        ComboBox_AddString(hComboNhomHang_, L"Thành phẩm");
        ComboBox_AddString(hComboNhomHang_, L"Vật tư");
        ComboBox_AddString(hComboNhomHang_, L"Khác");
        ComboBox_SetCurSel(hComboNhomHang_, 0);

        yPos += 35;

        // === GROUPBOX: GIÁ ===
        HWND hPriceGroup = CreateWindowW(L"BUTTON", L"Giá",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            25, yPos, 520, 100, hwnd_, nullptr, hInst, nullptr);

        // Giá mua
        CreateWindowW(L"STATIC", L"Giá mua:",
            WS_CHILD | WS_VISIBLE,
            40, yPos + 25, 120, 20, hwnd_, nullptr, hInst, nullptr);

        hEditGiaMua_ = CreateWindowW(L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
            155, yPos + 22, 150, 24, hwnd_, (HMENU)IDC_EDIT_GIAMUA, hInst, nullptr);

        CreateWindowW(L"STATIC", L"VNĐ",
            WS_CHILD | WS_VISIBLE,
            315, yPos + 25, 40, 20, hwnd_, nullptr, hInst, nullptr);

        // Giá bán
        CreateWindowW(L"STATIC", L"Giá bán:",
            WS_CHILD | WS_VISIBLE,
            40, yPos + 55, 120, 20, hwnd_, nullptr, hInst, nullptr);

        hEditGiaBan_ = CreateWindowW(L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
            155, yPos + 52, 150, 24, hwnd_, (HMENU)IDC_EDIT_GIABAN, hInst, nullptr);

        CreateWindowW(L"STATIC", L"VNĐ",
            WS_CHILD | WS_VISIBLE,
            315, yPos + 55, 40, 20, hwnd_, nullptr, hInst, nullptr);

        // Lãi/Lỗ
        CreateWindowW(L"STATIC", L"Lãi/lỗ:",
            WS_CHILD | WS_VISIBLE,
            370, yPos + 40, 60, 20, hwnd_, nullptr, hInst, nullptr);

        hEditLaiLo_ = CreateWindowW(L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_READONLY,
            435, yPos + 37, 100, 24, hwnd_, (HMENU)IDC_EDIT_LAILO, hInst, nullptr);

        yPos += 110;

        // Mô tả
        CreateWindowW(L"STATIC", L"Mô tả:",
            WS_CHILD | WS_VISIBLE,
            25, yPos, 150, 20, hwnd_, nullptr, hInst, nullptr);

        hEditMoTa_ = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            180, yPos - 3, 680, 60, hwnd_, (HMENU)IDC_EDIT_MOTA, hInst, nullptr);

        yPos += 70;

        // Trạng thái
        CreateWindowW(L"STATIC", L"Trạng thái:",
            WS_CHILD | WS_VISIBLE,
            25, yPos, 150, 20, hwnd_, nullptr, hInst, nullptr);

        hRadioActive_ = CreateWindowW(L"BUTTON", L"Đang kinh doanh",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
            180, yPos, 180, 20, hwnd_, (HMENU)IDC_RADIO_ACTIVE, hInst, nullptr);

        hRadioInactive_ = CreateWindowW(L"BUTTON", L"Ngừng kinh doanh",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            370, yPos, 180, 20, hwnd_, (HMENU)IDC_RADIO_INACTIVE, hInst, nullptr);

        Button_SetCheck(hRadioActive_, BST_CHECKED);

        // Apply font to all controls
        if (hFont_) {
            EnumChildWindows(hwnd_, [](HWND hwnd, LPARAM lParam) -> BOOL {
                SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
                return TRUE;
            }, (LPARAM)hFont_);
        }

        // Initially disable edit fields
        SetEditEnabled(false);
    }

    void ProductForm::CreateListView() {
        HINSTANCE hInst = GetModuleHandle(nullptr);

        // Create ListView
        hListView_ = CreateWindowExW(
            0,
            WC_LISTVIEWW,
            L"",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL |
            LVS_SHOWSELALWAYS | WS_BORDER,
            10, 350, 865, 280,
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
        CreateWindowW(L"STATIC", L"Tổng số: 0 sản phẩm",
            WS_CHILD | WS_VISIBLE,
            10, 640, 300, 20, hwnd_, nullptr, hInst, nullptr);
    }

    void ProductForm::SetupListViewColumns() {
        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

        // Mã SP
        lvc.fmt = LVCFMT_LEFT;
        lvc.pszText = const_cast<LPWSTR>(L"Mã SP");
        lvc.cx = 100;
        ListView_InsertColumn(hListView_, 0, &lvc);

        // Tên sản phẩm
        lvc.pszText = const_cast<LPWSTR>(L"Tên sản phẩm");
        lvc.cx = 300;
        ListView_InsertColumn(hListView_, 1, &lvc);

        // ĐVT
        lvc.pszText = const_cast<LPWSTR>(L"ĐVT");
        lvc.cx = 80;
        ListView_InsertColumn(hListView_, 2, &lvc);

        // Giá mua
        lvc.fmt = LVCFMT_RIGHT;
        lvc.pszText = const_cast<LPWSTR>(L"Giá mua");
        lvc.cx = 120;
        ListView_InsertColumn(hListView_, 3, &lvc);

        // Giá bán
        lvc.pszText = const_cast<LPWSTR>(L"Giá bán");
        lvc.cx = 120;
        ListView_InsertColumn(hListView_, 4, &lvc);

        // Trạng thái
        lvc.fmt = LVCFMT_CENTER;
        lvc.pszText = const_cast<LPWSTR>(L"TT");
        lvc.cx = 50;
        ListView_InsertColumn(hListView_, 5, &lvc);
    }

    void ProductForm::LoadProducts() {
        // Sample data (TODO: Load from database)
        products_.clear();

        Product p1;
        p1.MaSP = L"SP001";
        p1.TenSP = L"Bút bi xanh Thiên Long";
        p1.DonViTinh = L"Cái";
        p1.NhomHang = L"Văn phòng phẩm";
        p1.GiaMua = 2000;
        p1.GiaBan = 3000;
        p1.MoTa = L"Bút viết văn phòng";
        p1.TrangThai = true;
        products_.push_back(p1);

        Product p2;
        p2.MaSP = L"SP002";
        p2.TenSP = L"Vở 200 trang";
        p2.DonViTinh = L"Quyển";
        p2.NhomHang = L"Văn phòng phẩm";
        p2.GiaMua = 15000;
        p2.GiaBan = 20000;
        p2.MoTa = L"Vở kẻ ngang";
        p2.TrangThai = true;
        products_.push_back(p2);

        Product p3;
        p3.MaSP = L"SP003";
        p3.TenSP = L"Kẹp tài liệu";
        p3.DonViTinh = L"Hộp";
        p3.NhomHang = L"Văn phòng phẩm";
        p3.GiaMua = 25000;
        p3.GiaBan = 35000;
        p3.MoTa = L"Hộp 12 cái";
        p3.TrangThai = true;
        products_.push_back(p3);

        Logger::Info("Loaded %zu products", products_.size());
    }

    void ProductForm::RefreshListView() {
        ListView_DeleteAllItems(hListView_);

        for (size_t i = 0; i < products_.size(); i++) {
            const Product& p = products_[i];

            LVITEMW lvi = { 0 };
            lvi.mask = LVIF_TEXT;
            lvi.iItem = static_cast<int>(i);

            // Mã SP
            lvi.iSubItem = 0;
            lvi.pszText = const_cast<LPWSTR>(p.MaSP.c_str());
            ListView_InsertItem(hListView_, &lvi);

            // Tên sản phẩm
            lvi.iSubItem = 1;
            lvi.pszText = const_cast<LPWSTR>(p.TenSP.c_str());
            ListView_SetItem(hListView_, &lvi);

            // ĐVT
            lvi.iSubItem = 2;
            lvi.pszText = const_cast<LPWSTR>(p.DonViTinh.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Giá mua
            std::wstring giaMua = FormatMoney(p.GiaMua);
            lvi.iSubItem = 3;
            lvi.pszText = const_cast<LPWSTR>(giaMua.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Giá bán
            std::wstring giaBan = FormatMoney(p.GiaBan);
            lvi.iSubItem = 4;
            lvi.pszText = const_cast<LPWSTR>(giaBan.c_str());
            ListView_SetItem(hListView_, &lvi);

            // Trạng thái
            lvi.iSubItem = 5;
            lvi.pszText = const_cast<LPWSTR>(p.TrangThai ? L"✓" : L"");
            ListView_SetItem(hListView_, &lvi);
        }
    }

    std::wstring ProductForm::FormatMoney(double amount) {
        std::wstringstream ss;
        ss.imbue(std::locale(""));
        ss << std::fixed << std::setprecision(0) << amount;
        return ss.str();
    }

    double ProductForm::ParseMoney(const std::wstring& str) {
        std::wstring clean = str;
        clean.erase(std::remove(clean.begin(), clean.end(), L','), clean.end());
        clean.erase(std::remove(clean.begin(), clean.end(), L' '), clean.end());
        return _wtof(clean.c_str());
    }

    void ProductForm::SetEditEnabled(bool enabled) {
        EnableWindow(hEditMaSP_, enabled);
        EnableWindow(hEditTenSP_, enabled);
        EnableWindow(hComboDVT_, enabled);
        EnableWindow(hComboNhomHang_, enabled);
        EnableWindow(hEditGiaMua_, enabled);
        EnableWindow(hEditGiaBan_, enabled);
        EnableWindow(hEditMoTa_, enabled);
        EnableWindow(hRadioActive_, enabled);
        EnableWindow(hRadioInactive_, enabled);

        EnableWindow(hBtnLuu_, enabled);
        EnableWindow(hBtnHuy_, enabled);
    }

    void ProductForm::ClearForm() {
        SetWindowTextW(hEditMaSP_, L"");
        SetWindowTextW(hEditTenSP_, L"");
        ComboBox_SetCurSel(hComboDVT_, 0);
        ComboBox_SetCurSel(hComboNhomHang_, 0);
        SetWindowTextW(hEditGiaMua_, L"0");
        SetWindowTextW(hEditGiaBan_, L"0");
        SetWindowTextW(hEditLaiLo_, L"0");
        SetWindowTextW(hEditMoTa_, L"");
        Button_SetCheck(hRadioActive_, BST_CHECKED);
        Button_SetCheck(hRadioInactive_, BST_UNCHECKED);
    }

    void ProductForm::LoadProductToForm(const Product& product) {
        SetWindowTextW(hEditMaSP_, product.MaSP.c_str());
        SetWindowTextW(hEditTenSP_, product.TenSP.c_str());

        // Set combo boxes
        int idx = ComboBox_FindStringExact(hComboDVT_, -1, product.DonViTinh.c_str());
        if (idx != CB_ERR) ComboBox_SetCurSel(hComboDVT_, idx);

        idx = ComboBox_FindStringExact(hComboNhomHang_, -1, product.NhomHang.c_str());
        if (idx != CB_ERR) ComboBox_SetCurSel(hComboNhomHang_, idx);

        SetWindowTextW(hEditGiaMua_, FormatMoney(product.GiaMua).c_str());
        SetWindowTextW(hEditGiaBan_, FormatMoney(product.GiaBan).c_str());
        SetWindowTextW(hEditMoTa_, product.MoTa.c_str());

        Button_SetCheck(hRadioActive_, product.TrangThai ? BST_CHECKED : BST_UNCHECKED);
        Button_SetCheck(hRadioInactive_, product.TrangThai ? BST_UNCHECKED : BST_CHECKED);

        CalculateProfitLoss();
    }

    void ProductForm::CalculateProfitLoss() {
        wchar_t bufMua[64], bufBan[64];
        GetWindowTextW(hEditGiaMua_, bufMua, 64);
        GetWindowTextW(hEditGiaBan_, bufBan, 64);

        double giaMua = ParseMoney(bufMua);
        double giaBan = ParseMoney(bufBan);
        double laiLo = giaBan - giaMua;

        SetWindowTextW(hEditLaiLo_, FormatMoney(laiLo).c_str());
    }

    void ProductForm::OnCommand(WPARAM wParam, LPARAM lParam) {
        KETOAN_UNUSED(lParam);

        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId) {
            case IDC_BTN_THEM:
                OnBtnThem();
                break;

            case IDC_BTN_SUA:
                OnBtnSua();
                break;

            case IDC_BTN_XOA:
                OnBtnXoa();
                break;

            case IDC_BTN_LUU:
                OnBtnLuu();
                break;

            case IDC_BTN_HUY:
                OnBtnHuy();
                break;

            case IDC_BTN_TIMKIEM:
                OnBtnTimKiem();
                break;

            case IDC_EDIT_GIAMUA:
            case IDC_EDIT_GIABAN:
                if (wmEvent == EN_CHANGE) {
                    CalculateProfitLoss();
                }
                break;

            case IDCANCEL:
                OnClose();
                break;
        }
    }

    void ProductForm::OnNotify(LPARAM lParam) {
        LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);

        if (nmhdr->hwndFrom == hListView_) {
            if (nmhdr->code == LVN_ITEMCHANGED) {
                LPNMLISTVIEW pnmv = reinterpret_cast<LPNMLISTVIEW>(lParam);
                if (pnmv->uNewState & LVIS_SELECTED) {
                    selectedIndex_ = pnmv->iItem;
                    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(products_.size())) {
                        if (!isEditMode_) {
                            LoadProductToForm(products_[selectedIndex_]);
                        }
                    }
                }
            } else if (nmhdr->code == NM_DBLCLK) {
                OnListViewDblClick();
            }
        }
    }

    void ProductForm::OnBtnThem() {
        isEditMode_ = true;
        selectedIndex_ = -1;
        ClearForm();
        SetEditEnabled(true);

        // Auto-generate product code
        wchar_t code[32];
        swprintf_s(code, L"SP%03zu", products_.size() + 1);
        SetWindowTextW(hEditMaSP_, code);

        SetFocus(hEditTenSP_);
    }

    void ProductForm::OnBtnSua() {
        if (selectedIndex_ < 0) {
            MessageBoxW(hwnd_, L"Vui lòng chọn sản phẩm cần sửa!", L"Thông báo", MB_OK | MB_ICONWARNING);
            return;
        }

        isEditMode_ = true;
        SetEditEnabled(true);
        SetFocus(hEditTenSP_);
    }

    void ProductForm::OnBtnXoa() {
        if (selectedIndex_ < 0) {
            MessageBoxW(hwnd_, L"Vui lòng chọn sản phẩm cần xóa!", L"Thông báo", MB_OK | MB_ICONWARNING);
            return;
        }

        const Product& p = products_[selectedIndex_];
        std::wstring msg = L"Bạn có chắc muốn xóa sản phẩm \"" + p.TenSP + L"\"?";

        int result = MessageBoxW(hwnd_, msg.c_str(), L"Xác nhận xóa", MB_YESNO | MB_ICONQUESTION);
        if (result == IDYES) {
            products_.erase(products_.begin() + selectedIndex_);
            selectedIndex_ = -1;
            RefreshListView();
            ClearForm();

            MessageBoxW(hwnd_, L"Đã xóa sản phẩm thành công!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
        }
    }

    void ProductForm::OnBtnLuu() {
        if (!ValidateForm()) {
            return;
        }

        Product product;

        wchar_t buf[512];
        GetWindowTextW(hEditMaSP_, buf, 512);
        product.MaSP = buf;

        GetWindowTextW(hEditTenSP_, buf, 512);
        product.TenSP = buf;

        int idx = ComboBox_GetCurSel(hComboDVT_);
        ComboBox_GetLBText(hComboDVT_, idx, buf);
        product.DonViTinh = buf;

        idx = ComboBox_GetCurSel(hComboNhomHang_);
        ComboBox_GetLBText(hComboNhomHang_, idx, buf);
        product.NhomHang = buf;

        GetWindowTextW(hEditGiaMua_, buf, 512);
        product.GiaMua = ParseMoney(buf);

        GetWindowTextW(hEditGiaBan_, buf, 512);
        product.GiaBan = ParseMoney(buf);

        GetWindowTextW(hEditMoTa_, buf, 512);
        product.MoTa = buf;

        product.TrangThai = (Button_GetCheck(hRadioActive_) == BST_CHECKED);

        // Add or update
        if (selectedIndex_ < 0) {
            // Add new
            products_.push_back(product);
            MessageBoxW(hwnd_, L"Đã thêm sản phẩm mới!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
        } else {
            // Update existing
            products_[selectedIndex_] = product;
            MessageBoxW(hwnd_, L"Đã cập nhật sản phẩm!", L"Thông báo", MB_OK | MB_ICONINFORMATION);
        }

        RefreshListView();
        SetEditEnabled(false);
        isEditMode_ = false;
    }

    void ProductForm::OnBtnHuy() {
        SetEditEnabled(false);
        isEditMode_ = false;

        if (selectedIndex_ >= 0) {
            LoadProductToForm(products_[selectedIndex_]);
        } else {
            ClearForm();
        }
    }

    void ProductForm::OnBtnTimKiem() {
        wchar_t searchText[256];
        GetWindowTextW(hEditSearch_, searchText, 256);

        std::wstring search = searchText;
        if (search.empty()) {
            RefreshListView();
            return;
        }

        // Simple search - clear and show only matching items
        ListView_DeleteAllItems(hListView_);

        for (size_t i = 0; i < products_.size(); i++) {
            const Product& p = products_[i];

            // Check if search text is in product code or name
            if (p.MaSP.find(search) != std::wstring::npos ||
                p.TenSP.find(search) != std::wstring::npos) {

                LVITEMW lvi = { 0 };
                lvi.mask = LVIF_TEXT | LVIF_PARAM;
                lvi.iItem = ListView_GetItemCount(hListView_);
                lvi.lParam = static_cast<LPARAM>(i);  // Store original index

                // Add item...
                lvi.iSubItem = 0;
                lvi.pszText = const_cast<LPWSTR>(p.MaSP.c_str());
                ListView_InsertItem(hListView_, &lvi);

                lvi.iSubItem = 1;
                lvi.pszText = const_cast<LPWSTR>(p.TenSP.c_str());
                ListView_SetItem(hListView_, &lvi);

                lvi.iSubItem = 2;
                lvi.pszText = const_cast<LPWSTR>(p.DonViTinh.c_str());
                ListView_SetItem(hListView_, &lvi);

                std::wstring giaMua = FormatMoney(p.GiaMua);
                lvi.iSubItem = 3;
                lvi.pszText = const_cast<LPWSTR>(giaMua.c_str());
                ListView_SetItem(hListView_, &lvi);

                std::wstring giaBan = FormatMoney(p.GiaBan);
                lvi.iSubItem = 4;
                lvi.pszText = const_cast<LPWSTR>(giaBan.c_str());
                ListView_SetItem(hListView_, &lvi);

                lvi.iSubItem = 5;
                lvi.pszText = const_cast<LPWSTR>(p.TrangThai ? L"✓" : L"");
                ListView_SetItem(hListView_, &lvi);
            }
        }
    }

    void ProductForm::OnListViewDblClick() {
        OnBtnSua();
    }

    bool ProductForm::ValidateForm() {
        wchar_t buf[512];

        // Check Mã SP
        GetWindowTextW(hEditMaSP_, buf, 512);
        if (wcslen(buf) == 0) {
            MessageBoxW(hwnd_, L"Vui lòng nhập mã sản phẩm!", L"Lỗi", MB_OK | MB_ICONERROR);
            SetFocus(hEditMaSP_);
            return false;
        }

        // Check Tên SP
        GetWindowTextW(hEditTenSP_, buf, 512);
        if (wcslen(buf) == 0) {
            MessageBoxW(hwnd_, L"Vui lòng nhập tên sản phẩm!", L"Lỗi", MB_OK | MB_ICONERROR);
            SetFocus(hEditTenSP_);
            return false;
        }

        // Check Giá mua
        GetWindowTextW(hEditGiaMua_, buf, 512);
        double giaMua = ParseMoney(buf);
        if (giaMua < 0) {
            MessageBoxW(hwnd_, L"Giá mua phải >= 0!", L"Lỗi", MB_OK | MB_ICONERROR);
            SetFocus(hEditGiaMua_);
            return false;
        }

        // Check Giá bán
        GetWindowTextW(hEditGiaBan_, buf, 512);
        double giaBan = ParseMoney(buf);
        if (giaBan < 0) {
            MessageBoxW(hwnd_, L"Giá bán phải >= 0!", L"Lỗi", MB_OK | MB_ICONERROR);
            SetFocus(hEditGiaBan_);
            return false;
        }

        return true;
    }

    void ProductForm::OnClose() {
        if (isEditMode_) {
            int result = MessageBoxW(hwnd_,
                L"Bạn có muốn lưu thay đổi trước khi đóng?",
                L"Xác nhận",
                MB_YESNOCANCEL | MB_ICONQUESTION);

            if (result == IDYES) {
                if (!ValidateForm()) {
                    return;
                }
                OnBtnLuu();
            } else if (result == IDCANCEL) {
                return;
            }
        }

        DestroyWindow(hwnd_);
    }

} // namespace KeToanApp
