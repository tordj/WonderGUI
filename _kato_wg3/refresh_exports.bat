mkdir exports

cd exports
mkdir opengl
mkdir wg_soft
rm *.cpp *.h opengl/*.cpp opengl/*.h wg_soft/*.cpp wg_soft/*.h

cd ..

cp ../framework/wg3_* exports/
cp ../framework/opengl/wg3_* exports/opengl/
cp ../framework/wg_soft/wg3_* exports/wg_soft/

ruby flip_includes.rb exports/*.cpp exports/*.h exports/opengl/*.cpp exports/opengl/*.h exports/wg_soft/*.cpp exports/wg_soft/*.h