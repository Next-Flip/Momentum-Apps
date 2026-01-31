#!/bin/bash
# Build Flipper Wedge for multiple Flipper Zero firmwares

set -e  # Exit on error

APP_NAME="flipper_wedge"
APP_DIR="/home/work/flipper wedge"
DIST_DIR="${APP_DIR}/dist"

# Default values
BRANCH="release"
TAG=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --branch)
            BRANCH="$2"
            shift 2
            ;;
        --tag)
            TAG="$2"
            shift 2
            ;;
        *)
            echo "Unknown argument: $1"
            echo ""
            echo "Usage: $0 [--branch BRANCH] [--tag TAG]"
            echo ""
            echo "Flags:"
            echo "  --branch BRANCH  Git branch to checkout (default: release)"
            echo "  --tag TAG        Git tag to checkout (overrides --branch)"
            echo ""
            echo "Examples:"
            echo "  $0"
            echo "  $0 --branch dev"
            echo "  $0 --tag 1.2.3"
            exit 1
            ;;
    esac
done

# Firmware directories
FIRMWARE_OFFICIAL="/home/work/flipperzero-firmware"
FIRMWARE_UNLEASHED="/home/work/unleashed-firmware"
FIRMWARE_MOMENTUM="/home/work/Momentum-Firmware"
FIRMWARE_ROGUEMASTER="/home/work/roguemaster-firmware"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Create dist directory
mkdir -p "${DIST_DIR}"

# Git checkout function with version change detection
checkout_firmware_version() {
    local fw_path=$1
    local branch=$2
    local tag=$3
    local fw_name=$4

    echo -e "${BLUE}📥 Fetching firmware updates for ${fw_name}...${NC}"
    cd "$fw_path"

    # Store current state
    local current_ref=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || git rev-parse --short HEAD)

    # Fetch latest
    git fetch --all --tags --quiet

    # Determine target
    if [ -n "$tag" ]; then
        target="tags/$tag"
        target_name="tag $tag"
    else
        target="origin/$branch"
        target_name="branch $branch"
    fi

    # Validate target exists
    if ! git rev-parse --verify "$target" >/dev/null 2>&1; then
        echo -e "${RED}❌ Error: $target_name not found in ${fw_name} firmware${NC}"
        exit 1
    fi

    # Checkout
    git checkout "$target" --quiet

    # Detect change and alert
    local new_ref=$(git rev-parse --short HEAD)
    local old_commit=$(git rev-parse "$current_ref" 2>/dev/null || echo "unknown")
    local new_commit=$(git rev-parse HEAD)

    if [ "$old_commit" != "$new_commit" ]; then
        echo -e "${RED}⚠️  FIRMWARE VERSION CHANGED: $current_ref → $target_name ($(git rev-parse --short HEAD))${NC}"
    fi

    # Update submodules
    echo -e "${BLUE}🔄 Updating submodules...${NC}"
    git submodule update --init --recursive --quiet
}

build_for_firmware() {
    local fw_name=$1
    local fw_path=$2

    if [ ! -d "$fw_path" ]; then
        echo -e "${RED}⏭️  Skipping ${fw_name}: firmware not found at ${fw_path}${NC}"
        return
    fi

    echo -e "${BLUE}🔨 Building for ${fw_name}...${NC}"

    # Ensure symlink exists
    if [ ! -L "${fw_path}/applications_user/${APP_NAME}" ]; then
        ln -s "${APP_DIR}" "${fw_path}/applications_user/${APP_NAME}"
    fi

    # Checkout requested firmware version
    checkout_firmware_version "$fw_path" "$BRANCH" "$TAG" "$fw_name"

    # Build
    echo ""
    cd "${fw_path}"
    ./fbt fap_${APP_NAME}

    # Copy FAP to dist folder
    local fap_file=$(find .fap -name "${APP_NAME}.fap" | head -n 1)
    if [ -f "$fap_file" ]; then
        cp "$fap_file" "${DIST_DIR}/${APP_NAME}_${fw_name}.fap"
        echo -e "${GREEN}✅ Built: ${APP_NAME}_${fw_name}.fap${NC}"
    else
        echo -e "${RED}❌ Failed to find FAP file for ${fw_name}${NC}"
    fi

    echo ""
}

echo "========================================"
echo "Building Flipper Wedge"
echo "========================================"
echo ""

# Build for each firmware
build_for_firmware "official" "$FIRMWARE_OFFICIAL"
build_for_firmware "unleashed" "$FIRMWARE_UNLEASHED"
build_for_firmware "momentum" "$FIRMWARE_MOMENTUM"
build_for_firmware "roguemaster" "$FIRMWARE_ROGUEMASTER"

echo "========================================"
echo -e "${GREEN}Build complete!${NC}"
echo "FAP files saved to: ${DIST_DIR}"
echo "========================================"
ls -lh "${DIST_DIR}"
