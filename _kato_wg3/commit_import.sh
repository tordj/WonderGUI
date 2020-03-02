
# Preserve our tweaked files
mv ../framework/base/wg3_base.h ../framework/base/wg3_base.cpp .

rm -rf ../framework/base
rm -rf ../framework/components
rm -rf ../framework/gfxdevices
rm -rf ../framework/skins
rm -rf ../framework/fonts


cp -rf imports/* ../framework
ruby flip_files.rb ../framework/base/* ../framework/components/* ../framework/gfxdevices/**/* ../framework/skins/* ../framework/fonts/**/*

mv wg3_base.h wg3_base.cpp ../framework/base
