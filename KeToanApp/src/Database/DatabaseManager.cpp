#include "DatabaseManager.h"
#include "../Utils/Logger.h"

namespace KeToanApp {

    DatabaseManager::DatabaseManager(const AppSettings& settings)
        : settings_(settings)
        , connection_(nullptr)
        , connected_(false)
        , inTransaction_(false)
    {
    }

    DatabaseManager::~DatabaseManager() {
        Disconnect();
    }

    bool DatabaseManager::Connect() {
        if (connected_) {
            return true;
        }

        try {
            connection_ = std::make_unique<Connection>(settings_);

            if (!connection_->Open()) {
                Logger::Error("Failed to open database connection");
                return false;
            }

            // Set connected flag immediately after successful connection
            connected_ = true;

            // Check and create schema if needed
            if (!CheckSchema()) {
                Logger::Info("Database schema not found, creating...");
                if (!CreateTables()) {
                    Logger::Error("Failed to create database schema");
                    connected_ = false;
                    return false;
                }
            }

            Logger::Info("Database connected successfully");
            return true;
        }
        catch (const DatabaseException& e) {
            Logger::Error("Database connection error: %s", e.what());
            return false;
        }
    }

    void DatabaseManager::Disconnect() {
        if (!connected_) {
            return;
        }

        if (inTransaction_) {
            Rollback();
        }

        if (connection_) {
            connection_->Close();
        }

        connected_ = false;
        Logger::Info("Database disconnected");
    }

    bool DatabaseManager::CheckSchema() {
        // Check if system table exists
        std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='SystemInfo'";
        std::string result;
        return ExecuteScalar(query, result) && !result.empty();
    }

    bool DatabaseManager::CreateTables() {
        Logger::Info("Creating database schema...");

        if (!BeginTransaction()) {
            return false;
        }

        try {
            if (!CreateSystemTables()) {
                throw DatabaseException("Failed to create system tables");
            }

            if (!CreateKhoTables()) {
                throw DatabaseException("Failed to create warehouse tables");
            }

            if (!CreateKeToanTables()) {
                throw DatabaseException("Failed to create accounting tables");
            }

            if (!SetSchemaVersion("1.0.0")) {
                throw DatabaseException("Failed to set schema version");
            }

            // Insert sample data for catalog tables
            if (!InsertSampleData()) {
                throw DatabaseException("Failed to insert sample data");
            }

            if (!Commit()) {
                throw DatabaseException("Failed to commit schema creation");
            }

            Logger::Info("Database schema created successfully");
            return true;
        }
        catch (const DatabaseException& e) {
            Logger::Error("Schema creation error: %s", e.what());
            Rollback();
            return false;
        }
    }

    bool DatabaseManager::CreateSystemTables() {
        std::string query = R"(
            CREATE TABLE IF NOT EXISTS SystemInfo (
                Key TEXT PRIMARY KEY,
                Value TEXT NOT NULL
            );
        )";

