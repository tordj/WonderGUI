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
#include <wg_sysfont.h>

namespace wg
{
	const TypeInfo	SysFont::TYPEINFO = { "SysFont", &Font::TYPEINFO };

	unsigned char compressed_font[48][64] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x80, 0x0, 0x30, 0xc, 0x30, 0xc, 0x0, 0x3, 0x0, 0x0, 0xc0, 0x3, 0x0, 0x3, 0x0, 0xc, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xf, 0xc0, 0x3, 0xf0, 0xf, 0xf0, 0xf, 0x0, 0x7, 0xfc, 0x3f, 0xf0, 0xf, 0xfc, 0x3f, 0xf0, 0xf, 0xf0, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xf,
	 0x0, 0x0, 0xc0, 0x1, 0x30, 0xc, 0x30, 0xc, 0x0, 0x3, 0x30, 0x18, 0xe0, 0x7, 0x80, 0x3, 0x0, 0xe, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x1f, 0xe0, 0x3, 0xf8, 0x1f, 0xf8, 0x1f, 0x80, 0x7, 0xfc, 0x3f, 0xf8, 0xf, 0xfc, 0x3f, 0xf8, 0x1f, 0xf8, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x1f,
	 0x0, 0x0, 0xc0, 0x1, 0x30, 0xc, 0xfc, 0x3f, 0xe0, 0x1f, 0x78, 0x1c, 0x30, 0x6, 0xc0, 0x1, 0x0, 0x7, 0xe0, 0x0, 0x60, 0x18, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x1c, 0x3c, 0x70, 0x3, 0x1c, 0x38, 0x1c, 0x38, 0xc0, 0x7, 0xc, 0x0, 0x1c, 0x0, 0x0, 0x30, 0x1c, 0x38, 0xc, 0x38, 0x80, 0x0, 0x80, 0x0, 0x0, 0xc, 0x0, 0x0, 0xc0, 0x0, 0x1c, 0x38,
	 0x0, 0x0, 0xc0, 0x1, 0x20, 0x4, 0xfc, 0x3f, 0xf0, 0x3f, 0x78, 0xe, 0x70, 0x6, 0xc0, 0x0, 0x0, 0x3, 0xc0, 0x0, 0xe0, 0x1c, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0xc, 0x3e, 0x30, 0x3, 0xc, 0x30, 0xc, 0x30, 0xe0, 0x6, 0xc, 0x0, 0xc, 0x0, 0x0, 0x38, 0xc, 0x30, 0xc, 0x30, 0xc0, 0x1, 0xc0, 0x1, 0x0, 0xe, 0x0, 0x0, 0xc0, 0x1, 0xc, 0x38,
	 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0x30, 0xc, 0x30, 0x3, 0x30, 0x7, 0xe0, 0x3, 0x0, 0x0, 0x0, 0x3, 0xc0, 0x0, 0xc0, 0xf, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1c, 0xc, 0x37, 0x0, 0x3, 0x0, 0x30, 0x0, 0x1f, 0x70, 0x6, 0xfc, 0xf, 0xfc, 0xf, 0x0, 0x1c, 0x18, 0x18, 0x1c, 0x38, 0xc0, 0x1, 0xc0, 0x1, 0x0, 0x7, 0xf0, 0x3f, 0x80, 0x3, 0x0, 0x1c,
	 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0x30, 0xc, 0x30, 0x3, 0x80, 0x3, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x3, 0xc0, 0x0, 0x80, 0x7, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x8c, 0x33, 0x0, 0x3, 0x0, 0x38, 0x0, 0x1f, 0x38, 0x6, 0xfc, 0x1f, 0xfc, 0x1f, 0x0, 0xe, 0xf0, 0xf, 0xf8, 0x3f, 0x80, 0x0, 0x80, 0x0, 0x80, 0x3, 0xf0, 0x3f, 0x0, 0x7, 0x0, 0xe,
	 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x30, 0xc, 0xf0, 0x1f, 0xc0, 0x1, 0xf0, 0x33, 0x0, 0x0, 0x0, 0x3, 0xc0, 0x0, 0xf0, 0x3f, 0xf0, 0x3f, 0x0, 0x0, 0xf0, 0x3f, 0x0, 0x0, 0x0, 0x7, 0xcc, 0x31, 0x0, 0x3, 0xf0, 0x1f, 0x0, 0x30, 0x1c, 0x6, 0x0, 0x38, 0xc, 0x38, 0x0, 0x7, 0xf8, 0x1f, 0xf0, 0x3f, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0x0, 0xe, 0x0, 0x7,
	 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x30, 0xc, 0xe0, 0x3f, 0xe0, 0xc, 0x38, 0x3f, 0x0, 0x0, 0x0, 0x3, 0xc0, 0x0, 0xf0, 0x3f, 0xf0, 0x3f, 0x0, 0x0, 0xf0, 0x3f, 0x0, 0x0, 0x80, 0x3, 0xec, 0x30, 0x0, 0x3, 0xf8, 0xf, 0x0, 0x30, 0xc, 0x6, 0x0, 0x30, 0xc, 0x30, 0x80, 0x3, 0x1c, 0x38, 0x0, 0x30, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0x0, 0xe, 0x0, 0x3,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xfc, 0x3f, 0x0, 0x33, 0x70, 0x1e, 0x1c, 0x1e, 0x0, 0x0, 0x0, 0x3, 0xc0, 0x0, 0x80, 0x7, 0x0, 0x3, 0x0, 0x3, 0x0, 0x0, 0x80, 0x1, 0xc0, 0x1, 0x7c, 0x30, 0x0, 0x3, 0x1c, 0x0, 0xc, 0x30, 0xfc, 0x3f, 0xc, 0x30, 0xc, 0x30, 0xc0, 0x1, 0xc, 0x30, 0x0, 0x30, 0x80, 0x0, 0xc0, 0x0, 0x80, 0x3, 0xf0, 0x3f, 0x0, 0x7, 0x0, 0x0,
	 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0xfc, 0x3f, 0x0, 0x33, 0x38, 0x1e, 0x1c, 0xc, 0x0, 0x0, 0x0, 0x7, 0xe0, 0x0, 0xc0, 0xf, 0x0, 0x3, 0x0, 0x3, 0x0, 0x0, 0xc0, 0x3, 0xe0, 0x0, 0x3c, 0x38, 0x0, 0x3, 0xc, 0x0, 0x1c, 0x38, 0xfc, 0x3f, 0x1c, 0x38, 0x1c, 0x38, 0xc0, 0x0, 0x1c, 0x38, 0x0, 0x38, 0xc0, 0x1, 0xc0, 0x0, 0x0, 0x7, 0xf0, 0x3f, 0x80, 0x3, 0x0, 0x0,
	 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0x30, 0xc, 0xf0, 0x3f, 0x1c, 0xc, 0xf8, 0x3f, 0x0, 0x0, 0x0, 0xe, 0x70, 0x0, 0xe0, 0x1c, 0x0, 0x3, 0x0, 0x3, 0x0, 0x0, 0xc0, 0x3, 0x70, 0x0, 0xf8, 0x1f, 0xf0, 0x3f, 0xfc, 0x3f, 0xf0, 0x1f, 0x0, 0x6, 0xf8, 0x1f, 0xf8, 0x1f, 0xc0, 0x0, 0xf8, 0x1f, 0xf0, 0x1f, 0xc0, 0x1, 0xc0, 0x0, 0x0, 0xe, 0x0, 0x0, 0xc0, 0x1, 0x0, 0x3,
	 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0x30, 0xc, 0xe0, 0x1f, 0xc, 0x0, 0xf0, 0x33, 0x0, 0x0, 0x0, 0xc, 0x30, 0x0, 0x60, 0x18, 0x0, 0x3, 0x80, 0x3, 0x0, 0x0, 0x80, 0x1, 0x30, 0x0, 0xf0, 0xf, 0xf0, 0x3f, 0xfc, 0x3f, 0xf8, 0xf, 0x0, 0x6, 0xf0, 0xf, 0xf0, 0xf, 0xc0, 0x0, 0xf0, 0xf, 0xf0, 0xf, 0x80, 0x0, 0xe0, 0x0, 0x0, 0xc, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x3,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0xf0, 0xf, 0xf0, 0xf, 0xfc, 0xf, 0xf0, 0xf, 0xfc, 0x7, 0xfc, 0x3f, 0xfc, 0x3f, 0xf0, 0xf, 0xc, 0x30, 0xf0, 0x3f, 0x0, 0x30, 0xc, 0xc, 0xc, 0x0, 0xc, 0x30, 0xc, 0x30, 0xf0, 0xf, 0xfc, 0xf, 0xf0, 0xf, 0xfc, 0xf, 0xf0, 0xf, 0xff, 0x3f, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0x3, 0x30, 0xf8, 0x3f, 0x0, 0x3f, 0x0, 0x0, 0xfc, 0x0, 0xc0, 0x0, 0x0, 0x0,
	 0xf8, 0x1f, 0xf8, 0x1f, 0xfc, 0x1f, 0xf8, 0x1f, 0xfc, 0xf, 0xfc, 0x3f, 0xfc, 0x3f, 0xf8, 0x1f, 0xc, 0x30, 0xf0, 0x3f, 0x0, 0x30, 0xc, 0xe, 0xc, 0x0, 0x1c, 0x38, 0x1c, 0x30, 0xf8, 0x1f, 0xfc, 0x1f, 0xf8, 0x1f, 0xfc, 0x1f, 0xf8, 0x1f, 0xff, 0x3f, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0x1c, 0x38, 0x7, 0x38, 0xf8, 0x3f, 0x0, 0x3f, 0x0, 0x0, 0xfc, 0x0, 0xe0, 0x1, 0x0, 0x0,
	 0x1c, 0x38, 0x1c, 0x38, 0xc, 0x38, 0x1c, 0x38, 0xc, 0x1c, 0xc, 0x0, 0xc, 0x0, 0x1c, 0x38, 0xc, 0x30, 0x0, 0x3, 0x0, 0x30, 0xc, 0x7, 0xc, 0x0, 0x3c, 0x3c, 0x3c, 0x30, 0x1c, 0x38, 0xc, 0x38, 0x1c, 0x38, 0xc, 0x38, 0x1c, 0x18, 0xc0, 0x0, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0x38, 0x1c, 0xe, 0x1c, 0x0, 0x1c, 0x0, 0x3, 0xc, 0x0, 0xc0, 0x0, 0xf0, 0x3, 0x0, 0x0,
	 0xcc, 0x33, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x38, 0xc, 0x0, 0xc, 0x0, 0xc, 0x30, 0xc, 0x30, 0x0, 0x3, 0x0, 0x30, 0x8c, 0x3, 0xc, 0x0, 0x7c, 0x3e, 0x7c, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x0, 0xc0, 0x0, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0x70, 0xe, 0x1c, 0xe, 0x0, 0xe, 0x0, 0x3, 0x1c, 0x0, 0xc0, 0x0, 0xf8, 0x7, 0x0, 0x0,
	 0xec, 0x37, 0xc, 0x30, 0xfc, 0x1f, 0xc, 0x0, 0xc, 0x30, 0xfc, 0xf, 0xfc, 0xf, 0xc, 0x0, 0xfc, 0x3f, 0x0, 0x3, 0x0, 0x30, 0xfc, 0x1, 0xc, 0x0, 0xec, 0x37, 0xec, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0x1c, 0x0, 0xc0, 0x0, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xe0, 0x7, 0x38, 0x7, 0x0, 0x7, 0x0, 0x3, 0x38, 0x0, 0xc0, 0x0, 0xdc, 0xe, 0x0, 0x0,
	 0x6c, 0x36, 0xc, 0x30, 0xfc, 0xf, 0xc, 0x0, 0xc, 0x30, 0xfc, 0xf, 0xfc, 0xf, 0xc, 0x0, 0xfc, 0x3f, 0x0, 0x3, 0x0, 0x30, 0xfc, 0x1, 0xc, 0x0, 0xcc, 0x33, 0xcc, 0x31, 0xc, 0x30, 0xc, 0x38, 0xc, 0x30, 0xc, 0x38, 0xf8, 0xf, 0xc0, 0x0, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc0, 0x3, 0x70, 0x3, 0x80, 0x3, 0x0, 0x3, 0x70, 0x0, 0xc0, 0x0, 0xcc, 0xc, 0x0, 0x0,
	 0x6c, 0x34, 0xfc, 0x3f, 0xc, 0x18, 0xc, 0x0, 0xc, 0x30, 0xc, 0x0, 0xc, 0x0, 0x8c, 0x3f, 0xc, 0x30, 0x0, 0x3, 0xc, 0x30, 0x8c, 0x3, 0xc, 0x0, 0x8c, 0x31, 0x8c, 0x33, 0xc, 0x30, 0xfc, 0x1f, 0xc, 0x30, 0xfc, 0x1f, 0xf0, 0x1f, 0xc0, 0x0, 0xc, 0x30, 0xc, 0x30, 0x8c, 0x31, 0xc0, 0x3, 0xe0, 0x1, 0xc0, 0x1, 0x0, 0x3, 0xe0, 0x0, 0xc0, 0x0, 0xc0, 0x0, 0x0, 0x0,
	 0xec, 0x1f, 0xfc, 0x3f, 0xc, 0x30, 0xc, 0x0, 0xc, 0x30, 0xc, 0x0, 0xc, 0x0, 0x8c, 0x3f, 0xc, 0x30, 0x0, 0x3, 0xc, 0x30, 0xc, 0x7, 0xc, 0x0, 0xc, 0x30, 0xc, 0x37, 0xc, 0x30, 0xfc, 0xf, 0x8c, 0x31, 0xfc, 0xf, 0x0, 0x38, 0xc0, 0x0, 0xc, 0x30, 0x1c, 0x38, 0x8c, 0x31, 0xe0, 0x7, 0xc0, 0x0, 0xe0, 0x0, 0x0, 0x3, 0xc0, 0x1, 0xc0, 0x0, 0xc0, 0x0, 0x0, 0x0,
	 0xcc, 0xf, 0xc, 0x30, 0xc, 0x30, 0xc, 0x30, 0xc, 0x38, 0xc, 0x0, 0xc, 0x0, 0xc, 0x30, 0xc, 0x30, 0x0, 0x3, 0xc, 0x30, 0xc, 0xe, 0xc, 0x0, 0xc, 0x30, 0xc, 0x3e, 0xc, 0x30, 0xc, 0x0, 0x8c, 0x33, 0xc, 0xe, 0xc, 0x30, 0xc0, 0x0, 0xc, 0x30, 0x38, 0x1c, 0xcc, 0x33, 0x70, 0xe, 0xc0, 0x0, 0x70, 0x0, 0x0, 0x3, 0x80, 0x3, 0xc0, 0x0, 0xc0, 0x0, 0x0, 0x0,
	 0x1c, 0x0, 0xc, 0x30, 0xc, 0x38, 0x1c, 0x38, 0xc, 0x1c, 0xc, 0x0, 0xc, 0x0, 0x1c, 0x38, 0xc, 0x30, 0x0, 0x3, 0x1c, 0x38, 0xc, 0x1c, 0xc, 0x0, 0xc, 0x30, 0xc, 0x3c, 0x1c, 0x38, 0xc, 0x0, 0x1c, 0x3f, 0xc, 0x1c, 0x1c, 0x38, 0xc0, 0x0, 0x1c, 0x38, 0x70, 0xe, 0xfc, 0x3f, 0x38, 0x1c, 0xc0, 0x0, 0x38, 0x0, 0x0, 0x3, 0x0, 0x7, 0xc0, 0x0, 0xc0, 0x0, 0x0, 0x0,
	 0xf8, 0x1f, 0xc, 0x30, 0xfc, 0x1f, 0xf8, 0x1f, 0xfc, 0xf, 0xfc, 0x3f, 0xc, 0x0, 0xf8, 0x1f, 0xc, 0x30, 0xf0, 0x3f, 0xf8, 0x1f, 0xc, 0x38, 0xfc, 0x3f, 0xc, 0x30, 0xc, 0x38, 0xf8, 0x1f, 0xc, 0x0, 0xf8, 0x1f, 0xc, 0x38, 0xf8, 0x1f, 0xc0, 0x0, 0xf8, 0x1f, 0xe0, 0x7, 0x78, 0x1e, 0x1c, 0x38, 0xc0, 0x0, 0xfc, 0x3f, 0x0, 0x3f, 0x0, 0xe, 0xfc, 0x0, 0xc0, 0x0, 0x0, 0x0,
	 0xf0, 0xf, 0xc, 0x30, 0xfc, 0xf, 0xf0, 0xf, 0xfc, 0x7, 0xfc, 0x3f, 0xc, 0x0, 0xf0, 0xf, 0xc, 0x30, 0xf0, 0x3f, 0xf0, 0xf, 0xc, 0x30, 0xfc, 0x3f, 0xc, 0x30, 0xc, 0x30, 0xf0, 0xf, 0xc, 0x0, 0xf0, 0x3f, 0xc, 0x30, 0xf0, 0xf, 0xc0, 0x0, 0xf0, 0xf, 0xc0, 0x3, 0x30, 0xc, 0xc, 0x30, 0xc0, 0x0, 0xfc, 0x3f, 0x0, 0x3f, 0x0, 0xc, 0xfc, 0x0, 0xc0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x38, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xf,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x1f,
	 0xc0, 0xf, 0x0, 0x0, 0x30, 0x0, 0x0, 0x0, 0x0, 0xc, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0xc, 0x0, 0xc0, 0x0, 0x0, 0xc, 0x30, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1e, 0x0, 0x3, 0x78, 0x0, 0xc0, 0xc, 0x1c, 0x38,
	 0xe0, 0x1f, 0x0, 0x0, 0x30, 0x0, 0x0, 0x0, 0x0, 0xc, 0x0, 0x0, 0x80, 0xf, 0x0, 0x0, 0xc, 0x0, 0xc0, 0x0, 0x0, 0xc, 0x30, 0x6, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0x0, 0x3, 0xf8, 0x0, 0xe0, 0xe, 0xe, 0x70,
	 0x70, 0x38, 0xf0, 0x3, 0x30, 0x0, 0x0, 0x0, 0x0, 0xc, 0xf0, 0x3, 0xc0, 0x1, 0xf0, 0x7, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x7, 0xc0, 0x0, 0x3c, 0x3, 0xfc, 0x3, 0xf0, 0x3, 0xfc, 0x3, 0xf0, 0xf, 0xc0, 0xf, 0xf0, 0x3, 0xc0, 0x0, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xf8, 0xf, 0x0, 0x3, 0x0, 0x3, 0xc0, 0x0, 0xf0, 0x7, 0xc7, 0xe3,
	 0x30, 0x30, 0xf8, 0x7, 0x30, 0x0, 0xc0, 0xf, 0x0, 0xc, 0xf8, 0x7, 0xc0, 0x0, 0xf8, 0xf, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0xb0, 0x3, 0xc0, 0x0, 0xfc, 0x7, 0xfc, 0x7, 0xf8, 0x7, 0xfc, 0x7, 0xf8, 0xf, 0xe0, 0xf, 0xf8, 0x7, 0xf8, 0x7, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0x1c, 0xe, 0xc, 0xc, 0xfc, 0xf, 0x80, 0x3, 0x0, 0x3, 0xc0, 0x1, 0x30, 0x3, 0xe3, 0xc7,
	 0x30, 0x0, 0x8, 0xe, 0xf0, 0xf, 0xe0, 0xf, 0xf0, 0xf, 0xc, 0xc, 0xc0, 0x0, 0x1c, 0xc, 0xec, 0x3, 0xf0, 0x0, 0x0, 0xc, 0xf0, 0x1, 0xc0, 0x0, 0xec, 0xe, 0xc, 0xe, 0x1c, 0xe, 0xc, 0xe, 0x1c, 0xc, 0x70, 0x0, 0x1c, 0x0, 0xf8, 0x7, 0xc, 0xc, 0x1c, 0xe, 0xcc, 0xc, 0x38, 0x7, 0xc, 0xc, 0x0, 0x7, 0x80, 0x3, 0x0, 0x3, 0xc0, 0x1, 0x0, 0x0, 0x73, 0xcc,
	 0xfc, 0x3, 0x0, 0xc, 0xf0, 0x1f, 0x70, 0x0, 0xf8, 0xf, 0xc, 0xc, 0xf0, 0x3, 0xc, 0xc, 0xfc, 0x7, 0xf0, 0x0, 0x0, 0xc, 0xf0, 0x0, 0xc0, 0x0, 0xcc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0x30, 0x0, 0x1c, 0x0, 0xc0, 0x0, 0xc, 0xc, 0x18, 0x6, 0xcc, 0xc, 0xb0, 0x3, 0xc, 0xc, 0x80, 0x3, 0xe0, 0x1, 0x0, 0x3, 0x80, 0x7, 0x0, 0x0, 0x33, 0xc0,
	 0xfc, 0x3, 0xf0, 0xf, 0x30, 0x38, 0x30, 0x0, 0x1c, 0xc, 0xfc, 0x7, 0xf0, 0x3, 0xc, 0xc, 0x1c, 0xe, 0xc0, 0x0, 0x0, 0xc, 0xf0, 0x0, 0xc0, 0x0, 0xcc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0x30, 0x0, 0xf8, 0x3, 0xc0, 0x0, 0xc, 0xc, 0x38, 0x7, 0xcc, 0xc, 0xe0, 0x0, 0xc, 0xc, 0xc0, 0x1, 0xe0, 0x1, 0x0, 0x3, 0x80, 0x7, 0x0, 0x0, 0x33, 0xc0,
	 0x30, 0x0, 0xf8, 0xf, 0x30, 0x30, 0x30, 0x0, 0xc, 0xc, 0xfc, 0x3, 0xc0, 0x0, 0x1c, 0xc, 0xc, 0xc, 0xc0, 0x0, 0x0, 0xc, 0xf0, 0x1, 0xc0, 0x0, 0xcc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xe, 0x1c, 0xc, 0x30, 0x0, 0xf0, 0x7, 0xc0, 0x0, 0xc, 0xc, 0x30, 0x3, 0xcc, 0xc, 0xc0, 0x1, 0x1c, 0xe, 0xe0, 0x0, 0x80, 0x3, 0x0, 0x3, 0xc0, 0x1, 0x0, 0x0, 0x73, 0xcc,
	 0x30, 0x0, 0xc, 0xc, 0x30, 0x30, 0x30, 0x0, 0xc, 0xc, 0xc, 0x0, 0xc0, 0x0, 0xf8, 0xf, 0xc, 0xc, 0xc0, 0x0, 0x0, 0xc, 0xb0, 0x3, 0xc0, 0x0, 0xcc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xfc, 0x7, 0xf8, 0xf, 0x30, 0x0, 0x0, 0xe, 0xc0, 0x0, 0xc, 0xc, 0x30, 0x3, 0xcc, 0xc, 0x70, 0x3, 0xf8, 0xf, 0x70, 0x0, 0x80, 0x3, 0x0, 0x3, 0xc0, 0x1, 0x0, 0x0, 0xe3, 0xc7,
	 0x30, 0x0, 0xc, 0xc, 0x30, 0x38, 0x70, 0x0, 0x1c, 0xc, 0x1c, 0x0, 0xc0, 0x0, 0xf0, 0xf, 0xc, 0xc, 0xc0, 0x0, 0x0, 0xc, 0x30, 0x7, 0xc0, 0x1, 0xcc, 0xc, 0xc, 0xc, 0x1c, 0xe, 0xfc, 0x3, 0xf0, 0xf, 0x30, 0x0, 0x0, 0xe, 0xc0, 0x1, 0x1c, 0xe, 0xe0, 0x1, 0xdc, 0xe, 0x38, 0x7, 0xf0, 0xd, 0x38, 0x0, 0x0, 0x3, 0x0, 0x3, 0xc0, 0x0, 0x0, 0x0, 0xc7, 0xe3,
	 0xfc, 0x3f, 0xf8, 0xf, 0xf0, 0x1f, 0xe0, 0xf, 0xf8, 0xf, 0xf8, 0xf, 0xc0, 0x0, 0x0, 0xc, 0xc, 0xc, 0xf0, 0x3, 0x30, 0xc, 0x30, 0xe, 0x80, 0xf, 0xcc, 0xc, 0xc, 0xc, 0xf8, 0x7, 0xc, 0x0, 0x0, 0xc, 0x30, 0x0, 0xfc, 0x7, 0x80, 0xf, 0xf8, 0x7, 0xe0, 0x1, 0xf8, 0x7, 0x1c, 0xe, 0x0, 0xc, 0xfc, 0xf, 0x0, 0x1f, 0x0, 0x3, 0xf8, 0x0, 0x0, 0x0, 0xe, 0x70,
	 0xfc, 0x3f, 0xf0, 0xf, 0xf0, 0xf, 0xc0, 0xf, 0xf0, 0xf, 0xf0, 0xf, 0xc0, 0x0, 0x0, 0xe, 0xc, 0xc, 0xf0, 0x3, 0x70, 0xe, 0x30, 0xc, 0x0, 0xf, 0xcc, 0xc, 0xc, 0xc, 0xf0, 0x3, 0xc, 0x0, 0x0, 0xc, 0x30, 0x0, 0xf8, 0x3, 0x0, 0x7, 0xf0, 0x3, 0xc0, 0x0, 0x30, 0x3, 0xc, 0xc, 0x0, 0xe, 0xfc, 0xf, 0x0, 0x1e, 0x0, 0x3, 0x78, 0x0, 0x0, 0x0, 0x1c, 0x38,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x7, 0x0, 0x0, 0x0, 0x0, 0xe0, 0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0x0, 0x0, 0x1c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x1f,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3, 0x0, 0x0, 0x0, 0x0, 0xc0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0x0, 0x0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xf };


	//____ create _________________________________________________________________

	SysFont_p SysFont::create( SurfaceFactory * pFactory, Font * pBackupFont )
	{
		return SysFont_p(new SysFont(pFactory, pBackupFont));
	}


	//____ constructor ____________________________________________________________

	SysFont::SysFont(SurfaceFactory* pFactory, Font * pBackupFont ) : Font(pBackupFont)
	{
		m_pSurface = pFactory->createSurface( WGBP(Surface, _.size = { 512,48 }, _.format = PixelFormat::Alpha_8) );

		auto pixbuf = m_pSurface->allocPixelBuffer();

		uint8_t val[2] = { 0,255 };

		uint8_t* pPixel = pixbuf.pixels;
		uint8_t* pRead = compressed_font[0];
		for (int y = 0; y < 48; y++)
		{
			for (int x = 0; x < 512 / 8; x++)
			{
				int in = compressed_font[y][x];
				for (int i = 0; i < 8; i++)
				{
					*pPixel++ = val[in & 0x1];
					in >>= 1;
				}
			}
			pPixel += pixbuf.pitch - 512;
		}

		m_pSurface->pullPixels(pixbuf);
		m_pSurface->freePixelBuffer(pixbuf);
	
	}

	//____ destructor ____________________________________________________________

	SysFont::~SysFont()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SysFont::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ getGlyphWithoutBitmap() ____________________________________________

	void SysFont::getGlyphWithoutBitmap(uint16_t chr, Glyph& glyph)
	{
		if (chr > 32 && chr < 127)
		{
			glyph.advance = 14 * 64;
			glyph.kerningIndex = 0;
			glyph.fontRef = this;
		}
		else
		{
			glyph.advance = 0;
			glyph.kerningIndex = 0;
			glyph.fontRef = nullptr;
		}
	}

	//____ getGlyphWithBitmap() _______________________________________________

	void SysFont::getGlyphWithBitmap(uint16_t chr, Glyph& glyph)
	{
		if (chr > 32 && chr < 127)
		{
			glyph.advance = 14 * 64;
			glyph.kerningIndex = 0;
			glyph.fontRef = this;
			glyph.pSurface = m_pSurface;
			glyph.bearingX = 0;
			glyph.bearingY = -12*64;
			glyph.rect.x = ((chr - 32) % 32) * 16 * 64;
			glyph.rect.y = ((chr - 32) / 32) * 16 * 64;
			glyph.rect.w = 14 * 64;
			glyph.rect.h = 16 * 64;
		}
		else
		{
			glyph.advance = 0;
			glyph.kerningIndex = 0;
			glyph.fontRef = nullptr;
			glyph.pSurface = nullptr;
		}

	}

	//____ kerning() __________________________________________________________

	spx SysFont::kerning(Glyph& leftGlyph, Glyph& rightGlyph)
	{
		return 0;
	}

	//____ hasGlyph() _________________________________________________

	bool SysFont::hasGlyph(uint16_t chr)
	{
		return (chr > 32 && chr < 127);

	}

} // namespace
