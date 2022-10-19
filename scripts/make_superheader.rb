#!/usr/bin/ruby
# Finds .h files recursively starting from specified directories and generates wondergui.h

def printUsage()
  puts "make_superheader outputfile dir1 [dir2 [...]]"
  puts
end


if( $*.length < 2 )
  printUsage()
  exit
end


outputFileName = $*.shift

paths = []

for arg in $*
  paths += Dir.glob("#{arg}/**/*.h")
end

headerlist = []

for path in paths

  name = path.split('/').last

  if( !(name.include? '.impl.') && !(name.include? '.intrn.') && name != 'wondergui.h' )
	headerlist << name
  end
end

output = []

output << "/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/"

output << "
#ifndef WONDERGUI_DOT_H
#define WONDERGUI_DOT_H
#pragma once

"

headerlist.sort!

for header in headerlist
output << "#include <#{header}>\n"
end
output << "\n#endif //WONDERGUI_DOT_H\n"


f = File.new( outputFileName, "wb")
f.puts output
f.close


