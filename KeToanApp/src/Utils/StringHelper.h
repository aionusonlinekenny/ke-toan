#pragma once

#include "KeToanApp/Common.h"

namespace KeToanApp {
namespace StringHelper {

    // String conversion
    std::wstring ToWideString(const std::string& str);
    std::string ToNarrowString(const std::wstring& wstr);
    std::string ToUTF8(const std::wstring& wstr);
    std::wstring FromUTF8(const std::string& str);

    // String manipulation
    std::string Trim(const std::string& str);
    std::string TrimLeft(const std::string& str);
    std::string TrimRight(const std::string& str);

    std::vector<std::string> Split(const std::string& str, char delimiter);
    std::string Join(const std::vector<std::string>& vec, const std::string& delimiter);

    std::string ToUpper(const std::string& str);
    std::string ToLower(const std::string& str);

    bool StartsWith(const std::string& str, const std::string& prefix);
    bool EndsWith(const std::string& str, const std::string& suffix);
    bool Contains(const std::string& str, const std::string& substr);

    std::string Replace(const std::string& str, const std::string& from, const std::string& to);
    std::string ReplaceAll(const std::string& str, const std::string& from, const std::string& to);

    // Formatting
    std::string Format(const char* format, ...);

} // namespace StringHelper
} // namespace KeToanApp
