rm -rf exports
mkdir exports

cd exports
mkdir base
mkdir components
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
mkdir skins
mkdir fonts
cd ..


cp -r ../framework/base exports/
cp -r ../framework/components exports/
cp -r ../framework/gfxdevices exports/
cp -r ../framework/skins exports/
cp -r ../framework/fonts exports/ 

rm exports/base/wg3_base.h
rm exports/base/wg3_object.cpp


ruby flip_files.rb exports/**/*.cpp exports/**/*.h exports/gfxdevices/**/*.cpp exports/gfxdevices/**/*.h exports/fonts/**/*.cpp exports/fonts/**/*.h