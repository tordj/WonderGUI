cmake_minimum_required(VERSION 3.16)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

project(WonderGUI VERSION 3.0 DESCRIPTION "WonderGUI User Interface Library")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(WG_ROOT ${PROJECT_SOURCE_DIR}/..)

#------------------------------------------------------------------------------
# DIRECTORIES
#
# SDL2, SDL2_image, GLEW and FreeType libraries are needed by several compile targets,
# such as tools, workbenches, examples and some WonderApp frameworks.
#
# The base libraries (WonderGear, WonderGfx, WonderGlyphs and WonderGUI) do not need these.
#
#------------------------------------------------------------------------------

if(WIN32)
	set(SDL2_INCDIR ${WG_ROOT}/SDL2/include)
	set(SDL2_LIBDIR ${WG_ROOT}/SDL2/lib/x64/)
	set(SDL2_LIBS SDL2 SDL2main)

	set(SDL2IMAGE_INCDIR ${WG_ROOT}/SDL2_image/include)
	set(SDL2IMAGE_LIBDIR ${WG_ROOT}/SDL2_image/lib/x64/)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(FREETYPE2_INCDIR ${WG_ROOT}/freetype/include)
	set(FREETYPE2_LIBDIR ${WG_ROOT}/freetype/lib/x64/)
    set(FREETYPE2_LIB freetype)

	set(GLEW_INCDIR ${WG_ROOT}/glew/include)
	set(GLEW_LIBDIR ${WG_ROOT}/glew/lib/Release/x64/)	
	set(OPENGL_LIBS glew32 opengl32)

elseif(APPLE)
	set(FREETYPE2_INCDIR ${WG_ROOT}/freetype/include)
	set(FREETYPE2_LIBDIR ${WG_ROOT}/freetype/lib/)
    set(FREETYPE2_LIB freetype_arm64 freetype_x86_64 bz2 z )

	find_library(SDL2_LIBS SDL2)
	find_library(SDL2IMAGE_LIBS SDL2_image)

	find_library(OPENGL_LIBS OpenGL)

else()
    set(FREETYPE2_LIB freetype)

	set(SDL2_LIBS SDL2)
	set(SDL2IMAGE_LIBS SDL2_image)

	set(OPENGL_LIBS GLEW GL)

endif()


#------------------------------------------------------------------------------
# WONDERAPP FRAMEWORK SELECTION
#
# Select which WonderApp framework to use when building tools and other included
# applications.
#
#------------------------------------------------------------------------------


set(WAPPLIB wappsdl2)			# WonderApp framework based on SDL2. Works on Windows, Mac and Linux.


#------------------------------------------------------------------------------
# FUNCTIONS
#
# Used by subdirectories CMakeLists.
#------------------------------------------------------------------------------


function( target_working_directory target workpath )

	if(WIN32)
		set_target_properties( ${target} PROPERTIES 
			VS_DEBUGGER_WORKING_DIRECTORY ${workpath} 
		)

	elseif(APPLE)
		set_target_properties( ${target} PROPERTIES
			XCODE_GENERATE_SCHEME TRUE
			XCODE_SCHEME_WORKING_DIRECTORY "${workpath}"
		)
	endif()

endfunction()


#------------------------------------------------------------------------------
# COMPILER OPTIONS
#------------------------------------------------------------------------------

if(MSVC)
	add_compile_options("/MP")
endif()

#------------------------------------------------------------------------------

add_subdirectory(gear)
add_subdirectory(gfx)
add_subdirectory(glyphs)
add_subdirectory(gui)

add_subdirectory(glyphs/freetype)

add_subdirectory(legacygui)

if(APPLE)
	add_subdirectory(gfx/metal)
endif()

add_subdirectory(gfx/opengl)
add_subdirectory(gfx/software)


add_subdirectory(plugin)
add_subdirectory(wonderapp/${WAPPLIB})
add_subdirectory(tools)
add_subdirectory(examples)
