#!/bin/bash
set -e

KERNEL_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$KERNEL_DIR/build"
ISO_DIR="$BUILD_DIR/iso"
GRUB_CFG="$ISO_DIR/boot/grub/grub.cfg"

mkdir -p "$ISO_DIR/boot/grub"

cp "$KERNEL_DIR/mini-kernel.bin" "$ISO_DIR/boot/"

cat > "$GRUB_CFG" <<EOF
set timeout=0
set default=0

menuentry "mini-kernel" {
    multiboot /boot/mini-kernel.bin
}
EOF

grub-mkrescue -o "$BUILD_DIR/mini-kernel.iso" "$ISO_DIR"
echo "ISO ready: $BUILD_DIR/mini-kernel.iso"