
set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(legacyworkbench

	main.cpp
	sdl_wglib.cpp
	sdl_wglib.h
)

target_include_directories(legacyworkbench PRIVATE
	${SDL2_INCDIR}
	${SDL2IMAGE_INCDIR}
	${WGLEGACYGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
)

target_link_directories(legacyworkbench PRIVATE 
	${FREETYPE2_LIBDIR}
	${SDL2_LIBDIR}
	${SDL2IMAGE_LIBDIR}
	${GLEW_LIBDIR}
)

target_working_directory(legacyworkbench ${WG_ROOT} )


target_link_libraries(legacyworkbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wglegacygui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
	${FREETYPE2_LIB}
)

