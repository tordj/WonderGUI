source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgcabiwrapper

	wg3_cabi.cpp
	wg3_cabi.h
	wg3_cabi_bitmapcache.cpp
	wg3_cabi_bitmapcache.h
	wg3_cabi_bitmapfont.cpp
	wg3_cabi_bitmapfont.h
	wg3_cabi_canvaslayers.cpp
	wg3_cabi_canvaslayers.h
	wg3_cabi_font.cpp
	wg3_cabi_font.h
	wg3_cabi_freetypefont.cpp
	wg3_cabi_freetypefont.h
	wg3_cabi_gfxdevice.cpp
	wg3_cabi_gfxdevice.h
	wg3_cabi_surface.cpp
	wg3_cabi_surface.h
	wg3_cabi_surfacefactory.cpp
	wg3_cabi_surfacefactory.h
)

target_include_directories(wgcabiwrapper PRIVATE 
)

