source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgmetalgfx 

	wg3_metalgfxdevice.h
	wg3_metalgfxdevice.mm
	wg3_metalshaders.mm
	wg3_metalsurface.h
	wg3_metalsurface.mm
	wg3_metalsurface.h
	wg3_metalsurfacefactory.mm
)

set_source_files_properties(wg_metalshaders.mm.template wg3_shadercontent.mm PROPERTIES HEADER_FILE_ONLY YES)

target_include_directories(wgmetalgfx PRIVATE 
	
)

