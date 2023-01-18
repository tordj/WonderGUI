source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgcabiclient STATIC

	wg_cabi.h
	wg_cabi_base.cpp
	wg_cabi_base.h
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
	wg_cabi_hostbridge.cpp
	wg_cabi_hostbridge.h	
	wg_cabi_root.cpp
	wg_cabi_root.h
	wg_cabi_root_incalls.cpp
	wg_cabi_root_incalls.h
	wg_cabi_root_interface.h
	wg_cabi_surface.cpp
	wg_cabi_surface.h
	wg_cabi_surfacefactory.cpp
	wg_cabi_surfacefactory.h
)

add_library( wgcabihost STATIC

	wg_cabi_base.cpp
	wg_cabi_base.h
	wg_cabi_root_interface.h
	wg_cabi_root_outcalls.cpp
	wg_cabi_root_outcalls.h
	wg_cabi_capsule.cpp
	wg_cabi_capsule.h	
)

