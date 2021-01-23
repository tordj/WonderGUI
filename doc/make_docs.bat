
mkdir temp
rm -f temp/*
rm -rf html
cp ../src/base/* ../src/fonts/**/* ../src/components/* ../src/gfxdevices/**/* ../src/sizebrokers/* ../src/skins/* ../src/skins/**/* ../src/textmappers/* ../src/valueformatters/* ../src/widgets/* ../src/widgets/**/* temp
ruby ../scripts/predoxy.rb temp/*.h
doxygen
ruby ../scripts/postdoxy.rb html/class*.html
