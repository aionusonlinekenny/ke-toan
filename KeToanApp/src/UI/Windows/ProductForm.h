#pragma once

#include "KeToanApp/Common.h"
#include <vector>
#include <string>

namespace KeToanApp {

    // Product data structure
    struct Product {
        std::wstring MaSP;          // Product code
        std::wstring TenSP;         // Product name
        std::wstring DonViTinh;     // Unit of measure
        std::wstring NhomHang;      // Product category
        double GiaMua;              // Purchase price
        double GiaBan;              // Selling price
        std::wstring MoTa;          // Description
        bool TrangThai;             // Status (active/inactive)

        Product() : GiaMua(0.0), GiaBan(0.0), TrangThai(true) {}
    };

    class ProductForm {
    public:
        explicit ProductForm(HWND hParent);
        ~ProductForm();

        // Create and show the form window
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
        HWND hEditMaSP_;
        HWND hEditTenSP_;
        HWND hComboDVT_;
        HWND hComboNhomHang_;
        HWND hEditGiaMua_;
        HWND hEditGiaBan_;
        HWND hEditLaiLo_;
        HWND hEditMoTa_;
        HWND hRadioActive_;
        HWND hRadioInactive_;
        HWND hListView_;
        HWND hEditSearch_;

        // Buttons
        HWND hBtnThem_;
        HWND hBtnSua_;
        HWND hBtnXoa_;
        HWND hBtnLuu_;
        HWND hBtnHuy_;
        HWND hBtnTimKiem_;

        // Data
        std::vector<Product> products_;
        int selectedIndex_;
        bool isEditMode_;

        // Window procedure
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // Message handlers
        void OnCreate();
        void OnCommand(WPARAM wParam, LPARAM lParam);
        void OnNotify(LPARAM lParam);
        void OnClose();

        // UI creation
        void CreateControls();
        void CreateListView();
        void SetupListViewColumns();

        // Data operations
        void LoadProducts();
        void RefreshListView();
        void ClearForm();
        void LoadProductToForm(const Product& product);
        bool SaveProduct();
        bool DeleteProduct();
        void CalculateProfitLoss();

        // Validation
        bool ValidateForm();

        // Button handlers
        void OnBtnThem();
        void OnBtnSua();
        void OnBtnXoa();
        void OnBtnLuu();
        void OnBtnHuy();
        void OnBtnTimKiem();

        // ListView handlers
        void OnListViewItemChanged();
        void OnListViewDblClick();

        // Helper functions
        std::wstring FormatMoney(double amount);
        double ParseMoney(const std::wstring& str);
        void SetEditEnabled(bool enabled);

        // Control IDs
        enum ControlIDs {
            IDC_EDIT_MASP = 3001,
            IDC_EDIT_TENSP = 3002,
            IDC_COMBO_DVT = 3003,
            IDC_COMBO_NHOMHANG = 3004,
            IDC_EDIT_GIAMUA = 3005,
            IDC_EDIT_GIABAN = 3006,
            IDC_EDIT_LAILO = 3007,
            IDC_EDIT_MOTA = 3008,
            IDC_RADIO_ACTIVE = 3009,
            IDC_RADIO_INACTIVE = 3010,
            IDC_LISTVIEW = 3011,
            IDC_EDIT_SEARCH = 3012,

            IDC_BTN_THEM = 4001,
            IDC_BTN_SUA = 4002,
            IDC_BTN_XOA = 4003,
            IDC_BTN_LUU = 4004,
            IDC_BTN_HUY = 4005,
            IDC_BTN_TIMKIEM = 4006
        };
    };

} // namespace KeToanApp
