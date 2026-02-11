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

            // Check and create schema if needed
            if (!CheckSchema()) {
                Logger::Info("Database schema not found, creating...");
                if (!CreateTables()) {
                    Logger::Error("Failed to create database schema");
                    return false;
                }
            }

            connected_ = true;
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
        // SanPham table
        std::string querySanPham = R"(
            CREATE TABLE IF NOT EXISTS SanPham (
                MaSP TEXT PRIMARY KEY,
                TenSP TEXT NOT NULL,
                DonViTinh TEXT,
                GiaMua REAL DEFAULT 0,
                GiaBan REAL DEFAULT 0,
                MoTa TEXT,
                NhomHang TEXT,
                TrangThai INTEGER DEFAULT 1,
                NgayTao TEXT DEFAULT CURRENT_TIMESTAMP,
                NguoiTao TEXT
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

} // namespace KeToanApp
