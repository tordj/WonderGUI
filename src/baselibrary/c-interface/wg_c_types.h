/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_C_TYPES_DOT_H
#define WG_C_TYPES_DOT_H
#pragma once

#include <stdint.h>

typedef void* wg_obj;

typedef void* wg_component;

//____ wg_typeInfo ___________________________________________________________

struct wg_typeInfo
{
	const char* className;
	const wg_typeInfo* pSuperClass;
};

//____ wg_tintMode ___________________________________________________________

enum wg_tintMode
{
	WG_TINT_NONE = 0,
	WG_TINT_FLAT,
	WG_TINT_GRADIENT_X,
	WG_TINT_GRADIENT_Y,
	WG_TINT_GRADIENT_XY
};


//____ wg_blendMode ____________________________________________________________

enum wg_blendMode
{
	WG_BLENDMODE_UNDEFINED,
	WG_BLENDMODE_IGNORE,
	WG_BLENDMODE_REPLACE,
	WG_BLENDMODE_ADD,
	WG_BLENDMODE_SUBTRACT,
	WG_BLENDMODE_MULTIPLY,
	WG_BLENDMODE_INVERT,
	WG_BLENDMODE_MIN,
	WG_BLENDMODE_MAX,
	WG_BLENDMODE_MORPH
};

//____ wg_direction ____________________________________________________________

enum wg_direction
{
	WG_DIR_UP,
	WG_DIR_RIGHT,
	WG_DIR_DOWM,
	WG_DIR_LEFT
};

//____ wg_sampleMethod ____________________________________________________________

enum wg_sampleMethod
{
	WG_SAMPLE_NEAREST,
	WG_SAMPLE_BILINEAR,
	WG_SAMPLE_UNDEFINED			// Default to Bilinear if it is accelerated, otherwise Nearest.
};

//____ wg_pixelFormat _____________________________________________________________

enum wg_pixelFormat
{
	WG_PIXFMT_UNDEFINED,
	WG_PIXFMT_CUSTOM,
	WG_PIXFMT_BGR_8,				///< One byte of blue, green and red in exactly that order in memory.
	WG_PIXFMT_BGR_8_SRGB,			///< One byte of blue, green and red in exactly that order in memory.
	WG_PIXFMT_BGR_8_LINEAR,			///< One byte of blue, green and red in exactly that order in memory.

	WG_PIXFMT_BGRX_8,				///< One byte of blue, green, red and padding in exactly that order in memory.
	WG_PIXFMT_BGRX_8_SRGB,			///< One byte of blue, green, red and padding in exactly that order in memory.
	WG_PIXFMT_BGRX_8_LINEAR,		///< One byte of blue, green, red and padding in exactly that order in memory.

	WG_PIXFMT_BGRA_8,				///< One byte of blue, green, red and alpha in exactly that order in memory.
	WG_PIXFMT_BGRA_8_SRGB,			///< One byte of blue, green, red and alpha in exactly that order in memory.
	WG_PIXFMT_BGRA_8_LINEAR,		///< One byte of blue, green, red and alpha in exactly that order in memory.

	WG_PIXFMT_BGRA_4_LINEAR,		///< 4 bits each of blue, green, red and alpha in exactly that order in memory.
	WG_PIXFMT_BGR_565_LINEAR,		///< 5 bits of blue, 6 bits of green and 5 bits of red in exactly that order in memory.

	WG_PIXFMT_CLUT_8,				///< 8 bits of index into the CLUT (Color Lookup Table).
	WG_PIXFMT_CLUT_8_SRGB,			///< 8 bits of index into the CLUT (Color Lookup Table).
	WG_PIXFMT_CLUT_8_LINEAR,		///< 8 bits of index into the CLUT (Color Lookup Table).

	WG_PIXFMT_RGB_565_BIGENDIAN,	///< 3 high bits of green, 5 bits of blue, 5 bits of red and 3 low bits of green in exactly that order in memory.

	WG_PIXFMT_A_8
};


//____ wg_pixelDescription _____________________________________________________

