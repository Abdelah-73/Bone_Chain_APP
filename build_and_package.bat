@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo =============================================
echo  Bone Fertilizer OS — Build ^& Package
echo =============================================

REM ── Config ──────────────────────────────────
set BUILD_DIR=build
set INSTALL_DIR=install
set QT_DIR=C:\Qt\6.8.3\msvc2022_64
set NSIS_DIR=C:\Program Files (x86)\NSIS

REM ── 1. Configure with CMake ─────────────────
echo [1/4] Configuring CMake...
cmake -S . -B %BUILD_DIR% -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH=%QT_DIR% -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto error

REM ── 2. Build ────────────────────────────────
echo [2/4] Building...
cmake --build %BUILD_DIR% --config Release -j
if errorlevel 1 goto error

REM ── 3. Deploy Qt DLLs with windeployqt ─────
echo [3/4] Deploying Qt dependencies...
set PATH=%QT_DIR%\bin;%PATH%
windeployqt --release --dir %INSTALL_DIR%\bin %BUILD_DIR%\Release\BoneFertilizerApp.exe
if errorlevel 1 goto error

REM Copy data, themes, assets, i18n
xcopy /E /I /Y Data %INSTALL_DIR%\Data
xcopy /E /I /Y Themes %INSTALL_DIR%\Themes
xcopy /E /I /Y Assets %INSTALL_DIR%\Assets
xcopy /E /I /Y i18n %INSTALL_DIR%\i18n

REM Copy the exe
copy /Y %BUILD_DIR%\Release\BoneFertilizerApp.exe %INSTALL_DIR%\bin\

REM ── 4. Package with CPack ──────────────────
echo [4/4] Creating NSIS installer...
cd %BUILD_DIR%
cpack -G NSIS -C Release
if errorlevel 1 goto error

echo =============================================
echo  SUCCESS! Installer created in:
echo  %CD%\_CPack_Packages\win64\NSIS\
echo  or %CD%\BoneFertilizerApp-1.0.0-win64.exe
echo =============================================
goto end

:error
echo =============================================
echo  ERROR! Build failed. Review messages above.
echo =============================================
exit /b 1

:end
endlocal
pause
