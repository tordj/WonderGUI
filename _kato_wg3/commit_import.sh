

rm -rf ../framework/baselibrary/base
rm -rf ../framework/baselibrary/c-interface
rm -rf ../framework/baselibrary/skins
rm -rf ../framework/baselibrary/surfaceio
rm -rf ../framework/baselibrary/gfxstream
rm -rf ../framework/c-abi-wrapper
rm -rf ../framework/gfxdevices
rm -rf ../framework/fontengines


cp -rf imports/* ../framework
ruby flip_files.rb ../framework/baselibrary/base/* ../framework/baselibrary/gfxstream/* ../framework/baselibrary/surfaceio/* ../framework/baselibrary/c-interface/* ../framework/c-abi-wrapper/* ../framework/gfxdevices/**/* ../framework/baselibrary/skins/*.cpp ../framework/baselibrary/skins/*.h ../framework/baselibrary/skins/**/*  ../framework/fontengines/**/* 

