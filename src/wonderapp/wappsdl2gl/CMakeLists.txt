set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

set(SOURCES

	main.cpp
	window.h
	window.cpp
	tinyfiledialogs.h
	tinyfiledialogs.c
	../include/wonderapp.h
)

set(INCDIRS
	${CMAKE_CURRENT_LIST_DIR}
	${WONDERGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
	${PROJECT_SOURCE_DIR}/wonderapp/include
	${PROJECT_SOURCE_DIR}
)

add_library(wappsdl2gl STATIC ${SOURCES} )


target_include_directories(wappsdl2gl PRIVATE 
	${INCDIRS}
	${SDL2_INCDIR}
	${SDL2IMAGE_INCDIR}
)

set_target_properties(wappsdl2gl PROPERTIES FOLDER "WonderApp/Frameworks" )

target_link_directories(wappsdl2gl PUBLIC
	${SDL2_LIBDIR}
	${SDL2IMAGE_LIBDIR}
	${FREETYPE2_LIBDIR}
	${GLEW_LIBDIR}
)

target_link_libraries(wappsdl2gl PRIVATE
	wgopenglgfx
	wgfreetypefont
	wondergui
	${FREETYPE2_LIB}
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
	${OPENGL_LIBS}
)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/ )
set(WAPP_SOURCES ${SOURCES} PARENT_SCOPE)
set(WAPP_INCDIRS ${INCDIRS} PARENT_SCOPE)

