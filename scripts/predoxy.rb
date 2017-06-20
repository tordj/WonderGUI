#!/usr/bin/ruby
# Preprocesses WonderGUI header files for Doxygen.

def printUsage()
  puts "usage: doxify file1 [file2 [...]]"
  puts
end


def doxify( lines )

  output = []
  inPublic = false;
  inGroup = false;

  for line in lines
    if( line =~ /^\s*public:/ )
      inPublic = true
    elsif( line =~ /^\s*private:/ or line =~ /^\s*protected:/  )
      inPublic = false

      if( inGroup )
        output << '///@}'
        inGroup = false
      end

    end

    if( inPublic and line =~ /^\s*\/\/\._+\s+[A-Za-z]+\s+_+/ )
      groupName = line.gsub( /[.\s_\/]/, '' ).chomp
      puts "Found group: '#{groupName}'"

      if(inGroup)
        output << '///@}'
      end

      output << "/** @name #{groupName} */"
      output << '///@{'
      inGroup = true
    else
      output << line
    end
  end

  return output
end


if( $*.length < 1 )
  printUsage()
  exit
end

for fileName in $*

  puts "Doxifying: #{fileName}"

  content = doxify( IO.readlines(fileName) )

  f = File.new( fileName, "w")
	f.puts content
	f.close


end
