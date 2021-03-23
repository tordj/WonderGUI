project(SoftGfxDevice)

ADD_LIBRARY( wgsoftgfx STATIC 

	wg_softgfxdevice.cpp 
	wg_softsurface.cpp 
	wg_softsurfacefactory.cpp
)

target_include_directories(wgsoftgfx PRIVATE 

	.
)

