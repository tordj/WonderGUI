#!/usr/bin/ruby
# Adds #pragma once to header files.

def printUsage()
  puts "usage: deguard_headers file1 [file2 [...]]"
  puts
end


def deguard_headers( lines, filename )

  output = []

  ownGuard = filename.sub /.*wg_/, ''
  ownGuard.sub! /\.cpp/, ''
  ownGuard.sub! /\.h/, ''
  ownGuard.upcase!
  ownGuard = 'WG_' + ownGuard + '_DOT_H'
  puts ownGuard

  inConditional = false;

  for line in lines


	if( line =~ /^#ifndef\s*WG_[A-Z]+_DOT_H/ and !line[ownGuard] )
		inConditional = true;
	elsif( inConditional and line =~ /^#endif/ )
		inConditional = false;
	elsif( inConditional )
		line.sub! /#\s+include/, "#include"
		output << line
	else
	  output << line
	end
  end

  return output
end


def despace_includes( lines )

  output = []

  prevWasInclude = false;
  spaceAfterInclude = false;

  for line in lines

	if( line =~ /^#include.*<wg_/ )
		prevWasInclude = true;
		if( spaceAfterInclude )
		  output.pop
		end
	elsif( prevWasInclude and line !~ /[A-Za-z0-9]/ )
		spaceAfterInclude = true;
		prevWasInclude = false;
	else
	   prevWasInclude = false;
	   spaceAfterInclude = false;
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

  puts "Removing header guards: #{fileName}"

  content = deguard_headers( IO.readlines(fileName), fileName )
  content = despace_includes( content )


  f = File.new( fileName, "w")
	f.puts content
	f.close
end
