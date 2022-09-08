cmake_minimum_required(VERSION 3.16)

project(WonderGUI VERSION 1.9 DESCRIPTION "WonderGUI User Interface Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(WG_ROOT ${PROJECT_SOURCE_DIR})

include_directories( 
	.
	extras
	framework
	framework/baselibrary
	framework/baselibrary/base
	framework/baselibrary/c-interface
	framework/baselibrary/sizebrokers
	framework/baselibrary/gfxstream
	framework/baselibrary/skins/anim
	framework/baselibrary/skins/meter
	framework/baselibrary/skins
	framework/baselibrary/skins/misc
	framework/baselibrary/skins/state
	framework/baselibrary/skins/static

	framework/c-abi-wrapper

	framework/gfxdevices/software
	framework/gfxdevices/opengl
	framework/gfxdevices/stream
	framework/gfxdevices/metal

	framework/fontengines/freetype

	../SDL2/include
	../SDL2_image/include
	../glew/include
	freetype/include
)

link_directories(
	SDL2/lib/x64
	SDL2_image/lib/x64
	freetype/lib/x64
	glew/lib/Release/x64
    freetype/lib
)


if(WIN32)

	set(SDL2_LIBS SDL2 SDL2main)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(OPENGL_LIBS glew32 opengl32)

elseif(APPLE)

	find_library(SDL2_LIBS SDL2)
	find_library(SDL2IMAGE_LIBS SDL2_image)

	find_library(OPENGL_LIBS OpenGL)

else()

	set(SDL2_LIBS SDL2)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(OPENGL_LIBS GLEW GL)
endif()


function( target_working_directory target workpath )

	if(WIN32)
		set_target_properties( ${target} PROPERTIES 
			VS_DEBUGGER_WORKING_DIRECTORY ${workpath} 
		)

#		set_property(TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${workpath} )

	elseif(APPLE)
		set_target_properties( ${target} PROPERTIES
			XCODE_GENERATE_SCHEME TRUE
			XCODE_SCHEME_WORKING_DIRECTORY "${workpath}"
		)
	endif()

endfunction()


add_subdirectory(framework)
add_subdirectory(framework/c-abi-wrapper)
add_subdirectory(framework/gfxdevices/software)
add_subdirectory(framework/gfxdevices/stream)
#add_subdirectory(framework/gfxdevices/opengl)
#if(APPLE)
#	add_subdirectory(gfxdevices/metal)
#endif()
add_subdirectory(framework/fontengines/freetype)
add_subdirectory(bench)
#add_subdirectory(tools/gfxdevicetester)
#add_subdirectory(tools/unittester)
