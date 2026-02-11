# Phần Mềm Kế Toán - KeToanApp

Ứng dụng kế toán desktop được phát triển bằng C++ và Win32 API, migration từ Microsoft Access.

## 📋 Tổng quan

KeToanApp là phần mềm quản lý kế toán và kho hàng được xây dựng hoàn toàn bằng C++ với giao diện Win32 API native. Đây là kết quả migration từ hệ thống Microsoft Access cũ sang nền tảng hiện đại hơn, mang lại hiệu suất cao và khả năng mở rộng tốt hơn.

### Tính năng chính

#### 🏪 Quản lý Kho
- Danh mục sản phẩm/hàng hóa
- Nhập kho với phiếu nhập chi tiết
- Xuất kho với phiếu xuất chi tiết
- Theo dõi tồn kho thời gian thực
- Lịch sử xuất nhập kho

#### 💰 Quản lý Kế Toán
- Hệ thống tài khoản kế toán
- Ghi sổ kế toán (Định khoản)
- Sổ cái và sổ chi tiết
- Quản lý công nợ phải thu/phải trả
- Báo cáo tài chính

#### 📊 Báo cáo
- Báo cáo tồn kho
- Báo cáo xuất nhập tồn
- Báo cáo doanh thu
- Báo cáo công nợ
- Export PDF/Excel (Coming soon)

## 🛠️ Technology Stack

- **Language**: C++17
- **IDE**: Visual Studio 2022
- **GUI**: Win32 API (Native)
- **Database**: SQLite 3 (Embedded)
- **Build System**: MSBuild / CMake
- **Platform**: Windows 10/11 (64-bit)

## 📁 Cấu trúc Project

```
ke-toan/
├── KeToanApp/                 # Source code chính
│   ├── src/                   # Source files
│   │   ├── Core/             # Application core
│   │   ├── Database/         # Database layer
│   │   ├── Models/           # Data models
│   │   ├── Services/         # Business logic
│   │   ├── UI/              # User interface
│   │   └── Utils/           # Utilities
│   ├── include/              # Public headers
│   ├── lib/                  # Third-party libraries
│   ├── resources/            # Resources (icons, config)
│   ├── tests/               # Unit tests
│   └── docs/                # Documentation
├── KeToanApp.sln            # Visual Studio Solution
├── KeToanApp.vcxproj        # VS Project file
├── CMakeLists.txt           # CMake configuration
├── MIGRATION_PLAN.md        # Chi tiết kế hoạch migration
└── README.md                # This file
```

## 🚀 Bắt đầu

### Yêu cầu hệ thống

- Windows 10 hoặc Windows 11 (64-bit)
- Visual Studio 2022 (Community, Professional, hoặc Enterprise)
- Visual C++ Desktop Development workload
- 4GB RAM (khuyến nghị 8GB)
- 500MB dung lượng ổ cứng

### Cài đặt môi trường phát triển

1. **Cài đặt Visual Studio 2022**
   - Download từ: https://visualstudio.microsoft.com/vs/
   - Chọn workload: "Desktop development with C++"
   - Đảm bảo cài đặt Windows 10 SDK

2. **Clone repository**
   ```bash
   git clone https://github.com/aionusonlinekenny/ke-toan.git
   cd ke-toan
   ```

3. **Cài đặt dependencies (SQLite)**
   - Download SQLite amalgamation từ: https://www.sqlite.org/download.html
   - Extract vào thư mục `KeToanApp/lib/sqlite3/`
   - Hoặc sử dụng vcpkg:
   ```bash
   vcpkg install sqlite3:x64-windows
   ```

### Build Project

#### Sử dụng Visual Studio

1. Mở file `KeToanApp.sln` trong Visual Studio 2022
2. Chọn configuration: `Debug` hoặc `Release`
3. Chọn platform: `x64`
4. Build solution: `Ctrl+Shift+B` hoặc Menu `Build > Build Solution`
5. Run: `F5` hoặc `Ctrl+F5`

#### Sử dụng CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

#### Sử dụng MSBuild (Command Line)

```bash
msbuild KeToanApp.sln /p:Configuration=Release /p:Platform=x64
```

## 📖 Hướng dẫn sử dụng

### Cấu hình

File cấu hình `config.ini` sẽ được tạo tự động khi chạy lần đầu. Bạn có thể chỉnh sửa:

```ini
[Database]
Type=SQLite
Path=./data/ketoan.db

[Application]
Language=vi-VN
DateFormat=dd/MM/yyyy
NumberPrecision=2
```

### Khởi động ứng dụng

1. Chạy file `KeToanApp.exe`
2. Database sẽ được tạo tự động nếu chưa tồn tại
3. Giao diện chính hiển thị với menu:
   - **Tập tin**: Các chức năng file
   - **Kho**: Quản lý kho hàng
   - **Kế toán**: Quản lý kế toán
   - **Báo cáo**: Các loại báo cáo
   - **Trợ giúp**: Hướng dẫn và giới thiệu

### Luồng làm việc cơ bản

#### 1. Quản lý Sản phẩm
- Menu: `Kho > Danh mục Sản phẩm`
- Thêm/Sửa/Xóa thông tin sản phẩm
- Thiết lập giá mua, giá bán

#### 2. Nhập Kho
- Menu: `Kho > Nhập kho`
- Tạo phiếu nhập mới
- Chọn sản phẩm và số lượng
- Lưu phiếu nhập
- Tồn kho tự động cập nhật

