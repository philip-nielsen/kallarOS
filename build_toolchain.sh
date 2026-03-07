#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "Starting KallarOS Environment Setup..."

echo "Installing dependencies(sudo required)..."
sudo apt-get update
sudo apt-get install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo curl \
                        nasm genisoimage qemu-system-x86

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p ~/src
cd ~/src

echo "Starting i686-elf cross-compiler build..."
echo "This will take some time to complete"
echo "As to speed up the building of GCC, it will using all available threads"

echo "Downloading Binutils (2.41) and GCC(13.2.0)..."
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