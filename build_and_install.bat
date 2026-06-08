@echo off
setlocal enabledelayedexpansion

chcp 65001 >nul 2>&1

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

title Kexi Build + Install

echo ============================================
echo  Kexi Build + Install - Windows ^& Linux
echo ============================================
echo.

set "VS_PATH=C:\Program Files\Microsoft Visual Studio\18\Community"
set "CMAKE_EXE=%VS_PATH%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "VCPKG_DIR=C:\vcpkg"
set "VCPKG_TRIPLET=x64-windows"
set "BUILD_DIR=%SCRIPT_DIR%\build_msvc"
set "INSTALL_DIR=%BUILD_DIR%\install"
set "LINUX_BUILD_DIR=%SCRIPT_DIR%\build-linux"

echo [System]
echo   Script dir: %SCRIPT_DIR%
echo   VS:         %VS_PATH%
echo   cmake:      %CMAKE_EXE%
echo   vcpkg:      %VCPKG_DIR%\%VCPKG_TRIPLET%
echo   Build dir:  %BUILD_DIR%
echo.

if not exist "%CMAKE_EXE%" (
    echo ERROR: cmake not found. Install Visual Studio 18 with CMake tools.
    pause
    exit /b 1
)
if not exist "%VCPKG_DIR%" (
    echo ERROR: vcpkg not found at %VCPKG_DIR%
    pause
    exit /b 1
)
"%CMAKE_EXE%" --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: cmake is not functional.
    pause
    exit /b 1
)

echo [Ready] Starting build...
echo.

rem ===== 1. WINDOWS BUILD =====
echo =================================================================
echo  [1] Windows build (MSVC Debug)
echo =================================================================
echo.

call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 (
    echo ERROR: Failed to load VS environment.
    pause
    exit /b 1
)

set "STUBS_DIR=%SCRIPT_DIR%\stubs"
set "KDEWIN_DIR=%STUBS_DIR%\KDEWin"

if not exist "%KDEWIN_DIR%\KDEWinConfig.cmake" (
    echo [INFO] Creating KDEWin stub for cmake configure...
    if not exist "%KDEWIN_DIR%" mkdir "%KDEWIN_DIR%"
)

echo [Configuring]...
"%CMAKE_EXE%" -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -G "Visual Studio 18 2026" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=%VCPKG_TRIPLET% -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DECM_DIR="%VCPKG_DIR%\installed\%VCPKG_TRIPLET%\share\ECM" -DKDEWin_DIR="%KDEWIN_DIR%" -DKDb_DIR="%STUBS_DIR%\KDb" -DKPropertyCore_DIR="%STUBS_DIR%\KPropertyCore3" -DKPropertyWidgets_DIR="%STUBS_DIR%\KPropertyWidgets3" -DKReport_DIR="%STUBS_DIR%\KReport3"
if errorlevel 1 goto :warn_win_deps

echo [Building]...
"%CMAKE_EXE%" --build "%BUILD_DIR%" --config Debug -j%NUMBER_OF_PROCESSORS%
if errorlevel 1 goto :fail

echo [Installing]...
"%CMAKE_EXE%" --install "%BUILD_DIR%" --config Debug --prefix "%INSTALL_DIR%"
if errorlevel 1 echo [WARN] install failed.

if exist "%INSTALL_DIR%\bin\kexi.exe" (
    echo [Copying DLLs]...
    for %%f in (bin\Debug\*.dll) do (
        if exist "%BUILD_DIR%\%%f" copy /y "%BUILD_DIR%\%%f" "%INSTALL_DIR%\bin\" >nul 2>&1
    )
)
echo [OK] Windows build succeeded.
echo.
goto :build_linux

:warn_win_deps
echo [SKIP] Windows build cannot proceed.
echo   Missing dependencies. To build for Windows, install:
echo.
echo   1. KDEWin   - https://github.com/KDE/kdewin
echo   2. KDb3     - https://github.com/KDE/kdb
echo   3. KProperty3
echo   4. KReport3
echo.
echo   Place built deps in deps/installed/ then re-run.
echo   Continuing with Linux build...
echo.

