set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_executable(packlayouttester

	../../baseplates/sdl_software/main.cpp
	../../baseplates/sdl_software/baseplate.h
	wonderapp.cpp
	wonderapp.h
)

target_include_directories(packlayouttester PRIVATE 
	../../baseplates/sdl_software
)

target_link_directories(packlayouttester PRIVATE 
)

target_working_directory(packlayouttester ${WG_ROOT} )

target_link_libraries(packlayouttester PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


