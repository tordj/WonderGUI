#!/usr/bin/ruby
# Replace smart pointer references with real pointers.

def printUsage()
  puts "usage: fixit file1 [file2 [...]]"
  puts
end


def smartfix( lines )

  output = []

  for line in lines

	ofs = line.index( /const\s+[A-Za-z]+_p&/ )

	while( ofs != nil )

	  length = line.index( '&', ofs ) - ofs + 1

	  pointer = line[ofs, length-3].gsub( /const\s+/, '' ) + ' *'

	  line = line[0,ofs] + pointer + line[ofs+length,10000]

	  ofs = line.index( /const\s+[A-Za-z]+_p&/ )
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

  puts "Processing: #{fileName}"

  content = smartfix( IO.readlines(fileName) )

  f = File.new( fileName, "w")
	f.puts content
	f.close


end
