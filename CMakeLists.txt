cmake_minimum_required(VERSION 3.10)

project(WonderGUI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories( 
	baselibrary
	baselibrary/base
	baselibrary/components
	baselibrary/sizebrokers
	baselibrary/skins
	baselibrary/skins/anim
	baselibrary/skins/meter
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
)



add_subdirectory(baselibrary)
add_subdirectory(gfxdevices/software)
add_subdirectory(examples)