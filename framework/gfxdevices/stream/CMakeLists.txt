source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgstreamgfx 

	wg3_streamgfxdevice.cpp
	wg3_streamgfxdevice.h
	wg3_streamsurface.cpp
	wg3_streamsurface.h
	wg3_streamsurfacefactory.cpp
	wg3_streamsurfacefactory.h

	wg3_gfxstream.h

	wg3_gfxstreamencoder.cpp
	wg3_gfxstreamencoder.h

	wg3_gfxstreamdecoder.cpp
	wg3_gfxstreamdecoder.h

	wg3_gfxstreamsplitter.cpp
	wg3_gfxstreamsplitter.h

	wg3_gfxstreambuffer.cpp
	wg3_gfxstreambuffer.h


	wg3_gfxstreamlogger.cpp
	wg3_gfxstreamlogger.h
	wg3_gfxstreamplayer.cpp
	wg3_gfxstreamplayer.h
	wg3_gfxstreamreader.cpp
	wg3_gfxstreamreader.h
	wg3_gfxstreamwriter.cpp
	wg3_gfxstreamwriter.h

	wg3_cgfxinstream.cpp
	wg3_cgfxinstream.h
	wg3_cgfxoutstream.cpp
	wg3_cgfxoutstream.h

	wg3_gfxstreampump.cpp
	wg3_gfxstreampump.h

)

target_include_directories(wgstreamgfx PRIVATE 
)

