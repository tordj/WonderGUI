
project(unittester)

add_executable(unittester

	main.cpp
	testframework.cpp
	text_test.cpp
	valueformat_test.cpp
)


if(WIN32)
target_link_libraries(unittester PUBLIC 
	wgfreetypefont
	wondergui 
	freetype
)

else()
target_link_libraries(unittester PUBLIC 
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	SDL2
	SDL2_image
	GLEW
	GL
	freetype
)

endif()


