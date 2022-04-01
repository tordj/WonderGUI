source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgstreamgfx 

	wg_streamgfxdevice.cpp
	wg_streamgfxdevice.h
	wg_streamsurface.cpp
	wg_streamsurface.h
	wg_streamsurfacefactory.cpp
	wg_streamsurfacefactory.h
)

target_include_directories(wgstreamgfx PRIVATE 
)

