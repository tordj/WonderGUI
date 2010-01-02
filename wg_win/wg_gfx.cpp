
#include	"wg_gfx.h"
#include	"wg_color.h"
#include	"wg_surface.h"
#include	"wg_fontset.h"
#include	"wg_font.h"
#include	"wg_text.h"
#include	"wg_cursorinstance.h"
#include	"wg_gfxanim.h"
#include	"wg_texttool.h"
// #include	"../mig/Sdk/src/EObjects/ERenderer_w32.h"
#include <assert.h>
#include <string.h>

#include <mss.h>					// SHOULD ALWAYS BE THE LAST INCLUDE!!!


WgSurface *			WgGfx::m_pCanvas		= 0;
//int					WgGfx::m_blendMode		= ERenderer::eBlendModeBlend;
//EColor				WgGfx::m_tintColor		= EColor(1.f,1.f,1.f);
WgColor				WgGfx::m_wgTintColor	= WgColor(255,255,255);
WgGfx::BlendMode	WgGfx::m_wgBlendMode	 = WgGfx::WG_BLENDMODE_BLEND;


//____ setCanvas() ____________________________________________________________

bool WgGfx::setCanvas( WgSurface * _pSurf )
{
	m_pCanvas = _pSurf;
	return true;
}

//____ setTintColor() _________________________________________________________

void WgGfx::setTintColor( WgColor color )
{
/*
m_tintColor.a = color.a/255.f;
	m_tintColor.r = color.r/255.f;
	m_tintColor.g = color.g/255.f;
	m_tintColor.b = color.b/255.f;
*/

	m_wgTintColor = color;
}

//____ setBlendMode() _________________________________________________________

bool WgGfx::setBlendMode( WgGfx::BlendMode blendMode )
{
/*
switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
			m_blendMode = ERenderer::eBlendModeNone;
			break;

		case WG_BLENDMODE_BLEND:
			m_blendMode = ERenderer::eBlendModeBlend;
			break;

		case WG_BLENDMODE_ADD:
			m_blendMode = ERenderer::eBlendModeAlphaAdd;
			break;

		case WG_BLENDMODE_MULTIPLY:
			m_blendMode = ERenderer::eBlendModeMultiply;
			break;

		default:
			return false;
	}
*/

	m_wgBlendMode = blendMode;
	return true;
}


//____ fillRect() _____________________________________________________________

inline Uint8 Mul( Uint8 a, Uint8 b )
{
	return Uint8( (Uint32(a)*Uint32(b)) >> 8 );
}

inline Uint8 AddSat( Uint8 a, Uint8 b )
{
	Uint32 r = Uint32(a) + Uint32(b);
	if( r > 0xff ) r = 0xff;
	return Uint8(r);
}


template< int blendmode >
inline void BlendPixel( Uint8* pDest, const Uint8* pSource )
{
	
	switch( blendmode )
	{
	case WgGfx::WG_BLENDMODE_OPAQUE:
		pDest[0] = pSource[0];
		pDest[1] = pSource[1];
		pDest[2] = pSource[2];
		pDest[3] = pSource[3];
		break;
	case WgGfx::WG_BLENDMODE_BLEND:
		pDest[0] = Blend( pDest[0], pSource[0], pSource[3] );
		pDest[1] = Blend( pDest[1], pSource[1], pSource[3] );
		pDest[2] = Blend( pDest[2], pSource[2], pSource[3] );
		pDest[3] = Blend( pDest[3], pSource[3], pSource[3] );
		break;
	case WgGfx::WG_BLENDMODE_ADD:
		pDest[0] = AddSat( pDest[0], pSource[0] );
		pDest[1] = AddSat( pDest[1], pSource[1] );
		pDest[2] = AddSat( pDest[2], pSource[2] );
		pDest[3] = AddSat( pDest[3], pSource[3] );
		break;
	case WgGfx::WG_BLENDMODE_MULTIPLY:
		pDest[0] = Mul( pDest[0], pSource[0] );
		pDest[1] = Mul( pDest[1], pSource[1] );
		pDest[2] = Mul( pDest[2], pSource[2] );
		pDest[3] = Mul( pDest[3], pSource[3] );
		break;
	default:
		assert( false );
		break;
	}
}

template< int blendmode >
inline void BlendPixel( Uint8* pDest, const Uint8* pSource, const Uint8* pModulate )
{
	
	Uint8 aTemp[4];
	aTemp[0] = Mul( pSource[0], pModulate[0] );
	aTemp[1] = Mul( pSource[1], pModulate[1] );
	aTemp[2] = Mul( pSource[2], pModulate[2] );
	aTemp[3] = Mul( pSource[3], pModulate[3] );
	
	BlendPixel< blendmode >( pDest, aTemp );
}

template< int blendmode >
inline void TBlit( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h, const Uint8* pTint )
{
	for( Uint32 y=0; y<h; y++ )
	{
		for( Uint32 x=0; x<w; x++ )
			BlendPixel<blendmode>( pDest + 4*x, pSource + 4*x, pTint );

		pDest += destpitch;
		pSource += sourcepitch;
	}
}

//____ fillRect() _____________________________________________________________

