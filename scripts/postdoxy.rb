#!/usr/bin/ruby

require 'nokogiri'


def memberTableHeader( memberTableNode )
  return memberTableNode.css('tr')[0].css('td h2').text.gsub( /\n/, '' )
end

def mergeMemberTables( tables )
  for index in 0..tables.size-1
    for prev in 0..index-1
      if( memberTableHeader( tables[prev]) == memberTableHeader( tables[index]) )
#        puts 'Found duplicate ' + memberTableHeader( memberTables[prev] )

        rows = tables[index].css('tr')
        for row in 1..rows.size-1
          rows[row].parent = tables[prev]
        end
        tables[index].remove
      end
    end
  end
end

def sortMembers( table )

#  rows = tables[index].css('tr')
#  rows.sort_by! { |row| row.}

end

for fileName in $*
  puts "Processing: " + fileName

  file = File.new(fileName)
  doc = Nokogiri::HTML(file)

  memberTables = doc.css("table[class=memberdecls]")
  mergeMemberTables( memberTables )

  memberTables.each { |table| sortMembers(table) }

  fi = File.new( fileName, "w")
  fi.puts doc
  fi.close

end
