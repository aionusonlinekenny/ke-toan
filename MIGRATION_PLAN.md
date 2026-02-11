# KẾ HOẠCH MIGRATION: Microsoft Access → C++ Desktop Application

## 📋 Tổng quan

Migration từ Microsoft Access sang C++ Desktop Application với Win32 API cho hệ thống kế toán và quản lý kho.

### Database Files Hiện Tại
- **Kho_data.accdb** (~267 MB) - Quản lý kho hàng
- **ketnoi_ketoan.accdb** (~49 MB) - Kết nối và quản lý kế toán

---

## 🏗️ Kiến Trúc Hệ Thống Mới

### 1. Technology Stack
- **Language**: C++17/20
- **IDE**: Visual Studio 2022
- **GUI Framework**: Win32 API (Native)
- **Database**: SQLite 3 (embedded) hoặc SQL Server Express
- **Build System**: MSBuild / CMake
- **Version Control**: Git

### 2. Layered Architecture

```
┌─────────────────────────────────────┐
│     Presentation Layer (UI)         │
│        Win32 API Windows            │
├─────────────────────────────────────┤
│     Business Logic Layer            │
│  Controllers & Service Classes      │
├─────────────────────────────────────┤
│       Data Access Layer             │
│    Repository Pattern / DAO         │
├─────────────────────────────────────┤
│         Database Layer              │
│    SQLite / SQL Server              │
└─────────────────────────────────────┘
```

---

## 📁 Cấu Trúc Project

```
KeToanApp/
├── src/
│   ├── Core/                  # Core business logic
│   │   ├── Application.h/cpp
│   │   ├── Config.h/cpp
│   │   └── EventManager.h/cpp
│   │
│   ├── Database/              # Data Access Layer
│   │   ├── DatabaseManager.h/cpp
│   │   ├── Connection.h/cpp
│   │   ├── QueryBuilder.h/cpp
│   │   └── Repositories/
│   │       ├── KhoRepository.h/cpp
│   │       └── KeToanRepository.h/cpp
│   │
│   ├── Models/                # Data Models (POCOs)
│   │   ├── SanPham.h/cpp
│   │   ├── PhieuNhap.h/cpp
│   │   ├── PhieuXuat.h/cpp
│   │   ├── TonKho.h/cpp
│   │   ├── ChungTu.h/cpp
│   │   └── TaiKhoan.h/cpp
│   │
│   ├── Services/              # Business Services
│   │   ├── KhoService.h/cpp
│   │   ├── KeToanService.h/cpp
│   │   ├── ReportService.h/cpp
│   │   └── ValidationService.h/cpp
│   │
│   ├── UI/                    # User Interface (Win32)
│   │   ├── MainWindow.h/cpp
│   │   ├── Windows/
│   │   │   ├── KhoWindow.h/cpp
│   │   │   ├── NhapKhoWindow.h/cpp
│   │   │   ├── XuatKhoWindow.h/cpp
│   │   │   ├── KeToanWindow.h/cpp
│   │   │   └── ReportWindow.h/cpp
│   │   ├── Controls/
│   │   │   ├── DataGrid.h/cpp
│   │   │   ├── CustomButton.h/cpp
│   │   │   └── DatePicker.h/cpp
│   │   └── Resources/
│   │       ├── resource.h
│   │       └── resource.rc
│   │
│   ├── Utils/                 # Utilities
│   │   ├── StringHelper.h/cpp
│   │   ├── DateTimeHelper.h/cpp
│   │   ├── NumberHelper.h/cpp
│   │   ├── Logger.h/cpp
│   │   └── Validator.h/cpp
│   │
│   └── main.cpp              # Entry point
│
├── include/                   # Public headers
│   └── KeToanApp/
│       ├── Common.h
│       ├── Types.h
│       └── Exports.h
│
├── lib/                       # Third-party libraries
│   ├── sqlite3/
│   └── ...
│
├── resources/                 # Resources
│   ├── icons/
│   ├── images/
│   └── config/
│       └── database.ini
│
├── tests/                     # Unit tests
│   ├── DatabaseTests/
│   ├── ServiceTests/
│   └── UITests/
│
├── docs/                      # Documentation
│   ├── DATABASE_SCHEMA.md
│   ├── API_REFERENCE.md
│   ├── USER_GUIDE.md
│   └── DEVELOPMENT.md
│
├── scripts/                   # Build & deployment scripts
│   ├── setup_database.sql
│   └── deploy.bat
│
├── KeToanApp.sln             # Visual Studio Solution
├── KeToanApp.vcxproj         # VS Project file
├── CMakeLists.txt            # CMake configuration
├── .gitignore
└── README.md
```

---

## 🗄️ Database Migration Strategy

