source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgstreamgfx 

	wg3_streamgfxdevice.cpp
	wg3_streamgfxdevice.h
	wg3_streamsurface.cpp
	wg3_streamsurface.h
	wg3_streamsurfacefactory.cpp
	wg3_streamsurfacefactory.h
)

target_include_directories(wgstreamgfx PRIVATE 
)

