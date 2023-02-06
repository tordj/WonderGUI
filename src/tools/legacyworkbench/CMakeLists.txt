
set(CMAKE_CXX_STANDARD 20)

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


source_group(Files REGULAR_EXPRESSION ".*")

add_executable(legacyworkbench

	main.cpp
	sdl_wglib.cpp
	sdl_wglib.h
)

target_include_directories(legacyworkbench PRIVATE

	${WG_ROOT}/SDL2/include/
	${WG_ROOT}/SDL2_image/include/
)

target_link_directories(legacyworkbench PRIVATE 
)

target_working_directory(legacyworkbench ${WG_ROOT} )

target_include_directories(legacyworkbench PRIVATE 
	${WGLEGACYGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
)

target_link_libraries(legacyworkbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wglegacygui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)

