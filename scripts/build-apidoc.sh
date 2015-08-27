#!/bin/sh

PACKAGE="wg-doc-api-$1.zip"
ROOT="$PWD"

cd doc
doxygen
rm -f "../$PACKAGE"
zip -r "../$PACKAGE" html

