#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "Starting KallarOS Environment Setup..."

echo "Detecting package manager and installing dependencies (sudo required)..."

if command -v apt-get &> /dev/null; then
    echo "Detected apt. Updating and installing Debian/Ubuntu dependencies..."
    sudo apt-get update
    sudo apt-get install -y build-essential bison flex libgmp3-dev libmpc-dev \
                            libmpfr-dev texinfo curl nasm genisoimage qemu-system-x86

elif command -v dnf &> /dev/null; then
    echo "Detected dnf. Installing Fedora/Nobara dependencies..."
    sudo dnf install -y @development-tools bison flex gmp-devel libmpc-devel \
                        mpfr-devel texinfo curl nasm genisoimage qemu-system-x86

else
    echo "Error: Neither 'apt-get' nor 'dnf' found."
    echo "This script requires a Debian/Ubuntu or Fedora-based Linux distribution."
    exit 1
fi

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p ~/src
cd ~/src

echo "Starting i686-elf cross-compiler build..."
echo "This will take some time to complete."
echo "To speed up the building of GCC, it will use all available threads."

echo "Downloading Binutils (2.41) and GCC (13.2.0)..."
if [ ! -f "binutils-2.41.tar.gz" ]; then
    curl -O https://ftp.gnu.org/gnu/binutils/binutils-2.41.tar.gz
fi
if [ ! -f "gcc-13.2.0.tar.gz" ]; then
    curl -O https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
fi

echo "Extracting archives..."
tar -xf binutils-2.41.tar.gz
tar -xf gcc-13.2.0.tar.gz

CORES=$(nproc --all)

echo "Building Binutils..."
mkdir -p build-binutils
cd build-binutils
../binutils-2.41/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make -j$CORES install
cd ..

echo "Building GCC..."
mkdir -p build-gcc
cd build-gcc
../gcc-13.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j$CORES all-gcc
make -j$CORES all-target-libgcc
make install-gcc
make install-target-libgcc

echo "========================================"
echo "Cross-compiler successfully built!"
echo "You might want to add this to ~/.bashrc :)"
echo 'export PATH="$HOME/opt/cross/bin:$PATH"'
echo "========================================"