set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(softkernelgenerator

	../../baseplates/sdl_software/main.cpp
	../../baseplates/sdl_software/baseplate.h
	../../baseplates/sdl_software/tinyfiledialogs.h
	../../baseplates/sdl_software/tinyfiledialogs.c
	wonderapp.cpp
	wonderapp.h
	kerneldb.cpp
	kerneldb.h
)

target_include_directories(softkernelgenerator PRIVATE 
	../../baseplates/sdl_software
)

target_link_directories(softkernelgenerator PRIVATE 
)

target_working_directory(softkernelgenerator ${WG_ROOT} )

target_link_libraries(softkernelgenerator PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


