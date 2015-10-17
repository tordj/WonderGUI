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

#include <wg_gfxdevice.h>

#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_geo.h>
#include <wg_legacytextfield.h>
#include <wg_caretinstance.h>
#include <wg_font.h>
#include <wg_gfxanim.h>
#include <wg_util.h>
#include <wg_pen.h>
#include <wg_widget.h>
#include <algorithm>

namespace wg 
{
	
	const char GfxDevice::CLASSNAME[] = {"GfxDevice"};
	
	//____ Constructor _____________________________________________________________
	
	GfxDevice::GfxDevice( Size canvasSize )
	{
		m_tintColor 		= Color(255,255,255);
		m_blendMode 		= WG_BLENDMODE_BLEND;
		m_bSaveDirtyRects 	= false;
		m_renderFlags		= 0;
		m_canvasSize		= canvasSize;
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool GfxDevice::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * GfxDevice::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	GfxDevice_p GfxDevice::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return GfxDevice_p( static_cast<GfxDevice*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setSaveDirtyRects() _____________________________________________________
	
	bool GfxDevice::setSaveDirtyRects( bool bSave )
	{
		m_bSaveDirtyRects = bSave;
		return true;
	}
	
	//____ setTintColor() __________________________________________________________
	
	void GfxDevice::setTintColor( Color color )
	{
		m_tintColor = color;
	}
	
	//____ setBlendMode() __________________________________________________________
	
	bool GfxDevice::setBlendMode( BlendMode blendMode )
	{
		m_blendMode = blendMode;
		return false;				// Not implemented.
	}
	
	//______________________________________________________________________________
	uint32_t GfxDevice::setRenderFlags( uint32_t flags )
	{
		uint32_t oldFlags = m_renderFlags;
		m_renderFlags = flags;
		return oldFlags;
	}
	
	//____ beginRender() ___________________________________________________________
	
	bool GfxDevice::beginRender()
	{
		return true;	// Assumed to be ok if device doesn't have its own method.
	}
	
	//____ endRender() _____________________________________________________________
	
	bool GfxDevice::endRender()
	{
		return true;	// Assumed to be ok if device doesn't have its own method.
	}
	
	//____ blit() __________________________________________________________________
	
	void GfxDevice::blit( const Surface_p& pSrc )
	{
		blit( pSrc, Rect( 0, 0, pSrc->width(), pSrc->height() ), 0, 0 );
	}
	
	void GfxDevice::blit( const Surface_p& pSrc, int dx, int dy )
	{
		blit( pSrc, Rect( 0, 0, pSrc->width(), pSrc->height() ), dx, dy );
	}
	
	//____ stretchBlit() ___________________________________________________________
	
	void GfxDevice::stretchBlit( const Surface_p& pSrc, bool bTriLinear, float mipmapBias )
	{
		stretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->width(), (float) pSrc->height(), 0, 0, (float) m_canvasSize.w, (float) m_canvasSize.h, bTriLinear, mipmapBias );
	}
	
	void GfxDevice::stretchBlit( const Surface_p& pSrc, const Rect& dest, bool bTriLinear, float mipmapBias )
	{
		stretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->width(), (float) pSrc->height(), (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
	}
	
	void GfxDevice::stretchBlit( const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear, float mipmapBias )
	{
		stretchBlitSubPixel( pSrc, (float) src.x, (float) src.y, (float) src.w, (float) src.h, (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
	}
	
	//____ tileBlit() ______________________________________________________________
	
	void GfxDevice::tileBlit( const Surface_p& _pSrc )
	{
		tileBlit( _pSrc, Rect( 0, 0, _pSrc->width(), _pSrc->height() ), Rect(0,0,m_canvasSize) );
	}
	
	void GfxDevice::tileBlit( const Surface_p& _pSrc, const Rect& _dest )
	{
		tileBlit( _pSrc, Rect( 0, 0, _pSrc->width(), _pSrc->height() ), _dest );
	}
	
	void GfxDevice::tileBlit( const Surface_p& _pSrc, const Rect& _src, const Rect& _dest )
	{
		if( !_pSrc || _dest.h == 0 || _dest.w == 0 )
			return;
	
		Rect	r = _src;
		Rect	r2 = _src;
	
		int nCol = _dest.w / _src.w;
		r2.w = _dest.w % _src.w;
	
		int nRow = (_dest.h+(_src.h-1))/ _src.h;	// Including any cut row....
	
		int		destX = _dest.x;
		int		destY = _dest.y;
	
		for( int row = 1 ; row <= nRow ; row++ )
		{
			// Possibly cut the height if this is the last row...
	
			if( row == nRow )
			{
				r.h = _dest.y + _dest.h - destY;
				r2.h = r.h;
			}
	
			// Blit a row.
	
			for( int col = 0 ; col < nCol ; col++ )
			{
				blit( _pSrc, r, destX, destY );
				destX += r.w;
			}
	
			// Blit any left over part at end of row.
	
			if( r2.w > 0 )
				blit( _pSrc, r2, destX, destY );
	
			destX = _dest.x;
			destY += _src.h;
		}
		return;
	}
	
	
	//____ clipFill() ______________________________________________________________
	
	void GfxDevice::clipFill( const Rect& _clip, const Rect& _rect, const Color& _col )
	{
		fill( Rect( _clip, _rect ), _col );
	}
	
	//____ clipBlit() ______________________________________________________________
	
	void GfxDevice::clipBlit( const Rect& clip, const Surface_p& pSrc )
	{
		clipBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), 0, 0 );
	}
	
	void GfxDevice::clipBlit( const Rect& clip, const Surface_p& pSrc, int dx, int dy  )
	{
		clipBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), dx, dy );
	}
	
