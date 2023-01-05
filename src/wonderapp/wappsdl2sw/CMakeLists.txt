set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_library(wappsdl2sw

	main.cpp
	window.h
	window.cpp
	tinyfiledialogs.h
	tinyfiledialogs.c
	../include/wonderapp.h
)

target_include_directories(wappsdl2sw PRIVATE 
	../include
)

set_target_properties(wappsdl2sw PROPERTIES FOLDER "WonderApp/Frameworks" )

target_link_libraries(wappsdl2sw PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


