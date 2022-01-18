source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgstreamgfx 

	wg_streamgfxdevice.cpp
	wg_streamgfxdevice.h
	wg_streamsurface.cpp
	wg_streamsurface.h
	wg_streamsurfacefactory.cpp
	wg_streamsurfacefactory.h

	wg_gfxstream.h
	wg_gfxstreamlogger.cpp
	wg_gfxstreamlogger.h
	wg_gfxstreamplayer.cpp
	wg_gfxstreamplayer.h
	wg_gfxstreamplug.cpp
	wg_gfxstreamplug.h
	wg_gfxstreamreader.cpp
	wg_gfxstreamreader.h
	wg_gfxstreamwriter.cpp
	wg_gfxstreamwriter.h

	wg_cgfxinstream.cpp
	wg_cgfxinstream.h
	wg_cgfxoutstream.cpp
	wg_cgfxoutstream.h

)

target_include_directories(wgstreamgfx PRIVATE 
)

