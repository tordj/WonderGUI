source_group(Files REGULAR_EXPRESSION ".*")

set( CLIENT_SOURCES
	wonderplugin.h

	wg_c_plugincapsule.h
	wg_c_pluginroot.cpp
	wg_c_pluginroot.h

	wg_pluginbase.cpp
	wg_pluginbase.h
	wg_pluginbitmapcache.cpp
	wg_pluginbitmapcache.h
	wg_pluginbitmapfont.cpp
	wg_pluginbitmapfont.h
	wg_plugincanvaslayers.cpp
	wg_plugincanvaslayers.h
	wg_pluginfont.cpp
	wg_pluginfont.h
	wg_pluginfreetypefont.cpp
	wg_pluginfreetypefont.h
	wg_plugingfxdevice.cpp
	wg_plugingfxdevice.h
	wg_pluginhostbridge.cpp
	wg_pluginhostbridge.h
	wg_plugininterface.h
	wg_pluginroot.cpp
	wg_pluginroot.h
	wg_pluginsurface.cpp
	wg_pluginsurface.h
	wg_pluginsurfacefactory.cpp
	wg_pluginsurfacefactory.h

	wonderplugin.h
)

set( HOST_SOURCES

	wg_plugininterface.cpp
	wg_plugininterface.h

	wg_plugincapsule.cpp
	wg_plugincapsule.h

	wg_c_plugincapsule.cpp
	wg_c_plugincapsule.h
	
	wg_c_pluginroot.h
)


set(WGCLIENT_SOURCES ${CLIENT_SOURCES} PARENT_SCOPE)

add_library( wgpluginclient STATIC ${CLIENT_SOURCES} )

target_include_directories(wgpluginclient PRIVATE
	${WONDERGUI_INCDIRS}
	${WGPLUGIN_INCDIRS}
)

add_library( wgpluginhost STATIC ${HOST_SOURCES} )

target_include_directories(wgpluginhost PRIVATE
	${WONDERGUI_INCDIRS}
	${WGPLUGIN_INCDIRS}
)

set_target_properties( wgpluginhost wgpluginclient PROPERTIES FOLDER "Plugin" )

 
