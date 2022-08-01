source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgcapigfx 

	wg_capigfxdevice.cpp
	wg_capigfxdevice.h
	wg_capisurface.cpp
	wg_capisurface.h
	wg_capisurfacefactory.cpp
	wg_capisurfacefactory.h
	wg_capicanvaslayers.cpp
	wg_capicanvaslayers.h
)

target_include_directories(wgcapigfx PRIVATE 
)

