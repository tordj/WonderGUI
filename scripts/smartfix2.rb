#!/usr/bin/ruby
# Replace smart pointer references with real pointers.

def printUsage()
  puts "usage: fixit file1 [file2 [...]]"
  puts
end


def smartfix( lines )

  output = []
 
  for line in lines
    if( line =~ /typedef\s+WeakPtr<[A-Za-z]+,[A-Za-z]+_wp>/ ) 
      line = line.gsub( /,[A-Za-z]+_wp/, '' ) 
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