void WgGfx::fillRect( const WgRect& _rect, const WgColor& _col )
{
	if( m_pCanvas == 0 || _col.a == 0 || _rect.w < 1 || _rect.h < 1 )
		return;	

	assert( _rect.x >= 0 );
	assert( _rect.y >= 0 );
	assert( _rect.x + _rect.w <= m_pCanvas->width() );
	assert( _rect.y + _rect.h <= m_pCanvas->height() );

	Sint32 pitch =  m_pCanvas->pitch();

	if( m_pCanvas->m_bitdepth != 24 )
		return;

	Uint8* pDest = m_pCanvas->m_pData + _rect.y*pitch + 3*_rect.x;
	Uint32 w = _rect.w;
	Uint32 h = _rect.h;

	for( Uint32 y=0; y<h; y++ )
	{
		for( Uint32 x=0; x<w; x++ )
		{
			pDest[ 3*x + 0 ] = _col.r;
			pDest[ 3*x + 1 ] = _col.g;
			pDest[ 3*x + 2 ] = _col.b;
		}

		pDest += pitch;
	}
//	ERenderer::GetCurrent()->Blit( NULL, 0, 0, 0, 0, _rect.x, _rect.y, _rect.w, _rect.h, ERenderer::eBlendModeBlend, EColor(_col.r/255.f, _col.g/255.f, _col.b/255.f, _col.a/255.f) );
	m_pCanvas->m_changeCnt++;
}


//____ clipTileBlit() _________________________________________________________

// Note! pClip may NOT stretch outside pDest!