        return ExecuteQuery(query);
    }

    bool DatabaseManager::CreateKhoTables() {
        // NhomSanPham table (Product Groups)
        std::string queryNhomSP = R"(
            CREATE TABLE IF NOT EXISTS NhomSanPham (
                MaNhom TEXT PRIMARY KEY,
                TenNhom TEXT NOT NULL,
                MoTa TEXT,
                TrangThai INTEGER DEFAULT 1
            );
        )";

        if (!ExecuteQuery(queryNhomSP)) {
            return false;
        }

        // KhachHang table (Customers)
        std::string queryKhachHang = R"(
            CREATE TABLE IF NOT EXISTS KhachHang (
                MaKH TEXT PRIMARY KEY,
                TenKH TEXT NOT NULL,
                SoDT TEXT,
                DiaChi TEXT,
                Email TEXT,
                TrangThai INTEGER DEFAULT 1,
                NgayTao TEXT DEFAULT CURRENT_TIMESTAMP
            );
        )";

        if (!ExecuteQuery(queryKhachHang)) {
            return false;
        }

        // NhaCungCap table (Suppliers)
        std::string queryNhaCungCap = R"(
            CREATE TABLE IF NOT EXISTS NhaCungCap (
                MaNCC TEXT PRIMARY KEY,
                TenNCC TEXT NOT NULL,
                SoDT TEXT,
                DiaChi TEXT,
                Email TEXT,
                TrangThai INTEGER DEFAULT 1,
                NgayTao TEXT DEFAULT CURRENT_TIMESTAMP
            );
        )";

        if (!ExecuteQuery(queryNhaCungCap)) {
            return false;
        }

        // Kho table (Warehouses)
        std::string queryKho = R"(
            CREATE TABLE IF NOT EXISTS Kho (
                MaKho TEXT PRIMARY KEY,
                TenKho TEXT NOT NULL,
                DiaChi TEXT,
                QuanLy TEXT,
                TrangThai INTEGER DEFAULT 1,
                NgayTao TEXT DEFAULT CURRENT_TIMESTAMP
            );
        )";

        if (!ExecuteQuery(queryKho)) {
            return false;
        }

        // SanPham table
        std::string querySanPham = R"(
            CREATE TABLE IF NOT EXISTS SanPham (
                MaSP TEXT PRIMARY KEY,
                TenSP TEXT NOT NULL,
                DonViTinh TEXT,
                GiaMua REAL DEFAULT 0,
                GiaBan REAL DEFAULT 0,
                MoTa TEXT,
                MaNhom TEXT,
                TrangThai INTEGER DEFAULT 1,
                NgayTao TEXT DEFAULT CURRENT_TIMESTAMP,
                NguoiTao TEXT,
                FOREIGN KEY (MaNhom) REFERENCES NhomSanPham(MaNhom)
            );
        )";

        if (!ExecuteQuery(querySanPham)) {
            return false;
        }

        // TonKho table
        std::string queryTonKho = R"(
            CREATE TABLE IF NOT EXISTS TonKho (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                MaSP TEXT NOT NULL,
                SoLuongTon REAL DEFAULT 0,
                GiaTriTon REAL DEFAULT 0,
                NgayCapNhat TEXT DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (MaSP) REFERENCES SanPham(MaSP)
            );
        )";

        if (!ExecuteQuery(queryTonKho)) {
            return false;
        }

        // PhieuNhap table
        std::string queryPhieuNhap = R"(
            CREATE TABLE IF NOT EXISTS PhieuNhap (
                SoPhieu TEXT PRIMARY KEY,
                NgayNhap TEXT NOT NULL,
                NhaCungCap TEXT,
                NguoiNhap TEXT,
                TongTien REAL DEFAULT 0,
                GhiChu TEXT,
                TrangThai INTEGER DEFAULT 1
            );
        )";

        if (!ExecuteQuery(queryPhieuNhap)) {
            return false;
        }

        // ChiTietPhieuNhap table
        std::string queryChiTietNhap = R"(
            CREATE TABLE IF NOT EXISTS ChiTietPhieuNhap (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                SoPhieu TEXT NOT NULL,
                MaSP TEXT NOT NULL,
                SoLuong REAL NOT NULL,
                DonGia REAL NOT NULL,
                ThanhTien REAL NOT NULL,
                FOREIGN KEY (SoPhieu) REFERENCES PhieuNhap(SoPhieu),
                FOREIGN KEY (MaSP) REFERENCES SanPham(MaSP)
            );
        )";

        if (!ExecuteQuery(queryChiTietNhap)) {
            return false;
        }

        // PhieuXuat table
        std::string queryPhieuXuat = R"(
            CREATE TABLE IF NOT EXISTS PhieuXuat (
                SoPhieu TEXT PRIMARY KEY,
                NgayXuat TEXT NOT NULL,
                KhachHang TEXT,
                NguoiXuat TEXT,
                TongTien REAL DEFAULT 0,
                GhiChu TEXT,
                TrangThai INTEGER DEFAULT 1
            );
        )";

        if (!ExecuteQuery(queryPhieuXuat)) {
            return false;
        }

        // ChiTietPhieuXuat table
        std::string queryChiTietXuat = R"(
            CREATE TABLE IF NOT EXISTS ChiTietPhieuXuat (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                SoPhieu TEXT NOT NULL,
                MaSP TEXT NOT NULL,
                SoLuong REAL NOT NULL,
                DonGia REAL NOT NULL,
                ThanhTien REAL NOT NULL,
                FOREIGN KEY (SoPhieu) REFERENCES PhieuXuat(SoPhieu),
                FOREIGN KEY (MaSP) REFERENCES SanPham(MaSP)
            );
        )";

        return ExecuteQuery(queryChiTietXuat);
    }

    bool DatabaseManager::CreateKeToanTables() {
        // TaiKhoanKeToan table
        std::string queryTaiKhoan = R"(
            CREATE TABLE IF NOT EXISTS TaiKhoanKeToan (
                SoTK TEXT PRIMARY KEY,
                TenTK TEXT NOT NULL,
                LoaiTK INTEGER,
                TKCha TEXT,
                CapDo INTEGER DEFAULT 1,
                TrangThai INTEGER DEFAULT 1,
                FOREIGN KEY (TKCha) REFERENCES TaiKhoanKeToan(SoTK)
            );
        )";

        if (!ExecuteQuery(queryTaiKhoan)) {
            return false;
        }

        // ChungTuKeToan table
        std::string queryChungTu = R"(
            CREATE TABLE IF NOT EXISTS ChungTuKeToan (
                SoCT TEXT PRIMARY KEY,
                NgayCT TEXT NOT NULL,
                LoaiCT TEXT,
                DienGiai TEXT,
                NguoiLap TEXT,
                TrangThai INTEGER DEFAULT 1,
                NgayTao TEXT DEFAULT CURRENT_TIMESTAMP
            );
        )";

        if (!ExecuteQuery(queryChungTu)) {
            return false;
        }

        // DinhKhoan table
        std::string queryDinhKhoan = R"(
            CREATE TABLE IF NOT EXISTS DinhKhoan (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                SoCT TEXT NOT NULL,
                STT INTEGER,
                TKNo TEXT NOT NULL,
                TKCo TEXT NOT NULL,
                SoTien REAL NOT NULL,
                DienGiai TEXT,
                FOREIGN KEY (SoCT) REFERENCES ChungTuKeToan(SoCT),
                FOREIGN KEY (TKNo) REFERENCES TaiKhoanKeToan(SoTK),
                FOREIGN KEY (TKCo) REFERENCES TaiKhoanKeToan(SoTK)
            );
        )";

        if (!ExecuteQuery(queryDinhKhoan)) {
            return false;
        }

        // CongNo table
        std::string queryCongNo = R"(
            CREATE TABLE IF NOT EXISTS CongNo (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                LoaiCN INTEGER,
                MaDoiTuong TEXT,
                TenDoiTuong TEXT,
                SoCT TEXT,
                NgayCT TEXT,
                SoTien REAL NOT NULL,
                DaTra REAL DEFAULT 0,
                ConLai REAL,
                FOREIGN KEY (SoCT) REFERENCES ChungTuKeToan(SoCT)
            );
        )";

        return ExecuteQuery(queryCongNo);
    }

    bool DatabaseManager::BeginTransaction() {
        if (inTransaction_) {
            Logger::Warning("Transaction already in progress");
            return false;
        }

        if (ExecuteQuery("BEGIN TRANSACTION")) {
            inTransaction_ = true;
            return true;
        }

        return false;
    }

    bool DatabaseManager::Commit() {
        if (!inTransaction_) {
            Logger::Warning("No transaction to commit");
            return false;
        }

        if (ExecuteQuery("COMMIT")) {
            inTransaction_ = false;
            return true;
        }

        return false;
    }

    bool DatabaseManager::Rollback() {
        if (!inTransaction_) {
            Logger::Warning("No transaction to rollback");
            return false;
        }

        if (ExecuteQuery("ROLLBACK")) {
            inTransaction_ = false;
            return true;
        }

        return false;
    }

    bool DatabaseManager::ExecuteQuery(const std::string& query) {
        if (!connected_ || !connection_) {
            Logger::Error("Database not connected");
            return false;
        }

        return connection_->Execute(query);
    }

    bool DatabaseManager::ExecuteScalar(const std::string& query, std::string& result) {
        if (!connected_ || !connection_) {
            Logger::Error("Database not connected");
            return false;
        }

        return connection_->ExecuteScalar(query, result);
    }

    std::string DatabaseManager::GetSchemaVersion() {
        std::string version;
        std::string query = "SELECT Value FROM SystemInfo WHERE Key='SchemaVersion'";
        if (ExecuteScalar(query, version)) {
            return version;
        }
        return "";
    }

    bool DatabaseManager::SetSchemaVersion(const std::string& version) {
        std::string query = "INSERT OR REPLACE INTO SystemInfo (Key, Value) VALUES ('SchemaVersion', '"
                          + version + "')";
        return ExecuteQuery(query);
    }

    bool DatabaseManager::UpgradeSchema() {
        // TODO: Implement schema upgrade logic
        Logger::Info("Schema upgrade not yet implemented");
        return true;
    }

    bool DatabaseManager::InsertSampleData() {
        Logger::Info("Inserting sample catalog data...");

        // Insert sample product groups
        std::string insertNhomSP = R"(
            INSERT INTO NhomSanPham (MaNhom, TenNhom, MoTa) VALUES
            ('NHOM01', 'Văn phòng phẩm', 'Bút, giấy, dụng cụ văn phòng'),
            ('NHOM02', 'Điện tử', 'Máy tính, điện thoại, phụ kiện'),
            ('NHOM03', 'Nội thất', 'Bàn ghế, tủ, giá kệ'),
            ('NHOM04', 'Công cụ dụng cụ', 'Dụng cụ cầm tay, máy móc nhỏ');
        )";

        if (!ExecuteQuery(insertNhomSP)) {
            return false;
        }

        // Insert sample customers
        std::string insertKH = R"(
            INSERT INTO KhachHang (MaKH, TenKH, SoDT, DiaChi) VALUES
            ('KH001', 'Công ty TNHH ABC', '0901234567', '123 Nguyễn Huệ, Q.1, TP.HCM'),
            ('KH002', 'Công ty CP XYZ', '0912345678', '456 Lê Lợi, Q.3, TP.HCM'),
            ('KH003', 'Cửa hàng Minh Anh', '0923456789', '789 Trần Hưng Đạo, Q.5, TP.HCM');
        )";

        if (!ExecuteQuery(insertKH)) {
            return false;
        }

        // Insert sample suppliers
        std::string insertNCC = R"(
            INSERT INTO NhaCungCap (MaNCC, TenNCC, SoDT, DiaChi) VALUES
            ('NCC001', 'Công ty Vật tư Xây dựng', '0281234567', '12 Lý Thái Tổ, Q.10, TP.HCM'),
            ('NCC002', 'Nhà phân phối Điện máy', '0282345678', '34 Cách Mạng Tháng 8, Q.3, TP.HCM'),
            ('NCC003', 'Tập đoàn Thương mại', '0283456789', '56 Võ Văn Tần, Q.3, TP.HCM');
        )";

        if (!ExecuteQuery(insertNCC)) {
            return false;
        }

        // Insert sample warehouses
        std::string insertKho = R"(
            INSERT INTO Kho (MaKho, TenKho, DiaChi, QuanLy) VALUES
            ('KHO01', 'Kho trung tâm', '12 Nguyễn Trãi, Q.1, TP.HCM', 'Nguyễn Văn A'),
            ('KHO02', 'Kho chi nhánh 1', '34 Lê Văn Việt, Q.9, TP.HCM', 'Trần Thị B'),
            ('KHO03', 'Kho chi nhánh 2', '56 Quốc lộ 1A, Q.Bình Tân, TP.HCM', 'Lê Văn C');
        )";

        if (!ExecuteQuery(insertKho)) {
            return false;
        }

        // Insert sample products
        std::string insertSP = R"(
            INSERT INTO SanPham (MaSP, TenSP, DonViTinh, MaNhom, GiaMua, GiaBan, MoTa) VALUES
            ('SP001', 'Bút bi xanh Thiên Long', 'Cái', 'NHOM01', 2000, 3000, 'Bút viết văn phòng'),
            ('SP002', 'Vở 200 trang', 'Quyển', 'NHOM01', 15000, 20000, 'Vở kẻ ngang'),
            ('SP003', 'Kẹp tài liệu', 'Hộp', 'NHOM01', 25000, 35000, 'Hộp 12 cái');
        )";

        if (!ExecuteQuery(insertSP)) {
            return false;
        }

        // Insert sample accounting accounts
        std::string insertTK = R"(
            INSERT INTO TaiKhoanKeToan (SoTK, TenTK, LoaiTK) VALUES
            ('111', 'Tiền mặt', 1),
            ('112', 'Tiền gửi ngân hàng', 1),
            ('331', 'Phải trả người bán', 2),
            ('511', 'Doanh thu bán hàng', 3);
        )";

        if (!ExecuteQuery(insertTK)) {
            return false;
        }

        Logger::Info("Sample data inserted successfully");
        return true;
    }

} // namespace KeToanApp
