#!/bin/bash

set -e

SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

usage() {
    cat <<'EOF'
Usage:
  scripts/install.sh [--mode public|full] [--prefix <path>] [--build-type <Release|Debug|RelWithDebInfo>]

Options:
  --mode   Install mode (default: public). Also supports $CBMPC_INSTALL_MODE.
           - public: install only headers under include/
           - full:   also install internal headers under include-internal/
  --prefix Install prefix (default: <repo>/build/install/<mode>). Also supports $CBMPC_PREFIX.
  --build-type Build type for selecting the built library artifact to install
           (default: Release). Also supports $CBMPC_BUILD_TYPE.

Examples (flags can be in any order):
  scripts/install.sh --mode public
  scripts/install.sh --prefix /tmp/cbmpc --mode full
  CBMPC_PREFIX=/tmp/cbmpc scripts/install.sh --mode public
EOF
}

# Install mode:
# - public: install only curated public headers under include/
# - full:   additionally install internal headers under include-internal/
INSTALL_MODE="${CBMPC_INSTALL_MODE:-public}"
# Build type controls which lib artifact we copy from `lib/<build-type>/`.
BUILD_TYPE="${CBMPC_BUILD_TYPE:-Release}"
# Install prefix can be customized to avoid requiring sudo.
# Default: <repo>/build/install/<mode>
DST_PARENT_DIR="${CBMPC_PREFIX:-}"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --mode)
            if [[ $# -lt 2 ]]; then
                echo "Missing value for --mode"
                usage
                exit 1
            fi
            INSTALL_MODE="$2"
            shift 2
            ;;
        --prefix)
            if [[ $# -lt 2 ]]; then
                echo "Missing value for --prefix"
                usage
                exit 1
            fi
            DST_PARENT_DIR="$2"
            shift 2
            ;;
        --build-type)
            if [[ $# -lt 2 ]]; then
                echo "Missing value for --build-type"
                usage
                exit 1
            fi
            BUILD_TYPE="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            usage
            exit 1
            ;;
    esac
done

# Other necessary paths
ROOT_DIR="$SCRIPT_PATH/.."
PUBLIC_INCLUDE_DIR="$ROOT_DIR/include/"
INTERNAL_INCLUDE_DIR="$ROOT_DIR/include-internal/"

if [[ -z "$DST_PARENT_DIR" ]]; then
  DST_PARENT_DIR="$ROOT_DIR/build/install/$INSTALL_MODE"
fi

DST_DIR="$DST_PARENT_DIR/include/"
LIB_SRC_DIR="$ROOT_DIR/lib/$BUILD_TYPE"
LIB_DST_DIR="$DST_PARENT_DIR/lib/"

# Validate mode
case "$INSTALL_MODE" in
    public|full) ;;
    *)
        echo "Unknown install mode: $INSTALL_MODE (expected: public|full)"
        exit 1
        ;;
esac

# Validate build type / lib path.
LIB_FILE="$LIB_SRC_DIR/libcbmpc.a"
if [[ ! -f "$LIB_FILE" ]]; then
  echo "Missing built library: $LIB_FILE"
  echo "Build it first, e.g.: make build-no-test BUILD_TYPE=$BUILD_TYPE"
  exit 1
fi

# Ensure destination directories exist.
mkdir -p "$LIB_DST_DIR"

# Refresh include directory to avoid stale headers lingering across installs.
rm -rf "$DST_DIR"
mkdir -p "$DST_DIR"

# Copy public headers
rsync -avm \
  --include='*.h' \
  --include='*/' \
  --exclude='*' \
  "$PUBLIC_INCLUDE_DIR/" "$DST_DIR/"

# Copy internal headers (full mode only)
if [[ "$INSTALL_MODE" == "full" ]]; then
  rsync -avm \
    --include='*.h' \
    --include='*/' \
    --exclude='*' \
    "$INTERNAL_INCLUDE_DIR/" "$DST_DIR/"
fi

# Copy library files
FILES=("libcbmpc.a")
for file in "${FILES[@]}"; do
  rsync -av "$LIB_SRC_DIR/$file" "$LIB_DST_DIR/"
done

echo "Installed ($INSTALL_MODE) headers + library ($BUILD_TYPE) to $DST_PARENT_DIR"
