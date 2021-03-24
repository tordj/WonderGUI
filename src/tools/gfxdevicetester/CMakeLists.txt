
project(gfxdevicetester)

add_executable(gfxdevicetester

	framework_sdlgl/main.cpp
	gfxdevicetester.cpp
	device.cpp

)

target_link_directories(gfxdevicetester PRIVATE 
	../../SDL2/lib/x64

)


if(WIN32)
target_link_libraries(gfxdevicetester PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	SDL2
	SDL2main
	SDL2_image
	freetype
	glew32
	opengl32
)

else()
target_link_libraries(gfxdevicetester PRIVATE
	wgsoftgfx
	wgopenglgfx
	wgfreetypefont
	wondergui 
	SDL2
	SDL2_image
	freetype
	GLEW
	GL
)
endif()



