
project(example01)

add_executable(example01

	example01.cpp
)

target_link_libraries(example01 PUBLIC 
	wgsoftgfx
	wondergui 

	SDL2
)

target_include_directories(example01 PUBLIC	
	../../gfxdevices/software
)



