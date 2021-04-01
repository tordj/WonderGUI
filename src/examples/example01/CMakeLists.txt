
add_executable(example01

	example01.cpp
)

target_link_directories(example01 PRIVATE 
	../../SDL2/lib/x64
)

target_working_directory(example01, WG_ROOT)

target_link_libraries(example01 PRIVATE
	wgsoftgfx
	wondergui 
	${SDL2_LIBS}
)