rem ===== 2. LINUX BUILD (via WSL) =====
:build_linux
echo =================================================================
echo  [2] Linux build (WSL Ubuntu)
echo =================================================================
echo.

wsl --status >nul 2>&1
if errorlevel 1 (
    echo [SKIP] WSL is not available.
    echo   To build for Linux: wsl --install -d Ubuntu
    echo   Then run: bash build.sh linux install
    goto :summary
)

wsl -d Ubuntu echo "ready" >nul 2>&1
if errorlevel 1 (
    echo [INFO] Starting WSL Ubuntu...
    wsl -d Ubuntu bash -c "echo WSL ready" >nul 2>&1
)

echo [Running build.sh linux from WSL]...
echo.

set "WSL_SCRIPT_DIR=%SCRIPT_DIR:\=/%"
set "WSL_SCRIPT_DIR=%WSL_SCRIPT_DIR:C:=/mnt/c%"
set "WSL_SCRIPT_DIR=%WSL_SCRIPT_DIR:D:=/mnt/d%"
set "WSL_LINUX_BUILD=%LINUX_BUILD_DIR:\=/%"
set "WSL_LINUX_BUILD=%WSL_LINUX_BUILD:C:=/mnt/c%"
set "WSL_LINUX_BUILD=%WSL_LINUX_BUILD:D:=/mnt/d%"

wsl -d Ubuntu bash -c "cd '!WSL_SCRIPT_DIR!' && bash build.sh linux install"
if errorlevel 1 (
    echo [WARN] Linux build exited with errors.
) else (
    echo [OK] Linux build succeeded.
)

if exist "%LINUX_BUILD_DIR%\bin\kexi" (
    echo [Install confirmed] %LINUX_BUILD_DIR%\bin\kexi
)
echo.

rem ===== 3. WINDOWS BUILD FROM WSL =====
echo =================================================================
echo  [3] Windows build from WSL (MSVC cross)
echo =================================================================
echo.

wsl -d Ubuntu bash -c "cd '!WSL_SCRIPT_DIR!' && bash build.sh windows install"
if errorlevel 1 (
    echo [SKIP] Windows cross-build from WSL failed or skipped.
    echo   Windows may still build from [1] after deps are installed.
) else (
    echo [OK] Windows build from WSL succeeded.
)
echo.

rem ===== 4. SUMMARY =====
:summary
echo =================================================================
echo  [4] Build artifacts
echo =================================================================
echo.

if exist "%INSTALL_DIR%\bin\kexi.exe" (
    echo  [Windows] %INSTALL_DIR%\bin\kexi.exe
) else if exist "%BUILD_DIR%\bin\Debug\kexi.exe" (
    echo  [Windows] %BUILD_DIR%\bin\Debug\kexi.exe
) else if exist "%SCRIPT_DIR%\build_msvc\install\bin\kexi.exe" (
    echo  [Windows] %SCRIPT_DIR%\build_msvc\install\bin\kexi.exe
) else (
    echo  [Windows] Not built.
)

if exist "%LINUX_BUILD_DIR%\bin\kexi" (
    echo  [Linux]   %LINUX_BUILD_DIR%\bin\kexi
) else if exist "%LINUX_BUILD_DIR%\install\bin\kexi" (
    echo  [Linux]   %LINUX_BUILD_DIR%\install\bin\kexi
) else (
    echo  [Linux]   Not built.
)
echo.

echo =================================================================
echo  Done
echo =================================================================
echo.
echo  Windows: %INSTALL_DIR%\bin\kexi.exe
echo  Linux:   %LINUX_BUILD_DIR%\bin\kexi
echo.
echo  Build logs: %BUILD_DIR%  /  %LINUX_BUILD_DIR%
echo.

pause
exit /b 0

:fail
echo.
echo [FAIL] Windows build failed. Check '%BUILD_DIR%' for details.
pause
exit /b 1
