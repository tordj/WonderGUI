set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(cabiworkbench

	../../baseplates/sdl_software/main.cpp
	../../baseplates/sdl_software/baseplate.h
	../../baseplates/sdl_software/baseplate.cpp
	../../baseplates/sdl_software/tinyfiledialogs.h
	../../baseplates/sdl_software/tinyfiledialogs.c
	wonderapp.cpp
	wonderapp.h
)

target_include_directories(cabiworkbench PRIVATE 
	../../baseplates/sdl_software
)

target_link_directories(cabiworkbench PRIVATE 

)

target_working_directory(cabiworkbench ${WG_ROOT} )

target_link_libraries(cabiworkbench PRIVATE
	wgsoftgfx
	wgfreetypefont
	wgcabiwrapper
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


