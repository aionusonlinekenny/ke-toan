#pragma once

#include "KeToanApp/Common.h"

namespace KeToanApp {

    class QueryBuilder {
    public:
        QueryBuilder();
        ~QueryBuilder() = default;

        // SELECT query building
        QueryBuilder& Select(const std::string& columns = "*");
        QueryBuilder& From(const std::string& table);
        QueryBuilder& Where(const std::string& condition);
        QueryBuilder& And(const std::string& condition);
        QueryBuilder& Or(const std::string& condition);
        QueryBuilder& OrderBy(const std::string& column, bool ascending = true);
        QueryBuilder& Limit(int count);

        // INSERT query building
        QueryBuilder& InsertInto(const std::string& table);
        QueryBuilder& Values(const std::vector<std::string>& values);

        // UPDATE query building
        QueryBuilder& Update(const std::string& table);
        QueryBuilder& Set(const std::string& column, const std::string& value);

        // DELETE query building
        QueryBuilder& DeleteFrom(const std::string& table);

        // Build and reset
        std::string Build();
        void Reset();

    private:
        std::string query_;
        std::string table_;
        std::vector<std::string> conditions_;
        bool hasWhere_;
    };

} // namespace KeToanApp
