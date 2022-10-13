

add_subdirectory(fontgenerator)
add_subdirectory(gfxdevicetester)
add_subdirectory(gfxworkbench)
add_subdirectory(packlayouttester)
add_subdirectory(softkernelgenerator)
add_subdirectory(streamanalyzer)
add_subdirectory(streamgenerator)
#add_subdirectory(unittester)
add_subdirectory(workbench)

set_target_properties( fontgenerator gfxdevicetester gfxworkbench packlayouttester softkernelgenerator streamanalyzer streamgenerator workbench PROPERTIES FOLDER "Tools" )
