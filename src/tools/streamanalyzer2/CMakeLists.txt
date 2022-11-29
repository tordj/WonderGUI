set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(streamanalyzer2

	../../baseplates/sdl_software/main.cpp
	../../baseplates/sdl_software/baseplate.h
	../../baseplates/sdl_software/baseplate.cpp
	../../baseplates/sdl_software/tinyfiledialogs.h
	../../baseplates/sdl_software/tinyfiledialogs.c
	wonderapp.cpp
	wonderapp.h
)

target_include_directories(streamanalyzer2 PRIVATE 
	../../baseplates/sdl_software
)

target_link_directories(streamanalyzer2 PRIVATE 
)

target_working_directory(streamanalyzer2 ${WG_ROOT} )

target_link_libraries(streamanalyzer2 PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


