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

#ifdef __cplusplus
extern "C" {
#endif

typedef void* wg_obj;

typedef void* wg_component;

typedef	void(*wg_finalizer_func)(void*);


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

//____ wg_gfxChunkId ____________________________________________________

enum wg_gfxChunkId
{
	WG_GFXCHUNK_OUT_OF_DATA = 0,
	WG_GFXCHUNK_BEGIN_RENDER = 1,
	WG_GFXCHUNK_END_RENDER = 2,
	WG_GFXCHUNK_FLUSH = 3,

	WG_GFXCHUNK_BEGIN_CANVAS_UPDATE = 4,
	WG_GFXCHUNK_END_CANVAS_UPDATE = 5,

	WG_GFXCHUNK_SET_CLIP_LIST = 6,
	WG_GFXCHUNK_RESET_CLIP_LIST = 7,
	WG_GFXCHUNK_PUSH_CLIP_LIST = 8,
	WG_GFXCHUNK_POP_CLIP_LIST = 9,

	WG_GFXCHUNK_SET_TINT_COLOR = 10,
	WG_GFXCHUNK_SET_TINT_GRADIENT = 11,
	WG_GFXCHUNK_CLEAR_TINT_GRADIENT = 12,

	WG_GFXCHUNK_SET_BLEND_MODE = 13,
	WG_GFXCHUNK_SET_BLIT_SOURCE = 14,
	WG_GFXCHUNK_SET_MORPH_FACTOR = 15,
	WG_GFXCHUNK_SET_RENDER_LAYER = 16,

	WG_GFXCHUNK_FILL = 17,
	WG_GFXCHUNK_FILL_RECT = 18,
	//		FillRectF = 19,
	WG_GFXCHUNK_PLOT_PIXELS = 20,
	WG_GFXCHUNK_DRAW_LINE_FROM_TO = 21,
	WG_GFXCHUNK_DRAW_LINE_STRAIGHT = 22,

	WG_GFXCHUNK_BLIT = 23,
	WG_GFXCHUNK_BLIT_RECT = 24,
	WG_GFXCHUNK_FLIP_BLIT = 25,
	WG_GFXCHUNK_FLIP_BLIT_RECT = 26,

	WG_GFXCHUNK_STRETCH_BLIT = 27,
	WG_GFXCHUNK_STRETCH_BLIT_RECT = 28,
	//		StretchBlitRectF = 29,

	WG_GFXCHUNK_STRETCH_FLIP_BLIT = 30,
	WG_GFXCHUNK_STRETCH_FLIP_BLIT_RECT = 31,
	//		StretchFlipBlitRectF = 32,

	WG_GFXCHUNK_ROT_SCALE_BLIT = 33,
	WG_GFXCHUNK_TILE = 34,
	WG_GFXCHUNK_FLIP_TILE = 35,

	WG_GFXCHUNK_SCALE_TILE = 36,
	WG_GFXCHUNK_SCALE_FLIP_TILE = 37,

	WG_GFXCHUNK_DRAW_WAVE = 38,
	WG_GFXCHUNK_FLIP_DRAW_WAVE = 39,

	WG_GFXCHUNK_DRAW_ELIPSE = 40,
	WG_GFXCHUNK_DRAW_PIE_CHART = 41,

	WG_GFXCHUNK_DRAW_SEGMENTS = 42,
	WG_GFXCHUNK_FLIP_DRAW_SEGMENTS = 43,

	WG_GFXCHUNK_BLIT_NINE_PATCH = 44,

	WG_GFXCHUNK_EDGE_SAMPLES = 45,

	WG_GFXCHUNK_CREATE_SURFACE = 46,
	WG_GFXCHUNK_BEGIN_SURFACE_UPDATE = 47,
	WG_GFXCHUNK_SURFACE_PIXELS = 48,
	WG_GFXCHUNK_END_SURFACE_UPDATE = 49,
	WG_GFXCHUNK_FILL_SURFACE = 50,
	WG_GFXCHUNK_COPY_SURFACE = 51,
	WG_GFXCHUNK_DELETE_SURFACE = 52,

	WG_GFXCHUNK_PROTOCOL_VERSION = 53,
	WG_GFXCHUNK_TIME_STAMP_MS = 54
};


#ifdef __cplusplus
}
#endif

#endif
