#!/bin/sh

PACKAGE="wondergui_$1.zip"
ROOT="$PWD"

cd $ROOT
cd build/gnumake
make clean
rm ~*

cd $ROOT
cd build/codelite-linux
rm -rf Debug Release *.txt *.mk *~ Makefile compile_commands.json .build-debug .build-release .clang .codelite

cd $ROOT
cd build
rm *~

cd $ROOT
cd doc
rm -rf *~ doxygen_sqlite3.db html latex temp

cd $ROOT
rm -f $PACKAGE
zip -r "$PACKAGE" build doc src resources README

