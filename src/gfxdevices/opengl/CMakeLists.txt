project(GLGfxDevice)

ADD_LIBRARY( wgopenglgfx 

	wg_glgfxdevice.cpp
	wg_glshaders.cpp 
	wg_glsurface.cpp 
	wg_glsurfacefactory.cpp
)

target_include_directories(wgopenglgfx PRIVATE 
	
)