void WgGfx::clipTileBlit( const WgRect& _clip, const WgSurface* _pSrc, const WgRect& _src, const WgRect& _dest )
{
	if( !_pSrc )
		return;

	//EAssert(_src.w > 0 );
	//EAssert(_src.h > 0 );


	WgRect	myRect;

	WgRect	clip;
	if( !clip.Union( _dest, _clip ) )
		return;

	// Take care of start-offset change caused by clipping.

	Sint32		xStart = (clip.x - _dest.x) % _src.w;
	if( xStart < 0 )
		xStart += _src.w;
	xStart += _src.x;

	Sint32		yStart = (clip.y - _dest.y) % _src.h;
	if( yStart < 0 )
		yStart += _src.h;
	yStart += _src.y;


	Sint32		destY = clip.y, destX;

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

//____ clipBlitRow() __________________________________________________________

void WgGfx::clipBlitRow( const WgRect& clip, const WgSurface * pSrc, const WgRect& _srcrect, 
						 Uint32 tileOfs, Uint32 tileLen, Sint32 dx, Sint32 dy, Uint32 len, bool bStretch )
{
	/*
		This can be optimized by handling clipping directly instead of calling clipBlit().
	*/

	assert(tileLen > 0 );
	assert(tileOfs + tileLen <= (Uint32) _srcrect.w );


	// Blit left edge

	WgRect	r = _srcrect;
	r.w = tileOfs;
	clipBlit( clip, pSrc, r, dx, dy );

	len -= _srcrect.w - tileLen;			// Remove left and right edges from len.
	dx += tileOfs;

	// Blit tiling part

	r.x += tileOfs;
	r.w = tileLen;

	if( bStretch )
	{
		clipStretchBlit( clip, pSrc, r, WgRect( dx, dy, len, r.h ) );
		dx += len;
	}
	else
	{
		while( len > tileLen )
		{
			clipBlit( clip, pSrc, r, dx, dy );
			len -= tileLen;
			dx += tileLen;
		}
		if( len != 0 )
		{
			r.w = len;
			clipBlit( clip, pSrc, r, dx, dy );
			dx += len;
		}
	}

	// Blit right edge

	r.x += tileLen;
	r.w = _srcrect.x + _srcrect.w - r.x;
	clipBlit( clip, pSrc, r, dx, dy );
}


//____ clipBlitColumn() _______________________________________________________

void WgGfx::clipBlitColumn( const WgRect& clip, const WgSurface * pSrc, const WgRect& _srcrect, 
							  Uint32 tileOfs, Uint32 tileLen, Sint32 dx, Sint32 dy, Uint32 len, bool bStretch )
{
	/*
		This can be optimized by handling clipping directly instead of calling clipBlit().
	*/

	assert(tileLen > 0 );
	assert(tileOfs + tileLen <= (Uint32) _srcrect.h );

	// Blit top edge

	WgRect	r = _srcrect;
	r.h = tileOfs;
	clipBlit( clip, pSrc, r, dx, dy );

	len -= _srcrect.h - tileLen;			// Remove left and right edges from len.
	dy += tileOfs;

	// Blit tiling part

	r.y += tileOfs;
	r.h = tileLen;

	if( bStretch )
	{
		clipStretchBlit( clip, pSrc, r, WgRect( dx, dy, r.w, len ) );
		dy += len;
	}
	else
	{
		while( len > tileLen )
		{
			clipBlit( clip, pSrc, r, dx, dy );
			len -= tileLen;
			dy += tileLen;
		}
		if( len != 0 )
		{
			r.h = len;
			clipBlit( clip, pSrc, r, dx, dy );
			dy += len;
		}
	}

	// Blit bottom edge

	r.y += tileLen;
	r.h = _srcrect.y + _srcrect.h - r.y;
	clipBlit( clip, pSrc, r, dx, dy );
}

//____ clipBlitFramedTiles() __________________________________________________

void WgGfx::clipBlitFramedTiles( const WgRect& _clip, const WgSurface* pSrc, const WgRect& srcOuterFrame, 
								 const WgRect& srcInnerFrame, const WgRect& dest, bool bStretch )
{
	if( !pSrc )
		return;

	WgRect	dr;
	WgRect	sr;

	int			topBorder		= srcInnerFrame.y - srcOuterFrame.y;
	int			bottomBorder	= srcOuterFrame.y + srcOuterFrame.h - srcInnerFrame.y - srcInnerFrame.h;
	int			leftBorder		= srcInnerFrame.x - srcOuterFrame.x;
	int			rightBorder		= srcOuterFrame.x + srcOuterFrame.w - srcInnerFrame.x - srcInnerFrame.w;

	// Render upper row (top-left corner, top stretch area and top-right corner)

	if( topBorder > 0 )
	{
		sr.x = srcOuterFrame.x;
		sr.y = srcOuterFrame.y;
		sr.w = srcOuterFrame.w;
		sr.h = topBorder;
		WgGfx::clipBlitRow( _clip, pSrc, sr, leftBorder, sr.w - leftBorder - rightBorder, dest.x, dest.y, dest.w, bStretch );
	}

	// Render lowest row (bottom-left corner, bottom stretch area and bottom-right corner)

	if( bottomBorder > 0 )
	{
		sr.x = srcOuterFrame.x;
		sr.y = srcOuterFrame.y + srcOuterFrame.h - bottomBorder;
		sr.w = srcOuterFrame.w;
		sr.h = bottomBorder;
		WgGfx::clipBlitRow( _clip, pSrc, sr, leftBorder, sr.w - leftBorder - rightBorder, dest.x, dest.y + dest.h - bottomBorder, dest.w, bStretch );
	}

	// Render left and right stretch areas

	if( dest.h > topBorder + bottomBorder )
	{
		if( leftBorder > 0 )
		{
			sr.x = srcOuterFrame.x;
			sr.y = srcOuterFrame.y + topBorder;
			sr.w = leftBorder;
			sr.h = srcOuterFrame.h - topBorder - bottomBorder;

			dr.x = dest.x;
			dr.y = dest.y + topBorder;
			dr.w = leftBorder;
			dr.h = dest.h - topBorder - bottomBorder;

			if( bStretch )
				WgGfx::clipStretchBlit( _clip, pSrc, sr, dr );
			else
				WgGfx::clipTileBlit( _clip, pSrc, sr, dr );			
		}

		if( rightBorder > 0 )
		{
			sr.x = srcOuterFrame.x + srcOuterFrame.w - rightBorder;
			sr.y = srcOuterFrame.y + topBorder;
			sr.w = rightBorder;
			sr.h = srcOuterFrame.h - topBorder - bottomBorder;

			dr.x = dest.x + dest.w - rightBorder;
			dr.y = dest.y + topBorder;
			dr.w = rightBorder;
			dr.h = dest.h - topBorder - bottomBorder;

			if( bStretch )
				WgGfx::clipStretchBlit( _clip, pSrc, sr, dr );
			else
				WgGfx::clipTileBlit( _clip, pSrc, sr, dr );				
		}
	}


	// Render middle stretch area

	if( (dest.h > topBorder + bottomBorder) && (dest.w > leftBorder + rightBorder ) )
	{
		sr.x = srcOuterFrame.x + leftBorder;
		sr.y = srcOuterFrame.y + topBorder;
		sr.w = srcOuterFrame.w - leftBorder - rightBorder;
		sr.h = srcOuterFrame.h - topBorder - bottomBorder;

		dr.x = dest.x + leftBorder;
		dr.y = dest.y + topBorder;
		dr.h = dest.h - topBorder - bottomBorder;
		dr.w = dest.w - leftBorder - rightBorder;

		if( bStretch )
			WgGfx::clipStretchBlit( _clip, pSrc, sr, dr );
		else
			WgGfx::clipTileBlit( _clip, pSrc, sr, dr );					
	}
	

}


//____ blit() _________________________________________________________________

template< class T >
T TMin( const T& a, const T& b )
{
	return a < b ? a : b;
}

template< class T >
T TMax( const T& a, const T& b )
{
	return a > b ? a : b;
}

void WgGfx::blit( const WgSurface* _pSrc )
{
	if( !m_pCanvas || !_pSrc )
		return;


	blit( _pSrc, 0, 0 );


//	ERenderer::GetCurrent()->Blit(_pSrc->m_pTexture, 0, 0, 0, 0, _pSrc->width(), _pSrc->height(), m_blendMode, m_tintColor );
	m_pCanvas->m_changeCnt++;
}

void WgGfx::blit( const WgSurface* _pSrc, Sint32 _dx, Sint32 _dy )
{
	if( !m_pCanvas || !_pSrc )
		return;

	WgRect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = TMin( _pSrc->width(), m_pCanvas->width() - _dx );
	rect.h = TMin( _pSrc->height(), m_pCanvas->height() - _dy );

	blit( _pSrc, rect, _dx, _dy );

//	ERenderer::GetCurrent()->Blit(_pSrc->m_pTexture, 0, 0, _dx, _dy, _pSrc->width(), _pSrc->height(), m_blendMode, m_tintColor );
	m_pCanvas->m_changeCnt++;
}

void Blit24( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h )
{
	for( Uint32 y=0; y<h; y++ )
	{
		memcpy( pDest, pSource, w*3 );
		pSource += sourcepitch;
		pDest += destpitch;
	}
}

void Blit32( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h )
{
	for( Uint32 y=0; y<h; y++ )
	{
		for( Uint32 x=0; x<w; x++ )
		{
			pDest[ 3*x + 0 ] = pSource[ 4*x + 0 ];
			pDest[ 3*x + 1 ] = pSource[ 4*x + 1 ];
			pDest[ 3*x + 2 ] = pSource[ 4*x + 2 ];
		}

		pSource += sourcepitch;
		pDest += destpitch;
	}
}

inline Uint8 Blend( Uint8 a, Uint8 b, Uint8 k )
{
	if( k == 0 )
		return a;
	Uint32 r = Uint32(a)*Uint32(0xff-k) + Uint32(b)*Uint32(k);
	return Uint8( r >> 8 );
}

void Blend32( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h )
{
	for( Uint32 y=0; y<h; y++ )
	{
		for( Uint32 x=0; x<w; x++ )
		{
			Uint8 alpha = pSource[ 4*x + 3 ];
			if( alpha == 0 )
				continue;
			else if( alpha == 0xff )
			{
				pDest[ 3*x + 0 ] = pSource[ 4*x + 0 ];
				pDest[ 3*x + 1 ] = pSource[ 4*x + 1 ];
				pDest[ 3*x + 2 ] = pSource[ 4*x + 2 ];
			}
			else
			{
				pDest[ 3*x + 0 ] = Blend( pDest[ 3*x + 0 ], pSource[ 4*x + 0 ], alpha );
				pDest[ 3*x + 1 ] = Blend( pDest[ 3*x + 1 ], pSource[ 4*x + 1 ], alpha );
				pDest[ 3*x + 2 ] = Blend( pDest[ 3*x + 2 ], pSource[ 4*x + 2 ], alpha );
			}
		}

		pSource += sourcepitch;
		pDest += destpitch;
	}
}

Uint8 applymap( Uint8 a, Uint32 b, Uint8* pTable )
{
	return pTable[ (b<<8)+a ];
}

void BlendTint32( Uint8* pDest, Uint32 destpitch, const Uint8* pSource, Uint32 sourcepitch, Uint32 w, Uint32 h, const WgColor& tint, Uint8* pcTintmap )
{

	for( Uint32 y=0; y<h; y++ )
	{
		for( Uint32 x=0; x<w; x++ )
		{
			Uint8 alpha = pSource[ 4*x + 3 ];
			if( alpha == 0 )
				continue;
			else if( alpha == 0xff )
			{
				pDest[ 3*x + 0 ] = applymap( tint.b, pSource[ 4*x + 0 ], pcTintmap );
				pDest[ 3*x + 1 ] = applymap( tint.g, pSource[ 4*x + 1 ], pcTintmap );
				pDest[ 3*x + 2 ] = applymap( tint.r, pSource[ 4*x + 2 ], pcTintmap );
			}
			else
			{
				pDest[ 3*x + 0 ] = Blend( pDest[ 3*x + 0 ], applymap( tint.b, pSource[ 4*x + 0 ], pcTintmap ), alpha );
				pDest[ 3*x + 1 ] = Blend( pDest[ 3*x + 1 ], applymap( tint.g, pSource[ 4*x + 1 ], pcTintmap ), alpha );
				pDest[ 3*x + 2 ] = Blend( pDest[ 3*x + 2 ], applymap( tint.r, pSource[ 4*x + 2 ], pcTintmap ), alpha );
			}
		}

		pSource += sourcepitch;
		pDest += destpitch;
	}
}

void WgGfx::blit( const WgSurface* _pSrc, const WgRect& src, Sint32 _dx, Sint32 _dy )
{
	if( !m_pCanvas || !_pSrc )
		return;

	if( m_pCanvas->m_bitdepth != 24 )
		return;

	Uint32 destpitch = m_pCanvas->pitch();
	Uint32 sourcepitch = _pSrc->pitch();

	Uint32 bits = _pSrc->m_bitdepth;
	Uint8* pSource = _pSrc->m_pData + src.y*sourcepitch + (bits/8)*src.x;
	Uint8* pDest = m_pCanvas->m_pData + _dy*destpitch + 3*_dx;

	Uint32 w = src.w;
	Uint32 h = src.h;

	switch( m_wgBlendMode )
	{
		case WG_BLENDMODE_OPAQUE:
//			TBlit<WG_BLENDMODE_OPAQUE>( pDest, destpitch, pSource, sourcepitch, w, h, (Uint8*)(&m_wgTintColor) );
			break;
		case WG_BLENDMODE_BLEND:
			if( bits == 24 )
				Blit24( pDest, destpitch, pSource, sourcepitch, w, h );
			else
				Blend32( pDest, destpitch, pSource, sourcepitch, w, h );
			break;
		case WG_BLENDMODE_ADD:
//			TBlit<WG_BLENDMODE_ADD>( pDest, destpitch, pSource, sourcepitch, w, h, (Uint8*)(&m_wgTintColor) );
			break;
		case WG_BLENDMODE_MULTIPLY:
//			TBlit<WG_BLENDMODE_ADD>( pDest, destpitch, pSource, sourcepitch, w, h, (Uint8*)(&m_wgTintColor) );
			break;
		case WG_BLENDMODE_OVERLAYTINT:
			{
				static Uint8* s_pOverlay = 0;
				if( s_pOverlay == 0 )
				{
					s_pOverlay = new Uint8[0x100*0x100];
					for( Uint32 x=0; x<0x100; x++ )
					{
						for( Uint32 y=0; y<0x100; y++ )
						{
							if( y<0x80 )
								s_pOverlay[y*0x100+x] = (y*x>>7);
							else
								s_pOverlay[y*0x100+x] = 0xff - ( ((0xff-y)*(0xff-x))>>7 );
						}
					}
				}

				BlendTint32( pDest, destpitch, pSource, sourcepitch, w, h, m_wgTintColor, s_pOverlay );
			}

//			TBlit<WG_BLENDMODE_MULTIPLY>( pDest, destpitch, pSource, sourcepitch, w, h, (Uint8*)(&m_wgTintColor) );
			break;
		default:
			assert( false );
	}



//	ERenderer::GetCurrent()->Blit(pSrc->m_pTexture, src.x, src.y, dx, dy, src.w, src.h, m_blendMode, m_tintColor );
	m_pCanvas->m_changeCnt++;
}

//____ stretchBlit() __________________________________________________________

void WgGfx::stretchBlit( const WgSurface* _pSrc )
{
	if( !m_pCanvas || !_pSrc )
		return;

//	ERenderer* pE = ERenderer::GetCurrent();

//	pE->Blit(_pSrc->m_pTexture, 0.f, 0.f, (float) _pSrc->width(), (float) _pSrc->height(), 0, 0, pE->GetWidth(), pE->GetHeight() );
	
	// TODO Anders

	m_pCanvas->m_changeCnt++;
}

void WgGfx::stretchBlit( const WgSurface * _pSrc, const WgRect& _dest )
{
	if( !m_pCanvas )
		return;

	// TODO Anders

	//	ERenderer::GetCurrent()->Blit(_pSrc->m_pTexture, 0.f, 0.f, (float) _pSrc->width(), (float) _pSrc->height(), _dest.x, _dest.y, _dest.w, _dest.h );
	m_pCanvas->m_changeCnt++;
}

void WgGfx::stretchBlit( const WgSurface * _pSrc, const WgRect& _src, const WgRect& _dest )
{
	if( !m_pCanvas )
		return;

	// TODO Anders

//	ERenderer::GetCurrent()->Blit(_pSrc->m_pTexture, (float) _src.x, (float) _src.y, (float) _src.w, (float) _src.h, _dest.x, _dest.y, _dest.w, _dest.h );
	m_pCanvas->m_changeCnt++;
}

//____ clipStretchBlit() ______________________________________________________

void WgGfx::clipStretchBlit( const WgRect& _clip, const WgSurface* _pSrc )
{
	// TODO Anders
//	ERenderer* pE = ERenderer::GetCurrent();

//	clipStretchBlit( _clip, _pSrc, WgRect( 0,0,_pSrc->width(),_pSrc->height() ), WgRect( 0,0,pE->GetWidth(),pE->GetHeight() ) );
}

void WgGfx::clipStretchBlit( const WgRect& _clip, const WgSurface * _pSrc, const WgRect& _dest )
{
	clipStretchBlit( _clip, _pSrc, WgRect(0,0,_pSrc->width(),_pSrc->height()), _dest );
}

void WgGfx::clipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest )
{
	if( !m_pCanvas )
		return;

	WgRect	mydest;

	if( !mydest.Union(clip,dest) )
		return;

	float	sx = (float) src.x;
	float	sy = (float) src.y;
	float	sw = (float) src.w;
	float	sh = (float) src.h;

	if( dest.w > mydest.w )
	{
		float	sdxr = src.w / (float) dest.w;			// Source/Destination X Ratio.

		sw = sdxr * mydest.w;

		if( dest.x < mydest.x )
			sx += sdxr * (mydest.x - dest.x);
	}

	if( dest.h > mydest.h )
	{
		float	sdyr = src.h / (float) dest.h;			// Source/Destination Y Ratio.

		sh = sdyr * mydest.h;

		if( dest.y < mydest.y )
			sy += sdyr * (mydest.y - dest.y);
	}

	// TODO Anders
//	ERenderer::GetCurrent()->Blit(pSrc->m_pTexture, sx, sy, sw, sh, mydest.x, mydest.y, mydest.w, mydest.h, m_blendMode, m_tintColor );
	m_pCanvas->m_changeCnt++;
}



