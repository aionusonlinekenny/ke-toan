#pragma once

#ifndef KETOANAPP_COMMON_H
#define KETOANAPP_COMMON_H

// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

// Standard C++ headers
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>

// Application version
#define KETOANAPP_VERSION_MAJOR 1
#define KETOANAPP_VERSION_MINOR 0
#define KETOANAPP_VERSION_PATCH 0

// Macros
#ifdef _DEBUG
    #define KETOAN_DEBUG
#endif

// Helper macros
#define KETOAN_UNUSED(x) (void)(x)
#define KETOAN_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// String conversion helpers
#ifdef UNICODE
    using tstring = std::wstring;
    #define _T(x) L##x
#else
    using tstring = std::string;
    #define _T(x) x
#endif

// Namespace
namespace KeToanApp {
    // Forward declarations
    class Application;
    class MainWindow;
    class DatabaseManager;
}

#endif // KETOANAPP_COMMON_H
