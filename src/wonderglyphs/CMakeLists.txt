source_group(Code REGULAR_EXPRESSION "wg_*|wonder*")
source_group(C-API REGULAR_EXPRESSION "c-api//*")

set(SOURCES

	wg_bitmapfont.cpp
	wg_bitmapfont.h
	wg_dummyfont.cpp
	wg_dummyfont.h
	wg_font.cpp
	wg_font.h
	wg_glyphsutil.cpp
	wg_glyphsutil.h
	wg_gridwalker.cpp
	wg_gridwalker.h
	wg_printer.cpp
	wg_printer.h
	wg_sysfont.cpp
	wg_sysfont.h
	wonderglyphs.h

	c-api/wg_c_bitmapfont.cpp
	c-api/wg_c_bitmapfont.h
	c-api/wg_c_font.cpp
	c-api/wg_c_font.h
	c-api/wg_c_printer.cpp
	c-api/wg_c_printer.h
	c-api/wonderglyphs_c.h
)


set(INCDIRS
	${WONDERGFX_INCDIRS}
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/c-api
)

add_library( wonderglyphs STATIC ${SOURCES} )
target_include_directories(wonderglyphs PRIVATE ${INCDIRS} )
target_link_libraries(wonderglyphs PUBLIC wondergfx )

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
set(WONDERGLYPHS_SOURCES ${SOURCES} PARENT_SCOPE)
set(WONDERGLYPHS_INCDIRS ${INCDIRS} PARENT_SCOPE)


