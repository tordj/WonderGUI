

add_subdirectory(fontgenerator)
add_subdirectory(gfxdevicetester)
add_subdirectory(gfxworkbench)
add_subdirectory(image2surf)
add_subdirectory(packlayouttester)
add_subdirectory(pluginworkbench)
add_subdirectory(softkernelgenerator)
add_subdirectory(surfviewer)
add_subdirectory(streamanalyzer)
add_subdirectory(streamgenerator)
add_subdirectory(unittester)
add_subdirectory(workbench)
add_subdirectory(legacyworkbench)
add_subdirectory(embeddedworkbench)


set_target_properties( 
	gfxdevicetester 
	embeddedworkbench 
	legacyworkbench 
	legacytestplugin 
	testplugin 
	fontgenerator 
	gfxworkbench 
	image2surf 
	packlayouttester 
	pluginworkbench 
	softkernelgenerator 
	streamanalyzer 
	streamgenerator 
	surfviewer 
	unittester 
	workbench 
	PROPERTIES FOLDER "Tools" )
