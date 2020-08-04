#!/usr/bin/ruby
# Replaces keyword in template file with quoted lines of specified file.
# Saves to file with same name without .template extension.

def printUsage()
  puts "insert_quoted file.template"
  puts
end

for fileName in $*
  
   template = IO.readlines(fileName)
   output = []

   outputName = fileName.chomp(".template");

   for line in template
      if( line =~ /@INSERTLINEQUOTED/ )

        inputName = line[/.*<([^>]*)/,1]

        quotedFile = IO.readlines(inputName)
        for quote in quotedFile
          output << '"' + quote.chomp + '\n"'
        end
      else
        output << line;
      end
    end

    f = File.new( outputName, "w")
    f.puts output
    f.close


end

