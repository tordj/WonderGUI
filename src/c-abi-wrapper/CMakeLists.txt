source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgcabiwrapper

	wg_cabi.cpp
	wg_cabi.h
	wg_cabi_bitmapcache.cpp
	wg_cabi_bitmapcache.h
	wg_cabi_bitmapfont.cpp
	wg_cabi_bitmapfont.h
	wg_cabi_canvaslayers.cpp
	wg_cabi_canvaslayers.h
	wg_cabi_font.cpp
	wg_cabi_font.h
	wg_cabi_freetypefont.cpp
	wg_cabi_freetypefont.h
	wg_cabi_gfxdevice.cpp
	wg_cabi_gfxdevice.h
	wg_cabi_surface.cpp
	wg_cabi_surface.h
	wg_cabi_surfacefactory.cpp
	wg_cabi_surfacefactory.h
)

target_include_directories(wgcabiwrapper PRIVATE 
)

