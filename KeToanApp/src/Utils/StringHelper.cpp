#include "StringHelper.h"
#include <algorithm>
#include <cctype>
#include <cstdarg>

namespace KeToanApp {
namespace StringHelper {

    std::wstring ToWideString(const std::string& str) {
        if (str.empty()) return std::wstring();

        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
        return wstr;
    }

    std::string ToNarrowString(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();

        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string str(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
        return str;
    }

    std::string ToUTF8(const std::wstring& wstr) {
        return ToNarrowString(wstr);
    }

    std::wstring FromUTF8(const std::string& str) {
        return ToWideString(str);
    }

    std::string Trim(const std::string& str) {
        return TrimRight(TrimLeft(str));
    }

    std::string TrimLeft(const std::string& str) {
        auto it = std::find_if(str.begin(), str.end(),
            [](unsigned char ch) { return !std::isspace(ch); });
        return std::string(it, str.end());
    }

    std::string TrimRight(const std::string& str) {
        auto it = std::find_if(str.rbegin(), str.rend(),
            [](unsigned char ch) { return !std::isspace(ch); });
        return std::string(str.begin(), it.base());
    }

    std::vector<std::string> Split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }

        return result;
    }

    std::string Join(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::string result;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += delimiter;
            result += vec[i];
        }
        return result;
    }

    std::string ToUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    std::string ToLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    bool StartsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() &&
               str.compare(0, prefix.size(), prefix) == 0;
    }

    bool EndsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    bool Contains(const std::string& str, const std::string& substr) {
        return str.find(substr) != std::string::npos;
    }

    std::string Replace(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = result.find(from);
        if (pos != std::string::npos) {
            result.replace(pos, from.length(), to);
        }
        return result;
    }

    std::string ReplaceAll(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }

    std::string Format(const char* format, ...) {
        char buffer[4096];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        return buffer;
    }

} // namespace StringHelper
} // namespace KeToanApp
