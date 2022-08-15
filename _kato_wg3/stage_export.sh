rm -rf exports
mkdir exports

cd exports
mkdir baselibrary
mkdir baselibrary/base
mkdir baselibrary/c-interface
mkdir baselibrary/components
mkdir baselibrary/gfxstream
mkdir baselibrary/skins
mkdir c-abi-wrapper
mkdir gfxdevices
mkdir gfxdevices/opengl
mkdir gfxdevices/software
mkdir gfxdevices/metal
mkdir gfxdevices/stream
mkdir fontengines
cd ..


cp -r ../framework/baselibrary/base exports/baselibrary
cp -r ../framework/baselibrary/c-interface exports/baselibrary
cp -r ../framework/baselibrary/components exports/baselibrary
cp -r ../framework/baselibrary/gfxstream exports/baselibrary
cp -r ../framework/baselibrary/skins exports/baselibrary
cp -r ../framework/c-abi-wrapper exports/
cp -r ../framework/gfxdevices exports/
cp -r ../framework/fontengines exports/ 


ruby flip_files.rb exports/baselibrary/**/*.cpp exports/baselibrary/**/*.h exports/c-abi-wrapper/* exports/gfxdevices/**/* exports/fontengines/**/* exports/baselibrary/skins/**/*.cpp exports/baselibrary/skins/**/*.h
 