source_group(Files REGULAR_EXPRESSION ".*")

add_executable(streamgenerator

	main.cpp
)

target_link_directories(streamgenerator PRIVATE 
)

target_working_directory(streamgenerator ${WG_ROOT} )

target_link_libraries(streamgenerator PRIVATE
	wgsoftgfx
    wgstreamgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


