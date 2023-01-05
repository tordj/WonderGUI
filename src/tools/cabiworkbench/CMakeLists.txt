set(CMAKE_CXX_STANDARD 20)

source_group(Files REGULAR_EXPRESSION ".*")

add_library(cabiclient SHARED
	cabiclient.cpp
		../../c-abi-wrapper/wg_cabi.h
		../../c-abi-wrapper/wg_cabi_base.cpp
		../../c-abi-wrapper/wg_cabi_base.h
			../../c-abi-wrapper/wg_cabi_bitmapcache.cpp
			../../c-abi-wrapper/wg_cabi_bitmapcache.h
			../../c-abi-wrapper/wg_cabi_bitmapfont.cpp
			../../c-abi-wrapper/wg_cabi_bitmapfont.h
			../../c-abi-wrapper/wg_cabi_canvaslayers.cpp
			../../c-abi-wrapper/wg_cabi_canvaslayers.h
			../../c-abi-wrapper/wg_cabi_capsule.cpp
			../../c-abi-wrapper/wg_cabi_capsule.h
			../../c-abi-wrapper/wg_cabi_font.cpp
			../../c-abi-wrapper/wg_cabi_font.h
			../../c-abi-wrapper/wg_cabi_freetypefont.cpp
			../../c-abi-wrapper/wg_cabi_freetypefont.h
			../../c-abi-wrapper/wg_cabi_gfxdevice.cpp
			../../c-abi-wrapper/wg_cabi_gfxdevice.h
			../../c-abi-wrapper/wg_cabi_root.cpp
			../../c-abi-wrapper/wg_cabi_root.h
			../../c-abi-wrapper/wg_cabi_root_incalls.cpp
			../../c-abi-wrapper/wg_cabi_root_incalls.h
			../../c-abi-wrapper/wg_cabi_root_interface.h
			../../c-abi-wrapper/wg_cabi_surface.cpp
			../../c-abi-wrapper/wg_cabi_surface.h
			../../c-abi-wrapper/wg_cabi_surfacefactory.cpp
			../../c-abi-wrapper/wg_cabi_surfacefactory.h
	
	)

target_link_libraries(cabiclient PRIVATE
	wondergui
)

set_target_properties(cabiclient PROPERTIES POSITION_INDEPENDENT_CODE ON)


add_executable(cabiworkbench

	../../baseplates/sdl_software/main.cpp
	../../baseplates/sdl_software/baseplate.h
	../../baseplates/sdl_software/baseplate.cpp
	../../baseplates/sdl_software/tinyfiledialogs.h
	../../baseplates/sdl_software/tinyfiledialogs.c
	wonderapp.cpp
	wonderapp.h
		../../c-abi-wrapper/wg_cabi.h
		../../c-abi-wrapper/wg_cabi_base.cpp
		../../c-abi-wrapper/wg_cabi_base.h

		../../c-abi-wrapper/wg_cabi_capsule.cpp
		../../c-abi-wrapper/wg_cabi_capsule.h
		../../c-abi-wrapper/wg_cabi_root_outcalls.cpp
		../../c-abi-wrapper/wg_cabi_root_outcalls.h
		../../c-abi-wrapper/wg_cabi_root_interface.h
)

target_include_directories(cabiworkbench PRIVATE 
	../../baseplates/sdl_software
)

target_link_directories(cabiworkbench PRIVATE 

)

add_dependencies(cabiworkbench cabiclient)

target_working_directory(cabiworkbench ${WG_ROOT} )

target_link_libraries(cabiworkbench PRIVATE
	wgsoftgfx
	wgfreetypefont
	wondergui
	${SDL2_LIBS}
	${SDL2IMAGE_LIBS}
)