### Bước 1: Phân Tích Schema Access
Cần export schema từ Access databases:
- Tables structure (columns, types, constraints)
- Relationships (foreign keys)
- Queries (Views)
- Forms (→ UI mapping)
- Reports (→ Report generation)
- VBA Modules (→ Business logic)

### Bước 2: Thiết Kế Schema Mới

#### A. Kho_data.accdb → Tables

**Dự kiến tables chính:**

```sql
-- Sản phẩm / Hàng hóa
CREATE TABLE SanPham (
    MaSP VARCHAR(20) PRIMARY KEY,
    TenSP NVARCHAR(200) NOT NULL,
    DonViTinh NVARCHAR(50),
    GiaMua DECIMAL(18,2),
    GiaBan DECIMAL(18,2),
    MoTa NVARCHAR(500),
    NhomHang VARCHAR(50),
    TrangThai INT DEFAULT 1,
    NgayTao DATETIME DEFAULT CURRENT_TIMESTAMP,
    NguoiTao VARCHAR(50)
);

-- Tồn kho
CREATE TABLE TonKho (
    ID INT PRIMARY KEY AUTO_INCREMENT,
    MaSP VARCHAR(20) NOT NULL,
    SoLuongTon DECIMAL(18,3),
    GiaTriTon DECIMAL(18,2),
    NgayCapNhat DATETIME,
    FOREIGN KEY (MaSP) REFERENCES SanPham(MaSP)
);

-- Phiếu nhập kho
CREATE TABLE PhieuNhap (
    SoPhieu VARCHAR(20) PRIMARY KEY,
    NgayNhap DATE NOT NULL,
    NhaCungCap NVARCHAR(200),
    NguoiNhap VARCHAR(50),
    TongTien DECIMAL(18,2),
    GhiChu NVARCHAR(500),
    TrangThai INT DEFAULT 1
);

-- Chi tiết phiếu nhập
CREATE TABLE ChiTietPhieuNhap (
    ID INT PRIMARY KEY AUTO_INCREMENT,
    SoPhieu VARCHAR(20) NOT NULL,
    MaSP VARCHAR(20) NOT NULL,
    SoLuong DECIMAL(18,3),
    DonGia DECIMAL(18,2),
    ThanhTien DECIMAL(18,2),
    FOREIGN KEY (SoPhieu) REFERENCES PhieuNhap(SoPhieu),
    FOREIGN KEY (MaSP) REFERENCES SanPham(MaSP)
);

-- Phiếu xuất kho
CREATE TABLE PhieuXuat (
    SoPhieu VARCHAR(20) PRIMARY KEY,
    NgayXuat DATE NOT NULL,
    KhachHang NVARCHAR(200),
    NguoiXuat VARCHAR(50),
    TongTien DECIMAL(18,2),
    GhiChu NVARCHAR(500),
    TrangThai INT DEFAULT 1
);

-- Chi tiết phiếu xuất
CREATE TABLE ChiTietPhieuXuat (
    ID INT PRIMARY KEY AUTO_INCREMENT,
    SoPhieu VARCHAR(20) NOT NULL,
    MaSP VARCHAR(20) NOT NULL,
    SoLuong DECIMAL(18,3),
    DonGia DECIMAL(18,2),
    ThanhTien DECIMAL(18,2),
    FOREIGN KEY (SoPhieu) REFERENCES PhieuXuat(SoPhieu),
    FOREIGN KEY (MaSP) REFERENCES SanPham(MaSP)
);
```

#### B. ketnoi_ketoan.accdb → Tables

