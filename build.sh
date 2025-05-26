#!/bin/zsh

kernel=$(uname -s)
BIN=bin
BUILD=build
MAIN=$1
DIRNAME=$(dirname "$MAIN")
FILENAME=${$(basename "$MAIN")%.*}

echo "Compiling $FILENAME..."

mkdir -p bin
cp $BUILD/dd $BIN/$FILENAME

echo "Building $FILENAME"

./$BIN/$FILENAME $MAIN

mv $BUILD/$FILENAME.asm $BIN/$FILENAME.asm
# mv dd.asm $BIN/$FILENAME.asm
# echo $kernel

if [ "$kernel" = "Darwin" ]; then
    # macOS (M1 Mac)
    echo "linking and loading in m1"
    as $BIN/$FILENAME.asm -o $BIN/$FILENAME.o
    ld $BIN/$FILENAME.o -o $BIN/$FILENAME -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path`
elif [ "$kernel" = "Linux" ]; then
    # Linux (Raspberry Pi)
    echo "linking and loading with Pi"
    as $BIN/$FILENAME.asm -o $BIN/$FILENAME.o-gdwaef64 -g3
    ld $BIN/$FILENAME.o -o $BIN/$FILENAME -arch -arm64
else
    echo "Unsupported system"
    exit 1
fi

echo "Running binary $FILENAME"
./$BIN/$FILENAME
echo $?