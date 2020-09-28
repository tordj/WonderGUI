

rm -rf ../framework/base
rm -rf ../framework/components
rm -rf ../framework/gfxdevices
rm -rf ../framework/skins
rm -rf ../framework/fonts


cp -rf imports/* ../framework
ruby flip_files.rb ../framework/base/* ../framework/components/* ../framework/gfxdevices/**/* ../framework/skins/*.cpp ../framework/skins/*.h ../framework/skins/**/*  ../framework/fonts/**/*