```sql
-- Tài khoản kế toán
CREATE TABLE TaiKhoanKeToan (
    SoTK VARCHAR(20) PRIMARY KEY,
    TenTK NVARCHAR(200) NOT NULL,
    LoaiTK INT, -- 1: Tài sản, 2: Nguồn vốn, 3: Thu, 4: Chi
    TKCha VARCHAR(20),
    CapDo INT DEFAULT 1,
    TrangThai INT DEFAULT 1,
    FOREIGN KEY (TKCha) REFERENCES TaiKhoanKeToan(SoTK)
);

-- Chứng từ kế toán
CREATE TABLE ChungTuKeToan (
    SoCT VARCHAR(20) PRIMARY KEY,
    NgayCT DATE NOT NULL,
    LoaiCT VARCHAR(50), -- Phiếu thu, chi, nhập, xuất, etc.
    DienGiai NVARCHAR(500),
    NguoiLap VARCHAR(50),
    TrangThai INT DEFAULT 1,
    NgayTao DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Định khoản kế toán (Bút toán)
CREATE TABLE DinhKhoan (
    ID INT PRIMARY KEY AUTO_INCREMENT,
    SoCT VARCHAR(20) NOT NULL,
    STT INT,
    TKNo VARCHAR(20) NOT NULL,
    TKCo VARCHAR(20) NOT NULL,
    SoTien DECIMAL(18,2) NOT NULL,
    DienGiai NVARCHAR(500),
    FOREIGN KEY (SoCT) REFERENCES ChungTuKeToan(SoCT),
    FOREIGN KEY (TKNo) REFERENCES TaiKhoanKeToan(SoTK),
    FOREIGN KEY (TKCo) REFERENCES TaiKhoanKeToan(SoTK)
);

-- Công nợ
CREATE TABLE CongNo (
    ID INT PRIMARY KEY AUTO_INCREMENT,
    LoaiCN INT, -- 1: Phải thu, 2: Phải trả
    MaDoiTuong VARCHAR(50),
    TenDoiTuong NVARCHAR(200),
    SoCT VARCHAR(20),
    NgayCT DATE,
    SoTien DECIMAL(18,2),
    DaTra DECIMAL(18,2) DEFAULT 0,
    ConLai DECIMAL(18,2),
    FOREIGN KEY (SoCT) REFERENCES ChungTuKeToan(SoCT)
);
```

---

## 💻 Implementation Plan

### Phase 1: Setup & Infrastructure (Week 1)
- ✅ Setup Visual Studio 2022 project
- ✅ Configure project structure
- ✅ Setup SQLite/SQL Server connection
- ✅ Create database schema
- ✅ Implement Database Manager
- ✅ Create base models (POCOs)

### Phase 2: Data Access Layer (Week 1-2)
- ✅ Implement Repository pattern
- ✅ Create KhoRepository
- ✅ Create KeToanRepository
- ✅ Implement QueryBuilder utility
- ✅ Write unit tests for DAL

### Phase 3: Business Logic Layer (Week 2-3)
- ✅ Implement KhoService
- ✅ Implement KeToanService
- ✅ Implement ReportService
- ✅ Implement ValidationService
- ✅ Migrate VBA business logic to C++

### Phase 4: User Interface (Week 3-4)
- ✅ Create MainWindow (MDI or SDI)
- ✅ Implement KhoWindow (Quản lý kho)
- ✅ Implement NhapKhoWindow (Nhập kho)
- ✅ Implement XuatKhoWindow (Xuất kho)
- ✅ Implement KeToanWindow (Kế toán)
- ✅ Implement ReportWindow (Báo cáo)
- ✅ Create custom controls (DataGrid, etc.)

### Phase 5: Data Migration (Week 4)
- ✅ Export data from Access databases
- ✅ Write migration scripts
- ✅ Import data to new database
- ✅ Verify data integrity

### Phase 6: Testing & Deployment (Week 5)
- ✅ Integration testing
- ✅ User acceptance testing
- ✅ Performance optimization
- ✅ Create installer
- ✅ Write user documentation

---

## 🔄 Data Migration Process

### Option 1: Manual Export/Import
1. Export Access tables to CSV/Excel
2. Write C++ import tool using file parsers
3. Validate and import data

### Option 2: ODBC Bridge
```cpp
// Sử dụng ODBC để đọc trực tiếp từ Access
#include <sql.h>
#include <sqlext.h>

// Kết nối Access database
std::string connStr = "Driver={Microsoft Access Driver (*.mdb, *.accdb)};"
                      "Dbq=C:\\path\\to\\database.accdb;";
// Execute queries và migrate data
```

### Option 3: Python Script + C++ Import
```python
# Python script để export
import pyodbc
import csv

conn = pyodbc.connect(r'Driver={Microsoft Access Driver (*.mdb, *.accdb)};'
                      r'DBQ=path\to\database.accdb;')
# Export tables to CSV
# C++ reads CSV and imports
```

---

## 📊 Key Features to Implement

### 1. Quản Lý Kho (Warehouse Management)
- ✅ Danh mục sản phẩm
- ✅ Nhập kho
- ✅ Xuất kho
- ✅ Kiểm kê tồn kho
- ✅ Tra cứu lịch sử xuất nhập

### 2. Kế Toán (Accounting)
- ✅ Hệ thống tài khoản
- ✅ Ghi sổ kế toán (Định khoản)
- ✅ Sổ cái
- ✅ Quản lý công nợ
- ✅ Báo cáo tài chính

### 3. Báo Cáo (Reports)
- ✅ Báo cáo tồn kho
- ✅ Báo cáo xuất nhập tồn
- ✅ Báo cáo doanh thu
- ✅ Báo cáo công nợ
- ✅ Export to PDF/Excel

