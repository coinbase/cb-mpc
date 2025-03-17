#!/bin/bash

set -e

SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# Other necessary paths
ROOT_DIR="$SCRIPT_PATH/.."
DST_PARENT_DIR=/usr/local/opt/cbmpc
SRC_DIR="$ROOT_DIR/src/"
DST_DIR="$DST_PARENT_DIR/include/"
LIB_SRC_DIR="$ROOT_DIR/lib/Release"
LIB_DST_DIR="$DST_PARENT_DIR/lib/"

# Check if destination directory exists. If not, create it
if [ ! -d "$DST_PARENT_DIR" ]; then
    mkdir -p "$DST_DIR"
    mkdir -p "$LIB_DST_DIR"
fi

# Find and copy header files
rsync -avm \
  --exclude='*/build/*' \
  --include='*.h' \
  --include='*/' \
  --exclude='*' \
  "$SRC_DIR/" "$DST_DIR/"

# Copy library files
FILES=("libcbmpc.a")
for file in "${FILES[@]}"; do
  rsync -av "$LIB_SRC_DIR/$file" "$LIB_DST_DIR/"
done

echo "All header and library files have been copied to $DST_DIR"