//____ clipBlit() _____________________________________________________________

void WgGfx::clipBlit( const WgRect& clip, const WgSurface* pSrc )
{
	if( !pSrc )
		return;
	
	WgRect	srcRect( 0, 0, pSrc->width(), pSrc->height() );

	if( (clip.x <= 0) && (clip.x + clip.w > 0 + srcRect.w) &&
      (clip.y <= 0) && (clip.y + clip.h > 0 + srcRect.h) )
	{
		blit( pSrc, srcRect, 0, 0 );														// Totally inside clip-rect.
		return;
	}

	if( (clip.x > srcRect.w) || (clip.x + clip.w < 0) ||
      (clip.y > srcRect.h) || (clip.y + clip.h < 0) )
		return;																						// Totally outside clip-rect.

	// Do Clipping

	Sint32 x;
	Sint32 y;
	
	srcRect.w -= clip.x;
	srcRect.x += clip.x;
	x = clip.x;

	srcRect.h -= clip.y;
	srcRect.y += clip.y;
	y = clip.y;

	if( x + srcRect.w > clip.x + clip.w )
		srcRect.w = (clip.x + clip.w) - x;

	if( y + srcRect.h > clip.y + clip.h )
		srcRect.h = (clip.y + clip.h) - y;


	blit( pSrc, srcRect, x, y );
}


