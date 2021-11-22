
add_library( wgsoftgfx 

	wg3_softgfxdevice.cpp
	wg3_softgfxdevice.h
	wg3_softsurface.cpp
	wg3_softsurface.h
	wg3_softsurfacefactory.cpp
	wg3_softsurfacefactory.h
)

target_include_directories(wgsoftgfx PRIVATE 
)

