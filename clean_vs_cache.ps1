# ========================================
# Visual Studio Cache Cleaner (PowerShell)
# Run this AFTER closing Visual Studio
# ========================================

Write-Host ""
Write-Host "========================================"
Write-Host "Visual Studio Cache Cleaner"
Write-Host "========================================"
Write-Host ""
Write-Host "This will delete:"
Write-Host "- .vs folder (IntelliSense cache)"
Write-Host "- bin, obj folders (build outputs)"
Write-Host "- Intermediate files (*.suo, *.user, etc.)"
Write-Host ""
Write-Host "Press any key to continue..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

Write-Host ""
Write-Host "Cleaning cache folders..."
Write-Host ""

# Delete folders
$folders = @(".vs", "bin", "obj", "x64", "Debug", "Release", "ipch")
foreach ($folder in $folders) {
    if (Test-Path $folder) {
        Write-Host "Deleting $folder folder..." -ForegroundColor Yellow
        Remove-Item -Path $folder -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "  Done!" -ForegroundColor Green
    } else {
        Write-Host "  $folder not found" -ForegroundColor Gray
    }
}

Write-Host ""
Write-Host "Deleting intermediate files..."
Write-Host ""

# Delete files
$patterns = @("*.suo", "*.user", "*.aps", "*.ncb", "*.opendb", "*.opensdf", "*.sdf", "*.db", "*.ipch")
foreach ($pattern in $patterns) {
    $files = Get-ChildItem -Path . -Filter $pattern -Recurse -ErrorAction SilentlyContinue
    if ($files) {
        $files | Remove-Item -Force -ErrorAction SilentlyContinue
        Write-Host "  Deleted $($files.Count) $pattern files"
    }
}

Write-Host "  Done!" -ForegroundColor Green

Write-Host ""
Write-Host "========================================"
Write-Host "Cache cleaned successfully!" -ForegroundColor Green
Write-Host "========================================"
Write-Host ""
Write-Host "Next steps:"
Write-Host "1. Open Visual Studio 2022"
Write-Host "2. Open KeToanApp.sln"
Write-Host "3. Build > Rebuild Solution"
Write-Host ""
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
