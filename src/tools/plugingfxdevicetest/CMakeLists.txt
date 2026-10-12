set(CMAKE_CXX_STANDARD 20)

add_library(plugingfxdevicetestplugin SHARED
	plugin.cpp
	${WONDERGEAR_SOURCES}
	${WONDERGFX_SOURCES}
	${WONDERGUI_SOURCES}
	${WGPLUGIN_SOURCES}
)

target_include_directories(plugingfxdevicetestplugin PRIVATE
	${WGPLUGIN_INCDIRS}
	${WONDERGUI_INCDIRS}
	"${CMAKE_CURRENT_BINARY_DIR}"
)

set_target_properties(plugingfxdevicetestplugin PROPERTIES
	C_VISIBILITY_PRESET       hidden
	CXX_VISIBILITY_PRESET     hidden
	VISIBILITY_INLINES_HIDDEN YES
)

add_executable(plugingfxdevicetest
	host.cpp
)

target_include_directories(plugingfxdevicetest PRIVATE
	${WGPLUGIN_INCDIRS}
	${WGSOFTGFX_INCDIRS}
	${WONDERGUI_INCDIRS}
	"${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(plugingfxdevicetest PRIVATE
	wgpluginhost
	wgsoftgfx
	wondergui
	${CMAKE_DL_LIBS}
)

add_dependencies(plugingfxdevicetest plugingfxdevicetestplugin)
