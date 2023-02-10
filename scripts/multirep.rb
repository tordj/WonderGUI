#!/usr/bin/ruby

require 'tempfile'

Task = Struct.new( :regexp, :replace )

$version = 1.0
$statementFile = nil
$match = "any"					# word, begin, end, any
$separator = "="
$tasks = []
$bSilent = false
$bRecursive = false
$files = []
$bDryrun = false

def generateRegexp( match, string )
	if match == 'any'
		return Regexp.new Regexp.quote(string)
	elsif match == 'whole'
		return Regexp.new '\b' + Regexp.quote(string) + '\b'
	elsif match == 'begin'
		return Regexp.new '\b' + Regexp.quote(string)
	elsif match == 'end'
		return Regexp.new Regexp.quote(string) + '\b'
	end
end


def getParamsAndStatementFile()
	while $*.empty? == false
		param = $*.shift

		if param == "-match"
			$match = $*.shift
		elsif param == "-sep"
			$separator = $*.shift
		elsif param == "-silent"
			$bSilent = true
		elsif param == "-recursive"
			$bRecursive = true
		elsif param == "-dryrun"
			$bDryrun = true
		else
			$statementFile = param
			break
		end
	end
end

def loadStatements( file )
	File.open( file, "r") do |f|
		f.each_line do |line|
			line.chomp!
			pairs = line.split($separator).map(&:strip)
			if( pairs.size > 2 )
				printf( "ERROR: Too many search/replace separators in this line:\n%s\n", line )
				exit
			end
			$tasks << Task.new( generateRegexp( $match, pairs[0] ), pairs[1] )
	 	end
	end
end



def printBanner()
	printf "multirep %.1f - Replace mulitiple strings in mulitiple files\n\n", $version
end

def printUsage()
	printf "usage: multirep [params] [statementFile] file1 [file2 [...]]\n\n"
	puts "params:"
	puts "-dryrun          Do not save changes to files."
	puts "-match [part]    Part of words to match (whole/begin/end/any)."
	puts "-recursive	   Search for all files recursively."
	puts "-sep   [string]  Separator used in statementFile (default: '=')"
	puts "-silent          Less verbose output"
	puts
	puts "Takes a statement-file containing lines with search/replace statements like"
	puts "'searchterm = replacement' and performs all of them on all files specified."
	puts "Both searchterm and replacement are trimmed before operation and neither of"
	puts "them may contain whitespaces or the separator used (default is '=')."
	puts
end


### Initialize

printBanner()
getParamsAndStatementFile()

if( $*.length < 1 )
	printUsage()
	exit
end

loadStatements( $statementFile )


### Generate list of files to search

if( $bRecursive )

	for exp in $*
		$files += Dir.glob( File.join("**", exp ) )
	end

else
	$files = $*
end



### Start the processing

printf( "%d replace statements to run on %d files.\n", $tasks.size, $*.length )

for fileName in $files
	
	if( $bSilent == false )
		print "Processing: " + fileName + "..."
	end

	content = IO.readlines(fileName)

	modifiedLines = 0;
	content.map! do |line|

		lineModified = false;
		for task in $tasks
			if( line.gsub!( task[:regexp],task[:replace]) != nil )
				lineModified = true
			end
		end
		if( lineModified == true )
			modifiedLines += 1
		end
		line
	end

	if( modifiedLines > 0 )
		if( $bSilent == true )
			print " #{modifiedLines} lines changed in #{fileName}.\n"
		else
			print " #{modifiedLines} lines changed.\n"
		end
		if( $bDryrun == false )
			f = File.new( fileName, "w")
			f.puts content
			f.close
		end
	elsif( $bSilent == false )
		print "\n"
	end
end

puts 'DONE'

