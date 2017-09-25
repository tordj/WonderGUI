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

#include <wg_softgfxdevice.h>
#include <wg_util.h>
#include <math.h>
#include <algorithm>
#include <wg_base.h>

#include <assert.h>

using namespace std;

namespace wg 
{
	using namespace Util;
	
	const char SoftGfxDevice::CLASSNAME[] = {"SoftGfxDevice"};
	
	//____ create() _______________________________________________________________
	
	SoftGfxDevice_p SoftGfxDevice::create()
	{
		return SoftGfxDevice_p(new SoftGfxDevice());
	}
	
	SoftGfxDevice_p SoftGfxDevice::create( SoftSurface * pCanvas )
	{
		return SoftGfxDevice_p(new SoftGfxDevice(pCanvas));
	}
	
	
	//____ Constructor _____________________________________________________________
	
	SoftGfxDevice::SoftGfxDevice() : GfxDevice(Size(0,0))
	{
		m_pCanvas = 0;
		m_pCanvasPixels = 0;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		_initTables();
		_genCurveTab();
	}
	
	SoftGfxDevice::SoftGfxDevice( SoftSurface * pCanvas ) : GfxDevice( pCanvas?pCanvas->size():Size() )
	{
		m_pCanvas = pCanvas;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		_initTables();
		_genCurveTab();
	}
	
	//____ Destructor ______________________________________________________________
	
	SoftGfxDevice::~SoftGfxDevice()
	{
		delete [] m_pDivTab;
		delete [] m_pCurveTab;
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool SoftGfxDevice::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return GfxDevice::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * SoftGfxDevice::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	SoftGfxDevice_p SoftGfxDevice::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return SoftGfxDevice_p( static_cast<SoftGfxDevice*>(pObject) );
	
		return 0;
	}
	
	//____ surfaceClassName() _______________________________________________________
	
	const char * SoftGfxDevice::surfaceClassName( void ) const
	{
		return SoftSurface::CLASSNAME;
	}
	
	//____ surfaceClassName() _______________________________________________________

	SurfaceFactory_p SoftGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = SoftSurfaceFactory::create();
	
