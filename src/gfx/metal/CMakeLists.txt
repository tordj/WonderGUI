source_group(Files REGULAR_EXPRESSION ".*")

set(SOURCES

	wg_metalgfxdevice.h
	wg_metalgfxdevice.mm
	wg_metalshaders.mm
	wg_metalsurface.h
	wg_metalsurface.mm
	wg_metalsurfacefactory.h
	wg_metalsurfacefactory.mm
)

set_source_files_properties(wg3_metalshaders.mm.template wg3_shadercontent.mm PROPERTIES HEADER_FILE_ONLY YES)


set(WGMETALGFX_INCDIRS
	${CMAKE_CURRENT_LIST_DIR}

	PARENT_SCOPE
)

add_library( wgmetalgfx STATIC ${SOURCES} )

target_include_directories(wgmetalgfx PRIVATE 
	${WONDERGFX_INCDIRS}
	${WGMETALGFX_INCDIRS}
)

set_target_properties( wgmetalgfx PROPERTIES FOLDER "Graphics Devices" )

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
set(WGMETALGFX_SOURCES ${SOURCES} PARENT_SCOPE)
