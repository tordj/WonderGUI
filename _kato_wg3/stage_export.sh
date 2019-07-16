rm -rf exports
mkdir exports

cd exports
mkdir base
mkdir interfaces
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
cd ..


cp -r ../framework/base exports/
cp -r ../framework/interfaces exports/
cp -r ../framework/gfxdevices exports/

rm exports/base/wg3_base.h
rm exports/base/wg3_object.cpp


ruby flip_files.rb exports/**/*.cpp exports/**/*.h exports/gfxdevices/**/*.cpp exports/gfxdevices/**/*.h