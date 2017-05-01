#!/usr/bin/ruby

require 'nokogiri'


$prioTable = {
  'Creation' => 1,
  'Components' => 2,
  'Appearance' => 3,
  'Control' => 4,
  'Behavior' => 5,
  'State' => 6,
  'Rendering' => 7,
  'Geometry' => 8,
  'Hierarchy' => 9,
  'Debug' => 10,
  'Identification' => 11,
  'Misc' => 12,
  'Internal' => 13,

}

$unknownHeaders = Hash.new(0)


def memberTableHeader( memberTableNode )
  return memberTableNode.css('tr')[0].css('td h2').text.gsub( /\n/, '' )
end

def memberTablePrio( memberTableNode )

    header = memberTableHeader(memberTableNode)

    prio = $prioTable[header]
    if( prio == nil )
        prio = 1000
        $unknownHeaders[header] = $unknownHeaders[header]+1
    end

    return prio
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

def sortMemberTables( tables )

    sorted_tables = tables.sort_by { |node| memberTablePrio( node ) }


    for i in 0..sorted_tables.length-2
  		sorted_tables[i].add_next_sibling(sorted_tables[i+1])
    end

#  rows = tables[index].css('tr')
#  rows.sort_by! { |row| row.}

end


for fileName in $*
  puts "Processing: " + fileName

  file = File.new(fileName)
  doc = Nokogiri::HTML(file)

  memberTables = doc.css("table[class=memberdecls]")
  mergeMemberTables( memberTables )

  memberTables = doc.css("table[class=memberdecls]")
  sortMemberTables( memberTables );

  memberTables.each { |table| sortMembers(table) }

  fi = File.new( fileName, "w")
  fi.puts doc
  fi.close

end

$unknownHeaders.each { |key, value| puts "Warning: Unknow Section '#{key}' was found #{value} times." }
