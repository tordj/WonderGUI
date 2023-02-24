


def genTab( entries )

	print "uint8_t pixelConvTab_#{entries}[#{entries}] = { 0"
	for i in 1..entries-1
		print ", 0x#{(i*255/(entries-1)).to_s(16)}"
	end


	print " };\n"
	STDOUT.flush
end


genTab(2)
genTab(4)
genTab(8)
genTab(16)
genTab(32)
genTab(64)
genTab(128)
genTab(256)
