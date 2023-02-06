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

add_library(wappsdl2sw

	main.cpp
	window.h
	window.cpp
	tinyfiledialogs.h
	tinyfiledialogs.c
	../include/wonderapp.h
)

target_include_directories(wappsdl2sw PRIVATE 
	../include
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WG_ROOT}/SDL2/include/
	${WG_ROOT}/SDL2_image/include/	
)

set_target_properties(wappsdl2sw PROPERTIES FOLDER "WonderApp/Frameworks" )

target_link_libraries(wappsdl2sw PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


