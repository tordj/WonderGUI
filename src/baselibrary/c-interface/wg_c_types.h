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


typedef	void(*wg_finalizer_func)(wg_obj);

//____ wg_component __________________________________________________________

typedef struct wg_component_struct
{
	wg_obj object;
	void* interface;
} wg_component;



//____ wg_typeInfo ___________________________________________________________

typedef struct wg_typeInfo_struct
{
	const char* className;
	const struct wg_typeInfo_struct* pSuperClass;
} wg_typeInfo;

//____ wg_tintMode ___________________________________________________________

typedef enum
{
	WG_TINT_NONE = 0,
	WG_TINT_FLAT,
	WG_TINT_GRADIENT_X,
	WG_TINT_GRADIENT_Y,
	WG_TINT_GRADIENT_XY
} wg_tintMode;


//____ wg_blendMode ____________________________________________________________

typedef enum
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
} wg_blendMode;

//____ wg_direction ____________________________________________________________

typedef enum
{
	WG_DIR_UP,
	WG_DIR_RIGHT,
	WG_DIR_DOWM,
	WG_DIR_LEFT
} wg_direction;

//____ wg_sampleMethod ____________________________________________________________

typedef enum
{
	WG_SAMPLE_NEAREST,
	WG_SAMPLE_BILINEAR,
	WG_SAMPLE_UNDEFINED			// Default to Bilinear if it is accelerated, otherwise Nearest.
} wg_sampleMethod;

//____ wg_pixelFormat _____________________________________________________________

typedef enum
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
} wg_pixelFormat;


//____ wg_pixelDescription _____________________________________________________

typedef struct wg_pixelDescription_struct
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
} wg_pixelDescription;

//____ wg_gfxFlip ____________________________________________________________

typedef enum
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
} wg_gfxFlip;

//____ wg_canvasRef ____________________________________________________________

typedef enum
{
	WG_CANVAS_NONE,
	WG_CANVAS_DEFAULT,
	WG_CANVAS_1,
	WG_CANVAS_2,
	WG_CANVAS_3,
	WG_CANVAS_4,
	WG_CANVAS_5,
	WG_CANVAS_6,
	WG_CANVAS_7,
	WG_CANVAS_8,
	WG_CANVAS_9,
	WG_CANVAS_10,
	WG_CANVAS_11,
	WG_CANVAS_12,
	WG_CANVAS_13,
	WG_CANVAS_14,
	WG_CANVAS_15,
	WG_CANVAS_16,
	WG_CANVAS_17,
	WG_CANVAS_18,
	WG_CANVAS_19,
	WG_CANVAS_20,
	WG_CANVAS_21,
	WG_CANVAS_22,
	WG_CANVAS_23,
	WG_CANVAS_24,
	WG_CANVAS_25,
	WG_CANVAS_26,
	WG_CANVAS_27,
	WG_CANVAS_28,
	WG_CANVAS_29,
	WG_CANVAS_30,
	WG_CANVAS_31,
	WG_CANVAS_32
} wg_canvasRef;

//____ wg_gfxChunkId ____________________________________________________

typedef enum
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
} wg_gfxChunkId;


//____ wg_error_severity ________________________________________________

typedef enum
{
	WG_WARNING = 0,
	WG_SILENT_FAIL,
	WG_SERIOUS_ERROR,
	WG_CRITICAL_ERROR
} wg_error_severity;

//____ wg_error_code ____________________________________________________

typedef enum
{
	WG_ERROR_OUT_OF_RANGE = 0,
	WG_ERROR_INVALID_ITERATOR,
	WG_ERROR_INVALID_PARAM,
	WG_ERROR_FAILED_PREREQUISITE,
	WG_ERROR_OPENGL,						// OPENGL HAS ISSUED A GLERROR
	WG_ERROR_INTERNAL,
	WG_ERROR_SYSTEM_INTEGRITY,
	WG_ERROR_ILLEGAL_CALL,
	WG_ERROR_RESOURCE_EXHAUSTED,			// A LIMITED INTERNAL RESOURCE HAS BEEN EXHAUSTED
	WG_ERROR_OTHER
} wg_error_code;

//____ wg_error_info _____________________________________________________

typedef struct wg_error_info_struct
{
	wg_error_severity	severity;
	wg_error_code		code;
	const char * 		message;
	const void *		object;
	const char *		classname;
	const char *		function;
	const char *		file;
	int					line;
} wg_error_info;

#ifdef __cplusplus
}
#endif

#endif