void WgGfx::clipBlit( const WgRect& clip, const WgSurface* pSrc, Sint32 dx, Sint32 dy )
{
	if( !pSrc )
		return;

	WgRect	srcRect( 0, 0, pSrc->width(), pSrc->height() );
	clipBlit( clip, pSrc, srcRect, dx, dy );
}

void WgGfx::clipBlit( const WgRect& clip, const WgSurface* pSrc, const WgRect& srcRect, Sint32 x, Sint32 y )
{
	if( (clip.x <= x) && (clip.x + clip.w > x + srcRect.w) &&
      (clip.y <= y) && (clip.y + clip.h > y + srcRect.h) )
	{
		blit( pSrc, srcRect, x, y );														// Totally inside clip-rect.
		return;
	}

	if( (clip.x > x + srcRect.w) || (clip.x + clip.w < x) ||
      (clip.y > y + srcRect.h) || (clip.y + clip.h < y) )
		return;																						// Totally outside clip-rect.

	// Do Clipping

	WgRect	newSrc = srcRect;

	if( x < clip.x )
	{
		newSrc.w -= clip.x - x;
		newSrc.x += clip.x - x;
		x = clip.x;
	}

	if( y < clip.y )
	{
		newSrc.h -= clip.y - y;
		newSrc.y += clip.y - y;
		y = clip.y;
	}

	if( x + newSrc.w > clip.x + clip.w )
		newSrc.w = (clip.x + clip.w) - x;

	if( y + newSrc.h > clip.y + clip.h )
		newSrc.h = (clip.y + clip.h) - y;


	blit( pSrc, newSrc, x, y );
}

