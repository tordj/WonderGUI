

rm -rf ../framework/base
rm -rf ../framework/components
rm -rf ../framework/gfxdevices
rm -rf ../framework/skins
rm -rf ../framework/fonts


cp -rf imports/* ../framework
ruby flip_files.rb ../framework/baselibrary/base/* ../framework/baselibrary/components/* ../framework/gfxdevices/**/* ../framework/baselibrary/skins/*.cpp ../framework/baselibrary/skins/*.h ../framework/baselibrary/skins/**/*  ../framework/fontengines/**/*