	void GfxDevice::clipBlit( const Rect& clip, const Surface_p& pSrc, const Rect& srcRect, int dx, int dy  )
	{
		if( (clip.x <= dx) && (clip.x + clip.w > dx + srcRect.w) &&
	      (clip.y <= dy) && (clip.y + clip.h > dy + srcRect.h) )
		{
			blit( pSrc, srcRect, dx, dy );														// Totally inside clip-rect.
			return;
		}
	
		if( (clip.x > dx + srcRect.w) || (clip.x + clip.w < dx) ||
	      (clip.y > dy + srcRect.h) || (clip.y + clip.h < dy) )
			return;																						// Totally outside clip-rect.
	
		// Do Clipping
	
		Rect	newSrc = srcRect;
	
		if( dx < clip.x )
		{
			newSrc.w -= clip.x - dx;
			newSrc.x += clip.x - dx;
			dx = clip.x;
		}
	
		if( dy < clip.y )
		{
			newSrc.h -= clip.y - dy;
			newSrc.y += clip.y - dy;
			dy = clip.y;
		}
	
		if( dx + newSrc.w > clip.x + clip.w )
			newSrc.w = (clip.x + clip.w) - dx;
	
		if( dy + newSrc.h > clip.y + clip.h )
			newSrc.h = (clip.y + clip.h) - dy;
	
	
		blit( pSrc, newSrc, dx, dy );
	}
	