//____ printTextWithCursor() __________________________________________________

void WgGfx::printTextWithCursor( const WgText * pText, const WgCursorInstance& ci, const WgRect& dest )
{
	if( !pText || !pText->fonts() || !pText->fonts()->font(WG_MODE_NORMAL) )
		return;

	WgTextTool::TextParam param( * pText );
	const WgFont * pFont = param.pFont;

	const WgOrigo& origo = pText->alignment();
	int fontheight = pFont->height();
	int textheight = fontheight * pText->lines();

	int yPos = (int) (dest.y + dest.h * origo.anchorY()
							- textheight * origo.hotspotY());
	
	WgTextLine 	* p1, * p2;
	Uint32 			n1, n2;
	pText->getText( p1, n1, p2, n2 );
	
	for( int j = 0 ; j < 2 ; j++ )
	{
		
	
		for( int i = 0 ; i < (int) n1 ; i++ )
		{
			int linewidth = WgTextTool::lineWidth( param, p1[i].pText );
			if( ci.line() == i )
			{
				WgCursor *		cursor		= pText->fonts()->cursor();

				WgCursor::Mode	cursMode 	= ci.mode();
				int				cursSpacing = cursor->spacing(cursMode);
				int				cursCol 	= ci.column();
				
				linewidth += cursSpacing;
				int xPos = (int) (dest.x + dest.w * origo.anchorX()
									- linewidth * origo.hotspotX());
				int 			cursX 		= xPos + WgTextTool::lineWidthPart( param, p1[i].pText, cursCol );
					
				printLine( param, xPos, yPos, p1[i].pText, cursCol );
				printLine( param, cursX + cursSpacing, yPos, p1[i].pText + cursCol  );
				
				WgGfxAnim * pAnim	= cursor->anim(cursMode);
				WgGfxFrame * pFrame = pAnim->getFrame( ci.time(), 0 );
				
				blit( pFrame->pSurf, WgRect( pFrame->ofs.x, pFrame->ofs.y, pAnim->width(), pAnim->height() ), cursX + cursor->ofsX(cursMode), yPos + cursor->ofsY(cursMode) );
				
			}
			else
			{
				int xPos = (int) (dest.x + (dest.w - linewidth) * origo.anchorX()
									- linewidth * origo.hotspotX());
	
				printLine( param, xPos, yPos, p1[i].pText );
			}
			
			yPos += fontheight;
		}
		
		p1 = p2;
		n1 = n2;
	}

}