#### 3. Xuất Kho
- Menu: `Kho > Xuất kho`
- Tạo phiếu xuất mới
- Chọn sản phẩm và số lượng
- Lưu phiếu xuất
- Tồn kho tự động cập nhật

#### 4. Quản lý Kế Toán
- Menu: `Kế toán > Chứng từ`
- Tạo chứng từ kế toán
- Nhập định khoản (Nợ/Có)
- Lưu và cập nhật sổ cái

#### 5. Xem Báo Cáo
- Menu: `Báo cáo > [Chọn loại báo cáo]`
- Chọn khoảng thời gian
- Xem và in báo cáo

## 🔧 Development

### Coding Guidelines

- Follow C++ Core Guidelines
- Use modern C++ features (C++17)
- Smart pointers over raw pointers
- RAII pattern for resource management
- Const correctness
- Exception handling for errors

### Architecture

Project sử dụng kiến trúc phân lớp (Layered Architecture):

```
┌─────────────────────────────┐
│   Presentation Layer (UI)   │  Win32 Windows & Controls
├─────────────────────────────┤
│   Business Logic Layer      │  Services & Controllers
├─────────────────────────────┤
│   Data Access Layer         │  Repositories & DAOs
├─────────────────────────────┤
│   Database Layer            │  SQLite Database
└─────────────────────────────┘
```

### Thêm tính năng mới

1. Tạo model class trong `src/Models/`
2. Tạo repository trong `src/Database/Repositories/`
3. Tạo service class trong `src/Services/`
4. Tạo UI window/dialog trong `src/UI/Windows/`
5. Kết nối với MainWindow

### Testing

```bash
# Build tests
cmake --build build --target tests

# Run tests
cd build
ctest
```

## 📦 Database Schema

### Warehouse Tables

- **SanPham**: Danh mục sản phẩm
- **TonKho**: Tồn kho hiện tại
- **PhieuNhap**: Phiếu nhập kho
- **ChiTietPhieuNhap**: Chi tiết phiếu nhập
- **PhieuXuat**: Phiếu xuất kho
- **ChiTietPhieuXuat**: Chi tiết phiếu xuất

### Accounting Tables

- **TaiKhoanKeToan**: Hệ thống tài khoản
- **ChungTuKeToan**: Chứng từ kế toán
- **DinhKhoan**: Định khoản (Bút toán)
- **CongNo**: Công nợ phải thu/trả

Chi tiết schema xem file: [DATABASE_SCHEMA.md](docs/DATABASE_SCHEMA.md)

## 🔄 Migration từ Access

Nếu bạn đang migrate dữ liệu từ Access cũ:

1. Export dữ liệu từ Access ra CSV/Excel
2. Sử dụng script migration (đang phát triển)
3. Hoặc import thủ công qua UI

Xem chi tiết: [MIGRATION_PLAN.md](MIGRATION_PLAN.md)

## 🐛 Troubleshooting

### Lỗi khởi động

**Lỗi**: "Failed to initialize application"
- Kiểm tra file log: `ketoan.log`
- Đảm bảo quyền ghi vào thư mục
- Kiểm tra file config.ini

**Lỗi**: "Database connection failed"
- Kiểm tra đường dẫn database trong config.ini
- Đảm bảo thư mục data/ tồn tại
- Kiểm tra quyền ghi vào database file

### Lỗi Build

**Lỗi**: "Cannot open include file: 'sqlite3.h'"
- Cài đặt SQLite library
- Kiểm tra đường dẫn include trong project settings

**Lỗi**: "Unresolved external symbol"
- Link đúng SQLite library
- Kiểm tra linker settings

## 📝 Logging

Application log được ghi vào file `ketoan.log`:

```cpp
// Log levels
Logger::Debug("Debug message");
Logger::Info("Info message");
Logger::Warning("Warning message");
Logger::Error("Error message");
```

## 🤝 Contributing

1. Fork repository
2. Create feature branch: `git checkout -b feature/ten-tinh-nang`
3. Commit changes: `git commit -m 'Add some feature'`
4. Push to branch: `git push origin feature/ten-tinh-nang`
5. Submit Pull Request

## 📄 License

Copyright © 2026 KeToanApp. All rights reserved.

## 👥 Authors

- **Migration & Development**: Claude AI Assistant
- **Original Access Database**: [Original Author]

## 📞 Support

- GitHub Issues: https://github.com/aionusonlinekenny/ke-toan/issues
- Email: [your-email]
- Documentation: [Wiki link]

## 🗺️ Roadmap

### Version 1.0 (Current)
- ✅ Core application structure
- ✅ Database layer with SQLite
- ✅ Basic UI with Win32 API
- ✅ Warehouse management
- ✅ Accounting management
- ⏳ Report generation

### Version 1.1 (Planned)
- ⬜ PDF/Excel export
- ⬜ Advanced reporting
- ⬜ User authentication
- ⬜ Multi-user support
- ⬜ Backup/Restore functionality

### Version 2.0 (Future)
- ⬜ Client-Server architecture
- ⬜ Web interface
- ⬜ Mobile app
- ⬜ Cloud sync

## 📚 Resources

- [C++ Reference](https://en.cppreference.com/)
- [Win32 API Documentation](https://docs.microsoft.com/en-us/windows/win32/)
- [SQLite Documentation](https://www.sqlite.org/docs.html)
- [Visual Studio Documentation](https://docs.microsoft.com/en-us/visualstudio/)

---

**Last Updated**: 2026-02-11
**Version**: 1.0.0
**Status**: Active Development
