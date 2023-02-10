source_group(Base REGULAR_EXPRESSION "wg_*|wonder*")
source_group(C-API REGULAR_EXPRESSION "c-api//*")
source_group(Stream REGULAR_EXPRESSION "stream/wg_*")
source_group(C-API/Stream REGULAR_EXPRESSION "stream/c-api/wg_*")

set( SOURCES

	wg_bitmapcache.cpp
	wg_bitmapcache.h
	wg_canvaslayers.cpp
	wg_canvaslayers.h
	wg_color.cpp
	wg_color.h
	wg_geo.cpp
	wg_geo.h
	wg_gfxbase.cpp
	wg_gfxbase.h
	wg_gfxdevice.cpp
	wg_gfxdevice.h
	wg_gfxenumextras.cpp
	wg_gfxenumextras.h
	wg_gfxtypes.h
	wg_gfxutil.cpp
	wg_gfxutil.h
	wg_gradient.cpp
	wg_gradient.h
	wg_nullgfxdevice.cpp
	wg_nullgfxdevice.h
	wg_patches.cpp
	wg_patches.h
	wg_pixeltools.cpp
	wg_pixeltools.h
	wg_surface.cpp
	wg_surface.h
	wg_surfacefactory.cpp
	wg_surfacefactory.h
	wg_surfacefileheader.h
	wg_surfacefileinspector.cpp
	wg_surfacefileinspector.h
	wg_surfacereader.cpp
	wg_surfacereader.h
	wg_surfacewriter.cpp
	wg_surfacewriter.h
	wondergfx.h

	c-api/wg_c_bitmapcache.cpp
	c-api/wg_c_bitmapcache.h
	c-api/wg_c_canvaslayers.cpp
	c-api/wg_c_canvaslayers.h
	c-api/wg_c_color.cpp
	c-api/wg_c_color.h
	c-api/wg_c_geo.cpp
	c-api/wg_c_geo.h
	c-api/wg_c_gfxbase.cpp
	c-api/wg_c_gfxbase.h
	c-api/wg_c_gfxdevice.cpp
	c-api/wg_c_gfxdevice.h
	c-api/wg_c_gfxtypes.h
	c-api/wg_c_gradient.cpp
	c-api/wg_c_gradient.h
	c-api/wg_c_internal.cpp
	c-api/wg_c_internal.h
	c-api/wg_c_surface.cpp
	c-api/wg_c_surface.h
	c-api/wg_c_surfacefactory.cpp
	c-api/wg_c_surfacefactory.h
	c-api/wg_c_surfacereader.cpp
	c-api/wg_c_surfacereader.h
	c-api/wg_c_surfacewriter.cpp
	c-api/wg_c_surfacewriter.h
	c-api/wondergfx_c.h

	stream/wg_gfxstream.h
	stream/wg_gfxstreambuffer.cpp
	stream/wg_gfxstreambuffer.h
	stream/wg_gfxstreamdecoder.cpp
	stream/wg_gfxstreamdecoder.h
	stream/wg_gfxstreamdevice.cpp
	stream/wg_gfxstreamdevice.h
	stream/wg_gfxstreamencoder.cpp
	stream/wg_gfxstreamencoder.h
	stream/wg_gfxstreamlogger.cpp
	stream/wg_gfxstreamlogger.h
	stream/wg_gfxstreamloopwrapper.cpp
	stream/wg_gfxstreamloopwrapper.h
	stream/wg_gfxstreamplayer.cpp
	stream/wg_gfxstreamplayer.h
	stream/wg_gfxstreampump.cpp
	stream/wg_gfxstreampump.h
	stream/wg_gfxstreamreader.cpp
	stream/wg_gfxstreamreader.h
	stream/wg_gfxstreamsink.h
	stream/wg_gfxstreamsource.h
	stream/wg_gfxstreamsplitter.cpp
	stream/wg_gfxstreamsplitter.h
	stream/wg_gfxstreamsurface.cpp
	stream/wg_gfxstreamsurface.h
	stream/wg_gfxstreamsurfacefactory.cpp
	stream/wg_gfxstreamsurfacefactory.h
	stream/wg_gfxstreamwrapper.cpp
	stream/wg_gfxstreamwrapper.h
	stream/wg_gfxstreamwriter.cpp
	stream/wg_gfxstreamwriter.h
	stream/wondergfxstream.h

	stream/c-api/wg_c_gfxstreambuffer.cpp
	stream/c-api/wg_c_gfxstreambuffer.h
	stream/c-api/wg_c_gfxstreamlogger.cpp
	stream/c-api/wg_c_gfxstreamlogger.h
	stream/c-api/wg_c_gfxstreamloopwrapper.cpp
	stream/c-api/wg_c_gfxstreamloopwrapper.h
	stream/c-api/wg_c_gfxstreamplayer.cpp
	stream/c-api/wg_c_gfxstreamplayer.h
	stream/c-api/wg_c_gfxstreampump.cpp
	stream/c-api/wg_c_gfxstreampump.h
	stream/c-api/wg_c_gfxstreamreader.cpp
	stream/c-api/wg_c_gfxstreamreader.h
	stream/c-api/wg_c_gfxstreamsplitter.cpp
	stream/c-api/wg_c_gfxstreamsplitter.h
	stream/c-api/wg_c_gfxstreamwrapper.cpp
	stream/c-api/wg_c_gfxstreamwrapper.h
	stream/c-api/wg_c_gfxstreamwriter.cpp
	stream/c-api/wg_c_gfxstreamwriter.h
	stream/c-api/wondergfxstream_c.h
)

set(INCDIRS
	${WONDERGEAR_INCDIRS}
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/c-api
	${CMAKE_CURRENT_LIST_DIR}/stream
	${CMAKE_CURRENT_LIST_DIR}/stream/c-api
)


add_library( wondergfx STATIC ${SOURCES} )
target_include_directories(wondergfx PRIVATE ${INCDIRS} )
target_link_libraries(wondergfx PUBLIC wondergear )


list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
set(WONDERGFX_SOURCES ${SOURCES} PARENT_SCOPE)
set(WONDERGFX_INCDIRS ${INCDIRS} PARENT_SCOPE)


