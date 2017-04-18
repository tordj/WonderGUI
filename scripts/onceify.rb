#!/usr/bin/ruby
# Adds #pragma once to header files.

def printUsage()
  puts "usage: onceify file1 [file2 [...]]"
  puts
end


def onceify( lines )

  output = []
  inPublic = false;
  inGroup = false;

  for line in lines

    output << line

    if( line =~ /^#define\s*WG_[A-Z]+_DOT_H/ )
      output << '#pragma once'
    end
  end

  return output
end


if( $*.length < 1 )
  printUsage()
  exit
end

for fileName in $*

  puts "Onceifying: #{fileName}"

  content = onceify( IO.readlines(fileName) )

  f = File.new( fileName, "w")
	f.puts content
	f.close


end
