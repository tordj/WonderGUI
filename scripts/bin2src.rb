#!/usr/bin/ruby
# Generate c-file from a binary file.
# Saves to file with same name but with c-extension.

def printUsage()
  puts "bin2src file.template"
  puts
end

for fileName in $*
  
  data = File.open(fileName, 'rb') { |f| f.read }

  values = data.unpack("c*");

  arrayName = "waxfile"

  sourceName = fileName + ".c";
  headerName = fileName + ".h";



  f = File.new( headerName, "w")

    f.puts "extern char " + arrayName + "[" + values.length.to_s + "];"
    f.puts "#define " + arrayName + "_length " + values.length.to_s

  f.close


  rows = 0
  f = File.new( sourceName, "w")

  f.print "char " + arrayName + "[" + values.length.to_s + "] = { "


  for c in values

    f.print c.to_s + ", ";
    rows += 1

    if( rows > 20 )
      f.print "\n"
      rows = 0
    end


  end

  f.print "};"
 


    f.close


end