struct wg_pixelDescription
{
	uint8_t			format;			///< Enum specifying the format if it exacty matches a predefined format, otherwise set to CUSTOM or UNKNOWN.
	int				bits;			///< Number of bits for the pixel, includes any non-used padding bits.
	uint8_t			bIndexed;		///< True if pixels are index into CLUT, no RGB values in pixel.
	uint8_t			bLinear;		///< True if RGB values are linear (as opposed to in sRGB format, e.g. logarithmic with gamma 2.2).
	uint8_t			bBigEndian;		///< Set if 16- or 32-bit pixel is in big-endian format.

	uint32_t		R_mask;			///< bitmask for getting the red bits out of the pixel
	uint32_t		G_mask;			///< bitmask for getting the green bits out of the pixel
	uint32_t		B_mask;			///< bitmask for getting the blue bits out of the pixel
	uint32_t		A_mask;			///< bitmask for getting the alpha bits out of the pixel

	uint8_t			R_shift;		///< amount to shift the red bits to the right to get the value.
	uint8_t			G_shift;		///< amount to shift the green bits to the right to get the value.
	uint8_t			B_shift;		///< amount to shift the blue bits to the right to get the value.
	uint8_t			A_shift;		///< amount to shift the alpha bits to the right to get the value.

	uint8_t			R_loss;			///< amount to shift the red bits to the right to get the value.
	uint8_t			G_loss;			///< amount to shift the green bits to the right to get the value.
	uint8_t			B_loss;			///< amount to shift the blue bits to the right to get the value.
	uint8_t			A_loss;			///< amount to shift the alpha bits to the right to get the value.


	uint8_t			R_bits;				///< number of bits for red in the pixel
	uint8_t			G_bits;				///< number of bits for green in the pixel
	uint8_t			B_bits;				///< number of bits for blue in the pixel
	uint8_t			A_bits;				///< number of bits for alpha in the pixel
};

//____ wg_gfxFlip ____________________________________________________________

enum wg_gfxFlip
{
	WG_GFXFLIP_NORMAL = 0,
	WG_GFXFLIP_X,
	WG_GFXFLIP_Y,
	WG_GFXFLIP_ROT90,
	WG_GFXFLIP_ROT90_FLIP_X,
	WG_GFXFLIP_ROT90_FLIP_Y,
	WG_GFXFLIP_ROT180,
	WG_GFXFLIP_ROT180_FLIP_X,
	WG_GFXFLIP_ROT180_FLIP_Y,
	WG_GFXFLIP_ROT270,
	WG_GFXFLIP_ROT270_FLIP_X,
	WG_GFXFLIP_ROT270_FLIP_Y,
};

//____ wg_canvasRef ____________________________________________________________

enum wg_canvasRef
{
	WG_CANVASREF_NONE,
	WG_CANVASREF_DEFAULT,
	WG_CANVASREF_1,
	WG_CANVASREF_2,
	WG_CANVASREF_3,
	WG_CANVASREF_4,
	WG_CANVASREF_5,
	WG_CANVASREF_6,
	WG_CANVASREF_7,
	WG_CANVASREF_8,
	WG_CANVASREF_9,
	WG_CANVASREF_10,
	WG_CANVASREF_11,
	WG_CANVASREF_12,
	WG_CANVASREF_13,
	WG_CANVASREF_14,
	WG_CANVASREF_15,
	WG_CANVASREF_16,
	WG_CANVASREF_17,
	WG_CANVASREF_18,
	WG_CANVASREF_19,
	WG_CANVASREF_20,
	WG_CANVASREF_21,
	WG_CANVASREF_22,
	WG_CANVASREF_23,
	WG_CANVASREF_24,
	WG_CANVASREF_25,
	WG_CANVASREF_26,
	WG_CANVASREF_27,
	WG_CANVASREF_28,
	WG_CANVASREF_29,
	WG_CANVASREF_30,
	WG_CANVASREF_31,
	WG_CANVASREF_32
};

#endif
