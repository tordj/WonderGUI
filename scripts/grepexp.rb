#!/usr/bin/ruby

require 'tempfile'

exp = Regexp.new($*.shift)



for fileName in $*
#	puts "Processing: " + fileName

	content = IO.readlines(fileName)

	content.each_with_index do |line, linenb|

		res = line[exp,0]

		if( res != NIL )
			print( "#{fileName} : #{linenb}. #{line}" )
		end
	end
end

