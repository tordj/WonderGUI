
project(example01)

add_executable(example01

	example01.cpp
)

target_link_directories(example01 PRIVATE 
	../../SDL2/lib/x64
)


if(WIN32)
target_link_libraries(example01 PUBLIC 
	wgsoftgfx
	wondergui 
	SDL2
	SDL2main
)

else()
target_link_libraries(example01 PUBLIC 
	wgsoftgfx
	wondergui 
	SDL2
)

endif()