//____ printText() ____________________________________________________________

void WgGfx::printText( const WgText * pText, const WgRect& dest )
{
	if( !pText || !pText->fonts() || !pText->fonts()->font(WG_MODE_NORMAL) )
		return;

	const WgFontSet * pFontSet = pText->fonts();
	const WgOrigo& origo	= pText->alignment();
	WgTextTool::TextParam param(*pFontSet, pText->style(), pText->color() );
	WgFont * pFont = pFontSet->font(pText->style());

	int glyphheight = pFont->height();
	int textheight = glyphheight * pText->lines();

	int yPos = (int) (dest.y + dest.h * origo.anchorY()
							- textheight * origo.hotspotY());
	
	WgTextLine 	* p1, * p2;
	Uint32				n1, n2;
	pText->getText( p1, n1, p2, n2 );
	
	for( int i = 0 ; i < (int) n1 ; i++ )
	{
		int linewidth = WgTextTool::lineWidth( param, p1[i].pText );
		int xPos = (int) (dest.x + dest.w * origo.anchorX()
								- linewidth * origo.hotspotX());

		printLine( param, xPos, yPos, p1[i].pText );
		yPos += glyphheight;
	}		

	for( int i = 0 ; i < (int) n2 ; i++ )
	{
		int linewidth = WgTextTool::lineWidth( param, p2[i].pText );
		int xPos = (int) (dest.x + (dest.w - linewidth) * origo.anchorX()
								- linewidth * origo.hotspotX());

		printLine( param, xPos, yPos, p2[i].pText );
		yPos += glyphheight;
	}		
}


//____ clipPrintTextWithCursor() ______________________________________________

void WgGfx::clipPrintTextWithCursor( const WgRect& clip, const WgText * pText, 
									const WgCursorInstance& ci, const WgRect& dest )
{
	if( !pText || !pText->fonts() || !pText->fonts()->font(WG_MODE_NORMAL) )
		return;

	const WgFontSet * pFontSet = pText->fonts();
	const WgOrigo& origo = pText->alignment();
	WgTextTool::TextParam param(*pFontSet, pText->style(), pText->color() );
	WgFont * pFont = pFontSet->font(pText->style());

	int fontheight = pFont->height();
	int textheight = fontheight * pText->lines();

	int yPos = (int) (dest.y + dest.h * origo.anchorY()
							- textheight * origo.hotspotY());
	
	WgTextLine 	* p1, * p2;
	Uint32 			n1, n2;
	pText->getText( p1, n1, p2, n2 );
	
	for( int j = 0 ; j < 2 ; j++ )
	{	
		for( int i = 0 ; i < (int) n1 ; i++ )
		{
			int linewidth = WgTextTool::lineWidth( param, p1[i].pText );
			if( ci.line() == i )
			{
				WgCursor *		cursor		= pText->fonts()->cursor();

				WgCursor::Mode	cursMode 	= ci.mode();
				int				cursSpacing = cursor->spacing(cursMode);
				int				cursCol 	= ci.column();
				
				linewidth += cursSpacing;
				int xPos = (int) (dest.x + dest.w * origo.anchorX()
									- linewidth * origo.hotspotX());
				int 			cursX 		= xPos + WgTextTool::lineWidthPart( param, p1[i].pText, cursCol );
					
				clipPrintLine( clip, param, xPos, yPos, p1[i].pText, cursCol );
				clipPrintLine( clip, param, cursX + cursSpacing, yPos, p1[i].pText + cursCol  );
				
				WgGfxAnim * pAnim	= cursor->anim(cursMode);
				WgGfxFrame * pFrame = pAnim->getFrame( ci.time(), 0 );
				
				clipBlit( clip, pFrame->pSurf, WgRect( pFrame->ofs.x, pFrame->ofs.y, pAnim->width(), pAnim->height() ), cursX + cursor->ofsX(cursMode), yPos + cursor->ofsY(cursMode) );
				
			}
			else
			{
				int xPos = (int) (dest.x + (dest.w - linewidth) * origo.anchorX()
									- linewidth * origo.hotspotX());

				// printLine without clipping if we can, otherwise use clipPrintLine()

				if( clip.x <= xPos && clip.y <= yPos 
					&& clip.x + clip.w >= xPos+linewidth
					&& clip.y + clip.h >= yPos+fontheight )
					printLine( param, xPos, yPos, p1[i].pText );
				else
					clipPrintLine( clip, param, xPos, yPos, p1[i].pText );
			}
			
			yPos += fontheight;
		}
		
		p1 = p2;
		n1 = n2;
	}
}


//____ clipPrintText() ________________________________________________________

