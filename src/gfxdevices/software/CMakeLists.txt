source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgsoftgfx 

	wg_softgfxdevice.cpp
	wg_softgfxdevice.h
	wg_softsurface.cpp
	wg_softsurface.h
	wg_softsurfacefactory.cpp
	wg_softsurfacefactory.h
)

target_include_directories(wgsoftgfx PRIVATE 
)

