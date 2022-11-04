

add_subdirectory(fontgenerator)
add_subdirectory(gfxdevicetester)
add_subdirectory(gfxworkbench)
add_subdirectory(image2surf)
add_subdirectory(packlayouttester)
add_subdirectory(softkernelgenerator)
add_subdirectory(surfviewer)
add_subdirectory(streamanalyzer)
add_subdirectory(streamgenerator)
#add_subdirectory(unittester)
add_subdirectory(workbench)

set_target_properties( fontgenerator gfxdevicetester gfxworkbench image2surf packlayouttester softkernelgenerator streamanalyzer streamgenerator surfviewer workbench PROPERTIES FOLDER "Tools" )
