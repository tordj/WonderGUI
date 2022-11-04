set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(surfviewer

	../../baseplates/sdl_software/main.cpp
	../../baseplates/sdl_software/baseplate.h
	../../baseplates/sdl_software/tinyfiledialogs.h
	../../baseplates/sdl_software/tinyfiledialogs.c
	wonderapp.cpp
	wonderapp.h
)

target_include_directories(surfviewer PRIVATE 
	../../baseplates/sdl_software
)

target_link_directories(surfviewer PRIVATE 
)

target_working_directory(surfviewer ${WG_ROOT} )

target_link_libraries(surfviewer PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