		return m_pSurfaceFactory;
	}
	
	//____ setCanvas() _______________________________________________________________
	
	bool SoftGfxDevice::setCanvas( Surface * pCanvas )
	{
		if( (pCanvas->pixelFormat()->type != PixelType::BGRA_8) && (pCanvas->pixelFormat()->type != PixelType::BGR_8) )
			return false;
		
		
		m_pCanvas = pCanvas;
		if( pCanvas )
			m_canvasSize = pCanvas->size();
		else
			m_canvasSize = Size();
		
		return true;
	}

	//____ beginRender() _______________________________________________________

	bool SoftGfxDevice::beginRender()
	{
		if( !m_pCanvas)
			return false;
			
		m_pCanvasPixels = m_pCanvas->lock(AccessMode::ReadWrite);
		m_canvasPixelBits = m_pCanvas->pixelFormat()->bits;
		m_canvasPitch = m_pCanvas->pitch();
		
		if( !m_pCanvasPixels )
			return false;
			
		return true;	
	}

	//____ endRender() _________________________________________________________
	
	bool SoftGfxDevice::endRender()
	{
		if( !m_pCanvasPixels )
			return false;

		m_pCanvas->unlock();
		m_pCanvasPixels = 0;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		return true;
	}


	//____ fill() ____________________________________________________________________
	
	void SoftGfxDevice::fill( const Rect& rect, const Color& col )
	{
		if( !m_pCanvas || !m_pCanvasPixels )
			return;
	
		Color fillColor = col * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;
	
		// Optimize calls
	
		BlendMode blendMode = m_blendMode;
		if( blendMode == BlendMode::Blend && fillColor.a == 255 )
			blendMode = BlendMode::Replace;
        
		//
	
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * pDst = m_pCanvasPixels + rect.y *m_canvasPitch + rect.x * pixelBytes;
	

		switch( blendMode )
		{
			case BlendMode::Replace:
			{

                int dstPixelBytes = m_canvasPixelBits/8;

                if( dstPixelBytes == 4 )
                {
                    for( int y = 0 ; y < rect.h ; y++ )
                    {
                        uint32_t fillValue = ((int)fillColor.b) | (((int)fillColor.g) << 8) | (((int)fillColor.r) << 16);
                        
                        for( int x = 0 ; x < rect.w*pixelBytes ; x+=pixelBytes )
                        {
                            * ((uint32_t*)(&pDst[x])) = fillValue;
                        }
                        pDst +=m_canvasPitch;
                    }
                }
                else
                {
                    for( int y = 0 ; y < rect.h ; y++ )
                    {
                        for( int x = 0 ; x < rect.w*pixelBytes ; x+=pixelBytes )
                        {
                            pDst[x] = fillColor.b;
                            pDst[x+1] = fillColor.g;
                            pDst[x+2] = fillColor.r;
                        }
                        pDst +=m_canvasPitch;
                    }
                }
                break;
            }
            case BlendMode::Blend:
			{
				int storedRed = ((int)fillColor.r) * fillColor.a;
				int storedGreen = ((int)fillColor.g) * fillColor.a;
				int storedBlue = ((int)fillColor.b) * fillColor.a;
				int invAlpha = 255-fillColor.a;
	
				for( int y = 0 ; y < rect.h ; y++ )
				{
					for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
					{
						pDst[x] = m_pDivTab[pDst[x]*invAlpha + storedBlue];
						pDst[x+1] = m_pDivTab[pDst[x+1]*invAlpha + storedGreen];
						pDst[x+2] = m_pDivTab[pDst[x+2]*invAlpha + storedRed];
					}
					pDst +=m_canvasPitch;
				}
				break;
			}
			case BlendMode::Add:
			{
                int storedRed = (int) m_pDivTab[fillColor.r * fillColor.a];
				int storedGreen = (int) m_pDivTab[fillColor.g * fillColor.a];
				int storedBlue = (int) m_pDivTab[fillColor.b * fillColor.a];
                
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				for( int y = 0 ; y < rect.h ; y++ )
				{
					for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
					{
						pDst[x] = limitUint8(pDst[x] + storedBlue);
						pDst[x+1] = limitUint8(pDst[x+1] + storedGreen);
						pDst[x+2] = limitUint8(pDst[x+2] + storedRed);
					}
					pDst +=m_canvasPitch;
				}
				break;
			}
			case BlendMode::Subtract:
			{
				int storedRed = (int) m_pDivTab[fillColor.r * fillColor.a];
				int storedGreen = (int) m_pDivTab[fillColor.g * fillColor.a];
				int storedBlue = (int) m_pDivTab[fillColor.b * fillColor.a];
	
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				for( int y = 0 ; y < rect.h ; y++ )
				{
					for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
					{
						pDst[x] = limitUint8(pDst[x] - storedBlue);
						pDst[x+1] = limitUint8(pDst[x+1] - storedGreen);
						pDst[x+2] = limitUint8(pDst[x+2] - storedRed);
					}
					pDst +=m_canvasPitch;
				}
				break;
			}
			case BlendMode::Multiply:
			{
				int storedRed = (int)fillColor.r;
				int storedGreen = (int)fillColor.g;
				int storedBlue = (int)fillColor.b;

                if( storedRed + storedGreen + storedBlue == 768 )
                    return;
                
				for( int y = 0 ; y < rect.h ; y++ )
				{
					for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
					{
						pDst[x] = m_pDivTab[pDst[x] * storedBlue];
						pDst[x+1] = m_pDivTab[pDst[x+1] * storedGreen];
						pDst[x+2] = m_pDivTab[pDst[x+2] * storedRed];
					}
					pDst +=m_canvasPitch;
				}
				break;
			}
			case BlendMode::Invert:
			{
				int storedRed = (int)fillColor.r;
				int storedGreen = (int)fillColor.g;
				int storedBlue = (int)fillColor.b;
	
                if( storedRed + storedGreen + storedBlue == 0 )
                    return;
                
				int invertRed = 255 - (int)fillColor.r;
				int invertGreen = 255 - (int)fillColor.g;
				int invertBlue = 255 - (int)fillColor.b;
	
	
				for( int y = 0 ; y < rect.h ; y++ )
				{
					for( int x = 0 ; x < rect.w*pixelBytes ; x+= pixelBytes )
					{
						pDst[x] = m_pDivTab[(255-pDst[x]) * storedBlue + pDst[x] * invertBlue];
						pDst[x+1] = m_pDivTab[(255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen];
						pDst[x+2] = m_pDivTab[(255-pDst[x+2]) * storedRed + pDst[x+2] * invertRed];
					}
					pDst +=m_canvasPitch;
				}
				break;
			}
			default:
				break;
		}
    }
	
	//____ fillSubPixel() ____________________________________________________________________
	
	void SoftGfxDevice::fillSubPixel( const RectF& rect, const Color& col )
	{
		if( !m_pCanvas || !m_pCanvasPixels )
			return;
	
		Color fillColor = col * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;
	
		// Fill all but anti-aliased edges
	
		int x1 = (int) (rect.x + 0.999f);
		int y1 = (int) (rect.y + 0.999f);
		int x2 = (int) (rect.x + rect.w);
		int y2 = (int) (rect.y + rect.h);
	
		fill( Rect( x1,y1,x2-x1,y2-y1 ), col );
	
		// Optimize calls
	
		BlendMode blendMode = m_blendMode;
		if( blendMode == BlendMode::Blend && fillColor.a == 255 )
			blendMode = BlendMode::Replace;
	
		// Draw the sides
	
		int aaLeft = (256 - (int)(rect.x * 256)) & 0xFF;
		int aaTop = (256 - (int)(rect.y * 256)) & 0xFF;
		int aaRight = ((int)((rect.x + rect.w) * 256)) & 0xFF;
		int aaBottom = ((int)((rect.y + rect.h) * 256)) & 0xFF;
	
		if( aaTop != 0 )
			_drawHorrVertLineAA( x1, (int) rect.y, x2-x1, fillColor, blendMode, aaTop, Orientation::Horizontal );
	
		if( aaBottom != 0 )
			_drawHorrVertLineAA( x1, (int) y2, x2-x1, fillColor, blendMode, aaBottom, Orientation::Horizontal );
	
		if( aaLeft != 0 )
			_drawHorrVertLineAA( (int) rect.x, y1, y2-y1, fillColor, blendMode, aaLeft, Orientation::Vertical );
	
		if( aaRight != 0 )
			_drawHorrVertLineAA( (int) x2, y1, y2-y1, fillColor, blendMode, aaRight, Orientation::Vertical );
	
		// Draw corner pieces
	
		int aaTopLeft = aaTop * aaLeft / 256;
		int aaTopRight = aaTop * aaRight / 256;
		int aaBottomLeft = aaBottom * aaLeft / 256;
		int aaBottomRight = aaBottom * aaRight / 256;
	
		if( aaTopLeft != 0 )
			_plotAA( (int) rect.x, (int) rect.y, fillColor, blendMode, aaTopLeft );
	
		if( aaTopRight != 0 )
			_plotAA( x2, (int) rect.y, fillColor, blendMode, aaTopRight );
	
		if( aaBottomLeft != 0 )
			_plotAA( (int) rect.x, y2, fillColor, blendMode, aaBottomLeft );
	
		if( aaBottomRight != 0 )
			_plotAA( x2, y2, fillColor, blendMode, aaBottomRight );
	}
	
	//____ drawLine() ______________________________________________________________

	void SoftGfxDevice::drawLine( Coord beg, Coord end, Color color, float thickness )
	{
		if( !m_pCanvas || !m_pCanvasPixels )
			return;
	
		Color fillColor = color * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;


		uint8_t *	pRow;
		int		rowInc, pixelInc;
		int 	length, width;
		int		pos, slope;

		if( abs(beg.x-end.x) > abs(beg.y-end.y) )
		{
			// Prepare mainly horizontal line segment
			
			if( beg.x > end.x )
				swap( beg, end );
			
			length = end.x - beg.x;
			slope = ((end.y - beg.y) << 16) / length;

			width = _scaleLineThickness( thickness, slope );
			pos = (beg.y << 16) - width/2;		
					
			rowInc = m_canvasPixelBits/8;
			pixelInc =m_canvasPitch;

			pRow = m_pCanvasPixels + beg.x * rowInc;
		}
		else
		{
			// Prepare mainly vertical line segment
			
			if( beg.y > end.y )
				swap( beg, end );
			
			length = end.y - beg.y;
			if( length == 0 )
				return;											// TODO: Should stil draw the caps!

			slope = ((end.x - beg.x) << 16) / length;
			width = _scaleLineThickness( thickness, slope );
			pos = (beg.x << 16) - width/2;		
					
			rowInc =m_canvasPitch;
			pixelInc = m_canvasPixelBits/8;

			pRow = m_pCanvasPixels + beg.y * rowInc;		
		}

		_drawLineSegment( pRow, rowInc, pixelInc, length, width, pos, slope, fillColor );
	}

	//____ clipDrawLine() __________________________________________________________

	void SoftGfxDevice::clipDrawLine( const Rect& clip, Coord beg, Coord end, Color color, float thickness )
	{
		if( !m_pCanvas || !m_pCanvasPixels )
			return;
	
		Color fillColor = color * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;
		
		
		uint8_t *	pRow;
		int		rowInc, pixelInc;
		int 	length, width;
		int		pos, slope;
		int		clipStart, clipEnd;

		if( abs(beg.x-end.x) > abs(beg.y-end.y) )
		{
			// Prepare mainly horizontal line segment
			
			if( beg.x > end.x )
				swap( beg, end );
			
			length = end.x - beg.x;
			slope = ((end.y - beg.y) << 16) / length;

			width = _scaleLineThickness( thickness, slope );
			pos = (beg.y << 16) - width/2;		
					
			rowInc = m_canvasPixelBits/8;
			pixelInc =m_canvasPitch;

			pRow = m_pCanvasPixels + beg.x * rowInc;

			// Do clipping for line segment
			
			if( beg.x > clip.x + clip.w || end.x < clip.x )
				return;										// Segement not visible.
				
			if( beg.x < clip.x )
			{
				int cut = clip.x - beg.x;
				length -= cut;
				pRow += rowInc*cut;
				pos += slope*cut;
			}

			if( end.x > clip.x + clip.w )
				length -= end.x - (clip.x+clip.w);

			clipStart = clip.y << 16;
			clipEnd = (clip.y + clip.h) <<16;
		}
		else
		{
			// Prepare mainly vertical line segment
			
			if( beg.y > end.y )
				swap( beg, end );
			
			length = end.y - beg.y;
			if( length == 0 )
				return;											// TODO: Should stil draw the caps!

			slope = ((end.x - beg.x) << 16) / length;
			width = _scaleLineThickness( thickness, slope );
			pos = (beg.x << 16) - width/2;		
					
			rowInc =m_canvasPitch;
			pixelInc = m_canvasPixelBits/8;

			pRow = m_pCanvasPixels + beg.y * rowInc;		

			// Do clipping for line segment
			
			if( beg.y > clip.y + clip.h || end.y < clip.y )
				return;										// Segement not visible.
				
			if( beg.y < clip.y )
			{
				int cut = clip.y - beg.y;
				length -= cut;
				pRow += rowInc*cut;
				pos += slope*cut;
			}

			if( end.y > clip.y + clip.h )
				length -= end.y - (clip.y+clip.h);
				
			clipStart = clip.x << 16;
			clipEnd = (clip.x + clip.w) <<16;
		}

		_clipDrawLineSegment( clipStart, clipEnd, pRow, rowInc, pixelInc, length, width, pos, slope, fillColor );
	}

	//____ _drawLineSegment() ______________________________________________________

	void SoftGfxDevice::_drawLineSegment( uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color )
	{
		//TODO: Translate to use m_pDivTab

		for( int i = 0 ; i < length ; i++ )
		{
			
			int beg = pos >> 16;
			int end = (pos + width) >> 16;
			int ofs = beg;
			
			if( beg == end )
			{
				// Special case, one pixel wide row
				
				int alpha = (color.a * width) >> 16;

				int invAlpha = 255 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = m_pDivTab[pDst[0]*invAlpha + color.b*alpha];
				pDst[1] = m_pDivTab[pDst[1]*invAlpha + color.g*alpha];
				pDst[2] = m_pDivTab[pDst[2]*invAlpha + color.r*alpha];			
			}
			else
			{
				// First anti-aliased pixel of column
				
				int alpha = (color.a * (65536 - (pos & 0xFFFF))) >> 16;
				
				int invAlpha = 255 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = m_pDivTab[pDst[0]*invAlpha + color.b*alpha];
				pDst[1] = m_pDivTab[pDst[1]*invAlpha + color.g*alpha];
				pDst[2] = m_pDivTab[pDst[2]*invAlpha + color.r*alpha];			
				ofs++;
				
				// All non-antialiased middle pixels of column
				
				
				if( ofs < end )
				{					
					alpha = color.a;	
					invAlpha = 255 - alpha;

					int storedRed = color.r * alpha;
					int storedGreen = color.g * alpha;
					int storedBlue = color.b * alpha;

					do 
					{
						pDst = pRow + ofs*pixelInc;						
						pDst[0] = m_pDivTab[pDst[0]*invAlpha + storedBlue];
						pDst[1] = m_pDivTab[pDst[1]*invAlpha + storedGreen];
						pDst[2] = m_pDivTab[pDst[2]*invAlpha + storedRed];			
						ofs++;
						
					} while( ofs < end );
				}

				// Last anti-aliased pixel of column

				int overflow = (pos+width) & 0xFFFF;
				if( overflow > 0 )
				{
					alpha = (color.a * overflow) >> 16;
					
					invAlpha = 255 - alpha;
					pDst = pRow + ofs*pixelInc;
					
					pDst[0] = m_pDivTab[pDst[0]*invAlpha + color.b*alpha];
					pDst[1] = m_pDivTab[pDst[1]*invAlpha + color.g*alpha];
					pDst[2] = m_pDivTab[pDst[2]*invAlpha + color.r*alpha];			
				}
			}
			
			pRow += rowInc;
			pos += slope;
		}
	}

	//____ _clipDrawLineSegment() ______________________________________________________

	void SoftGfxDevice::_clipDrawLineSegment( int clipStart, int clipEnd, uint8_t * pRow, int rowInc, int pixelInc, int length, int width, int pos, int slope, Color color )
	{
		//TODO: Translate to use m_pDivTab

		for( int i = 0 ; i < length ; i++ )
		{
			
			if( pos >= clipEnd || pos + width <= clipStart )
			{
				pRow += rowInc;
				pos += slope;
				continue;
			}
			
			int clippedPos = pos;
			int clippedWidth = width;
			
			if( clippedPos < clipStart )
			{
				clippedWidth -= clipStart - clippedPos;
				clippedPos = clipStart;
			}
			
			if( clippedPos + clippedWidth > clipEnd )
				clippedWidth = clipEnd - clippedPos;
			
			
			int beg = clippedPos >> 16;
			int end = (clippedPos + clippedWidth) >> 16;
			int ofs = beg;
			
			if( beg == end )
			{
				// Special case, one pixel wide row
				
				int alpha = (color.a * clippedWidth) >> 16;

				int invAlpha = 255 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = m_pDivTab[pDst[0]*invAlpha + color.b*alpha];
				pDst[1] = m_pDivTab[pDst[1]*invAlpha + color.g*alpha];
				pDst[2] = m_pDivTab[pDst[2]*invAlpha + color.r*alpha];			
			}
			else
			{
				// First anti-aliased pixel of column
				
				int alpha = (color.a * (65536 - (clippedPos & 0xFFFF))) >> 16;
				
				int invAlpha = 255 - alpha;
				uint8_t * pDst = pRow + ofs*pixelInc;
				
				pDst[0] = m_pDivTab[pDst[0]*invAlpha + color.b*alpha];
				pDst[1] = m_pDivTab[pDst[1]*invAlpha + color.g*alpha];
				pDst[2] = m_pDivTab[pDst[2]*invAlpha + color.r*alpha];			
				ofs++;
				
				// All non-antialiased middle pixels of column
				
				
				if( ofs < end )
				{					
					alpha = color.a;	
					invAlpha = 255 - alpha;

					int storedRed = color.r * alpha;
					int storedGreen = color.g * alpha;
					int storedBlue = color.b * alpha;

					do 
					{
						pDst = pRow + ofs*pixelInc;						
						pDst[0] = m_pDivTab[pDst[0]*invAlpha + storedBlue];
						pDst[1] = m_pDivTab[pDst[1]*invAlpha + storedGreen];
						pDst[2] = m_pDivTab[pDst[2]*invAlpha + storedRed];			
						ofs++;
						
					} while( ofs < end );
				}

				// Last anti-aliased pixel of column

				int overflow = (clippedPos+clippedWidth) & 0xFFFF;
				if( overflow > 0 )
				{
					alpha = (color.a * overflow) >> 16;
					invAlpha = 255 - alpha;
					pDst = pRow + ofs*pixelInc;
				
					pDst[0] = m_pDivTab[pDst[0]*invAlpha + color.b*alpha];
					pDst[1] = m_pDivTab[pDst[1]*invAlpha + color.g*alpha];
					pDst[2] = m_pDivTab[pDst[2]*invAlpha + color.r*alpha];			
				}
			}
			
			pRow += rowInc;
			pos += slope;
		}
	}
	 
	//____ _traceLine() __________________________________________________________

	void SoftGfxDevice::_traceLine(int * pDest, int * pSrc, int nPoints, float thickness)
	{
		static int brush[128];
		static float prevThickness = -1.f;

		int brushSteps = (int)(thickness / 2 + 0.99f);

		// Generate brush

		if (thickness != prevThickness)
		{
			int scaledThickness = (int)(thickness / 2 * 256);
			for (int i = 0; i < brushSteps; i++)
			{
				brush[i] = (scaledThickness * m_pCurveTab[c_nCurveTabEntries-(i*c_nCurveTabEntries)/brushSteps-1]) >> 16;
//				printf( "%d - %d - %d\n", i, brush[i], m_pCurveTab[(c_nCurveTabEntries - 1) - (i * c_nCurveTabEntries) / brushSteps]);
			}
		}

		// Trace...

		for (int i = 0; i < nPoints; i++)
		{
			// Start with top and bottom for current point

			int top = pSrc[i] - brush[0];
			int bottom = pSrc[i] + brush[0];

			// Check brush's coverage from previous points

			int end = min(i + 1, brushSteps);

			for (int j = 1; j < end ; j++)
			{
				int topCover = pSrc[i - j] - brush[j];
				int bottomCover = pSrc[i - j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Check brush's coverage from following points

			end = min(nPoints-i, brushSteps);

			for (int j = 1; j < end ; j++)
			{
				int topCover = pSrc[i + j] - brush[j];
				int bottomCover = pSrc[i + j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Save traced values

			*pDest++ = top;
			*pDest++ = bottom;
		}
	}

	//____ clipDrawHorrShape() _____________________________________________________

	void SoftGfxDevice::clipDrawHorrShape(const Rect&clip, Coord begin, int length, const WaveLine& topLine, const WaveLine& bottomLine, Color frontColor, Color backColor)
	{
		if (!m_pCanvas || !m_pCanvasPixels)
			return;

		if (topLine.length <= length || bottomLine.length <= length)
			length = min(topLine.length, bottomLine.length) - 1;

		// Do early rough X-clipping with margin (need to trace lines with margin of thickest line).

		int ofs = 0;
		if (clip.x > begin.x || clip.x + clip.w < begin.x + length)
		{
			int margin = (int)(max(topLine.thickness, bottomLine.thickness) / 2 + 0.99);

			if (clip.x > begin.x + margin )
			{
				ofs = clip.x - begin.x - margin;
				begin.x += ofs;
				length -= ofs;
			}

			if (begin.x + length - margin > clip.x + clip.w)
				length = clip.x + clip.w - begin.x + margin;

			if (length <= 0)
				return;
		}

		// Generate line traces

		int	bufferSize = (length+1) * 2 * sizeof(int) *2;	// length+1 * values per point * sizeof(int) * 2 separate traces.
		char * pBuffer = Base::memStackAlloc(bufferSize);
		int * pTopLineTrace = (int*)pBuffer;
		int * pBottomLineTrace = (int*) (pBuffer + bufferSize/2);

		_traceLine(pTopLineTrace, topLine.pWave+ofs, length+1, topLine.thickness);
		_traceLine(pBottomLineTrace, bottomLine.pWave+ofs, length+1, bottomLine.thickness);

		// Do proper X-clipping

		int startColumn = 0;
		if (begin.x < clip.x)
		{
			startColumn = clip.x - begin.x;
			length -= startColumn;
			begin.x += startColumn;
		}

		if (begin.x + length > clip.x + clip.w)
			length = clip.x + clip.w - begin.x;

		// Render columns

		uint8_t * pColumn = m_pCanvasPixels + begin.y * m_canvasPitch + begin.x * (m_canvasPixelBits / 8);
		int pos[2][4];						// Startpositions for the 4 fields of the column (topline, fill, bottomline, line end) for left and right edge of pixel column. 16 binals.

		int clipBeg = clip.y - begin.y;
		int clipLen = clip.h;

		Color	col[4];
		col[0] = topLine.color;
		col[1] = frontColor;
		col[2] = bottomLine.color;
		col[3] = backColor;


		for (int i = startColumn; i <= length; i++)
		{
			// Old right pos becomes new left pos and old left pos will be reused for new right pos

			int * pLeftPos = pos[i % 2];
			int * pRightPos = pos[(i + 1) % 2];

			// Check if lines have intersected and in that case swap top and bottom lines and colors

			if (pTopLineTrace[i * 2] > pBottomLineTrace[i * 2])
			{
				swap(col[0], col[2]);
				swap(col[1], col[3]);
				swap(pTopLineTrace, pBottomLineTrace);

				// We need to regenerate leftpos since we now have swapped top and bottom line.

				if (i > startColumn)
				{
					int j = i - 1;
					pLeftPos[0] = pTopLineTrace[j * 2] << 8;
					pLeftPos[1] = pTopLineTrace[j * 2 + 1] << 8;

					pLeftPos[2] = pBottomLineTrace[j * 2] << 8;
					pLeftPos[3] = pBottomLineTrace[j * 2 + 1] << 8;

					if (pLeftPos[2] < pLeftPos[1])
					{
						pLeftPos[2] = pLeftPos[1];
						if (pLeftPos[3] < pLeftPos[2])
							pLeftPos[3] = pLeftPos[2];
					}
				}
			}

			// Generate new rightpos table

			pRightPos[0] = pTopLineTrace[i * 2] << 8;
			pRightPos[1] = pTopLineTrace[i * 2 + 1] << 8;

			pRightPos[2] = pBottomLineTrace[i * 2] << 8;
			pRightPos[3] = pBottomLineTrace[i * 2 + 1] << 8;


			if (pRightPos[2] < pRightPos[1])
			{
				pRightPos[2] = pRightPos[1];
				if (pRightPos[3] < pRightPos[2])
					pRightPos[3] = pRightPos[2];
			}

			// Render the column

			if (i > startColumn)
			{
				_clipDrawShapeColumn(clipBeg, clipLen, pColumn, pLeftPos, pRightPos, col, m_canvasPitch);
				pColumn += m_canvasPixelBits / 8;
			}
		}

		// Free temporary work memory

		Base::memStackRelease(bufferSize);
	}


	//_____ _clipDrawShapeColumn() ________________________________________________

	void SoftGfxDevice::_clipDrawShapeColumn(int clipBeg, int clipLen, uint8_t * pColumn, int leftPos[4], int rightPos[4], Color col[3], int linePitch)
	{
		// 16 binals on leftPos, rightPos and most calculations.

		int i = 0;

		int amount[4];
		int inc[4];

		int columnBeg = (min(leftPos[0], rightPos[0]) & 0xFFFF0000) + 32768;		// Column starts in middle of first pixel

		// Calculate start amount and increment for our 4 fields

		for (int i = 0; i < 4; i++)
		{
			int yBeg;
			int64_t xInc;

			if (leftPos[i] < rightPos[i])
			{
				yBeg = leftPos[i];
				xInc = (int64_t) 65536*65536 / (rightPos[i] - leftPos[i]+1);
			}
			else
			{
				yBeg = rightPos[i];
				xInc = (int64_t) 65536*65536 / (leftPos[i] - rightPos[i]+1);
			}

			limit(xInc, (int64_t) 0, (int64_t) 65536);

			inc[i] = (int) xInc;

			int64_t startAmount = -((xInc * (yBeg-columnBeg)) >> 16);
			amount[i] = (int)startAmount;
		}

		// Do clipping

		if (columnBeg < (clipBeg<<16))
		{
			int64_t forwardAmount = (clipBeg<<16) - columnBeg;

			for (int i = 0; i < 4; i++)
				amount[i] += (inc[i]*forwardAmount) >> 16;

			columnBeg = (clipBeg<<16);
		}

		uint8_t * pDstClip = pColumn + (clipBeg + clipLen) * linePitch;

		// Render the column

		uint8_t * pDst = pColumn + linePitch * (columnBeg>>16);

		switch (m_blendMode)
		{
			case BlendMode::Blend:
			{
				// First render loop, run until we are fully into fill (or later section).
				// This needs to cover all possibilities since topLine, fill and bottomLine might be less than 1 pixel combined
				// in which case they should all be rendered.

				while (amount[1] < 65536 && pDst < pDstClip)
				{
					int aFrac = amount[0];
					int bFrac = amount[1];
					int cFrac = amount[2];
					int dFrac = amount[3];
					limit(aFrac, 0, 65536);
					limit(bFrac, 0, 65536);
					limit(cFrac, 0, 65536);
					limit(dFrac, 0, 65536);

					aFrac = ((aFrac - bFrac)*col[0].a) / 255;
					bFrac = ((bFrac - cFrac)*col[1].a) / 255;
					cFrac = ((cFrac - dFrac)*col[2].a) / 255;

					int backFraction = 65536 - aFrac - bFrac - cFrac;

					pDst[0] = (pDst[0] * backFraction + col[0].b * aFrac + col[1].b * bFrac + col[2].b * cFrac) >> 16;
					pDst[1] = (pDst[1] * backFraction + col[0].g * aFrac + col[1].g * bFrac + col[2].g * cFrac) >> 16;
					pDst[2] = (pDst[2] * backFraction + col[0].r * aFrac + col[1].r * bFrac + col[2].r * cFrac) >> 16;
					pDst += linePitch;

					for (int i = 0; i < 4; i++)
						amount[i] += inc[i];
				}

				// Second render loop, optimzed fill-section loop until bottomLine starts to fade in.

				if(amount[2] <= 0 && pDst < pDstClip)
				{
					if (col[1].a == 255)
					{
						while (amount[2] <= 0 && pDst < pDstClip)
						{
							pDst[0] = col[1].b;
							pDst[1] = col[1].g;
							pDst[2] = col[1].r;
							pDst += linePitch;

							amount[2] += inc[2];
							amount[3] += inc[3];
						}
					}
					else
					{
						int fillFrac = (65536 * col[1].a) / 255;

						int fillB = col[1].b * fillFrac;
						int fillG = col[1].g * fillFrac;
						int fillR = col[1].r * fillFrac;
						int backFraction = 65536 - fillFrac;

						while (amount[2] <= 0 && pDst < pDstClip)
						{
							pDst[0] = (pDst[0] * backFraction + fillB) >> 16;
							pDst[1] = (pDst[1] * backFraction + fillG) >> 16;
							pDst[2] = (pDst[2] * backFraction + fillR) >> 16;
							pDst += linePitch;

							amount[2] += inc[2];
							amount[3] += inc[3];
						}
					}
				}


				// Third render loop, from when bottom line has started to fade in.
				// We can safely ignore topLine (not visible anymore) and amount[2] is guaranteed to have reached 65536.

				while (amount[3] < 65536 && pDst < pDstClip)
				{
					int cFrac = amount[2];
					int dFrac = amount[3];
					limit(cFrac, 0, 65536);
					limit(dFrac, 0, 65536);

					int bFrac = ((65536 - cFrac)*col[1].a) / 255;
					cFrac = ((cFrac - dFrac)*col[2].a) / 255;

					int backFraction = 65536 - bFrac - cFrac;

					pDst[0] = (pDst[0] * backFraction + col[1].b * bFrac + col[2].b * cFrac) >> 16;
					pDst[1] = (pDst[1] * backFraction + col[1].g * bFrac + col[2].g * cFrac) >> 16;
					pDst[2] = (pDst[2] * backFraction + col[1].r * bFrac + col[2].r * cFrac) >> 16;
					pDst += linePitch;

					amount[2] += inc[2];
					amount[3] += inc[3];
				}
				break;
			}

			case BlendMode::Add:
				break;

			case BlendMode::Subtract:
				break;

			case BlendMode::Invert:
				break;

			case BlendMode::Multiply:
				break;

			case BlendMode::Replace:
				break;

			default:
				break;

		}
	}


	
	//____ clipDrawHorrLine() _____________________________________________________
	
	void SoftGfxDevice::clipDrawHorrLine( const Rect& clip, const Coord& start, int length, const Color& col )
	{
		if( start.y < clip.y || start.y >= clip.y + clip.h || start.x >= clip.x + clip.w || start.x + length <= clip.x )
			return;
	
		int x = start.x;
	
		if( x < clip.x )
		{
			length = start.x + length - clip.x;
			x = clip.x;
		}
	
		if( x + length > clip.x + clip.w )
			length = clip.x + clip.w - x;
	
		_drawHorrVertLine( x, start.y, length, col, Orientation::Horizontal );
	}
	
	//____ clipDrawVertLine() _____________________________________________________
	
	void SoftGfxDevice::clipDrawVertLine( const Rect& clip, const Coord& start, int length, const Color& col )
	{
		if( start.x < clip.x || start.x >= clip.x + clip.w || start.y >= clip.y + clip.h || start.y + length <= clip.y )
			return;
	
		int y = start.y;
	
		if( y < clip.y )
		{
			length = start.y + length - clip.y;
			y = clip.y;
		}
	
		if( y + length > clip.y + clip.h )
			length = clip.y + clip.h - y;
	
		_drawHorrVertLine( start.x, y, length, col, Orientation::Vertical );
	}
	
	//____ clipPlotPixels() ____________________________________________________
	
	void SoftGfxDevice::clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * colors)
	{
		const int pitch =m_canvasPitch;
		const int pixelBytes = m_canvasPixelBits/8;

		for( int i = 0 ; i < nCoords ; i++ )
		{
			const int x = pCoords[i].x;
			const int y = pCoords[i].y;

			uint8_t * pDst = m_pCanvasPixels + y * pitch + x * pixelBytes;

			if( y >= clip.y && y <= clip.y + clip.h -1 && x >= clip.x && x <= clip.x + clip.w -1 )
			{
                const int alpha = colors[i].a;
                const int invAlpha = 255-alpha;

                pDst[0] = (uint8_t) ((pDst[0]*invAlpha + (int)colors[i].b*alpha) >> 8);
                pDst[1] = (uint8_t) ((pDst[1]*invAlpha + (int)colors[i].g*alpha) >> 8);
                pDst[2] = (uint8_t) ((pDst[2]*invAlpha + (int)colors[i].r*alpha) >> 8);
			}
		}
	}

    //____ plotPixels() ____________________________________________________
    
    void SoftGfxDevice::plotPixels( int nCoords, const Coord * pCoords, const Color * colors)
    {
        const int pitch =m_canvasPitch;
        const int pixelBytes = m_canvasPixelBits/8;
        
        for( int i = 0 ; i < nCoords ; i++ )
        {
            const int x = pCoords[i].x;
            const int y = pCoords[i].y;
            
            uint8_t * pDst = m_pCanvasPixels + y * pitch + x * pixelBytes;
            
            const int alpha = colors[i].a;
            const int invAlpha = 255-alpha;
                
            pDst[0] = (uint8_t) ((pDst[0]*invAlpha + (int)colors[i].b*alpha) >> 8);
            pDst[1] = (uint8_t) ((pDst[1]*invAlpha + (int)colors[i].g*alpha) >> 8);
            pDst[2] = (uint8_t) ((pDst[2]*invAlpha + (int)colors[i].r*alpha) >> 8);
        }
    }
	
	
	//____ _drawHorrVertLine() ________________________________________________
	
	void SoftGfxDevice::_drawHorrVertLine( int _x, int _y, int _length, const Color& _col, Orientation orientation  )
	{
		if( !m_pCanvas || !m_pCanvasPixels || _length <= 0  )
			return;
	
		Color fillColor = _col * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add) )
			return;
	
		// Optimize calls
	
		BlendMode blendMode = m_blendMode;
		if( blendMode == BlendMode::Blend && fillColor.a == 255 )
			blendMode = BlendMode::Replace;
	
		//
	
		int pitch =m_canvasPitch;
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * pDst = m_pCanvasPixels + _y *m_canvasPitch + _x * pixelBytes;
	
		int inc;
	
		if( orientation == Orientation::Horizontal )
			inc = pixelBytes;
		else
			inc = pitch;
	
		//
	
		switch( blendMode )
		{
			case BlendMode::Replace:
			{
				for( int x = 0 ; x < _length*inc ; x+=inc )
				{
					pDst[x] = fillColor.b;
					pDst[x+1] = fillColor.g;
					pDst[x+2] = fillColor.r;
				}
			}
			break;
			case BlendMode::Blend:
			{
				int storedRed = ((int)fillColor.r) * fillColor.a;
				int storedGreen = ((int)fillColor.g) * fillColor.a;
				int storedBlue = ((int)fillColor.b) * fillColor.a;
				int invAlpha = 255-fillColor.a;
	
				for( int x = 0 ; x < _length*inc ; x+=inc )
				{
					pDst[x] = m_pDivTab[pDst[x]*invAlpha + storedBlue];
					pDst[x+1] = m_pDivTab[pDst[x+1]*invAlpha + storedGreen];
					pDst[x+2] = m_pDivTab[pDst[x+2]*invAlpha + storedRed];
				}
	
				break;
			}
			case BlendMode::Add:
			{
				int storedRed = m_pDivTab[fillColor.r * (int) fillColor.a];
				int storedGreen = m_pDivTab[fillColor.g * (int) fillColor.a];
				int storedBlue = m_pDivTab[fillColor.b * (int) fillColor.a];
	
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = limitUint8(pDst[x] + storedBlue);
					pDst[x+1] = limitUint8(pDst[x+1] + storedGreen);
					pDst[x+2] = limitUint8(pDst[x+2] + storedRed);
				}
				break;
			}
			case BlendMode::Subtract:
			{
				int storedRed = m_pDivTab[fillColor.r * (int) fillColor.a];
				int storedGreen = m_pDivTab[fillColor.g * (int) fillColor.a];
				int storedBlue = m_pDivTab[fillColor.b * (int) fillColor.a];
	
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = limitUint8(pDst[x] - storedBlue);
					pDst[x+1] = limitUint8(pDst[x+1] - storedGreen);
					pDst[x+2] = limitUint8(pDst[x+2] - storedRed);
				}
				break;
			}
			case BlendMode::Multiply:
			{
				int storedRed = (int)fillColor.r;
				int storedGreen = (int)fillColor.g;
				int storedBlue = (int)fillColor.b;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = m_pDivTab[pDst[x] * storedBlue];
					pDst[x+1] = m_pDivTab[pDst[x+1] * storedGreen];
					pDst[x+2] = m_pDivTab[pDst[x+2] * storedRed];
				}
				break;
			}
			case BlendMode::Invert:
			{
				int storedRed = (int)fillColor.r;
				int storedGreen = (int)fillColor.g;
				int storedBlue = (int)fillColor.b;
	
				int invertRed = 255 - (int)fillColor.r;
				int invertGreen = 255 - (int)fillColor.g;
				int invertBlue = 255 - (int)fillColor.b;
	
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = m_pDivTab[(255-pDst[x]) * storedBlue + pDst[x] * invertBlue];
					pDst[x+1] = m_pDivTab[(255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen];
					pDst[x+2] = m_pDivTab[(255-pDst[x+2]) * storedRed + pDst[x+2] * invertRed];
				}
				break;
			}
			default:
				break;
		}
	}
	
	//____ _drawHorrVertLineAA() ________________________________________________
	
	void SoftGfxDevice::_drawHorrVertLineAA( int _x, int _y, int _length, const Color& _col, BlendMode blendMode, int _aa, Orientation orientation )
	{
		int pitch =m_canvasPitch;
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * pDst = m_pCanvasPixels + _y *m_canvasPitch + _x * pixelBytes;
	
		int inc;
		if( orientation == Orientation::Horizontal )
			inc = pixelBytes;
		else
			inc = pitch;
	
		switch( blendMode )
		{
			case BlendMode::Replace:
			{
				int storedRed = ((int)_col.r) * _aa;
				int storedGreen = ((int)_col.g) * _aa;
				int storedBlue = ((int)_col.b) * _aa;
				int invAlpha = 255- _aa;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = m_pDivTab[pDst[x]*invAlpha + storedBlue];
					pDst[x+1] = m_pDivTab[pDst[x+1]*invAlpha + storedGreen];
					pDst[x+2] = m_pDivTab[pDst[x+2]*invAlpha + storedRed];
				}
				break;
			}
			case BlendMode::Blend:
			{
				int aa = m_pDivTab[_col.a * _aa];
				
				int storedRed = _col.r * aa;
				int storedGreen = _col.g * aa;
				int storedBlue = _col.b * aa;
				int invAlpha = 255-aa;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = m_pDivTab[pDst[x]*invAlpha + storedBlue];
					pDst[x+1] = m_pDivTab[pDst[x+1]*invAlpha + storedGreen];
					pDst[x+2] = m_pDivTab[pDst[x+2]*invAlpha + storedRed];
				}
				break;
			}
			case BlendMode::Add:
			{
				int aa = m_pDivTab[_col.a * _aa];
				
				int storedRed = m_pDivTab[_col.r * aa];
				int storedGreen = m_pDivTab[_col.g * aa];
				int storedBlue = m_pDivTab[_col.b * aa];
				
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = limitUint8(pDst[x] + storedBlue);
					pDst[x+1] = limitUint8(pDst[x+1] + storedGreen);
					pDst[x+2] = limitUint8(pDst[x+2] + storedRed);
				}
				break;
			}
			case BlendMode::Subtract:
			{
				int aa = m_pDivTab[_col.a * _aa];
				
				int storedRed = m_pDivTab[_col.r * aa];
				int storedGreen = m_pDivTab[_col.g * aa];
				int storedBlue = m_pDivTab[_col.b * aa];
				
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = limitUint8(pDst[x] - storedBlue);
					pDst[x+1] = limitUint8(pDst[x+1] - storedGreen);
					pDst[x+2] = limitUint8(pDst[x+2] - storedRed);
				}
				break;
			}
			case BlendMode::Multiply:
			{
				int storedRed = (int) m_pDivTab[_col.r*_aa];
				int storedGreen = (int) m_pDivTab[_col.g*_aa];
				int storedBlue = (int) m_pDivTab[_col.b*_aa];
				
				int invAlpha = 255 - _aa;
				
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = m_pDivTab[(pDst[x]*invAlpha) + (pDst[x] * storedBlue)];
					pDst[x+1] = m_pDivTab[(pDst[x+1]*invAlpha) + (pDst[x+1] * storedGreen)];
					pDst[x+2] = m_pDivTab[(pDst[x+2]*invAlpha) + (pDst[x+2] * storedRed)];
				}
				break;
			}
			case BlendMode::Invert:
			{
				//TODO: Translate to use m_pDivTab
	
				int storedRed = (int)_col.r;
				int storedGreen = (int)_col.g;
				int storedBlue = (int)_col.b;
	
				int invertRed = 255 - (int)_col.r;
				int invertGreen = 255 - (int)_col.g;
				int invertBlue = 255 - (int)_col.b;
	
				int invAlpha = (255 - _aa) << 8;
	
				for( int x = 0 ; x < _length*inc ; x+= inc )
				{
					pDst[x] = ( (pDst[x]*invAlpha) + _aa * ((255-pDst[x]) * storedBlue + pDst[x] * invertBlue) ) >> 16;
					pDst[x+1] = ( (pDst[x+1]*invAlpha) + _aa * ((255-pDst[x+1]) * storedGreen + pDst[x+1] * invertGreen) )  >> 16;
					pDst[x+2] = ( (pDst[x+2]*invAlpha) + _aa * ((255-pDst[x+2]) * storedRed + pDst[x+2] * invertRed) ) >> 16;
				}
				break;
			}
			default:
				break;
		}
	}
	
	//____ _plotAA() ________________________________________________
	
	void SoftGfxDevice::_plotAA( int _x, int _y, const Color& _col, BlendMode blendMode, int _aa )
	{
		//TODO: Translate to use m_pDivTab
	
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * pDst = m_pCanvasPixels + _y *m_canvasPitch + _x * pixelBytes;
	
		switch( blendMode )
		{
			case BlendMode::Replace:
			{
				int storedRed = ((int)_col.r) * _aa;
				int storedGreen = ((int)_col.g) * _aa;
				int storedBlue = ((int)_col.b) * _aa;
				int invAlpha = 255- _aa;
	
				pDst[0] = (uint8_t) ((pDst[0]*invAlpha + storedBlue) >> 8);
				pDst[1] = (uint8_t) ((pDst[1]*invAlpha + storedGreen) >> 8);
				pDst[2] = (uint8_t) ((pDst[2]*invAlpha + storedRed) >> 8);
				break;
			}
			case BlendMode::Blend:
			{
				int aa = _col.a * _aa;
	
				int storedRed = (((int)_col.r) * aa) >> 8;
				int storedGreen = (((int)_col.g) * aa) >> 8;
				int storedBlue = (((int)_col.b) * aa) >> 8;
				int invAlpha = 255-(aa>>8);
	
				pDst[0] = (uint8_t) ((pDst[0]*invAlpha + storedBlue) >> 8);
				pDst[1] = (uint8_t) ((pDst[1]*invAlpha + storedGreen) >> 8);
				pDst[2] = (uint8_t) ((pDst[2]*invAlpha + storedRed) >> 8);
				break;
			}
			case BlendMode::Add:
			{
				int aa = _col.a * _aa;
	
				int storedRed = (((int)_col.r) * aa) >> 16;
				int storedGreen = (((int)_col.g) * aa) >> 16;
				int storedBlue = (((int)_col.b) * aa) >> 16;
	
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				pDst[0] = limitUint8(pDst[0] + storedBlue);
				pDst[1] = limitUint8(pDst[1] + storedGreen);
				pDst[2] = limitUint8(pDst[2] + storedRed);
				break;
			}
			case BlendMode::Subtract:
			{
				int aa = _col.a * _aa;
	
				int storedRed = (((int)_col.r) * aa) >> 16;
				int storedGreen = (((int)_col.g) * aa) >> 16;
				int storedBlue = (((int)_col.b) * aa) >> 16;
	
				if( storedRed + storedGreen + storedBlue == 0 )
					return;
	
				pDst[0] = limitUint8(pDst[0] - storedBlue);
				pDst[1] = limitUint8(pDst[1] - storedGreen);
				pDst[2] = limitUint8(pDst[2] - storedRed);
				break;
			}
			case BlendMode::Multiply:
			{
				int storedRed = (int)_col.r;
				int storedGreen = (int)_col.g;
				int storedBlue = (int)_col.b;
	
				int invAlpha = (255 - _aa) << 8;
	
				pDst[0] = ( (pDst[0]*invAlpha) + (_aa * pDst[0] * storedBlue) ) >> 16;
				pDst[1] = ( (pDst[1]*invAlpha) + (_aa * pDst[1] * storedGreen) ) >> 16;
				pDst[2] = ( (pDst[2]*invAlpha) + (_aa * pDst[2] * storedRed) ) >> 16;
				break;
			}
			case BlendMode::Invert:
			{
				int storedRed = (int)_col.r;
				int storedGreen = (int)_col.g;
				int storedBlue = (int)_col.b;
	
				int invertRed = 255 - (int)_col.r;
				int invertGreen = 255 - (int)_col.g;
				int invertBlue = 255 - (int)_col.b;
	
				int invAlpha = (255 - _aa) << 8;
	
				pDst[0] = ( (pDst[0]*invAlpha) + _aa * ((255-pDst[0]) * storedBlue + pDst[0] * invertBlue) ) >> 16;
				pDst[1] = ( (pDst[1]*invAlpha) + _aa * ((255-pDst[1]) * storedGreen + pDst[1] * invertGreen) )  >> 16;
				pDst[2] = ( (pDst[2]*invAlpha) + _aa * ((255-pDst[2]) * storedRed + pDst[2] * invertRed) ) >> 16;
				break;
			}
			default:
				break;
		}
	}
	
	
	//____ _genCurveTab() ___________________________________________________________
	
	void SoftGfxDevice::_genCurveTab()
	{
		m_pCurveTab = new int[c_nCurveTabEntries];
	
//		double factor = 3.14159265 / (2.0 * c_nCurveTabEntries);
	
		for( int i = 0 ; i < c_nCurveTabEntries ; i++ )
		{
			double y = 1.f - i/(double)c_nCurveTabEntries;
			m_pCurveTab[i] = (int) (sqrt(1.f - y*y)*65536.f);
		}
	}
	
	
	//____ drawElipse() _______________________________________________________________
	
	void SoftGfxDevice::drawElipse( const Rect& rect, Color color )
	{
		if( !m_pCanvas || !m_pCanvasPixels || rect.h < 2 || rect.w < 1 )
			return;
	
		Color fillColor = color * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;
		
			
		int sectionHeight = rect.h/2;
		int maxWidth = rect.w/2;
	
		uint8_t * pLineBeg = m_pCanvasPixels + rect.y *m_canvasPitch;
		int pitch =m_canvasPitch;
	
		int center = (rect.x + rect.w/2) << 8;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / sectionHeight;
		int sinOfs = 0;
	
		int begOfs = 0;
		int peakOfs = 0;
		int endOfs = 0;
	
		for( int i = 0 ; i < sectionHeight ; i++ )
		{
			peakOfs = ((m_pCurveTab[sinOfs>>16] * maxWidth) >> 8);
			endOfs = (m_pCurveTab[(sinOfs+(sinOfsInc-1))>>16] * maxWidth) >> 8;
	
			_drawHorrFadeLine( pLineBeg + i*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
			_drawHorrFadeLine( pLineBeg + i*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
	
			_drawHorrFadeLine( pLineBeg + (sectionHeight*2-i-1)*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
			_drawHorrFadeLine( pLineBeg + (sectionHeight*2-i-1)*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
	
			begOfs = peakOfs;
			sinOfs += sinOfsInc;
		}
	}
	
	//____ _clipDrawHorrFadeLine() _______________________________________________________________
	
	void SoftGfxDevice::_clipDrawHorrFadeLine( int clipX1, int clipX2, uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color )
	{
		//TODO: Translate to use m_pDivTab
	
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * p = pLineStart + (begOfs>>8) * pixelBytes;
		uint8_t * pClip1 = pLineStart + clipX1*pixelBytes;
		uint8_t * pClip2 = pLineStart + clipX2*pixelBytes;
	
		int alphaInc, alpha, len;
	
		if( (peakOfs>>8) == (begOfs>>8) )
		{
			alphaInc = 0;
			alpha = (256-(peakOfs&0xff) + (peakOfs-begOfs)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (peakOfs - begOfs);			// alpha inc per pixel with 14 binals.
			alpha = ((256 - (begOfs&0xff)) * alphaInc) >> 9;		// alpha for ramp up start pixel with 14 binals.
			len = ((peakOfs+256) >> 8) - (begOfs >> 8);
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			if( p >= pClip1 && p < pClip2 )
			{
				int invAlpha = (255 << 14) - alpha;
	
				p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
				p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
				p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			}
			alpha += alphaInc;
			if( alpha > 255 << 14 )
				alpha = 255 << 14;
	
			p += pixelBytes;
		}
	
		if( (endOfs>>8) == ((peakOfs + 256)>>8) )
		{
			alphaInc = 0;
			alpha = ((peakOfs&0xff)+(endOfs-peakOfs-256)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (endOfs - (peakOfs+256));						// alpha dec per pixel with 14 binals.
			alpha = (255 << 14) - (((256 - (peakOfs&0xff)) * alphaInc) >> 9);	// alpha for ramp down start pixel with 14 binals.
			len = (endOfs >> 8) - ((peakOfs+256) >> 8);
			alphaInc = -alphaInc;
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			if( p >= pClip1 && p < pClip2 )
			{
				int invAlpha = (255 << 14) - alpha;
	
				p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
				p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
				p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			}
			alpha += alphaInc;
			p += pixelBytes;
		}
	}
	
	
	//____ _drawHorrFadeLine() _______________________________________________________________
	
	void SoftGfxDevice::_drawHorrFadeLine( uint8_t * pLineStart, int begOfs, int peakOfs, int endOfs, Color color )
	{
		//TODO: Translate to use m_pDivTab
	
		int pixelBytes = m_canvasPixelBits/8;
		uint8_t * p = pLineStart + (begOfs>>8) * pixelBytes;
	
		int alphaInc, alpha, len;
	
		if( (peakOfs>>8) == (begOfs>>8) )
		{
			alphaInc = 0;
			alpha = (256-(peakOfs&0xff) + (peakOfs-begOfs)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (peakOfs - begOfs);			// alpha inc per pixel with 14 binals.
			alpha = ((256 - (begOfs&0xff)) * alphaInc) >> 9;		// alpha for ramp up start pixel with 14 binals.
			len = ((peakOfs+256) >> 8) - (begOfs >> 8);
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			int invAlpha = (255 << 14) - alpha;
	
			p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
			p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
			p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			alpha += alphaInc;
			if( alpha > 255 << 14 )
				alpha = 255 << 14;
	
			p += pixelBytes;
		}
	
		if( (endOfs>>8) == ((peakOfs + 256)>>8) )
		{
			alphaInc = 0;
			alpha = ((peakOfs&0xff)+(endOfs-peakOfs-256)/2) << 14;
			len = 1;
		}
		else
		{
			alphaInc = (255 << 22) / (endOfs - (peakOfs+256));						// alpha dec per pixel with 14 binals.
			alpha = (255 << 14) - (((256 - (peakOfs&0xff)) * alphaInc) >> 9);	// alpha for ramp down start pixel with 14 binals.
			len = (endOfs >> 8) - ((peakOfs+256) >> 8);
			alphaInc = -alphaInc;
		}
	
		for( int i = 0 ; i < len ; i++ )
		{
			int invAlpha = (255 << 14) - alpha;
	
			p[0] = ((color.b * alpha) + (p[0]*invAlpha)) >> 22;
			p[1] = ((color.g * alpha) + (p[1]*invAlpha)) >> 22;
			p[2] = ((color.r * alpha) + (p[2]*invAlpha)) >> 22;
			alpha += alphaInc;
			p += pixelBytes;
		}
	
	}
	
	//____ clipDrawElipse() _______________________________________________________________
	
	void SoftGfxDevice::clipDrawElipse( const Rect& clip, const Rect& rect, Color color )
	{
		if( !m_pCanvas || !m_pCanvasPixels || rect.h < 2 || rect.w < 1 )
			return;
	
		Color fillColor = color * m_tintColor;
	
		// Skip calls that won't affect destination
	
		if( fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract) )
			return;

	
		if( !rect.intersectsWith(clip) )
			return;
	
		if( clip.contains(rect) )
			return drawElipse(rect,color);
	
		int sectionHeight = rect.h/2;
		int maxWidth = rect.w/2;
	
		uint8_t * pLineBeg = m_pCanvasPixels + rect.y*m_canvasPitch;
		int pitch =m_canvasPitch;
	
		int center = (rect.x + rect.w/2) << 8;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / sectionHeight;
		int sinOfs = 0;
	
		int begOfs = 0;
		int peakOfs = 0;
		int endOfs = 0;
	
		for( int i = 0 ; i < sectionHeight ; i++ )
		{
			peakOfs = ((m_pCurveTab[sinOfs>>16] * maxWidth) >> 8);
			endOfs = (m_pCurveTab[(sinOfs+(sinOfsInc-1))>>16] * maxWidth) >> 8;
	
			if( rect.y + i >= clip.y && rect.y + i < clip.y + clip.h )
			{
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + i*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + i*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
			}
	
			int y2 = sectionHeight*2-i-1;
			if( rect.y + y2 >= clip.y && rect.y + y2 < clip.y + clip.h )
			{
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + y2*pitch, center + begOfs -256, center + peakOfs -256, center + endOfs, fillColor );
				_clipDrawHorrFadeLine( clip.x, clip.x+clip.w, pLineBeg + y2*pitch, center - endOfs, center - peakOfs, center - begOfs +256, fillColor );
			}
	
			begOfs = peakOfs;
			sinOfs += sinOfsInc;
		}
	}
	
	//____ drawFilledElipse() _____________________________________________________
	
	void SoftGfxDevice::drawFilledElipse( const Rect& rect, Color color )
	{
		int pixelBytes = m_canvasPixelBits/8;
	
		uint8_t * pLineCenter = m_pCanvasPixels + rect.y *m_canvasPitch + (rect.x+rect.w/2) * pixelBytes;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / (rect.h/2);
		int sinOfs = sinOfsInc >> 1;
	
		for( int j = 0 ; j < 2 ; j++ )
		{
			for( int i = 0 ; i < rect.h/2 ; i++ )
			{
				int lineLen = ((m_pCurveTab[sinOfs>>16] * rect.w/2 + 32768)>>16)*pixelBytes;
				uint8_t * pLineBeg = pLineCenter - lineLen;
				uint8_t * pLineEnd = pLineCenter + lineLen;
	
				for( uint8_t * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
				{
					p[0] = color.b;
					p[1] = color.g;
					p[2] = color.r;
				}
	
				sinOfs += sinOfsInc;
				pLineCenter +=m_canvasPitch;
			}
			sinOfsInc = -sinOfsInc;
			sinOfs = (c_nCurveTabEntries << 16) + (sinOfsInc >> 1);
		}
	}
	
	//____ clipDrawFilledElipse() _____________________________________________________
	
	void SoftGfxDevice::clipDrawFilledElipse( const Rect& clip, const Rect& rect, Color color )
	{
		if( !rect.intersectsWith(clip) )
			return;
	
		if( clip.contains(rect) )
			return drawFilledElipse(rect,color);
	
		int pixelBytes = m_canvasPixelBits/8;
	
		uint8_t * pLine = m_pCanvasPixels + rect.y *m_canvasPitch;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / (rect.h/2);
		int sinOfs = sinOfsInc >> 1;
	
		for( int j = 0 ; j < 2 ; j++ )
		{
			for( int i = 0 ; i < rect.h/2 ; i++ )
			{
				if( rect.y + j*(rect.h/2) + i >= clip.y && rect.y + j*(rect.h/2) + i < clip.y + clip.h )
				{
					int lineLen = ((m_pCurveTab[sinOfs>>16] * rect.w/2 + 32768)>>16);
	
					int beg = rect.x + rect.w/2 - lineLen;
					int end = rect.x + rect.w/2 + lineLen;
	
					if( beg < clip.x )
						beg = clip.x;
	
					if( end > clip.x + clip.w )
						end = clip.x + clip.w;
	
					if( beg < end )
					{
						uint8_t * pLineBeg = pLine + beg * pixelBytes;
						uint8_t * pLineEnd = pLine + end * pixelBytes;
	
						for( uint8_t * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
						{
							p[0] = color.b;
							p[1] = color.g;
							p[2] = color.r;
						}
					}
				}
	
				sinOfs += sinOfsInc;
				pLine +=m_canvasPitch;
			}
	
			sinOfsInc = -sinOfsInc;
			sinOfs = (c_nCurveTabEntries << 16) + (sinOfsInc >> 1);
		}
	}
	
	
	//____ drawArcNE() ____________________________________________________________
	
	void SoftGfxDevice::drawArcNE( const Rect& rect, Color color )
	{
		int pixelBytes = m_canvasPixelBits/8;
	
		uint8_t * pLineBeg = m_pCanvasPixels + rect.y *m_canvasPitch + rect.x * pixelBytes;
	
		int sinOfsInc = (c_nCurveTabEntries << 16) / rect.h;
		int sinOfs = sinOfsInc >> 1;
	
		for( int i = 0 ; i < rect.h ; i++ )
		{
			uint8_t * pLineEnd = pLineBeg + ((m_pCurveTab[sinOfs>>16] * rect.w + 32768)>>16)*pixelBytes;
	
			for( uint8_t * p = pLineBeg ; p < pLineEnd ; p += pixelBytes )
			{
				p[0] = color.b;
				p[1] = color.g;
				p[2] = color.r;
			}
	
			sinOfs += sinOfsInc;
			pLineBeg +=m_canvasPitch;
		}
	
	}
	
	//____ clipDrawArcNE() _________________________________________________________
	
	void SoftGfxDevice::clipDrawArcNE( const Rect& clip, const Rect& rect, Color color )
	{
		//TODO: Implement!!!
	}
	
	
	//____ blit() __________________________________________________________________
	
	void SoftGfxDevice::blit( Surface * pSrcSurf, const Rect& srcrect, Coord dest  )
	{
		Surface * pSrc = pSrcSurf;
	
		if( m_tintColor.argb == 0xFFFFFFFF )
			_blit( pSrc, srcrect, dest.x, dest.y );
		else
			_tintBlit( pSrc, srcrect, dest.x, dest.y );
	}
	
	//____ _blit() _____________________________________________________________
	
	void SoftGfxDevice::_blit( const Surface* _pSrcSurf, const Rect& srcrect, int dx, int dy  )
	{
		if( !_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME) )
			return;
	
		SoftSurface * pSrcSurf = (SoftSurface*) _pSrcSurf;
	
		if( !m_pCanvasPixels || !pSrcSurf->m_pData )
			return;
	
		int srcPixelBytes = pSrcSurf->m_pixelFormat.bits/8;
		int dstPixelBytes = m_canvasPixelBits/8;
	
		int	srcPitchAdd = pSrcSurf->m_pitch - srcrect.w*srcPixelBytes;
		int	dstPitchAdd =m_canvasPitch - srcrect.w*dstPixelBytes;
	
		uint8_t * pDst = m_pCanvasPixels + dy *m_canvasPitch + dx * dstPixelBytes;
		uint8_t * pSrc = pSrcSurf->m_pData + srcrect.y * pSrcSurf->m_pitch + srcrect.x * srcPixelBytes;
	
		BlendMode		blendMode = m_blendMode;
		if( srcPixelBytes == 3 && blendMode == BlendMode::Blend )
			blendMode = BlendMode::Replace;
	
		switch( blendMode )
		{
			case BlendMode::Replace:
			{
				if( srcPixelBytes == 4 && dstPixelBytes == 4 )
				{
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							* ((uint32_t*)pDst) = * ((uint32_t*)pSrc) & 0x00FFFFFF;
							pSrc += 4;
							pDst += 4;
						}
	
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				else
				{
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = pSrc[0];
							pDst[1] = pSrc[1];
							pDst[2] = pSrc[2];
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
	
				break;
			}
			case BlendMode::Blend:
			{
				if( srcPixelBytes == 4 )
				{
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = pSrc[3];
							int invAlpha = 255-alpha;
	
							pDst[0] = m_pDivTab[pDst[0]*invAlpha + pSrc[0]*alpha];
							pDst[1] = m_pDivTab[pDst[1]*invAlpha + pSrc[1]*alpha];
							pDst[2] = m_pDivTab[pDst[2]*invAlpha + pSrc[2]*alpha];
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				else
				{
					// Should never get here, skips to blendmode opaque instead.
				}
				break;
			}
			case BlendMode::Add:
			{
				if( srcPixelBytes == 4 )
				{
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = pSrc[3];
	
							pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[pSrc[0]*alpha]);
							pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[pSrc[1]*alpha]);
							pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[pSrc[2]*alpha]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				else
				{
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = limitUint8(pDst[0] + pSrc[0]);
							pDst[1] = limitUint8(pDst[1] + pSrc[1]);
							pDst[2] = limitUint8(pDst[2] + pSrc[2]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				break;
			}
			case BlendMode::Subtract:
			{
				if( srcPixelBytes == 4 )
				{
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = pSrc[3];
	
							pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[pSrc[0]*alpha]);
							pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[pSrc[1]*alpha]);
							pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[pSrc[2]*alpha]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				else
				{
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = limitUint8(pDst[0] - pSrc[0]);
							pDst[1] = limitUint8(pDst[1] - pSrc[1]);
							pDst[2] = limitUint8(pDst[2] - pSrc[2]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				break;
			}

			case BlendMode::Multiply:
			{
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						pDst[0] = m_pDivTab[ pDst[0]*pSrc[0] ];
						pDst[1] = m_pDivTab[ pDst[1]*pSrc[1] ];
						pDst[2] = m_pDivTab[ pDst[2]*pSrc[2] ];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
				break;
			}
			case BlendMode::Invert:
			{
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						pDst[0] = m_pDivTab[pSrc[0]*(255-pDst[0]) + pDst[0]*(255-pSrc[0])];
						pDst[1] = m_pDivTab[pSrc[1]*(255-pDst[1]) + pDst[1]*(255-pSrc[0])];
						pDst[2] = m_pDivTab[pSrc[2]*(255-pDst[2]) + pDst[2]*(255-pSrc[0])];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
				break;
			}
			default:
				break;
		}
	}
	
	
	
	//____ _tintBlit() _____________________________________________________________
	
	void SoftGfxDevice::_tintBlit( const Surface* _pSrcSurf, const Rect& srcrect, int dx, int dy  )
	{
		if( !_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME) )
			return;
	
		SoftSurface * pSrcSurf = (SoftSurface*) _pSrcSurf;
	
		if( !m_pCanvasPixels || !pSrcSurf->m_pData )
			return;
	
		int srcPixelBytes = pSrcSurf->m_pixelFormat.bits/8;
		int dstPixelBytes = m_canvasPixelBits/8;
	
		int	srcPitchAdd = pSrcSurf->m_pitch - srcrect.w*srcPixelBytes;
		int	dstPitchAdd =m_canvasPitch - srcrect.w*dstPixelBytes;
	
		uint8_t * pDst = m_pCanvasPixels + dy *m_canvasPitch + dx * dstPixelBytes;
		uint8_t * pSrc = pSrcSurf->m_pData + srcrect.y * pSrcSurf->m_pitch + srcrect.x * srcPixelBytes;
	
		BlendMode		blendMode = m_blendMode;
		if( srcPixelBytes == 3 && blendMode == BlendMode::Blend && m_tintColor.a == 255 )
			blendMode = BlendMode::Replace;

        
        
		switch( blendMode )
		{
			case BlendMode::Replace:
			{
				int tintRed = (int) m_tintColor.r;
				int tintGreen = (int) m_tintColor.g;
				int tintBlue = (int) m_tintColor.b;
	
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						pDst[0] = m_pDivTab[pSrc[0]*tintBlue];
						pDst[1] = m_pDivTab[pSrc[1]*tintGreen];
						pDst[2] = m_pDivTab[pSrc[2]*tintRed];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
				break;
			}
			case BlendMode::Blend:
			{
                if( m_tintColor.a == 0 )
                    break;
                
                if( srcPixelBytes == 4 )
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_tintColor.r;
					int tintGreen = (int) m_tintColor.g;
					int tintBlue = (int) m_tintColor.b;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = m_pDivTab[pSrc[3]*tintAlpha];
							int invAlpha = 255-alpha;
	
							int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
							int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
							int srcRed		= m_pDivTab[pSrc[2] * tintRed];
							
	
							pDst[0] = m_pDivTab[ pDst[0]*invAlpha + srcBlue*alpha ];
							pDst[1] = m_pDivTab[ pDst[1]*invAlpha + srcGreen*alpha ];
							pDst[2] = m_pDivTab[ pDst[2]*invAlpha + srcRed*alpha ];
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				else
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_pDivTab[ m_tintColor.r * tintAlpha ];
					int tintGreen = (int) m_pDivTab[ m_tintColor.g * tintAlpha ];
					int tintBlue = (int) m_pDivTab[ m_tintColor.b * tintAlpha ];
					int invAlpha = 255-tintAlpha;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = m_pDivTab[ pDst[0]*invAlpha + pSrc[0]*tintBlue ];
							pDst[1] = m_pDivTab[ pDst[1]*invAlpha + pSrc[1]*tintGreen ];
							pDst[2] = m_pDivTab[ pDst[2]*invAlpha + pSrc[2]*tintRed ];
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				break;
			}
			case BlendMode::Add:
			{
                if( m_tintColor.a == 0 )
                    break;
                
                if( srcPixelBytes == 4 )
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_tintColor.r;
					int tintGreen = (int) m_tintColor.g;
					int tintBlue = (int) m_tintColor.b;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = m_pDivTab[ pSrc[3]*tintAlpha ];
	
							int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
							int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
							int srcRed		= m_pDivTab[pSrc[2] * tintRed];
	
							pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[srcBlue*alpha]);
							pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[srcGreen*alpha]);
							pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[ srcRed*alpha]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				else
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_pDivTab[m_tintColor.r * tintAlpha];
					int tintGreen = (int) m_pDivTab[m_tintColor.g * tintAlpha];
					int tintBlue = (int) m_pDivTab[m_tintColor.b * tintAlpha];
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[pSrc[0]*tintBlue]);
							pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[pSrc[1]*tintGreen]);
							pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[pSrc[2]*tintRed]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				break;
			}
			case BlendMode::Subtract:
			{
                if( m_tintColor.a == 0 )
                    break;
                
                if( srcPixelBytes == 4 )
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_tintColor.r;
					int tintGreen = (int) m_tintColor.g;
					int tintBlue = (int) m_tintColor.b;
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							int alpha = m_pDivTab[ pSrc[3]*tintAlpha ];
	
							int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
							int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
							int srcRed		= m_pDivTab[pSrc[2] * tintRed];
	
							pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[srcBlue*alpha]);
							pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[srcGreen*alpha]);
							pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[ srcRed*alpha]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				else
				{
					int tintAlpha = (int) m_tintColor.a;
					int tintRed = (int) m_pDivTab[m_tintColor.r * tintAlpha];
					int tintGreen = (int) m_pDivTab[m_tintColor.g * tintAlpha];
					int tintBlue = (int) m_pDivTab[m_tintColor.b * tintAlpha];
	
					for( int y = 0 ; y < srcrect.h ; y++ )
					{
						for( int x = 0 ; x < srcrect.w ; x++ )
						{
							pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[pSrc[0]*tintBlue]);
							pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[pSrc[1]*tintGreen]);
							pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[pSrc[2]*tintRed]);
							pSrc += srcPixelBytes;
							pDst += dstPixelBytes;
						}
						pSrc += srcPitchAdd;
						pDst += dstPitchAdd;
					}
				}
				break;
			}
			case BlendMode::Multiply:
			{
				int tintRed = (int) m_tintColor.r;
				int tintGreen = (int) m_tintColor.g;
				int tintBlue = (int) m_tintColor.b;
	
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						int srcBlue		= m_pDivTab[pSrc[0] * tintBlue];
						int srcGreen	= m_pDivTab[pSrc[1] * tintGreen];
						int srcRed		= m_pDivTab[pSrc[2] * tintRed];
	
	
						pDst[0] = m_pDivTab[srcBlue*pDst[0]];
						pDst[1] = m_pDivTab[srcGreen*pDst[1]];
						pDst[2] = m_pDivTab[srcRed*pDst[2]];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
				break;
			}
			case BlendMode::Invert:
			{
				int tintRed = (int) m_tintColor.r;
				int tintGreen = (int) m_tintColor.g;
				int tintBlue = (int) m_tintColor.b;
	
				for( int y = 0 ; y < srcrect.h ; y++ )
				{
					for( int x = 0 ; x < srcrect.w ; x++ )
					{
						int srcBlue = m_pDivTab[tintBlue*pSrc[0]];
						int srcGreen = m_pDivTab[tintGreen*pSrc[1]];
						int srcRed = m_pDivTab[tintRed*pSrc[2]];
	
						pDst[0] = m_pDivTab[srcBlue*(255-pDst[0]) + pDst[0]*(255-srcBlue)];
						pDst[1] = m_pDivTab[srcGreen*(255-pDst[1]) + pDst[1]*(255-srcGreen)];
						pDst[2] = m_pDivTab[srcRed*(255-pDst[2]) + pDst[2]*(255-srcRed)];
						pSrc += srcPixelBytes;
						pDst += dstPixelBytes;
					}
					pSrc += srcPitchAdd;
					pDst += dstPitchAdd;
				}
				break;
			}
			default:
				break;
		}
	}
	
	
	//____ stretchBlitSubPixel() ___________________________________________________
	
	void SoftGfxDevice::stretchBlitSubPixel( Surface * _pSrcSurf, float sx, float sy, float sw, float sh,
							   		 float _dx, float _dy, float _dw, float _dh )
	{
		if( !_pSrcSurf || !m_pCanvas || !_pSrcSurf->isInstanceOf(SoftSurface::CLASSNAME) )
			return;
	
		SoftSurface * pSrcSurf = (SoftSurface*) _pSrcSurf;
	
		if( !m_pCanvasPixels || !pSrcSurf->m_pData )
			return;
	
		int dx = (int) _dx;
		int dy = (int) _dy;
		int dw = (int) _dw;
		int dh = (int) _dh;
	
		BlendMode		blendMode = m_blendMode;
		if( pSrcSurf->m_pixelFormat.bits == 24 && blendMode == BlendMode::Blend && m_tintColor.a == 255 )
			blendMode = BlendMode::Replace;
	
		if( m_tintColor == 0xFFFFFFFF )
		{
			switch( blendMode )
			{
				case BlendMode::Replace:
					_stretchBlitOpaque( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Blend:
					if( pSrcSurf->m_pixelFormat.bits == 24 )
						assert(0);							// SHOULD NEVER GET HERE!
					else
						_stretchBlitBlend32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Add:
					if( pSrcSurf->m_pixelFormat.bits == 24 )
						_stretchBlitAdd24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					else
						_stretchBlitAdd32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Subtract:
					if( pSrcSurf->m_pixelFormat.bits == 24 )
						_stretchBlitSub24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					else
						_stretchBlitSub32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Multiply:
					_stretchBlitMultiply( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Invert:
					_stretchBlitInvert( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
                default:        // Ignore should do nothing and we never get Undefined inside here.
                    break;
			}
		}
		else
		{
			switch( blendMode )
			{
				case BlendMode::Replace:
					_stretchBlitTintedOpaque( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Blend:
                    if( m_tintColor.a == 0 )
                        break;
                    
					if( pSrcSurf->m_pixelFormat.bits == 24 )
						_stretchBlitTintedBlend24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					else
						_stretchBlitTintedBlend32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Add:
                    if( m_tintColor.a == 0 )
                        break;

                    if( pSrcSurf->m_pixelFormat.bits == 24 )
						_stretchBlitTintedAdd24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					else
						_stretchBlitTintedAdd32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Subtract:
                    if( m_tintColor.a == 0 )
                        break;

                    if( pSrcSurf->m_pixelFormat.bits == 24 )
						_stretchBlitTintedSub24( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					else
						_stretchBlitTintedSub32( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Multiply:
					_stretchBlitTintedMultiply( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
				case BlendMode::Invert:
					_stretchBlitTintedInvert( pSrcSurf, sx, sy, sw, sh, dx, dy, dw, dh );
					break;
                default:        // Ignore should do nothing and we never get Undefined inside here.
                    break;
			}
		}
	}
	
	
	#define STRETCHBLIT( _bReadAlpha_, _init_, _loop_ )										\
	{																						\
		int srcPixelBytes = pSrcSurf->m_pixelFormat.bits/8;									\
		int dstPixelBytes = m_canvasPixelBits/8;								\
																							\
		int	srcPitch = pSrcSurf->m_pitch;													\
		int	dstPitch =m_canvasPitch;													\
																							\
		_init_																				\
																							\
		if( pSrcSurf->scaleMode() == ScaleMode::Interpolate )									\
		{																					\
			int ofsY = (int) (sy*32768);		/* We use 15 binals for all calculations */	\
			int incY = (int) (sh*32768/dh);													\
																							\
			for( int y = 0 ; y < dh ; y++ )													\
			{																				\
				int fracY2 = ofsY & 0x7FFF;													\
				int fracY1 = 32768 - fracY2;												\
																							\
				int ofsX = (int) (sx*32768);												\
				int incX = (int) (sw*32768/dw);												\
																							\
				uint8_t * pDst = m_pCanvasPixels + (dy+y) * dstPitch + dx * dstPixelBytes;	\
				uint8_t * pSrc = pSrcSurf->m_pData + (ofsY>>15) * srcPitch;					\
																							\
				for( int x = 0 ; x < dw ; x++ )												\
				{																			\
					int fracX2 = ofsX & 0x7FFF;												\
					int fracX1 = 32768 - fracX2;											\
																							\
					uint8_t * p = pSrc + (ofsX >> 15)*srcPixelBytes;						\
																							\
					int mul11 = fracX1*fracY1 >> 15;										\
					int mul12 = fracX2*fracY1 >> 15;										\
					int mul21 = fracX1*fracY2 >> 15;										\
					int mul22 = fracX2*fracY2 >> 15;										\
																							\
					int srcBlue = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15; 	\
					p++;																												\
					int srcGreen = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;	\
					p++;																												\
					int srcRed = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;		\
					int srcAlpha;																										\
					if( _bReadAlpha_ )																									\
					{																													\
						p++;																											\
						srcAlpha = (p[0]*mul11 + p[srcPixelBytes]*mul12 + p[srcPitch]*mul21 + p[srcPitch+srcPixelBytes]*mul22) >> 15;	\
					}																		\
																							\
					_loop_																	\
																							\
					ofsX += incX;															\
					pDst += dstPixelBytes;													\
				}																			\
				ofsY += incY;																\
			}																				\
		}																					\
		else	/* UNFILTERED */															\
		{																					\
			int ofsY = (int) (sy*32768);		/* We use 15 binals for all calculations */	\
			int incY = (int) (sh*32768/dh);													\
																							\
			for( int y = 0 ; y < dh ; y++ )													\
			{																				\
				int ofsX = (int) (sx*32768);												\
				int incX = (int) (sw*32768/dw);												\
																							\
				uint8_t * pDst = m_pCanvasPixels + (dy+y) * dstPitch + dx * dstPixelBytes;	\
				uint8_t * pSrc = pSrcSurf->m_pData + (ofsY>>15) * srcPitch;					\
																							\
				for( int x = 0 ; x < dw ; x++ )												\
				{																			\
					uint8_t * p = pSrc + (ofsX >> 15)*srcPixelBytes;						\
																							\
					int srcBlue = p[0]; 													\
					int srcGreen = p[1];													\
					int srcRed = p[2];														\
					int srcAlpha;															\
					if( _bReadAlpha_ )														\
						srcAlpha = p[3];													\
																							\
					_loop_																	\
																							\
					ofsX += incX;															\
					pDst += dstPixelBytes;													\
				}																			\
				ofsY += incY;																\
			}																				\
		}																					\
	}																						\
	
	
	
	//____ _stretchBlitTintedOpaque() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedOpaque( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		int tintRed = (int) m_tintColor.r;
		int tintGreen = (int) m_tintColor.g;
		int tintBlue = (int) m_tintColor.b;
	
		,
	
		pDst[0] = m_pDivTab[srcBlue*tintBlue];
		pDst[1] = m_pDivTab[srcGreen*tintGreen];
		pDst[2] = m_pDivTab[srcRed*tintRed];
	
		)
	}
	
	//____ _stretchBlitTintedBlend32() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedBlend32( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															 int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( true,
	
		int tintAlpha = (int) m_tintColor.a;
		int tintRed = (int) m_tintColor.r;
		int tintGreen = (int) m_tintColor.g;
		int tintBlue = (int) m_tintColor.b;
	
		,
	
		int alpha = m_pDivTab[srcAlpha*tintAlpha];
		int invAlpha = 255-alpha;
	
		srcBlue = m_pDivTab[srcBlue * tintBlue];
		srcGreen = m_pDivTab[srcGreen * tintGreen];
		srcRed = m_pDivTab[srcRed * tintRed];
	
	
		pDst[0] = m_pDivTab[pDst[0]*invAlpha + srcBlue*alpha];
		pDst[1] = m_pDivTab[pDst[1]*invAlpha + srcGreen*alpha];
		pDst[2] = m_pDivTab[pDst[2]*invAlpha + srcRed*alpha];
	
		)
	}
	
	//____ _stretchBlitTintedBlend24() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedBlend24( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															 int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		int tintAlpha = (int) m_tintColor.a;
		int tintRed = (int) m_pDivTab[m_tintColor.r * tintAlpha];
		int tintGreen = (int) m_pDivTab[m_tintColor.g * tintAlpha];
		int tintBlue = (int) m_pDivTab[m_tintColor.b * tintAlpha];
		int invAlpha = 255-tintAlpha;
	
		,
	
		pDst[0] = m_pDivTab[pDst[0]*invAlpha + srcBlue*tintBlue];
		pDst[1] = m_pDivTab[pDst[1]*invAlpha + srcGreen*tintGreen];
		pDst[2] = m_pDivTab[pDst[2]*invAlpha + srcRed*tintRed];
	
		)
	}
	
	
	//____ _stretchBlitTintedAdd32() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedAdd32( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( true,
	
		int tintAlpha = (int) m_tintColor.a;
		int tintRed = (int) m_tintColor.r;
		int tintGreen = (int) m_tintColor.g;
		int tintBlue = (int) m_tintColor.b;
	
		,
	
		int alpha = m_pDivTab[srcAlpha*tintAlpha];
	
		srcBlue = m_pDivTab[srcBlue * tintBlue];
		srcGreen = m_pDivTab[srcGreen * tintGreen];
		srcRed = m_pDivTab[srcRed * tintRed];
	
		pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[srcBlue*alpha]);
		pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[srcGreen*alpha]);
		pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[srcRed*alpha]);
	
		)
	}
	
	
	//____ _stretchBlitTintedAdd24() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedAdd24( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		int tintAlpha = (int) m_tintColor.a;
		int tintRed = (int) m_pDivTab[m_tintColor.r * tintAlpha];
		int tintGreen = (int) m_pDivTab[m_tintColor.g * tintAlpha];
		int tintBlue = (int) m_pDivTab[m_tintColor.b * tintAlpha];
	
		,
	
		pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[srcBlue*tintBlue]);
		pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[srcGreen*tintGreen]);
		pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[srcRed*tintRed]);
	
		)
	}

	//____ _stretchBlitTintedSub32() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedSub32( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( true,
	
		int tintAlpha = (int) m_tintColor.a;
		int tintRed = (int) m_tintColor.r;
		int tintGreen = (int) m_tintColor.g;
		int tintBlue = (int) m_tintColor.b;
	
		,
	
		int alpha = m_pDivTab[srcAlpha*tintAlpha];
	
		srcBlue = m_pDivTab[srcBlue * tintBlue];
		srcGreen = m_pDivTab[srcGreen * tintGreen];
		srcRed = m_pDivTab[srcRed * tintRed];
	
		pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[srcBlue*alpha]);
		pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[srcGreen*alpha]);
		pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[srcRed*alpha]);
	
		)
	}
	
	
	//____ _stretchBlitTintedSub24() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedSub24( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		int tintAlpha = (int) m_tintColor.a;
		int tintRed = (int) m_pDivTab[m_tintColor.r * tintAlpha];
		int tintGreen = (int) m_pDivTab[m_tintColor.g * tintAlpha];
		int tintBlue = (int) m_pDivTab[m_tintColor.b * tintAlpha];
	
		,
	
		pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[srcBlue*tintBlue]);
		pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[srcGreen*tintGreen]);
		pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[srcRed*tintRed]);
	
		)
	}
	
	
	//____ _stretchBlitTintedMultiply() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedMultiply( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
															  int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		int tintRed = (int) m_tintColor.r;
		int tintGreen = (int) m_tintColor.g;
		int tintBlue = (int) m_tintColor.b;
	
		,
	
		srcBlue = m_pDivTab[srcBlue * tintBlue];
		srcGreen = m_pDivTab[srcGreen * tintGreen];
		srcRed = m_pDivTab[srcRed * tintRed];
	
		pDst[0] = m_pDivTab[pDst[0]*srcBlue];
		pDst[1] = m_pDivTab[pDst[1]*srcGreen];
		pDst[2] = m_pDivTab[pDst[2]*srcRed];
	
		)
	}
	
	//____ _stretchBlitTintedInvert() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitTintedInvert( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
												      int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		int tintRed = (int) m_tintColor.r;
		int tintGreen = (int) m_tintColor.g;
		int tintBlue = (int) m_tintColor.b;
	
		,
	
		srcBlue = m_pDivTab[srcBlue * tintBlue];
		srcGreen = m_pDivTab[srcGreen * tintGreen];
		srcRed = m_pDivTab[srcRed * tintRed];
	
		pDst[0] = m_pDivTab[srcBlue*(255-pDst[0]) + pDst[0]*(255-srcBlue)];
		pDst[1] = m_pDivTab[srcGreen*(255-pDst[1]) + pDst[1]*(255-srcGreen)];
		pDst[2] = m_pDivTab[srcRed*(255-pDst[2]) + pDst[2]*(255-srcRed)];
	
		)
	}
	
	
	//____ _stretchBlitOpaque() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitOpaque( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
													  int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		,
	
		pDst[0] = srcBlue;
		pDst[1] = srcGreen;
		pDst[2] = srcRed;
	
		)
	}
	
	//____ _stretchBlitBlend32() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitBlend32( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
													   int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( true,
	
		,
	
		int invAlpha = 255-srcAlpha;
	
		pDst[0] = m_pDivTab[pDst[0]*invAlpha + srcBlue*srcAlpha];
		pDst[1] = m_pDivTab[pDst[1]*invAlpha + srcGreen*srcAlpha];
		pDst[2] = m_pDivTab[pDst[2]*invAlpha + srcRed*srcAlpha];
	
		)
	}
	
	//____ _stretchBlitAdd32() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitAdd32( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
													 int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( true,
	
		,
	
		pDst[0] = limitUint8(pDst[0] + (int) m_pDivTab[srcBlue*srcAlpha]);
		pDst[1] = limitUint8(pDst[1] + (int) m_pDivTab[srcGreen*srcAlpha]);
		pDst[2] = limitUint8(pDst[2] + (int) m_pDivTab[srcRed*srcAlpha]);
	
		)
	}
	
	
	//____ _stretchBlitAdd24() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitAdd24( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
													 int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		,
	
		pDst[0] = limitUint8(pDst[0] + srcBlue);
		pDst[1] = limitUint8(pDst[1] + srcGreen);
		pDst[2] = limitUint8(pDst[2] + srcRed);
	
		)
	}
	
	//____ _stretchBlitSub32() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitSub32( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
													 int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( true,
	
		,
	
		pDst[0] = limitUint8(pDst[0] - (int) m_pDivTab[srcBlue*srcAlpha]);
		pDst[1] = limitUint8(pDst[1] - (int) m_pDivTab[srcGreen*srcAlpha]);
		pDst[2] = limitUint8(pDst[2] - (int) m_pDivTab[srcRed*srcAlpha]);
	
		)
	}
	
	
	//____ _stretchBlitSub24() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitSub24( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
													 int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		,
	
		pDst[0] = limitUint8(pDst[0] - srcBlue);
		pDst[1] = limitUint8(pDst[1] - srcGreen);
		pDst[2] = limitUint8(pDst[2] - srcRed);
	
		)
	}


	
	//____ _stretchBlitMultiply() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitMultiply( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
													    int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		,
	
		pDst[0] = m_pDivTab[pDst[0]*srcBlue];
		pDst[1] = m_pDivTab[pDst[1]*srcGreen];
		pDst[2] = m_pDivTab[pDst[2]*srcRed];
	
		)
	}
	
	//____ _stretchBlitInvert() ____________________________________________
	
	void SoftGfxDevice::_stretchBlitInvert( const SoftSurface * pSrcSurf, float sx, float sy, float sw, float sh,
												      int dx, int dy, int dw, int dh )
	{
		STRETCHBLIT( false,
	
		,
	
		pDst[0] = m_pDivTab[(srcBlue*(255-pDst[0]) + pDst[0]*(255-srcBlue))];
		pDst[1] = m_pDivTab[(srcGreen*(255-pDst[1]) + pDst[1]*(255-srcGreen))];
		pDst[2] = m_pDivTab[(srcRed*(255-pDst[2]) + pDst[2]*(255-srcRed))];
	
		)
	}
	
	
	//____ _initTables() ___________________________________________________________
	
	void SoftGfxDevice::_initTables()
	{	
		// Init divTable
	
		m_pDivTab = new uint8_t[65536];
	
		for( int i = 0 ; i < 65536 ; i++ )
			m_pDivTab[i] = i / 255;

		// Init lineThicknessTable
		
		for( int i = 0 ; i < 17 ; i++ )
		{
			double b = i/16.0;
			m_lineThicknessTable[i] = (int) (sqrt( 1.0 + b*b ) * 65536);
		}
	}


	//____ _scaleLineThickness() ___________________________________________________
	
	int SoftGfxDevice::_scaleLineThickness( float thickness, int slope )
	{
		slope = abs(slope);
		
		int scale = m_lineThicknessTable[slope>>12];
		
		if( slope < (1 << 16) )
		{
			int scale2 = m_lineThicknessTable[(slope>>12)+1];
			scale += ((scale2-scale)*(slope & 0xFFF)) >> 12;
		}
		
		return (int) (thickness * scale);
	}
	


} // namespace wg
