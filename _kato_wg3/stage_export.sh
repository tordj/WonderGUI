rm -rf exports
mkdir exports

cd exports
mkdir baselibrary
mkdir baselibrary/base
mkdir baselibrary/components
mkdir baselibrary/skins
mkdir baselibrary/stream
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
mkdir gfxdevices/metal
mkdir gfxdevices/stream
mkdir fontengines
cd ..


cp -r ../framework/baselibrary/base exports/baselibrary
cp -r ../framework/baselibrary/components exports/baselibrary
cp -r ../framework/baselibrary/stream exports/stream
cp -r ../framework/gfxdevices exports/
cp -r ../framework/baselibrary/skins exports/baselibrary
cp -r ../framework/fontengines exports/ 


ruby flip_files.rb exports/baselibrary/**/*.cpp exports/baselibrary/**/*.h exports/gfxdevices/**/* exports/fontengines/**/* exports/baselibrary/skins/**/*.cpp exports/baselibrary/skins/**/*.h
 