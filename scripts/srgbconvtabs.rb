




# sRGB to Linear


	max = 255 ** 2.2
	print "uint8_t linearToSRGBTable[256] = {"
	for i in 0..255
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

# Linear to sRGB

	max = 255 ** 2.2
	print "uint8_t sRGBToLinearTable[256] = {"
	for i in 0..255
		val = ((i * max / 255) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"

# 

print "uint8_t conv_4_to_8_sRGB[16] 	= {"

	max = 15 ** 2.2
	for i in 0..15
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_5_to_8_sRGB[32] 	= {"

	max = 31 ** 2.2
	for i in 0..31
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_6_to_8_sRGB[64] 	= {"

	max = 63 ** 2.2
	for i in 0..63
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end
