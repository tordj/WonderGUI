
add_library( wgstreamgfx 

	wg3_streamgfxdevice.cpp
	wg3_streamgfxdevice.h
	wg3_streamsurface.cpp
	wg3_streamsurface.h
	wg3_streamsurfacefactory.cpp
	wg3_streamsurfacefactory.h

	wg3_gfxstream.h
	wg3_gfxstreamlogger.cpp
	wg3_gfxstreamlogger.h
	wg3_gfxstreamplayer.cpp
	wg3_gfxstreamplayer.h
	wg3_gfxstreamplug.cpp
	wg3_gfxstreamplug.h
	wg3_gfxstreamreader.cpp
	wg3_gfxstreamreader.h
	wg3_gfxstreamwriter.cpp
	wg3_gfxstreamwriter.h

	wg3_cgfxinstream.cpp
	wg3_cgfxinstream.h
	wg3_cgfxoutstream.cpp
	wg3_cgfxoutstream.h

)

target_include_directories(wgstreamgfx PRIVATE 
)

