#pragma once

#ifndef KETOANAPP_TYPES_H
#define KETOANAPP_TYPES_H

#include "Common.h"
#include <ctime>
#include <chrono>

namespace KeToanApp {

    // Enumerations
    enum class DatabaseType {
        SQLite,
        SQLServer
    };

    enum class TrangThai {
        KhongHoatDong = 0,
        HoatDong = 1,
        DaXoa = 2
    };

    enum class LoaiTaiKhoan {
        TaiSan = 1,
        NguonVon = 2,
        ThuNhap = 3,
        ChiPhi = 4
    };

    enum class LoaiCongNo {
        PhaiThu = 1,
        PhaiTra = 2
    };

    // Basic structures
    struct Date {
        int day;
        int month;
        int year;

        Date() : day(0), month(0), year(0) {}
        Date(int d, int m, int y) : day(d), month(m), year(y) {}

        std::string ToString() const {
            char buffer[32];
            sprintf_s(buffer, "%02d/%02d/%04d", day, month, year);
            return buffer;
        }

        static Date Today();
    };

    struct Decimal {
        double value;

        Decimal() : value(0.0) {}
        Decimal(double v) : value(v) {}

        operator double() const { return value; }

        std::string ToString(int precision = 2) const {
            char buffer[64];
            sprintf_s(buffer, "%.*f", precision, value);
            return buffer;
        }
    };

    // Common data structures
    struct AppSettings {
        std::string databasePath;
        DatabaseType dbType;
        std::string language;
        std::string dateFormat;
        int numberPrecision;

        AppSettings()
            : databasePath("./data/ketoan.db")
            , dbType(DatabaseType::SQLite)
            , language("vi-VN")
            , dateFormat("dd/MM/yyyy")
            , numberPrecision(2)
        {}
    };

    // Exception types
    class KeToanException : public std::runtime_error {
    public:
        explicit KeToanException(const std::string& message)
            : std::runtime_error(message) {}
    };

    class DatabaseException : public KeToanException {
    public:
        explicit DatabaseException(const std::string& message)
            : KeToanException("Database Error: " + message) {}
    };

    class ValidationException : public KeToanException {
    public:
        explicit ValidationException(const std::string& message)
            : KeToanException("Validation Error: " + message) {}
    };

} // namespace KeToanApp

#endif // KETOANAPP_TYPES_H
