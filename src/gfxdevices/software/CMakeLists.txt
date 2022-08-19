source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgsoftgfx 

	wg_c_softgfx.cpp
	wg_c_softgfx.h

	wg_softkernels.cpp
	wg_softkernelstanza.impl.h
	wg_softgfxdevice.cpp
	wg_softgfxdevice.h
	wg_softsurface.cpp
	wg_softsurface.h
	wg_softsurfacefactory.cpp
	wg_softsurfacefactory.h
)

target_include_directories(wgsoftgfx PRIVATE 
)

