set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")


set(SOURCES

	main.cpp
    	sdlwindow.h
	sdlwindow.cpp
#	sdlwindow_sw.h
#	sdlwindow_sw.cpp
	sdlwindow_gl.h
	sdlwindow_gl.cpp
	tinyfiledialogs.h
	tinyfiledialogs.c
	../common/wonderapp.h
	../common/window.h
	../common/window.cpp
)

set(INCDIRS
	${CMAKE_CURRENT_LIST_DIR}
	${WONDERGUI_INCDIRS}
	${WGFREETYPEFONT_INCDIRS}
	${WGOPENGLGFX_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${PROJECT_SOURCE_DIR}/wonderapp/common
	${PROJECT_SOURCE_DIR}
)

add_library(wappsdl2 STATIC ${SOURCES} )

target_compile_definitions(wappsdl2 PRIVATE USE_OPENGL)

target_include_directories(wappsdl2 PRIVATE 
	${INCDIRS}
	${SDL2_INCDIR}
	${SDL2IMAGE_INCDIR}
)

set_target_properties(wappsdl2 PROPERTIES FOLDER "WonderApp/Frameworks" )

target_link_directories(wappsdl2 PUBLIC
	${SDL2_LIBDIR}
	${SDL2IMAGE_LIBDIR}
	${FREETYPE2_LIBDIR}
	${GLEW_LIBDIR}
)

target_link_libraries(wappsdl2 PRIVATE
	wgopenglgfx
	wgsoftgfx
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

