
set( SOURCES

	wg_gfxstream.h
	wg_gfxstreambuffer.cpp
	wg_gfxstreambuffer.h
	wg_gfxstreamdecoder.cpp
	wg_gfxstreamdecoder.h
	wg_gfxstreamdevice.cpp
	wg_gfxstreamdevice.h
	wg_gfxstreamencoder.cpp
	wg_gfxstreamencoder.h
	wg_gfxstreamfastencoder.cpp
	wg_gfxstreamfastencoder.h
	wg_gfxstreamlogger.cpp
	wg_gfxstreamlogger.h
	wg_gfxstreamloopwrapper.cpp
	wg_gfxstreamloopwrapper.h
	wg_gfxstreamplayer.cpp
	wg_gfxstreamplayer.h
	wg_gfxstreampump.cpp
	wg_gfxstreampump.h
	wg_gfxstreamreader.cpp
	wg_gfxstreamreader.h
	wg_gfxstreamsink.h
	wg_gfxstreamsource.h
	wg_gfxstreamsplitter.cpp
	wg_gfxstreamsplitter.h
	wg_gfxstreamsurface.cpp
	wg_gfxstreamsurface.h
	wg_gfxstreamsurfacefactory.cpp
	wg_gfxstreamsurfacefactory.h
	wg_gfxstreamtrimencoder.cpp
	wg_gfxstreamtrimencoder.h	
	wg_gfxstreamedgemap.cpp
	wg_gfxstreamedgemap.h
	wg_gfxstreamedgemapfactory.cpp
	wg_gfxstreamedgemapfactory.h
	wg_gfxstreamwrapper.cpp
	wg_gfxstreamwrapper.h
	wg_gfxstreamwriter.cpp
	wg_gfxstreamwriter.h
	wondergfxstream.h

	c-api/wg_c_gfxstreambuffer.h
	c-api/wg_c_gfxstreambuffer.cpp
	c-api/wg_c_gfxstreamlogger.cpp
	c-api/wg_c_gfxstreamlogger.h
	c-api/wg_c_gfxstreamloopwrapper.cpp
	c-api/wg_c_gfxstreamloopwrapper.h
	c-api/wg_c_gfxstreamplayer.cpp
	c-api/wg_c_gfxstreamplayer.h
	c-api/wg_c_gfxstreampump.cpp
	c-api/wg_c_gfxstreampump.h
	c-api/wg_c_gfxstreamreader.cpp
	c-api/wg_c_gfxstreamreader.h
	c-api/wg_c_gfxstreamsink.cpp
	c-api/wg_c_gfxstreamsink.h
	c-api/wg_c_gfxstreamsource.cpp
	c-api/wg_c_gfxstreamsource.h
	c-api/wg_c_gfxstreamsplitter.cpp
	c-api/wg_c_gfxstreamsplitter.h
	c-api/wg_c_gfxstreamwrapper.cpp
	c-api/wg_c_gfxstreamwrapper.h
	c-api/wg_c_gfxstreamwriter.cpp
	c-api/wg_c_gfxstreamwriter.h
	c-api/wondergfxstream_c.h
)

set(INCDIRS
	${WONDERGEAR_INCDIRS}
	${WONDERGFX_INCDIRS}
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/c-api
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library( wggfxstream STATIC ${SOURCES} )
target_include_directories(wggfxstream PRIVATE ${INCDIRS} )
target_link_libraries(wggfxstream PUBLIC wondergear )

set_target_properties( wggfxstream PROPERTIES FOLDER "Extensions" )

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
set(GFXSTREAM_SOURCES ${SOURCES} PARENT_SCOPE)
set(GFXSTREAM_INCDIRS ${INCDIRS} PARENT_SCOPE)


