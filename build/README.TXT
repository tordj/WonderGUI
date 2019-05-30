
This directory contains sub-directories with project files and build script
for various development environments. All output such as intermediary files,
libraries and executables remain within the various build folders such as 
not to interfere with each other.

Gnumake, VisualStudio and Xcode projects should all be up to date in all stable
and unstable releases but might be out of date if you grab the project from GitHub.


codelite-linux/		
	Workspace and project files for building WonderGUI and example files
	using the CodeLite IDE in combination with GCC on a Linux environment.
	
	This is only occasionally used and therefore likely to be out of date.


gnumake/
	Makefile for building WonderGUI and example files using GNU Make and
	GCC in a Linux or Unix environment. This is provided to make it easy
	to build WonderGUI from sources for most Linux/Unix developers.
	It should be easy to replace GCC with the compiler of your choice.

	Build options are presented in the comments at the top of the script.

	Please note that you need to run "make clean" before you can make 
	any of the other targets in order to setup the directory structure.

	You need to install development files for freetype6, SDL2, SDL2_image
	and glew in order to build everything.


VisualStudio/
	Solution for building WonderGUI and example files in VisualStudio IDE.
	You will need to add include and library paths for SDL2, SDL2_Image
	and FreeType.

	Development packages (binaries) for SDL2 and SDL2_Image containing library
	and include files can be downloaded from www.libsdl.org and the same for 
	FreeType can be downloaded from www.freetype.org. 

	Also, the OpenGL graphics device (gfxdevice_opengl) and any examples
	using OpenGL relies on GLEW to provide access to modern OpenGL. 
	A development package for GLEW can be downloaded from glew.sourceforge.net.

	Please unpack the development packages to the same directory as WonderGUI.
	Remove version numbering from the packages base directories so that you
	have the following directory structure and you should be ready to build:

	freetype\
	glew\				(optional, only for OpenGL projects)
	SDL2\
	SDL2_Image\
	WonderGUI\


Xcode/
	Project for building WonderGUI and example files in Xcode on macOS.

	You need to install the SDL2, SDL2_image and Freetype frameworks to
	build everything.
