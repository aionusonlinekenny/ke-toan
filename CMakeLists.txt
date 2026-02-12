cmake_minimum_required(VERSION 3.20)
project(KeToanApp VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/KeToanApp/include
    ${PROJECT_SOURCE_DIR}/KeToanApp/src
    ${PROJECT_SOURCE_DIR}/lib
)

# Source files
set(CORE_SOURCES
    KeToanApp/src/Core/Application.cpp
    KeToanApp/src/Core/Config.cpp
)

set(DATABASE_SOURCES
    KeToanApp/src/Database/DatabaseManager.cpp
    KeToanApp/src/Database/Connection.cpp
    KeToanApp/src/Database/QueryBuilder.cpp
)

set(UI_SOURCES
    KeToanApp/src/UI/MainWindow.cpp
    KeToanApp/src/UI/Windows/ProductForm.cpp
)

set(UTILS_SOURCES
    KeToanApp/src/Utils/Logger.cpp
    KeToanApp/src/Utils/StringHelper.cpp
    KeToanApp/src/Utils/DateTimeHelper.cpp
)

# Header files
set(HEADERS
    KeToanApp/include/KeToanApp/Common.h
    KeToanApp/include/KeToanApp/Types.h
    KeToanApp/src/Core/Application.h
    KeToanApp/src/Core/Config.h
    KeToanApp/src/Database/DatabaseManager.h
    KeToanApp/src/Database/Connection.h
    KeToanApp/src/UI/MainWindow.h
    KeToanApp/src/UI/Windows/ProductForm.h
    KeToanApp/src/Utils/Logger.h
)

# Main executable
add_executable(KeToanApp
    KeToanApp/src/main.cpp
    ${CORE_SOURCES}
    ${DATABASE_SOURCES}
    ${UI_SOURCES}
    ${UTILS_SOURCES}
    ${HEADERS}
)

# Windows specific settings
if(WIN32)
    target_compile_definitions(KeToanApp PRIVATE UNICODE _UNICODE)
    target_link_libraries(KeToanApp PRIVATE comctl32)

    # Set subsystem to Windows (GUI application)
    set_target_properties(KeToanApp PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(KeToanApp PRIVATE
        /W4
        /utf-8  # Force UTF-8 source and execution character sets
    )
else()
    target_compile_options(KeToanApp PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation
install(TARGETS KeToanApp DESTINATION bin)
install(DIRECTORY resources/ DESTINATION resources)
install(FILES README.md MIGRATION_PLAN.md DESTINATION .)
