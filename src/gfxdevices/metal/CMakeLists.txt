source_group(Files REGULAR_EXPRESSION ".*")

add_library( wgmetalgfx 

	wg_metalgfxdevice.h
	wg_metalgfxdevice.mm
	wg_metalshaders.mm
	wg_metalsurface.h
	wg_metalsurface.mm
	wg_metalsurface.h
	wg_metalsurfacefactory.mm
)

set_source_files_properties(wg3_metalshaders.mm.template wg3_shadercontent.mm PROPERTIES HEADER_FILE_ONLY YES)

target_include_directories(wgmetalgfx PRIVATE 
	
)

