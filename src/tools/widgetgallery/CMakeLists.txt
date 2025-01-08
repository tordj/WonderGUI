set(CMAKE_CXX_STANDARD 20)

set( SOURCES

	widgetgallery.cpp
	widgetgallery.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(widgetgallery
	${SOURCES}
)

target_include_directories(widgetgallery PRIVATE 
	${WAPP_INCDIRS}
)

target_working_directory(widgetgallery ${WG_ROOT} )

target_link_libraries(widgetgallery PRIVATE
	${WAPPLIB}
	wg_theme_steel_n_red
)


