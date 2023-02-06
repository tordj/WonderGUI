
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

add_executable(workbench

	main.cpp
)

target_link_directories(workbench PRIVATE 
)

target_working_directory(workbench ${WG_ROOT} )

target_include_directories(workbench PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
	${WG_ROOT}/SDL2/include/
	${WG_ROOT}/SDL2_image/include/

)

target_link_libraries(workbench PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)

