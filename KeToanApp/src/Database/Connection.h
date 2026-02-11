#pragma once

#include "KeToanApp/Common.h"
#include "KeToanApp/Types.h"

// Forward declaration for SQLite
struct sqlite3;

namespace KeToanApp {

    class Connection {
    public:
        explicit Connection(const AppSettings& settings);
        ~Connection();

        // Connection management
        bool Open();
        void Close();
        bool IsOpen() const { return isOpen_; }

        // Query execution
        bool Execute(const std::string& query);
        bool ExecuteScalar(const std::string& query, std::string& result);

        // Prepared statements support
        // TODO: Add prepared statement methods

        // Error handling
        std::string GetLastError() const { return lastError_; }

    private:
        AppSettings settings_;
        sqlite3* db_;
        bool isOpen_;
        std::string lastError_;

        // Helper methods
        void SetLastError(const std::string& error);
    };

} // namespace KeToanApp
