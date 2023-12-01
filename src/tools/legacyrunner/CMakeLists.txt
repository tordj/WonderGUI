set(CMAKE_CXX_STANDARD 20)


set( SOURCES
	legacyrunner.cpp
	legacyrunner.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_executable(legacyrunner
	${SOURCES}
)

target_include_directories(legacyrunner PRIVATE 
	${WAPP_INCDIRS}
	${WGLEGACYGUI_INCDIRS}

)

target_working_directory(legacyrunner ${WG_ROOT} )

target_link_libraries(legacyrunner PRIVATE
	${WAPPLIB}
	wglegacygui

)

