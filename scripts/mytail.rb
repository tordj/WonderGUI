#!/usr/bin/ruby
# Primitive but portable tail

def printUsage()
  puts "usage: mytail logfile [regexp]"
  puts
end



#======================================
# => MAIN CODE
#======================================

if( $*.length < 1 )
  printUsage()
  exit
end

filename = $*.shift

if( !$*.empty? )
	exp = Regexp.new($*.shift)
else
	exp = nil
end


oldSize = 0

while( true )

	newSize = File.size?(filename)
	if( newSize != nil )

		if( newSize < oldSize )
			oldSize = 0							# Log has been cleaned

			puts '=============================================='
			puts ">>>   File cleared. New content follows.   <<<"
			puts '=============================================='
		end

		if( newSize > oldSize )
			f = File.open( filename, "rb")
			f.seek( oldSize )
			lines = f.readlines
			lines.each do | line | 
				if( exp == nil || line =~ exp ) 
					printf( line ) 
				end
			end
			f.close
			oldSize = newSize
		end

	end
	sleep 0.2


end



