source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgsoftgfx 

	c-api/wg_c_softgfx.cpp
	c-api/wg_c_softgfx.h
	c-api/wg_c_softkernels_default.h
	c-api/wg_c_softkernels_rgb565be_base.h
	c-api/wg_c_softkernels_rgb565be_extras.h

	wg_softkernels_default.cpp
	wg_softkernels_default.h
	wg_softkernels_rgb565be_base.cpp
	wg_softkernels_rgb565be_base.h
	wg_softkernels_rgb565be_extras.cpp
	wg_softkernels_rgb565be_extras.h
	wg_softkernelstanza.impl.h
	wg_softgfxdevice.cpp
	wg_softgfxdevice.h
	wg_softsurface.cpp
	wg_softsurface.h
	wg_softsurfacefactory.cpp
	wg_softsurfacefactory.h
)

target_include_directories(wgsoftgfx PRIVATE 
	${WONDERGFX_INCDIRS}
	${WGSOFTGFX_INCDIRS}
)

set_target_properties( wgsoftgfx PROPERTIES FOLDER "Graphics Devices" )