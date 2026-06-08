#!/bin/bash
# Build script for Akexi
# Supports Linux (native/WSL), Windows (MSVC + vcpkg), and both
# Usage: ./build.sh [linux|windows|both|all|clean]
#        ./build.sh [linux|windows] install  (build + install)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_TYPE="${1:-}"
DO_INSTALL="${2:-}"

# --- Detect platform ---
case "$(uname -s)" in
    Linux*)  PLATFORM="linux" ;;
    MINGW*|MSYS*|CYGWIN*) PLATFORM="windows" ;;
    *)       echo "Unknown platform: $(uname -s)"; exit 1 ;;
esac

# --- Default target ---
if [ -z "$BUILD_TYPE" ]; then
    BUILD_TYPE="$PLATFORM"
fi
if [ "$BUILD_TYPE" = "all" ]; then
    BUILD_TYPE="both"
fi

NPROC="$(nproc 2>/dev/null || echo 4)"

# --- Windows cmake path (for cross-build from WSL) ---
CMAKE_MSVC="/mnt/c/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
VCPKG_DIR="/mnt/c/vcpkg"
VCPKG_TRIPLET="x64-windows"
VCPKG_INSTALLED="$VCPKG_DIR/installed/$VCPKG_TRIPLET"

# ============================================================
#  Utility functions
# ============================================================
clean_build() {
    local dir="$1"
    if [ -d "$dir" ]; then
        echo "=== Cleaning $dir ==="
        rm -rf "$dir"
    fi
}

cmake_install() {
    local build_dir="$1"
    local prefix="$2"
    if command -v cmake &>/dev/null; then
        cmake --install "$build_dir" --prefix "$prefix" 2>/dev/null || true
    fi
}

check_windows_deps() {
    local deps_dir="$SCRIPT_DIR/deps/installed"
    local missing=()
    for dep in KDb3 KPropertyCore3 KPropertyWidgets3 KReport3; do
        if [ ! -f "$deps_dir/lib/cmake/$dep/${dep}Config.cmake" ]; then
            missing+=("$dep")
        fi
    done
    if [ ! -f "$deps_dir/lib/cmake/KDEWin/KDEWinConfig.cmake" ]; then
        missing+=("KDEWin")
    fi
    echo "${missing[@]}"
}

install_vcpkg_pkgs() {
    if [ -f "$VCPKG_DIR/vcpkg.exe" ]; then
        echo "=== Installing vcpkg packages ==="
        "$VCPKG_DIR/vcpkg.exe" install --triplet "$VCPKG_TRIPLET" 2>/dev/null || true
    fi
}

# ============================================================
#  Linux / WSL native build
# ============================================================
build_linux() {
    local BUILD_DIR="$SCRIPT_DIR/build-linux"
    local INSTALL_DIR="${BUILD_DIR}/install"

    echo "=== Building Akexi for Linux ==="
    echo "Build dir: $BUILD_DIR"

    cmake -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5

    cmake --build "$BUILD_DIR" -j"$NPROC"

    if [ "$DO_INSTALL" = "install" ]; then
        echo "=== Installing Linux build ==="
        cmake --install "$BUILD_DIR" --prefix "$INSTALL_DIR" 2>/dev/null || true
    fi

    # --- Post-build: copy plugins to binary-relative paths for development ---
    local AKEXI_DISTRIBUTION_VERSION
    AKEXI_DISTRIBUTION_VERSION=$(grep 'AKEXI_DISTRIBUTION_VERSION' "$BUILD_DIR/src/KexiVersion.h" 2>/dev/null | grep -v '^\s*/\*\|^\s*\*' | sed 's/.*"\(.*\)".*/\1/' | head -1)
    : "${AKEXI_DISTRIBUTION_VERSION:=0.0}"

    local BIN_DIR="$BUILD_DIR/bin"
    local PLUGIN_DIR="$BIN_DIR/akexi/$AKEXI_DISTRIBUTION_VERSION"
    local FORM_WIDGETS_DIR="$PLUGIN_DIR/forms/widgets"
    local MIGRATE_DIR="$PLUGIN_DIR/migrate"
    local REAL_BIN="$BIN_DIR/akexi-$AKEXI_DISTRIBUTION_VERSION"

    mkdir -p "$PLUGIN_DIR" "$FORM_WIDGETS_DIR" "$MIGRATE_DIR" 2>/dev/null || true

    for plugin in akexi_tableplugin akexi_queryplugin akexi_formplugin \
                  akexi_reportplugin akexi_migrationplugin \
                  akexi_csvimportexportplugin; do
        local found
        found=$(find "$BUILD_DIR/src" -name "${plugin}.so" -type f 2>/dev/null | head -1)
        if [ -n "$found" ]; then
            cp -u "$found" "$PLUGIN_DIR/" 2>/dev/null || true
        fi
    done

    for widget in akenejie.akexi-project.form.widgets.main \
                  akenejie.akexi-project.form.widgets.web-browser; do
        local found
        found=$(find "$BUILD_DIR/src" -name "${widget}.so" -type f 2>/dev/null | head -1)
        if [ -n "$found" ]; then
            cp -u "$found" "$FORM_WIDGETS_DIR/" 2>/dev/null || true
        fi
    done

    for migrate_driver in akeximigrate_mysql akeximigrate_mdb akeximigrate_tsv; do
        local found
        found=$(find "$BUILD_DIR/plugins" -name "${migrate_driver}.so" -type f 2>/dev/null | head -1)
        if [ -n "$found" ]; then
            cp -u "$found" "$MIGRATE_DIR/" 2>/dev/null || true
        fi
    done

    echo "=== Linux build complete ==="
    if [ -f "$REAL_BIN" ]; then
        echo "Binary: $REAL_BIN"
    fi
}

