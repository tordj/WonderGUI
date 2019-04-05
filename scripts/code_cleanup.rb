#!/usr/bin/ruby
# Does a cleanup of input text files, such as replacing CR/LF with LF etc


def printUsage()
  puts "usage: code_cleanup file1 [file2 [...]]"
  puts
end


def cleanup( lines)

	for line in lines

		line.chomp!							# Remove line endings
		line.gsub!( /\p{Blank}+$/, '' )		# Remove trailing spaces and tabs

		line.gsub!( /^    /, "\t" )			# Replace initial spaces with tabs

		res = 1
		while( res != nil )
			res = line.gsub!( /\t    /, "\t\t" )	# Replace spaces following tabs with tabs
		end

		res = 1
		while( res != nil )
			res = line.gsub!( /    \t/, "\t\t" )	# Replace spaces preceeded by tabs with tabs
		end

	end
end



#======================================
# => MAIN CODE
#======================================

if( $*.length < 1 )
  printUsage()
  exit
end

for fileName in $*

  puts "Cleaning up: #{fileName}"

  content = cleanup( IO.readlines(fileName) )

  f = File.new( fileName, "wb")
	f.puts content
	f.close


end
