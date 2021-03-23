cmake_minimum_required(VERSION 3.10)

project(wondergui VERSION 3.0 DESCRIPTION "WonderGUI User Interface Library")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})


include_directories( 
	baselibrary
	baselibrary/base
	baselibrary/components
	baselibrary/sizebrokers
	baselibrary/skins/anim
	baselibrary/skins/meter
	baselibrary/skins
	baselibrary/skins/misc
	baselibrary/skins/state
	baselibrary/skins/static
	baselibrary/textmappers
	baselibrary/valueformatters
	baselibrary/widgets
	baselibrary/widgets/capsules
	baselibrary/widgets/layers
	baselibrary/widgets/lists
	baselibrary/widgets/panels

	gfxdevices/software
	gfxdevices/opengl
	gfxdevices/stream
	gfxdevices/metal

	fontengines/freetype

	../SDL2/include
	../SDL2_image/include
)

link_directories(
	../SDL2/lib/x64
	../SDL2_image/lib/x64
)


add_subdirectory(baselibrary)
add_subdirectory(gfxdevices/software)
add_subdirectory(gfxdevices/opengl)
add_subdirectory(fontengines/freetype)
add_subdirectory(examples)
add_subdirectory(tools/workbench)
add_subdirectory(tools/gfxdevicetester)
#add_subdirectory(tools/unittester)
