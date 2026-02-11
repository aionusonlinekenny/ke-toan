@echo off
REM ========================================
REM Visual Studio Cache Cleaner
REM Run this AFTER closing Visual Studio
REM ========================================

echo.
echo ========================================
echo Visual Studio Cache Cleaner
echo ========================================
echo.
echo This will delete:
echo - .vs folder (IntelliSense cache)
echo - bin, obj folders (build outputs)
echo - Intermediate files (*.suo, *.user, etc.)
echo.
pause

echo.
echo Cleaning cache folders...
echo.

if exist ".vs" (
    echo Deleting .vs folder...
    rmdir /s /q .vs
    echo   Done!
) else (
    echo   .vs not found
)

if exist "bin" (
    echo Deleting bin folder...
    rmdir /s /q bin
    echo   Done!
) else (
    echo   bin not found
)

if exist "obj" (
    echo Deleting obj folder...
    rmdir /s /q obj
    echo   Done!
) else (
    echo   obj not found
)

if exist "x64" (
    echo Deleting x64 folder...
    rmdir /s /q x64
    echo   Done!
) else (
    echo   x64 not found
)

if exist "Debug" (
    echo Deleting Debug folder...
    rmdir /s /q Debug
    echo   Done!
) else (
    echo   Debug not found
)

if exist "Release" (
    echo Deleting Release folder...
    rmdir /s /q Release
    echo   Done!
) else (
    echo   Release not found
)

if exist "ipch" (
    echo Deleting ipch folder...
    rmdir /s /q ipch
    echo   Done!
) else (
    echo   ipch not found
)

echo.
echo Deleting intermediate files...
echo.

del /s /q *.suo 2>nul
del /s /q *.user 2>nul
del /s /q *.aps 2>nul
del /s /q *.ncb 2>nul
del /s /q *.opendb 2>nul
del /s /q *.opensdf 2>nul
del /s /q *.sdf 2>nul
del /s /q *.db 2>nul
del /s /q *.ipch 2>nul

echo   Done!

echo.
echo ========================================
echo Cache cleaned successfully!
echo ========================================
echo.
echo Next steps:
echo 1. Open Visual Studio 2022
echo 2. Open KeToanApp.sln
echo 3. Build ^> Rebuild Solution
echo.
pause
