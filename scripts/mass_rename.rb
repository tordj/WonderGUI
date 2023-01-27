#!/usr/bin/ruby
# Replace smart pointer references with real pointers.

# Copied from:
# https://stackoverflow.com/questions/10758694/how-to-mass-rename-files-in-ruby


puts "Enter the file search query"
searchPattern = gets.strip
puts "Enter the target to replace"
target = gets.strip
puts "Enter the new target name"
newTarget = gets.strip
Dir.glob(searchPattern).sort.each do |entry|
  if File.basename(entry, File.extname(entry)).include?(target)
    newEntry = entry.gsub(target, newTarget)
    File.rename( entry, newEntry )
    puts "Rename from " + entry + " to " + newEntry
  end
end