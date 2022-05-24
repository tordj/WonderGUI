#!/usr/bin/ruby
# 

def printUsage()
  puts "usage: flip_includes file1 [file2 [...]]"
  puts
end


def flip_includes( lines )

  output = []

  for line in lines
    if( line =~ /^#include.*<wg_/ )
    	line.sub! /<wg_/, '<wg3_'
    elsif( line =~ /^#include.*<wg3_/ )
    	line.sub! /<wg3_/, '<wg_'
    elsif( line =~ /WG_\w+_DOT_H/ )
    	line.sub! /WG_/, 'WG3_'
    elsif( line =~ /WG3_\w+_DOT_H/ )
    	line.sub! /WG3_/, 'WG_'
    end
    output << line
  end

  return output
end


def flip_cmakelist( lines )

  output = []

  for line in lines
    if( line =~ /wg_/ )
      line.sub! /wg_/, 'wg3_'
    elsif( line =~ /wg3_/ )
      line.sub! /wg3_/, 'wg_'
    end
    output << line
  end

  return output

end



if( $*.length < 1 )
  printUsage()
  exit
end

for fileName in $*

  if( fileName =~ /CMakeLists.txt/ )
    content = flip_cmakelist( IO.readlines(fileName) )
  else
    content = flip_includes( IO.readlines(fileName) )
  end


  if( fileName =~ /wg3_/ )
  	newFileName = fileName.sub( /wg3_/, 'wg_' )
  else
 	  newFileName = fileName.sub( /wg_/, 'wg3_' )
  end

 # puts "Flipping #{fileName} to #{newFileName}"

  File.delete( fileName )

  f = File.new( newFileName, "w")
	f.puts content
	f.close
end
