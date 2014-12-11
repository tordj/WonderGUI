
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


