#!/bin/sh

cd build/gnumake
make clean
cd ../../

cd build/codelite-linux
rm -rf Debug Release *.txt *.mk Makefile compile_commands.json .build-debug .build-release .clang .codelite
cd ../../

cd doc
rm -rf Doxyfile~ doxygen_sqlite3.db html latex
cd ..

rm package.zip
zip -r package.zip build doc src resources README

