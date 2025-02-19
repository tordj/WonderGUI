




# sRGB to Linear


	max = 255 ** 2.2
	print "uint8_t sRGBToLinearTable[256] = {"
	for i in 0..255
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

# Linear to sRGB

	max = 255 ** 2.2
	print "uint8_t linearToSRGBTable[256] = {"
	for i in 0..255
		val = ((i * max / 255) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"

# 

print "uint8_t conv_2_linear_to_8_sRGB[4] 	= {"

	max = 255 ** 2.2
	for i in 0..3
		val = ((i * max / 3) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"


print "uint8_t conv_3_linear_to_8_sRGB[8] 	= {"

	max = 255 ** 2.2
	for i in 0..7
		val = ((i * max / 7) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_4_linear_to_8_sRGB[16] 	= {"

	max = 255 ** 2.2
	for i in 0..15
		val = ((i * max / 15) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_5_linear_to_8_sRGB[32] 	= {"

	max = 255 ** 2.2
	for i in 0..31
		val = ((i * max / 31) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_6_linear_to_8_sRGB[64] 	= {"

	max = 255 ** 2.2
	for i in 0..63
		val = ((i * max / 63) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_7_linear_to_8_sRGB[128] 	= {"

	max = 255 ** 2.2
	for i in 0..127
		val = ((i * max / 127) ** (1/2.2)).round
		print "#{val}, "
	end

	print "\n\n"



print "uint8_t conv_2_sRGB_to_8_linear[4] 	= {"

	max = 3 ** 2.2
	for i in 0..3
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_3_sRGB_to_8_linear[8] 	= {"

	max = 7 ** 2.2
	for i in 0..7
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"


print "uint8_t conv_4_sRGB_to_8_linear[16] 	= {"

	max = 15 ** 2.2
	for i in 0..15
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_5_sRGB_to_8_linear[32] 	= {"

	max = 31 ** 2.2
	for i in 0..31
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_6_sRGB_to_8_linear[64] 	= {"

	max = 63 ** 2.2
	for i in 0..63
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

print "uint8_t conv_7_sRGB_to_8_linear[128] 	= {"

	max = 127 ** 2.2
	for i in 0..127
		val = (((i ** 2.2) / max) * 255).round
		print "#{val}, "
	end

	print "\n\n"

print "uint16_t conv_8_sRGB_to_16_linear[256] = {"

	max = 255 ** 2.2
	for i in 0..255
		val = (((i ** 2.2) / max) * 65535).round
		print "#{val}, "
	end

	print "\n\n"

print "int16_t s_channel_5_sRGB[32] 	= {"

	max = 31 ** 2.2
	for i in 0..31
		val = (((i ** 2.2) / max) * 4096).round
		print "#{val}, "
	end

	print "\n\n"

print "int16_t s_channel_6_sRGB[64] 	= {"

	max = 63 ** 2.2
	for i in 0..63
		val = (((i ** 2.2) / max) * 4096).round
		print "#{val}, "
	end

	print "\n\n"
