#pragma once

#include "KeToanApp/Common.h"
#include "KeToanApp/Types.h"

namespace KeToanApp {
namespace DateTimeHelper {

    // Current date/time
    Date Today();
    std::string CurrentDateString(const std::string& format = "dd/MM/yyyy");
    std::string CurrentTimeString(const std::string& format = "HH:mm:ss");
    std::string CurrentDateTimeString(const std::string& format = "dd/MM/yyyy HH:mm:ss");

    // Date parsing
    Date ParseDate(const std::string& str, const std::string& format = "dd/MM/yyyy");
    bool TryParseDate(const std::string& str, Date& date, const std::string& format = "dd/MM/yyyy");

    // Date formatting
    std::string FormatDate(const Date& date, const std::string& format = "dd/MM/yyyy");

    // Date calculations
    int DaysBetween(const Date& date1, const Date& date2);
    Date AddDays(const Date& date, int days);
    Date AddMonths(const Date& date, int months);
    Date AddYears(const Date& date, int years);

    // Date validation
    bool IsValidDate(int day, int month, int year);
    bool IsLeapYear(int year);
    int DaysInMonth(int month, int year);

    // Date comparison
    bool IsBefore(const Date& date1, const Date& date2);
    bool IsAfter(const Date& date1, const Date& date2);
    bool IsEqual(const Date& date1, const Date& date2);

} // namespace DateTimeHelper
} // namespace KeToanApp
