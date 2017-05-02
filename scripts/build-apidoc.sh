#!/bin/sh

PACKAGE="wg-doc-api-$1.zip"
ROOT="$PWD"

cd doc
./make_docs.bat
rm -f "../$PACKAGE"
zip -r "../$PACKAGE" html

