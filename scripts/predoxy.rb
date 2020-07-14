#!/usr/bin/ruby
# Preprocesses WonderGUI header files for Doxygen.

#======================================
# => printUsage
#======================================

def printUsage()
  puts "usage: doxify file1 [file2 [...]]"
  puts
end


#======================================
# => doxify
#======================================

def doxify( lines )

  scope = []

  output = []
  inPublic = false
  inGroup = false

  for line in lines

	if( line =~ /^\s*{/ )
		scope.push inPublic
		scope.push inGroup
		inPublic = false
		inGroup = false
	end

	if( line =~ /^\s*}/ )

	  if( inGroup )
		output << '///@}'
	  end

		inGroup = scope.pop
		inPublic = scope.pop
	end


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


#======================================
# => MAIN CODE
#======================================


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