# ============================================================
#  Windows build using MSVC + vcpkg (from WSL or native)
# ============================================================
build_windows() {
    local BUILD_DIR="$SCRIPT_DIR/build_msvc"
    local BIN_DIR="$BUILD_DIR/bin/Debug"
    local INSTALL_DIR="${BUILD_DIR}/install"
    local DEPS_DIR="$SCRIPT_DIR/deps/installed"
    local CMAKE_CMD=""

    echo "=== Building Akexi for Windows ==="
    echo "Build dir: $BUILD_DIR"

    # Detect if we're running from WSL (can use Windows cmake) or native Windows
    if [ "$PLATFORM" = "linux" ]; then
        # From WSL: use Windows cmake via /mnt/c/
        if [ -x "$CMAKE_MSVC" ]; then
            CMAKE_CMD="$CMAKE_MSVC"
            echo "Using Windows cmake from WSL"
        else
            echo "ERROR: Windows cmake not found at $CMAKE_MSVC"
            echo "  Install Visual Studio 18 with CMake tools"
            return 1
        fi
    else
        CMAKE_CMD="cmake"
    fi

    # Check vcpkg
    if [ ! -d "$VCPKG_INSTALLED" ]; then
        echo "INFO: vcpkg packages not fully installed. Installing..."
        install_vcpkg_pkgs
    fi

    # Check KDE deps and use stubs as fallback
    local STUBS_DIR="$SCRIPT_DIR/stubs"
    local missing_deps
    missing_deps=($(check_windows_deps))
    if [ -d "$DEPS_DIR" ]; then
        local KDB_DIR="$DEPS_DIR/lib/cmake/KDb3"
        local KPC_DIR="$DEPS_DIR/lib/cmake/KPropertyCore3"
        local KPW_DIR="$DEPS_DIR/lib/cmake/KPropertyWidgets3"
        local KREPORT_DIR="$DEPS_DIR/lib/cmake/KReport3"
        local KDEWIN_DIR="$DEPS_DIR/lib/cmake/KDEWin"
    else
        local KDB_DIR="$STUBS_DIR/KDb"
        local KPC_DIR="$STUBS_DIR/KPropertyCore3"
        local KPW_DIR="$STUBS_DIR/KPropertyWidgets3"
        local KREPORT_DIR="$STUBS_DIR/KReport3"
        local KDEWIN_DIR="$STUBS_DIR/KDEWin"
    fi

    # CMake configure
    "$CMAKE_CMD" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -G "Visual Studio 18 2026" \
        -DCMAKE_TOOLCHAIN_FILE="$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake" \
        -DVCPKG_TARGET_TRIPLET="$VCPKG_TRIPLET" \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
        -DECM_DIR="$VCPKG_INSTALLED/share/ECM" \
        ${DEPS_DIR:+-DCMAKE_PREFIX_PATH="$DEPS_DIR;$VCPKG_INSTALLED"} \
        -DKDb_DIR="$KDB_DIR" \
        -DKPropertyCore_DIR="$KPC_DIR" \
        -DKPropertyWidgets_DIR="$KPW_DIR" \
        -DKReport_DIR="$KREPORT_DIR" \
        -DKDEWin_DIR="$KDEWIN_DIR"

    if [ $? -ne 0 ]; then
        echo "ERROR: CMake configure failed for Windows build."
        return 1
    fi

    # CMake build
    if [ "$PLATFORM" = "linux" ]; then
        # From WSL: cmake --build works via the Windows cmake
        "$CMAKE_CMD" --build "$BUILD_DIR" --config Debug -j"$NPROC"
    else
        cmake --build "$BUILD_DIR" --config Debug -j"$NPROC"
    fi

    if [ $? -ne 0 ]; then
        echo "ERROR: Windows build failed."
        return 1
    fi

    # Install
    if [ "$DO_INSTALL" = "install" ]; then
        echo "=== Installing Windows build ==="
        "$CMAKE_CMD" --install "$BUILD_DIR" --config Debug --prefix "$INSTALL_DIR" 2>/dev/null || true
    fi

    # --- Post-build: runtime setup ---
    if [ -d "$BUILD_DIR/bin/data" ] && [ ! -d "$BIN_DIR/data" ]; then
        echo "=== Copying data/ to output ==="
        cp -r "$BUILD_DIR/bin/data" "$BIN_DIR/" 2>/dev/null || true
    fi

    local PLUGIN_SUBDIR="$BIN_DIR/plugins/akexi/3.2"
    mkdir -p "$PLUGIN_SUBDIR" 2>/dev/null || true

    for plugin in akexi_tableplugin akexi_queryplugin akexi_formplugin \
                  akexi_reportplugin akexi_migrationplugin \
                  akexi_csvimportexportplugin; do
        if [ -f "$BIN_DIR/${plugin}.dll" ]; then
            cp -u "$BIN_DIR/${plugin}.dll" "$PLUGIN_SUBDIR/" 2>/dev/null || true
        fi
    done
    if [ -f "$BIN_DIR/org.akexi-project.form.widgets.main.dll" ]; then
        cp -u "$BIN_DIR/org.akexi-project.form.widgets.main.dll" "$PLUGIN_SUBDIR/" 2>/dev/null || true
    fi

    if [ -d "$DEPS_DIR/plugins/kdb3" ]; then
        mkdir -p "$BIN_DIR/kdb3/sqlite3" 2>/dev/null || true
        cp -u "$DEPS_DIR/plugins/kdb3/kdb_sqlitedriver.dll" "$BIN_DIR/kdb3/" 2>/dev/null || true
        cp -u "$DEPS_DIR/plugins/kdb3/sqlite3/kdb_sqlite_icu.dll" "$BIN_DIR/kdb3/sqlite3/" 2>/dev/null || true
    fi

    if [ -d "$DEPS_DIR/plugins/kreport3" ]; then
        mkdir -p "$BIN_DIR/kreport3" 2>/dev/null || true
        for dll in "$DEPS_DIR"/plugins/kreport3/*.dll; do
            [ -f "$dll" ] && cp -u "$dll" "$BIN_DIR/kreport3/" 2>/dev/null || true
        done
    fi

    echo "=== Windows build complete ==="
    if [ -f "$BIN_DIR/akexi.exe" ]; then
        echo "Binary: $BIN_DIR/akexi.exe"
    fi
}

# ============================================================
#  Both platforms
# ============================================================
build_both() {
    echo "===================================================="
    echo " Building for both Linux and Windows"
    echo "===================================================="
    echo ""
    build_linux
    echo ""
    build_windows || echo "[WARN] Windows build had issues (see above)"
    echo ""
    echo "===================================================="
    echo " Both builds attempted."
    echo " Linux:   $SCRIPT_DIR/build-linux"
    echo " Windows: $SCRIPT_DIR/build_msvc"
    echo "===================================================="
}

# ============================================================
#  Main
# ============================================================
case "$BUILD_TYPE" in
    linux)
        build_linux
        ;;
    windows)
        build_windows
        ;;
    both)
        build_both
        ;;
    clean-linux)
        clean_build "$SCRIPT_DIR/build-linux"
        ;;
    clean-windows)
        clean_build "$SCRIPT_DIR/build_msvc"
        ;;
    clean)
        clean_build "$SCRIPT_DIR/build-linux"
        clean_build "$SCRIPT_DIR/build_msvc"
        ;;
    *)
        echo "Usage: $0 [linux|windows|both|all|clean|clean-linux|clean-windows] [install]"
        echo ""
        echo "  (default)   Auto-detect platform and build"
        echo "  linux       Build for Linux (native/WSL)"
        echo "  windows     Build for Windows (MSVC + vcpkg)"
        echo "  both|all    Build for both platforms"
        echo "  install     Append to also run cmake --install"
        echo "  clean*      Remove build directories"
        exit 1
        ;;
esac
