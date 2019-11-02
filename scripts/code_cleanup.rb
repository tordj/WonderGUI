#!/usr/bin/ruby
# Does a cleanup of input text files, such as replacing CR/LF with LF etc


def printUsage()
  puts "usage: code_cleanup file_or_directory1 [file_or_directory2 [...]]"
  puts
  puts "specifying a directory runs cleanup on all files recursively."
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

for param in $*

	fileNames = [] 

	if( File.directory?(param) )
		fileNames = Dir[param + '/**/*'].reject {|fn| File.directory?(fn) }
	else
		fileNames << param
	end


	for file in fileNames

		puts "Cleaning up: #{file}"

  		content = cleanup( IO.readlines(file) )

  		f = File.new( file, "wb")
		f.puts content
		f.close
	end

end
