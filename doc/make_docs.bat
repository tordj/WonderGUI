
mkdir temp
rm -f temp/*
rm -rf html
cp ../src/base/* ../src/fonts/freetype/* ../src/gfxdevices/opengl/* ../src/gfxdevices/software/* ../src/interfaces/* ../src/sizebrokers/* ../src/skins/* ../src/textmappers/* ../src/valueformatters/* ../src/widgets/* ../src/widgets/capsules/* ../src/widgets/layers/* ../src/widgets/lists/* ../src/widgets/panels/* temp
ruby ../scripts/predoxy.rb temp/*.h
doxygen
ruby ../scripts/postdoxy.rb html/class*.html
