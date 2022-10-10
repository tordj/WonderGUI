set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(fontgenerator

	../../baseplates/sdl_software/main.cpp
	../../baseplates/sdl_software/baseplate.h
	wonderapp.cpp
	wonderapp.h
)

target_include_directories(fontgenerator PRIVATE 
	../../baseplates/sdl_software
)

target_link_directories(fontgenerator PRIVATE 
)

target_working_directory(fontgenerator ${WG_ROOT} )

target_link_libraries(fontgenerator PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


