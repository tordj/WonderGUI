
This directory contains sub-directories with project files and build script
for various development environments. All output such as intermediary files,
libraries and executables should remain within the various build folders
such as not to interfere with each other.


codelite-linux/		
	Workspace and project files for building WonderGUI and example files
	using the CodeLite IDE in combination with GCC on a Linux environment.
	This is the development environment of the lead WonderGUI developer
	and should therefore always be up-to-date.

gnumake/
	Makefile for building WonderGUI and example files using GNU Make and
	GCC in a Linux or Unix environment. This is provided to make it easy
	to build WonderGUI from sources for most Linux/Unix developers.
	It should be easy to replace GCC with the compiler of your choice.
	This makefile should be up-to-date in all stable and unstable
	releases but might be out-of-date if you grab the project from GitHub.

VisualStudio/
	Solution for building WonderGUI and example files in VisualStudio 2015.
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
	Solution for building WonderGUI and example files in Xcode on macOS. This
	is currently unmaintained so please see the project only as a starting 
	point. You will need to make modifications to the project files in order
	to have a working Xcode solution.