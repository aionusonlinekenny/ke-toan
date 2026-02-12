#pragma once

#include "KeToanApp/Common.h"
#include "KeToanApp/Types.h"
#include "Connection.h"

namespace KeToanApp {

    class DatabaseManager {
    public:
        explicit DatabaseManager(const AppSettings& settings);
        ~DatabaseManager();

        // Connection management
        bool Connect();
        void Disconnect();
        bool IsConnected() const { return connected_; }

        // Database operations
        bool CreateTables();
        bool UpgradeSchema();
        bool CheckSchema();

        // Transaction support
        bool BeginTransaction();
        bool Commit();
        bool Rollback();

        // Query execution
        bool ExecuteQuery(const std::string& query);
        bool ExecuteScalar(const std::string& query, std::string& result);

        // Getters
        Connection* GetConnection() { return connection_.get(); }

    private:
        AppSettings settings_;
        std::unique_ptr<Connection> connection_;
        bool connected_;
        bool inTransaction_;

        // Schema management
        bool CreateKhoTables();
        bool CreateKeToanTables();
        bool CreateSystemTables();
        bool InsertSampleData();

        // Helper methods
        std::string GetSchemaVersion();
        bool SetSchemaVersion(const std::string& version);
    };

} // namespace KeToanApp
