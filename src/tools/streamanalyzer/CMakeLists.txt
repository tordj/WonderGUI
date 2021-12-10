source_group(Files REGULAR_EXPRESSION ".*")

add_executable(streamanalyzer

	main.cpp
)

target_link_directories(streamanalyzer PRIVATE 
)

target_working_directory(streamanalyzer ${WG_ROOT} )

target_link_libraries(streamanalyzer PRIVATE
	wgsoftgfx
    wgstreamgfx
	wgfreetypefont
	wondergui 
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


