#include "DateTimeHelper.h"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace KeToanApp {
namespace DateTimeHelper {

    Date Today() {
        time_t now = time(nullptr);
        tm timeinfo;
        localtime_s(&timeinfo, &now);
        return Date(timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    }

    std::string CurrentDateString(const std::string& format) {
        KETOAN_UNUSED(format);
        return Today().ToString();
    }

    std::string CurrentTimeString(const std::string& format) {
        KETOAN_UNUSED(format);

        time_t now = time(nullptr);
        tm timeinfo;
        localtime_s(&timeinfo, &now);

        char buffer[32];
        sprintf_s(buffer, sizeof(buffer), "%02d:%02d:%02d",
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        return buffer;
    }

    std::string CurrentDateTimeString(const std::string& format) {
        KETOAN_UNUSED(format);
        return CurrentDateString() + " " + CurrentTimeString();
    }

    Date ParseDate(const std::string& str, const std::string& format) {
        KETOAN_UNUSED(format);

        // Simple dd/MM/yyyy parser
        int day, month, year;
        char sep1, sep2;

        std::stringstream ss(str);
        ss >> day >> sep1 >> month >> sep2 >> year;

        if (ss.fail() || sep1 != '/' || sep2 != '/') {
            return Date();
        }

        return Date(day, month, year);
    }

    bool TryParseDate(const std::string& str, Date& date, const std::string& format) {
        try {
            date = ParseDate(str, format);
            return IsValidDate(date.day, date.month, date.year);
        }
        catch (...) {
            return false;
        }
    }

    std::string FormatDate(const Date& date, const std::string& format) {
        KETOAN_UNUSED(format);
        return date.ToString();
    }

    int DaysBetween(const Date& date1, const Date& date2) {
        // Simplified calculation
        tm tm1 = { 0 };
        tm1.tm_mday = date1.day;
        tm1.tm_mon = date1.month - 1;
        tm1.tm_year = date1.year - 1900;

        tm tm2 = { 0 };
        tm2.tm_mday = date2.day;
        tm2.tm_mon = date2.month - 1;
        tm2.tm_year = date2.year - 1900;

        time_t time1 = mktime(&tm1);
        time_t time2 = mktime(&tm2);

        return static_cast<int>(difftime(time2, time1) / (60 * 60 * 24));
    }

    Date AddDays(const Date& date, int days) {
        tm timeinfo = { 0 };
        timeinfo.tm_mday = date.day;
        timeinfo.tm_mon = date.month - 1;
        timeinfo.tm_year = date.year - 1900;

        time_t time = mktime(&timeinfo);
        time += days * 24 * 60 * 60;

        localtime_s(&timeinfo, &time);
        return Date(timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    }

    Date AddMonths(const Date& date, int months) {
        int newMonth = date.month + months;
        int newYear = date.year;

        while (newMonth > 12) {
            newMonth -= 12;
            newYear++;
        }

        while (newMonth < 1) {
            newMonth += 12;
            newYear--;
        }

        int daysInNewMonth = DaysInMonth(newMonth, newYear);
        int newDay = (date.day > daysInNewMonth) ? daysInNewMonth : date.day;

        return Date(newDay, newMonth, newYear);
    }

    Date AddYears(const Date& date, int years) {
        return Date(date.day, date.month, date.year + years);
    }

    bool IsValidDate(int day, int month, int year) {
        if (year < 1900 || year > 9999) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > DaysInMonth(month, year)) return false;
        return true;
    }

    bool IsLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    int DaysInMonth(int month, int year) {
        static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if (month < 1 || month > 12) return 0;

        if (month == 2 && IsLeapYear(year)) {
            return 29;
        }

        return days[month - 1];
    }

    bool IsBefore(const Date& date1, const Date& date2) {
        if (date1.year != date2.year) return date1.year < date2.year;
        if (date1.month != date2.month) return date1.month < date2.month;
        return date1.day < date2.day;
    }

    bool IsAfter(const Date& date1, const Date& date2) {
        return IsBefore(date2, date1);
    }

    bool IsEqual(const Date& date1, const Date& date2) {
        return date1.day == date2.day &&
               date1.month == date2.month &&
               date1.year == date2.year;
    }

} // namespace DateTimeHelper

// Implementation of Date::Today() static method
Date Date::Today() {
    return DateTimeHelper::Today();
}

} // namespace KeToanApp