	//____ clipStretchBlit() _______________________________________________________
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, bool bTriLinear, float mipBias )
	{
		clipStretchBlit( clip, pSrc, Rect(0,0,pSrc->width(), pSrc->height()), Rect( 0,0,m_canvasSize), bTriLinear, mipBias );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& dest, bool bTriLinear, float mipBias )
	{
		clipStretchBlit( clip, pSrc, Rect(0,0,pSrc->width(), pSrc->height()), dest, bTriLinear, mipBias );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear, float mipBias )
	{
		clipStretchBlit( clip, pSrc, (float)src.x, (float)src.y, (float)src.w, (float)src.h, (float)dest.x, (float)dest.y, (float)dest.w, (float)dest.h, false );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias)
	{
		float cx = std::max(float(clip.x), dx);
		float cy = std::max(float(clip.y), dy);
		float cw = std::min(float(clip.x + clip.w), dx + dw) - cx;
		float ch = std::min(float(clip.y + clip.h), dy + dh) - cy;
	
		if(cw <= 0 || ch <= 0)
			return;
	
		if( dw > cw )
		{
			float	sdxr = sw / dw;			// Source/Destination X Ratio.
	
			sw = sdxr * cw;
	
			if( dx < cx )
				sx += sdxr * (cx - dx);
		}
	
		if( dh > ch )
		{
			float	sdyr = sh / dh;			// Source/Destination Y Ratio.
	
			sh = sdyr * ch;
	
			if( dy < cy )
				sy += sdyr * (cy - dy);
		}
	
		stretchBlitSubPixel( pSrc, sx, sy, sw, sh, cx, cy, cw, ch, bTriLinear, mipBias );
	}
	
	//____ clipTileBlit() __________________________________________________________
	
	void GfxDevice::clipTileBlit( const Rect& clip, const Surface_p& pSrc )
	{
		clipTileBlit( clip, pSrc, Rect( 0,0,pSrc->width(),pSrc->height() ),
					  Rect(0,0,m_canvasSize) );
	}
	
	void GfxDevice::clipTileBlit( const Rect& clip, const Surface_p& pSrc,
									  const Rect& dest )
	{
		clipTileBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), dest );
	}
	
	
	void GfxDevice::clipTileBlit( const Rect& _clip, const Surface_p& _pSrc, const Rect& _src, const Rect& _dest )
	{
		if( !_pSrc )
			return;
	
		Rect	myRect;
	
		Rect	clip;
		if( !clip.intersection( _dest, _clip ) )
			return;
	
		// Take care of start-offset change caused by clipping.
	
		int		xStart = (clip.x - _dest.x) % _src.w;
		if( xStart < 0 )
			xStart += _src.w;
		xStart += _src.x;
	
		int		yStart = (clip.y - _dest.y) % _src.h;
		if( yStart < 0 )
			yStart += _src.h;
		yStart += _src.y;
	
	
		int		destY = clip.y, destX;
	
		myRect.y = yStart;
		myRect.h =_src.y + _src.h - yStart;
	
		while( destY < clip.y + clip.h )
		{
			if( myRect.h > clip.y + clip.h - destY )
				myRect.h = clip.y + clip.h - destY;
	
			myRect.x = xStart;
			myRect.w = _src.x + _src.w - xStart;
			if( myRect.w > clip.w )
				myRect.w = clip.w;
	
	
			// Blit a row.
	
			destX = clip.x;
			blit( _pSrc, myRect, destX, destY );
			destX += myRect.w;
			myRect.x = _src.x;
			myRect.w = _src.w;
	
			while( destX <= clip.x + clip.w - _src.w )
			{
				blit( _pSrc, myRect, destX, destY );
				destX += myRect.w;
			}
			myRect.w = clip.x + clip.w - destX;
			if( myRect.w > 0 )
				blit( _pSrc, myRect, destX, destY );
	
			destY += myRect.h;
			myRect.y = _src.y;
			myRect.h = _src.h;
		}
		return;
	}
	
	
	//____ clipBlitHorrBar() ______________________________________________________
	
	void GfxDevice::clipBlitHorrBar(	const Rect& _clip, const Surface_p& _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, int _dx, int _dy, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		_dx += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_len -= r.w;
				_dx += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_dx += _len;
			}
		}
		else
		{
			clipStretchBlit( _clip, _pSurf, r, Rect( _dx, _dy, _len, r.h ) );
			_dx += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	}
	
	//____ clipBlitVertBar() ______________________________________________________
	
	void GfxDevice::clipBlitVertBar(	const Rect& _clip, const Surface_p& _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, int _dx, int _dy, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	
		_len -= _borders.height();			// Remove top and bottom edges from len.
		_dy += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_len -= r.h;
				_dy += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_dy += _len;
			}
		}
		else
		{
			clipStretchBlit( _clip, _pSurf, r, Rect( _dx, _dy, r.w, _len ) );
			_dy += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	}
	
	
	//____ blitHorrBar() __________________________________________________________
	
	void GfxDevice::blitHorrBar(	const Surface_p& _pSurf, const Rect& _src,
									const Border& _borders, bool _bTile,
									int _dx, int _dy, int _len )
	{
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		blit( _pSurf, r, _dx, _dy );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		_dx += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				blit( _pSurf, r, _dx, _dy );
				_len -= r.w;
				_dx += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				blit( _pSurf, r, _dx, _dy );
				_dx += _len;
			}
		}
		else
		{
			stretchBlit( _pSurf, r, Rect( _dx, _dy, _len, r.h ) );
			_dx += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		blit( _pSurf, r, _dx, _dy );
	}
	
	//____ blitVertBar() __________________________________________________________
	
	void GfxDevice::blitVertBar(	const Surface_p& _pSurf, const Rect& _src,
									const Border& _borders, bool _bTile,
									int _dx, int _dy, int _len )
	{
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		blit( _pSurf, r, _dx, _dy );
	
		_len -= _borders.height();			// Remove top and bottom borders from len.
		_dy += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				blit( _pSurf, r, _dx, _dy );
				_len -= r.h;
				_dy += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				blit( _pSurf, r, _dx, _dy );
				_dy += _len;
			}
		}
		else
		{
			stretchBlit( _pSurf, r, Rect( _dx, _dy, r.w, _len ) );
			_dy += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		blit( _pSurf, r, _dx, _dy );
	}
	
	//____ printText() ____________________________________________________________
	
	bool GfxDevice::printText( const Rect& clip, const LegacyTextField * pText, const Rect& dest )
	{
		if( !pText || dest.w <= 0 )
			return false;
	
		_drawTextBg(clip, pText, dest);
	
		Pen pen;
		pen.setDevice( this );
	
		TextAttr	attr;
		pText->getBaseAttr(attr);
	
		if( !attr.pFont )
			return false;
	
		pen.setAttributes(attr);
	
		Size	textSize( pText->width(), pText->height() );
	
		if( dest.h < (int) textSize.h || dest.w < (int) textSize.w || !clip.contains( dest ) || pText->isCursorShowing() )
			pen.setClipRect( clip );
	
		const CaretInstance* pCursor = 0;
		int cursLine = -1, cursCol = -1;
	
		if( pText->isCursorShowing() )
		{
			pCursor = pText->getCursor();
			pCursor->getSoftPos( cursLine, cursCol );
		}
	
		bool bEllipsisActive = false;
		if( pText->autoEllipsis() && !pText->isCursorShowing() && (textSize.w > dest.w || textSize.h > dest.h) )
			bEllipsisActive = true;
	
		Coord	pos;
		pos.y = pText->lineStartY( 0, dest ) + pText->getSoftLine(0)->baseline;
	
		int					nLines = pText->nbSoftLines();
		const LegacyTextLine *	pLines = pText->getSoftLines();
	
		for( int i = 0 ; i < nLines ; i++ )
		{
			pos.x = pText->lineStartX( i, dest );
			pen.setOrigo( pos );		// So tab positions will start counting from start of line.
			pen.setPos( pos );
			pen.flushChar();			// Remove kerning info for previous char.
	
			bool bLastFullyVisibleLine = false;
			if( (i < nLines-1) && (pos.y + pLines[i].lineSpacing + pLines[i+1].height - pLines[i+1].baseline > dest.y + dest.h) )
				bLastFullyVisibleLine = true;
	
			if( cursLine == i )
			{
				// Draw line parts, make space for cursor.
	
				_printTextSpan( pen, pText, pLines[i].ofs, cursCol, false );
				Coord cursorPos = pen.getPos();
				pen.advancePosCursor( *pCursor );
				_printTextSpan( pen, pText, pLines[i].ofs + cursCol, pLines[i].nChars - cursCol, true );
	
				// Blit the cursor
	
				Coord restorePos = pen.getPos();
				pen.setPos( cursorPos );
				pen.blitCursor( *pCursor );
				pen.setPos( restorePos );
			}
			else
			{
				if( bEllipsisActive && (bLastFullyVisibleLine || pLines[i].width > dest.w) )
				{
					_printEllipsisTextSpan( pen, pText, pLines[i].ofs, pLines[i].nChars, dest.x + dest.w );
					break;
				}
				else
					_printTextSpan( pen, pText, pLines[i].ofs, pLines[i].nChars, true );
			}
	
			pos.y += pLines[i].lineSpacing;
		}
	
		if( dest.w >= textSize.w && (dest.h >= textSize.h || nLines == 1) )
			return true;
		else
			return false;
	}
	
	//____ _printTextSpan() ________________________________________________________
	
	void GfxDevice::_printTextSpan( Pen& pen, const LegacyTextField * pText, int ofs, int len, bool bLineEnding )
	{
		Color baseCol	= m_tintColor;
		Color	color	= baseCol;
	
		const Char * pChars = pText->getText();
		uint16_t	hProp	= 0xFFFF;		// Setting to impossible value forces setting of properties in first loop.
		TextAttr		attr;
	
		Range	selection = pText->getSelection();
	
		// Print loop
	
		for( int i = ofs ; i < ofs + len ; i++ )
	 	{
			// Act on possible change of character attributes.
	
			if( pChars[i].propHandle() != hProp || i == selection.begin() || i == selection.end() )
			{
				bool bWasUnderlined = attr.bUnderlined;
	
				hProp = pChars[i].propHandle();
	
				pText->getCharAttr( attr, i );
				pen.setAttributes(attr);
				if( !pen.getGlyphset() )
					return;											// Better not to print than to crash...
	
				// Set tint colors (if changed)
	
				if( pen.getColor() != color )
				{
					color = pen.getColor();
					setTintColor( baseCol * color );
				}
	
				// Check if this is start of underlined text and in that case draw the underline.
	
				if( attr.bUnderlined && (i==0 || !bWasUnderlined) )
				{
					Rect clip = pen.hasClipRect()?pen.getClipRect():Rect(0,0,65535,65535);
					_drawUnderline( clip, pText, pen.getPosX(), pen.getPosY(), i, (ofs+len)-i );
				}
	
			}
	
			// Calculate position and blit the glyph.
	
			uint16_t ch = pChars[i].getGlyph();
	
			bool bBlit = pen.setChar( ch );
			pen.applyKerning();
			if( bBlit )
			{
	
	/*			if(selStartX == -1 && i >= iSelStart)
					selStartX = pen.getBlitPosX();
	
				if(selStartX >= 0 && i < iSelEnd)
					selEndX = pen.getBlitPosX();
	*/
				pen.blitChar();
			}
	
			pen.advancePos();
	
	 	}
	
		// Render line-endings.
	
		if( bLineEnding )
		{
			// If character after line-end was a WG_HYPHEN_BREAK_PERMITTED we need
			// to render a normal hyphen.
	
			if( pChars[ofs+len].getGlyph() == WG_HYPHEN_BREAK_PERMITTED )
			{
				if( pen.setChar( '-' ) )
				{
					pen.applyKerning();
					pen.blitChar();
				}
			}
	
			// TODO: print LF-character if there is one following and properties says it should be displayed.
	
		}
	
		// Restore tint color.
	
		if( m_tintColor != baseCol )
			setTintColor(baseCol);
	}
	
	
	//____ _printEllipsisTextSpan() ________________________________________________________
	
	void GfxDevice::_printEllipsisTextSpan( Pen& pen, const LegacyTextField * pText, int ofs, int len, int endX )
	{
		Color baseCol	= m_tintColor;
		Color	color	= baseCol;
	
		const Char * pChars = pText->getText();
		uint16_t	hProp	= 0xFFFF;		// Setting to impossible value forces setting of properties in first loop.
		TextAttr		attr;
		TextAttr		baseAttr;
	
		Range	selection = pText->getSelection();
		int		ellipsisWidth = 0;
	
	
		// Get the width of an ellipsis
	
		pText->getBaseAttr( baseAttr );	// Ellipsis are always rendered using the base attributes.
		pen.setAttributes( baseAttr );
	
		uint16_t	ellipsisChar = WG_ELLIPSIS;
		ellipsisWidth = 0;
		Glyph_p pEllipsis = pen.getFont()->getGlyph( WG_ELLIPSIS, pen.getStyle(), pen.getSize() );
	
		if( !pEllipsis )
		{
			pEllipsis = pen.getFont()->getGlyph( '.', pen.getStyle(), pen.getSize() );
			ellipsisChar = '.';
		}
	
		if( pEllipsis )
		{
			const GlyphBitmap * pBitmap = pEllipsis->getBitmap();
			if( pBitmap )
			{
				if( ellipsisChar == WG_ELLIPSIS )
					ellipsisWidth = pBitmap->rect.w + pBitmap->bearingX;
				else
					ellipsisWidth = pEllipsis->advance()*2+pBitmap->rect.w + pBitmap->bearingX;
			}
		}
	
		// Print loop
	
		for( int i = ofs ; i < ofs + len ; i++ )
	 	{
			// Act on possible change of character attributes.
	
			if( pChars[i].propHandle() != hProp || i == selection.begin() || i == selection.end() )
			{
				bool bWasUnderlined = attr.bUnderlined;
	
				hProp = pChars[i].propHandle();
	
				pText->getCharAttr( attr, i );
				pen.setAttributes(attr);
				if( !pen.getGlyphset() )
					return;											// Better not to print than to crash...
	
				// Set tint colors (if changed)
	
				if( pen.getColor() != color )
				{
					color = pen.getColor();
					setTintColor( baseCol * color );
				}
	
				// Check if this is start of underlined text and in that case draw the underline.
	
				if( attr.bUnderlined && (i==0 || !bWasUnderlined) )
				{
					Rect clip = pen.hasClipRect()?pen.getClipRect():Rect(0,0,65535,65535);
					_drawUnderline( clip, pText, pen.getPosX(), pen.getPosY(), i, (ofs+len)-i );
				}
			}
	
			// Calculate position and blit the glyph.
	
			uint16_t ch = pChars[i].getGlyph();
	
			bool bBlit = pen.setChar( ch );
	
			Coord savedPos = pen.getPos();
			pen.applyKerning();
			Glyph_p pGlyph = pen.getGlyph();
			if( pen.getPosX() +  pGlyph->advance() + ellipsisWidth > endX )
			{
				pen.setPos( savedPos );
				break;
			}
	
			if( bBlit )
				pen.blitChar();
	
			pen.advancePos();
	
	 	}
	
		// Render ellipsis.
	
		pen.setAttributes(baseAttr);		// Ellipsis are always rendered using the base attributes.
		pen.setChar( ellipsisChar );
	
		// Set tint colors (if changed)
	
		if( pen.getColor() != color )
		{
			color = pen.getColor();
			setTintColor( baseCol * color );
		}	
		
		if( ellipsisChar == '.' )
		{
			pen.setChar( ellipsisChar );		// Set once more to get kerning between dots to work.
			pen.blitChar();
			pen.advancePos();
			pen.applyKerning();
			pen.blitChar();
			pen.advancePos();
			pen.applyKerning();
			pen.blitChar();
		}
		else
			pen.blitChar();						// We could have kerning here but we have screwed up previous glyph...
	
		// Restore tint color.
	
		if( m_tintColor != baseCol )
			setTintColor(baseCol);
	}
	
	
	//____ _drawTextBg() ___________________________________________________________
	
	void GfxDevice::_drawTextBg( const Rect& _clip, const LegacyTextField * pText, const Rect& dest )
	{
		Rect		clip(_clip,dest);		// Make sure clipping rect is inside dest.
	
		State state = pText->state();
	
		// Take care of selection background color (if we have any)
	
		int selStart, selEnd;
	
		int startLine, startCol, endLine, endCol;
		pText->getSelection( startLine, startCol, endLine, endCol );
		selStart = pText->lineColToOffset(startLine, startCol);
		selEnd = pText->lineColToOffset(endLine,endCol);
	
		Textprop_p pSelProp = TextTool::getSelectionProperties(pText);
	
		if( selStart != selEnd && pSelProp->isBgColored() )
		{
			_drawTextSectionBg( clip, pText, dest, selStart, selEnd, pSelProp->bgColor(state) );
		}
		else
		{
			selStart = -1;
			selEnd = -1;
		}
	
		// Scan through the text, drawing character specific backgrounds
		// (and in the future punching holes in the general background?)
	
		uint16_t	hProp = 0xFFFF;
		Color	color;
		int		startOfs = 0;
	
		const Char * pChars = pText->getText();
		int nChars = pText->length();
	
		for( int ofs = 0 ; ofs < nChars ; ofs++ )
		{
			if( ofs == selStart )
			{
				if( color.a != 0 )
					_drawTextSectionBg( clip, pText, dest, startOfs, ofs, color );
	
				startOfs = selEnd;			//TODO: We should not skip rendering background color, there migth be on link or character (or selection might not have one).
				ofs = startOfs;
			}
	
			if( pChars[ofs].propHandle() != hProp )
			{
				// Update hProp and get background color
	
				hProp = pChars[ofs].propHandle();
	
				Color newColor = pText->getCharBgColor(ofs);
	
				if( newColor != color )
				{
					// Draw previous bg section which now ended
	
					if( ofs != startOfs && color.a != 0 )
						_drawTextSectionBg( clip, pText, dest, startOfs, ofs, color );
	
					// Set start and color of current background section
	
					color = newColor;
					startOfs = ofs;
				}
	
			}
		}
	
		// Draw last background section if it is colored
	
		if( startOfs != nChars && color.a != 0 )
			_drawTextSectionBg( clip, pText, dest, startOfs, nChars, color );
	}
	
	//____ _drawTextSectionBg() ___________________________________________________
	
	void GfxDevice::_drawTextSectionBg( const Rect& clip, const LegacyTextField * pText, const Rect& dstRect,
										  int iStartOfs, int iEndOfs, Color color )
	{
		const LegacyTextLine *	pLines = pText->getSoftLines();
	
		TextPos startPos = pText->ofsToPos( iStartOfs );
		TextPos endPos = pText->ofsToPos( iEndOfs );
	
		int xs = pText->posToCoordX( startPos, dstRect );
		int xe = pText->posToCoordX( endPos, dstRect );
	
		int dstPosY = pText->lineStartY( 0, dstRect );
	
		Rect r;
	
		if(startPos.line == endPos.line)
		{
			r.x = xs;
			r.y = dstPosY + pText->getLineOfsY(startPos.line);
			r.w = xe - xs;
			r.h = pLines[startPos.line].height;
			clipFill(clip, r, color);
		}
		else
		{
			r.x = xs;
			r.y = dstPosY + pText->getLineOfsY(startPos.line);
			r.w = pText->lineStartX(startPos.line, dstRect) + pText->getSoftLineSelectionWidth(startPos.line) - xs;
			r.h = pLines[startPos.line].height;
			clipFill(clip, r, color);
			r.y += pLines[startPos.line].lineSpacing;
	
			++startPos.line;
			for(; startPos.line < endPos.line; ++startPos.line)
			{
				r.x = pText->lineStartX( startPos.line, dstRect );
				r.w = pText->getSoftLineSelectionWidth(startPos.line);
				r.h = pLines[startPos.line].height;
				clipFill(clip, r, color);
				r.y += pLines[startPos.line].lineSpacing;
			}
	
			r.x = pText->lineStartX( startPos.line, dstRect );
			r.w = xe - r.x;
			r.h = pLines[startPos.line].height;
			clipFill(clip, r, color);
		}
	}
	
	//____ printLine() ________________________________________________________
	
	void GfxDevice::printLine( Pen& pen, const TextAttr& baseAttr, const Char * _pLine, int maxChars, State state )
	{
		if( !_pLine )
			return;
	
		Color baseCol	= m_tintColor;
		Color	color	= baseCol;
	
		uint16_t	hProp				= 0xFFFF;		// Setting to impossible value forces setting of properties in first loop.
		TextAttr	attr;
	
		pen.flushChar();
	
		// Print loop
	
		int i;
		for( i = 0 ; i < maxChars && !_pLine[i].isEndOfLine(); i++ )
	 	{
			// Act on possible change of character attributes.
	
			if( _pLine[i].propHandle() != hProp )
			{
				bool bWasUnderlined = attr.bUnderlined;
	
				attr = baseAttr;
	
				TextTool::addPropAttributes( attr, _pLine[i].getProperties(), state );
	
				hProp = _pLine[i].propHandle();
	
				pen.setAttributes( attr );
				if( !pen.getGlyphset() )
					return;											// No glyphset, better to leave than to crash...
	
				// Set tint colors (if changed)
	
				if( pen.getColor() != color )
				{
					color = pen.getColor();
					setTintColor( baseCol * color );
				}
	
				// Check if this is start of underlined text and in that case draw the underline.
	/*
			TODO: Figure out how to do this properly, taking mode and char-props correctly into account.
	
				if( attr.bUnderlined && (i==0 || !bWasUnderlined) )
				{
					Rect clip = pen.hasClipRect()?pen.getClipRect():Rect(0,0,65535,65535);
					DrawUnderline( clip, pText, pen.getPosX(), pen.getPosY(), i, len-i );
				}
	*/
			}
	
			// Calculate position and blit the glyph.
	
			uint16_t ch = _pLine[i].getGlyph();
	
			bool bBlit = pen.setChar( ch );
			pen.applyKerning();
			if( bBlit )
			{
	
	/*			if(selStartX == -1 && i >= iSelStart)
					selStartX = pen.getBlitPosX();
	
				if(selStartX >= 0 && i < iSelEnd)
					selEndX = pen.getBlitPosX();
	*/
				pen.blitChar();
			}
	
			pen.advancePos();
	
	 	}
	
		// Restore tint color.
	
		if( m_tintColor != baseCol )
			setTintColor(baseCol);
	}
	
	
	//____ _drawUnderline() ________________________________________________________
	
	void GfxDevice::_drawUnderline( const Rect& clip, const LegacyTextField * pText, int _x, int _y, int ofs, int maxChars )
	{
		uint32_t hProp = 0xFFFF;
	
		Pen pen;
		const Char * pChars = pText->getText();
	
		for( int i = ofs ; i < ofs + maxChars && !pChars[i].isEndOfLine() ; i++ )
		{
			if( pChars[i].propHandle() != hProp )
			{
				TextAttr attr;
				pText->getCharAttr( attr, i );
	
				if( attr.bUnderlined )
				{
					hProp = pChars[i].propHandle();
					pen.setAttributes( attr );
				}
				else
					break;
			}
	
			pen.setChar( pChars[i].getGlyph() );
			pen.applyKerning();
			pen.advancePos();
		}
	
		const WgUnderline * pUnderline = pen.getFont()->getUnderline( pen.getSize() );
	
		clipBlitHorrBar( clip, pUnderline->pSurf, pUnderline->rect, Border( pUnderline->leftBorder, pUnderline->rightBorder, 0, 0 ), false,
						_x + pUnderline->bearingX, _y + pUnderline->bearingY, pen.getPosX() );
	}
	
	

} // namespace wg
