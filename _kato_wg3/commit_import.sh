
# Preserve our tweaked files
mv ../framework/base/wg3_base.h ../framework/base/wg3_object.cpp .

rm -rf ../framework/base
rm -rf ../framework/components
rm -rf ../framework/gfxdevices


cp -rf imports/* ../framework
ruby flip_files.rb ../framework/base/* ../framework/components/* ../framework/gfxdevices/**/*

mv wg3_base.h wg3_object.cpp ../framework/base
