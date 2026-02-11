#include "QueryBuilder.h"

namespace KeToanApp {

    QueryBuilder::QueryBuilder()
        : query_("")
        , table_("")
        , conditions_()
        , hasWhere_(false)
    {
    }

    QueryBuilder& QueryBuilder::Select(const std::string& columns) {
        query_ = "SELECT " + columns;
        return *this;
    }

    QueryBuilder& QueryBuilder::From(const std::string& table) {
        table_ = table;
        query_ += " FROM " + table;
        return *this;
    }

    QueryBuilder& QueryBuilder::Where(const std::string& condition) {
        if (!hasWhere_) {
            query_ += " WHERE " + condition;
            hasWhere_ = true;
        } else {
            query_ += " AND " + condition;
        }
        return *this;
    }

    QueryBuilder& QueryBuilder::And(const std::string& condition) {
        if (hasWhere_) {
            query_ += " AND " + condition;
        }
        return *this;
    }

    QueryBuilder& QueryBuilder::Or(const std::string& condition) {
        if (hasWhere_) {
            query_ += " OR " + condition;
        }
        return *this;
    }

    QueryBuilder& QueryBuilder::OrderBy(const std::string& column, bool ascending) {
        query_ += " ORDER BY " + column + (ascending ? " ASC" : " DESC");
        return *this;
    }

    QueryBuilder& QueryBuilder::Limit(int count) {
        query_ += " LIMIT " + std::to_string(count);
        return *this;
    }

    QueryBuilder& QueryBuilder::InsertInto(const std::string& table) {
        table_ = table;
        query_ = "INSERT INTO " + table;
        return *this;
    }

    QueryBuilder& QueryBuilder::Values(const std::vector<std::string>& values) {
        query_ += " VALUES (";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) query_ += ", ";
            query_ += "'" + values[i] + "'";
        }
        query_ += ")";
        return *this;
    }

    QueryBuilder& QueryBuilder::Update(const std::string& table) {
        table_ = table;
        query_ = "UPDATE " + table;
        return *this;
    }

    QueryBuilder& QueryBuilder::Set(const std::string& column, const std::string& value) {
        if (query_.find(" SET ") == std::string::npos) {
            query_ += " SET ";
        } else {
            query_ += ", ";
        }
        query_ += column + " = '" + value + "'";
        return *this;
    }

    QueryBuilder& QueryBuilder::DeleteFrom(const std::string& table) {
        table_ = table;
        query_ = "DELETE FROM " + table;
        return *this;
    }

    std::string QueryBuilder::Build() {
        return query_;
    }

    void QueryBuilder::Reset() {
        query_.clear();
        table_.clear();
        conditions_.clear();
        hasWhere_ = false;
    }

} // namespace KeToanApp
