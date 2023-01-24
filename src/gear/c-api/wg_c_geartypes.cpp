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

#include <wg_c_geartypes.h>
#include <wg_geartypes.h>

/*

static_assert( WG_TINT_NONE == (int) wg::TintMode::None, "TintMode enum differs in value between C and C++ API!" );
WG_TINT_NONE = 0,
WG_TINT_FLAT,
WG_TINT_GRADIENT_X,
WG_TINT_GRADIENT_Y,
WG_TINT_GRADIENT_XY

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

WG_DIR_UP,
WG_DIR_RIGHT,
WG_DIR_DOWM,
WG_DIR_LEFT

WG_SAMPLE_NEAREST,
WG_SAMPLE_BILINEAR,
WG_SAMPLE_UNDEFINED	

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
*/
