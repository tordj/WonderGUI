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

#ifndef	WG_GFXDEVICE_DOT_H
#define WG_GFXDEVICE_DOT_H

#include <climits>

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef	WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif


#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif


#include <wg3_gfxdevice.h>
#include <wg_geo.h>

//____ WgWaveLine ___________________________________________________________

typedef wg::WaveLine        WgWaveLine;

class	WgBlock;
class	WgText;
class	WgCursorInstance;
class 	WgPen;

class WgGfxDevice
{
public:



	// High-level print methods

    static bool		PrintText( wg::GfxDevice * pDevice, const WgText * pText, const WgRect& dest );

	// Low-level print methods

	static void		PrintLine( wg::GfxDevice * pDevice, WgPen& pen, const WgTextAttr& baseAttr, const WgChar * _pLine, int maxChars = INT_MAX, WgMode mode = WG_MODE_NORMAL );

    static void     BlitBlock( wg::GfxDevice * pDevice, const WgBlock& block, const WgRect& dest, bool bTriLinear = false, float mipmapbias = 0.f );

	
protected:

	static void _printTextSpan(wg::GfxDevice * pDevice, WgPen& pen, const WgText * pText, int ofs, int len, bool bLineEnding);
	static void _printEllipsisTextSpan(wg::GfxDevice * pDevice, WgPen& pen, const WgText * pText, int ofs, int len, int endX);
	static void _drawTextBg(wg::GfxDevice * pDevice, const WgText * pText, const WgRect& dest);
	static void _drawTextSectionBg(wg::GfxDevice * pDevice, const WgText * pText, const WgRect& dstRect, int iStartOfs, int iEndOfs, WgColor color);
	static void _drawUnderline(wg::GfxDevice * pDevice, const WgText * pText, int _x, int _y, int ofs, int maxChars);

};

#endif	// WG_GFXDEVICE_DOT_H