void WgGfx::clipPrintText( const WgRect& clip, const WgText * pText, const WgRect& dest )
{
	if( !pText || !pText->fonts() || !pText->fonts()->font(WG_MODE_NORMAL) )
		return;

	const WgFontSet * pFontSet = pText->fonts();
	const WgOrigo& origo	= pText->alignment();
	WgTextTool::TextParam param(*pFontSet, pText->style(), pText->color() );
	WgFont * pFont = pFontSet->font(pText->style());

	int glyphheight = pFont->height();
	int textheight = glyphheight * pText->lines();

	int yPos = (int) (dest.y + dest.h * origo.anchorY()
							- textheight * origo.hotspotY());
	
	WgTextLine 	* p1, * p2;
	Uint32				n1, n2;
	pText->getText( p1, n1, p2, n2 );
	
	for( int j = 0 ; j < 2 ; j++ )
	{	
		for( int i = 0 ; i < (int) n1 ; i++ )
		{
			int linewidth = WgTextTool::lineWidth( param, p1[i].pText );
			int xPos = (int) (dest.x + dest.w * origo.anchorX()
									- linewidth * origo.hotspotX());
			
			// printLine without clipping if we can, otherwise use clipPrintLine()

			if( clip.x <= xPos && clip.y <= yPos 
				&& clip.x + clip.w >= xPos+linewidth
				&& clip.y + clip.h >= yPos+glyphheight )
				printLine( param, xPos, yPos, p1[i].pText );
			else
				clipPrintLine( clip, param, xPos, yPos, p1[i].pText );

			yPos += glyphheight;
		}
		p1 = p2;
		n1 = n2;
	}
}


//____ printLine() ____________________________________________________________

void WgGfx::printLine( WgTextTool::TextParam& _param, int _x, int _y, const Uint16 * _pLine, Uint32 nChars )
{
	if( !_pLine || !_param.pFont || !_param.pFont->hasGlyphs() )
		return;

//	EColor oldCol = m_tintColor;
//	m_tintColor.Set( _param.color.r/255.f, _param.color.g/255.f, _param.color.b/255.f, _param.color.a/255.f );
//	m_tintColor.Set( (_param.color.r*oldCol.r)/255.f, (_param.color.g*oldCol.g)/255.f, (_param.color.b*oldCol.b)/255.f, (_param.color.a*oldCol.a)/255.f );

	const WgFont * pFont = _param.pFont;

 	for( int i = 0 ; _pLine[i] != 0 && i < (int) nChars ; i++ )
 	{
 		if( _pLine[i] == 32 )
			_x += pFont->whitespace();
		else if( _pLine[i] >= 0x80 && _pLine[i] <= 0x9F )
		{
			i += WgTextTool::execTextCmd( _param, &_pLine[i] );
			pFont = _param.pFont;
//			m_tintColor.Set( (_param.color.r*oldCol.r)/255.f, (_param.color.g*oldCol.g)/255.f, (_param.color.b*oldCol.b)/255.f, (_param.color.a*oldCol.a)/255.f );
//			m_tintColor.Set( _param.color.r/255.f, _param.color.g/255.f, _param.color.b/255.f, _param.color.a/255.f );
		}
		else
		{
			const WgGlyph * pGlyph = pFont->glyph( _pLine[i] );
 			WgGfx::blit( pFont->surface(), pGlyph->rect, _x, _y );
			_x += pGlyph->spacing;
		}
 	}

//	m_tintColor = oldCol;
}



//____ clipPrintLine() ________________________________________________________

void WgGfx::clipPrintLine( const WgRect& clip, WgTextTool::TextParam& _param, int _x, int _y, const Uint16 * _pLine, Uint32 nChars )
{
	if( !_pLine ||  !_param.pFont || !_param.pFont->hasGlyphs() )
		return;

//	EColor oldCol = m_tintColor;
//	m_tintColor.Set( _param.color.r/255.f, _param.color.g/255.f, _param.color.b/255.f, _param.color.a/255.f );
//	m_tintColor.Set( (_param.color.r*oldCol.r)/255.f, (_param.color.g*oldCol.g)/255.f, (_param.color.b*oldCol.b)/255.f, (_param.color.a*oldCol.a)/255.f );

	const WgFont * pFont = _param.pFont;

 	for( int i = 0 ; _pLine[i] != 0 && i < (int) nChars ; i++ )
 	{
 		if( _pLine[i] == 32 )
			_x += pFont->whitespace();
		else if( _pLine[i] >= 0x80 && _pLine[i] <= 0x9F )
		{
			i += WgTextTool::execTextCmd( _param, &_pLine[i] );
			pFont = _param.pFont;
//			m_tintColor.Set( (_param.color.r*oldCol.r)/255.f, (_param.color.g*oldCol.g)/255.f, (_param.color.b*oldCol.b)/255.f, (_param.color.a*oldCol.a)/255.f );
//			m_tintColor.Set( _param.color.r/255.f, _param.color.g/255.f, _param.color.b/255.f, _param.color.a/255.f );
		}
		else
		{
			const WgGlyph * pGlyph = pFont->glyph( _pLine[i] );
 			WgGfx::clipBlit( clip, pFont->surface(), pGlyph->rect, _x, _y );
			_x += pGlyph->spacing;
		}
 	}

//	m_tintColor = oldCol;
}

