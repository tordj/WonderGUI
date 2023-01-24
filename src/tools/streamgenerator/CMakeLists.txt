set(CMAKE_CXX_STANDARD 20)


if(WIN32)
	set(SDL2_LIBS SDL2 SDL2main)
	set(SDL2IMAGE_LIBS SDL2_image)
elseif(APPLE)
	find_library(SDL2_LIBS SDL2)
	find_library(SDL2IMAGE_LIBS SDL2_image)
else()
	set(SDL2_LIBS SDL2)
	set(SDL2IMAGE_LIBS SDL2_image)
endif()


source_group(Files REGULAR_EXPRESSION ".*")

add_executable(streamgenerator

	main.cpp
)

target_link_directories(streamgenerator PRIVATE 
)

target_include_directories(streamgenerator PRIVATE 
	${WONDERGUI_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
)


target_working_directory(streamgenerator ${WG_ROOT} )

target_link_libraries(streamgenerator PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