### 4. Utilities
- ✅ Backup/Restore database
- ✅ User authentication
- ✅ Activity logging
- ✅ Settings management
- ✅ Multi-user support (optional)

---

## 🎨 UI Design Principles

### Win32 API Implementation
```cpp
// MainWindow structure
class MainWindow {
private:
    HWND hwnd_;
    HMENU menuBar_;
    HWND statusBar_;
    HWND toolbar_;

public:
    bool Create();
    void Show();
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    // Child windows
    void ShowKhoWindow();
    void ShowKeToanWindow();
    void ShowReportWindow();
};
```

### Dialog-based Forms
```cpp
// Dialog for Nhập Kho
class NhapKhoDialog {
private:
    HWND hwndDlg_;
    PhieuNhap currentPhieu_;

public:
    INT_PTR ShowModal(HWND parent);
    void LoadData();
    bool ValidateInput();
    bool SavePhieu();
};
```

---

## 🛠️ Development Guidelines

### 1. Coding Standards
- Follow C++ Core Guidelines
- Use smart pointers (std::unique_ptr, std::shared_ptr)
- RAII for resource management
- Exception handling
- Const correctness

### 2. Error Handling
```cpp
class KeToanException : public std::runtime_error {
public:
    explicit KeToanException(const std::string& msg);
};

// Usage
try {
    service.SavePhieuNhap(phieu);
} catch (const KeToanException& e) {
    MessageBoxA(hwnd, e.what(), "Lỗi", MB_ICONERROR);
}
```

### 3. Logging
```cpp
Logger::Info("Phiếu nhập đã được lưu: " + soPhieu);
Logger::Error("Không thể kết nối database");
Logger::Debug("Query: " + query);
```

### 4. Configuration
```ini
# config/database.ini
[Database]
Type=SQLite
Path=./data/ketoan.db

[Application]
Language=vi-VN
DateFormat=dd/MM/yyyy
NumberFormat=#,##0.00
```

---

## 📦 Third-party Libraries

### Recommended Libraries
1. **SQLite3** - Embedded database
   - Lightweight, serverless
   - Single file database
   - ACID compliant

2. **SQLiteCpp** - C++ wrapper for SQLite
   ```cpp
   #include <SQLiteCpp/SQLiteCpp.h>
   ```

3. **{fmt}** - Modern formatting library
   ```cpp
   std::string msg = fmt::format("Tồn kho: {:.2f}", tonkho);
   ```

4. **spdlog** - Fast logging library
   ```cpp
   spdlog::info("Application started");
   ```

5. **nlohmann/json** - JSON for config
   ```cpp
   json config = json::parse(configFile);
   ```

---

## 🔐 Security Considerations

1. **SQL Injection Prevention**
   - Use parameterized queries
   - Validate all user input

2. **Data Encryption**
   - Encrypt sensitive data
   - Secure password storage

3. **Access Control**
   - User authentication
   - Role-based permissions

4. **Audit Trail**
   - Log all important operations
   - Track data changes

---

## 📈 Performance Optimization

1. **Database**
   - Proper indexing
   - Connection pooling
   - Prepared statements
   - Query optimization

2. **UI**
   - Lazy loading
   - Virtual scrolling for large lists
   - Background threading
   - Responsive design

3. **Memory**
   - Smart pointer usage
   - RAII pattern
   - Avoid memory leaks

---

## 🚀 Deployment

### Build Configuration
- Debug: Development with symbols
- Release: Optimized, no symbols
- Create installer using:
  - WiX Toolset
  - Inno Setup
  - NSIS

### System Requirements
- Windows 10/11 (64-bit)
- Visual C++ Redistributable 2022
- 4GB RAM minimum
- 500MB disk space
- .NET Framework 4.8 (if needed)

---

## 📝 Next Steps

1. **Immediate Actions:**
   - [ ] Export database schema từ Access files
   - [ ] Document business logic từ VBA modules
   - [ ] Identify key reports cần implement

2. **Setup Development Environment:**
   - [ ] Install Visual Studio 2022
   - [ ] Install SQLite
   - [ ] Clone repository
   - [ ] Setup build system

3. **Begin Implementation:**
   - [ ] Create project structure
   - [ ] Implement database layer
   - [ ] Migrate sample data
   - [ ] Create first UI window

---

## 📞 Support & Resources

### Documentation
- C++ Reference: https://en.cppreference.com/
- Win32 API: https://docs.microsoft.com/en-us/windows/win32/
- SQLite: https://www.sqlite.org/docs.html

### Tools
- Visual Studio 2022
- DB Browser for SQLite
- Git for version control
- Dependency Walker

---

**Last Updated**: 2026-02-11
**Author**: Claude AI Assistant
**Project**: KeToanApp Migration from Access to C++
