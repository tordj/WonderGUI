

rm -rf ../framework/baselibrary/base
rm -rf ../framework/baselibrary/components
rm -rf ../framework/baselibrary/skins
rm -rf ../framework/gfxdevices
rm -rf ../framework/fontengines


cp -rf imports/* ../framework
ruby flip_files.rb ../framework/baselibrary/base/* ../framework/baselibrary/components/* ../framework/gfxdevices/**/* ../framework/baselibrary/skins/*.cpp ../framework/baselibrary/skins/*.h ../framework/baselibrary/skins/**/*  ../framework/fontengines/**/*

