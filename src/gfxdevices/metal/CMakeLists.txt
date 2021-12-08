
add_library( wgmetalgfx 

	wg_metalgfxdevice.h
	wg_metalgfxdevice.mm
	wg_metalshaders.mm
	wg_metalsurface.h
	wg_metalsurface.mm
	wg_metalsurfacefactory.h
	wg_metalsurfacefactory.mm
)

target_include_directories(wgmetalgfx PRIVATE 
	
)

