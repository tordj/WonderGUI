#!/usr/bin/ruby

require 'rexml/document'

include REXML

$memberTables = []


class Members

	def initialize( node, name )
		@xmlNode = node
		@name = name
	end

	def name()
		return @name
	end

end

class MemberTable

	def initialize( xmlNode )
		@xmlNode = xmlNode
		@header = extractHeader()
		@members = []
		extractMembers( xmlNode )
	end

	def extractHeader()
		@xmlNode.each_element { |child| if( child.attributes["class"] == "heading") then return recurseToText(child) end }
		return nil
	end

	def extractMembers( node )
		for elem in node.elements
			if( elem.attributes["class"] =~ /^memitem:/ )
				memberName = recurseToText( elem.elements[2] )
				@members << Members.new( elem, memberName )
			elsif( elem.attributes["class"] =~ /^memdesc:/ )
			end

		end

	end



	def header()
		return @header
	end

	def members()
		return @members
	end
end



def searchRecursivelyForMemberTables( node )

	if( node.has_attributes?() and node.attributes["class"] == "memberdecls" )
			$memberTables << MemberTable.new( node )
	end

	node.each_element { |child| searchRecursivelyForMemberTables(child) }
end


def recurseToText( node )

	if( node.has_text? and node.text =~ /[A-Za-z]/ )
		return node.text.gsub( /\n/, ''  )
	else
		node.each_element { |child| res = recurseToText( child ); if( res != nil ) then return res end }
		return nil
	end
end

def printUsage()
	puts "usage: postdoxy file1 [file2 [...]]"
	puts
end



if( $*.length < 1 )
	printUsage()
	exit
end

for fileName in $*
  puts "Processing: " + fileName

  file = File.new(fileName)
  doc = Document.new(file)

	body = doc.root.elements[2]

	searchRecursivelyForMemberTables( body )

	for table in $memberTables

		puts ">" + table.header + "<"

		for member in table.members()

			puts "       = " + member.name()
		end
	end




#	doc.root.elements.each("*/table") { |element| puts "hej" }
#	puts root.elements[@class="memberdecls"]


#  puts doc

end
