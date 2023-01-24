
source_group(Files REGULAR_EXPRESSION ".*")


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


add_executable(gfxworkbench

	main.cpp
)

target_link_directories(gfxworkbench PRIVATE 
)

target_include_directories(gfxworkbench PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
)

target_working_directory(gfxworkbench ${WG_ROOT})

target_link_libraries(gfxworkbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)
