#!/usr/bin/ruby

require 'tempfile'

exp = Regexp.new($*.shift)



for fileName in $*
	puts "Processing: " + fileName

	content = IO.readlines(fileName)

	for line in content

		res = line[exp,0]

		puts res
	end
end

