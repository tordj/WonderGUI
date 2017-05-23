#!/usr/bin/ruby
# Replace smart pointer references with real pointers.

def printUsage()
  puts "usage: fixit file1 [file2 [...]]"
  puts
end


def smartfix( lines )

  output = []
 
  for line in lines
    if( line =~ /typedef\s+StrongPtr<[A-Za-z]+,[A-Za-z]+_p>/ ) 
      line = line.gsub( /,[A-Za-z]+_p/, '' ) 
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
