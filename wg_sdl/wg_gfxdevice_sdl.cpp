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

#include <SDL.h>
#include <wg_gfxdevice_sdl.h>
#include <wg_surface_sdl.h>

#include <memory.h>
#include <stdio.h>

typedef struct tColorRGBA {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} tColorRGBA;

int SDL_SoftStretchModified( SDL_Surface *src , SDL_Rect *aSrcRect , SDL_Surface *dst , SDL_Rect *aDstRect, WgBlendMode blendMode );

//____ Constructor _____________________________________________________________

WgGfxDeviceSDL::WgGfxDeviceSDL( WgSurfaceSDL * pCanvas)
{
	SetCanvas( pCanvas );
}

//____ Destructor ______________________________________________________________

WgGfxDeviceSDL::~WgGfxDeviceSDL()
{
}

//____ SetCanvas() __________________________________________________________________

void WgGfxDeviceSDL::SetCanvas( WgSurfaceSDL * pCanvas )
{
	m_pCanvas 		= pCanvas;

	if( pCanvas )
	{
	m_canvasHeight 	= pCanvas->GetHeight();
	m_canvasWidth 	= pCanvas->GetWidth();
	}
	else
	{
		m_canvasHeight 	= 0;
		m_canvasWidth 	= 0;
	}
}


void WgGfxDeviceSDL::SetTintColor( WgColor color )
{
}

bool WgGfxDeviceSDL::SetBlendMode( WgBlendMode blendMode )
{
	WgGfxDevice::SetBlendMode(blendMode);
	return true;
}

//____ Fill() __________________________________________________________________

void WgGfxDeviceSDL::Fill( const WgRect& _rect, const WgColor& _col )
{
	if( m_pCanvas == 0 || _col.a == 0 || _rect.w < 1 || _rect.h < 1 )
		return;	


	SDL_Surface * pSurf = m_pCanvas->SDL_Surf();

	if( _col.a == 255 )
	{
	   	Uint32 col = SDL_MapRGB(pSurf->format, _col.r, _col.g, _col.b);

		SDL_Rect	rect;
		rect.x = _rect.x;
		rect.y = _rect.y;
		rect.w = _rect.w;
		rect.h = _rect.h;
		
     
     	Sint32 a = SDL_FillRect( pSurf, &rect, col );

	  	if( a == -1 )
   			printf( "Error: %s\n", SDL_GetError() );
	}
	else
	{
//    	if ( SDL_MUSTLOCK(pSurf) )
      	if ( SDL_LockSurface(pSurf) < 0 )
        	return;																								// Couldn't lock surface.

		SDL_PixelFormat * pForm = pSurf->format;

		// Create Conversion table

		static  Uint32 convTab[3][256];
		static	Uint8	tableR, tableG, tableB, tableA = 255;
		float		r, g, b;
			
		// Create conversion table if not the same as previous.

		if( tableR != _col.r || tableG != _col.g || tableB != _col.b || tableA != _col.a )
		{
			tableR = _col.r;
			tableG = _col.g;
			tableB = _col.b;
			tableA = _col.a;

			r = (_col.r >> pForm->Rloss) * _col.a/255.f;
			g = (_col.g >> pForm->Gloss) * _col.a/255.f;
			b = (_col.b >> pForm->Bloss) * _col.a/255.f;

			Sint32 minLoss = pForm->Rloss;
			if( pForm->Gloss < minLoss )
				minLoss = pForm->Gloss;
			if( pForm->Bloss < minLoss )
				minLoss = pForm->Bloss;

			int	entries = 256 >> minLoss;

			if( pSurf->format->BytesPerPixel == 3 )
			{
				for( int i = 0 ; i < entries ; i++ )				// Takes for granted we have RGB and not BGR order...
				{
					float	j = i * (1.f - _col.a/255.f);
					convTab[0][i] = (Uint32) (j+r);
					convTab[1][i] = (Uint32) (j+g);
					convTab[2][i] = (Uint32) (j+b);
				}
			}
			else
			{
				for( int i = 0 ; i < entries ; i++ )
				{
					float	j = i * (1.f - _col.a/255.f);
					convTab[0][i] = ((Uint32)(j+r)) << pForm->Rshift;
					convTab[1][i] = ((Uint32)(j+g)) << pForm->Gshift;
					convTab[2][i] = ((Uint32)(j+b)) << pForm->Bshift;
				}
			}
		}

		// Do the blitting

		switch( pSurf->format->BytesPerPixel )
		{
			case	2:
				for( int y = _rect.y ; y < _rect.y + _rect.h ; y++ )
				{
					Uint16	* pDest = ((Uint16 *)pSurf->pixels)+pSurf->pitch*y/2 + _rect.x;
					for( int x = 0 ; x < _rect.w ; x++ )
					{
						Uint32	org = (Uint32) * pDest;
						Uint32	dest = convTab[0][(org & pForm->Rmask)>>pForm->Rshift];
						dest += convTab[1][(org & pForm->Gmask)>>pForm->Gshift];
						dest += convTab[2][(org & pForm->Bmask)>>pForm->Bshift];
						* pDest++ = (Uint16) dest;
					}
				}
				break;
			case	3:
				for( int y = _rect.y ; y < _rect.y + _rect.h ; y++ )
				{
					Uint8	* pDest = ((Uint8 *)pSurf->pixels)+pSurf->pitch + _rect.x*3;
					for( int x = 0 ; x < _rect.w ; x++ )
					{
						pDest[0] = convTab[0][pDest[0]];
						pDest[1] = convTab[1][pDest[1]];
						pDest[2] = convTab[2][pDest[2]];
						pDest += 3;
					}
				}
				break;
			case	4:
				for( int y = _rect.y ; y < _rect.y + _rect.h ; y++ )
				{
					Uint32	* pDest = ((Uint32 *)pSurf->pixels)+pSurf->pitch*y/4 + _rect.x;
					for( int x = 0 ; x < _rect.w ; x++ )
					{
						Uint32	org = * pDest;
						Uint32	dest = convTab[0][(org & pForm->Rmask)>>pForm->Rshift];
						dest += convTab[1][(org & pForm->Gmask)>>pForm->Gshift];
						dest += convTab[2][(org & pForm->Bmask)>>pForm->Bshift];
						* pDest++ = dest;
					}
				}
				break;
			default:																			// Unsupported pixelsize.
				if ( SDL_MUSTLOCK(pSurf) )
			SDL_UnlockSurface(pSurf);
				return;
		}

//		if ( SDL_MUSTLOCK(pSurf) )
		SDL_UnlockSurface(pSurf);
	}

	return;
}

//____ Blit() __________________________________________________________________

void WgGfxDeviceSDL::Blit( const WgSurface* pSrc, const WgRect& src, Sint32 dx, Sint32 dy  )
{
	if( !m_pCanvas || !pSrc )
		return;

	SDL_Rect	drect;
	SDL_Rect	srect;
	
	srect.x = src.x;
	srect.y = src.y;
	srect.w = src.w;
	srect.h = src.h;
	
	drect.x = dx;
	drect.y = dy;
	drect.w = src.w;
	drect.h = src.h;

	SDL_BlitSurface( ((const WgSurfaceSDL*)pSrc)->SDL_Surf(), &srect, m_pCanvas->SDL_Surf(), &drect  );
}

//____ StretchBlitSubPixel() ___________________________________________________

void WgGfxDeviceSDL::StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
								   		  float dx, float dy, float dw, float dh )
{
	if( !m_pCanvas || !pSrc )
		return;

	SDL_Rect	drect;
	SDL_Rect	srect;
	
	srect.x = (int)sx;
	srect.y = (int)sy;
	srect.w = (int)sw;
	srect.h = (int)sh;
	
	drect.x = (int)dx;
	drect.y = (int)dy;
	drect.w = (int)dw;
	drect.h = (int)dh;

	if(sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0)
		return;

	SDL_SoftStretchModified( ((const WgSurfaceSDL*)pSrc)->SDL_Surf(), &srect, m_pCanvas->SDL_Surf(), &drect, GetBlendMode() );
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
int SDL_SoftStretchModified( SDL_Surface *src , SDL_Rect *aSrcRect , SDL_Surface *dst , SDL_Rect *aDstRect, WgBlendMode blendMode )
{
	SDL_Rect srcRect = { 0 , 0 , src->w , src->h };
	SDL_Rect dstRect = { 0 , 0 , dst->w , dst->h };
	
	if( aSrcRect )
		srcRect = *aSrcRect;
	if( aDstRect )
		dstRect = *aDstRect;

	if( srcRect.w == dstRect.w && srcRect.h == dstRect.h )
		return SDL_BlitSurface( src , &srcRect , dst , &dstRect );

	if(SDL_LockSurface( src ) < 0 )
		return -1;

	if( SDL_LockSurface( dst ) < 0 )
		return -1;

    // Calc deltas

	int	*dxLUT = new int[dstRect.w + 1];
	int	*dyLUT = new int[dstRect.h + 1];

	int	dx = (int) (65536.f * (float) srcRect.w / (float) dstRect.w);
	int	dy = (int) (65536.f * (float) srcRect.h / (float) dstRect.h);

	int y, x;
	int	ex, ey, t1, t2, sstep;
	tColorRGBA *c00, *c01, *c10, *c11;
	tColorRGBA col;

	dxLUT[0] = 0;
    for(x = 1; x <= dstRect.w ; x++)
		dxLUT[x] = (dxLUT[x - 1] & 0xffff) + dx;

	dyLUT[0] = 0;
	for(y = 1; y <= dstRect.h; y++)
		dyLUT[y] = (dyLUT[y - 1] & 0xffff) + dy;

    // Pointer setup
	tColorRGBA *csp = (tColorRGBA *) src->pixels;
    tColorRGBA *pDst = (tColorRGBA *) dst->pixels;
    int	dstSkip = dst->pitch - dstRect.w * 4;
    int	srcSkip = src->pitch - srcRect.w * 4;

	csp = (tColorRGBA *) ((Uint8 *) csp + src->pitch * srcRect.y);
	csp += srcRect.x;
	pDst = (tColorRGBA *) ((Uint8 *)pDst + dst->pitch * dstRect.y);
	pDst += dstRect.x;
	int* csay = dyLUT;

	for( y = 0 ; y < dstRect.h ; y++ )
	{
		c00 = csp;
		c01 = csp;
		c01++;
		c10 = (tColorRGBA *) ((Uint8 *) csp + src->pitch);
		c11 = c10;
		c11++;
		int* csax = dxLUT;
		for( x = 0 ; x < dstRect.w ; x++ )  
		{
			// Bilinear filtering
			ex = (*csax & 0xffff);
			ey = (*csay & 0xffff);
			t1 = ((((c01->r - c00->r) * ex) >> 16) + c00->r) & 0xff;
			t2 = ((((c11->r - c10->r) * ex) >> 16) + c10->r) & 0xff;

			col.b = (((t2 - t1) * ey) >> 16) + t1;						// MOD: swapped r/b 
			t1 = ((((c01->g - c00->g) * ex) >> 16) + c00->g) & 0xff;
			t2 = ((((c11->g - c10->g) * ex) >> 16) + c10->g) & 0xff;
			col.g = (((t2 - t1) * ey) >> 16) + t1;
			t1 = ((((c01->b - c00->b) * ex) >> 16) + c00->b) & 0xff;
			t2 = ((((c11->b - c10->b) * ex) >> 16) + c10->b) & 0xff;
			col.r = (((t2 - t1) * ey) >> 16) + t1;
			t1 = ((((c01->a - c00->a) * ex) >> 16) + c00->a) & 0xff;
			t2 = ((((c11->a - c10->a) * ex) >> 16) + c10->a) & 0xff;
			col.a = (((t2 - t1) * ey) >> 16) + t1;

			// MOD: added blend modes. crazy inefficient
			switch(blendMode)
			{
			case WG_BLENDMODE_OPAQUE:
				pDst->r = col.r;
				pDst->g = col.g;
				pDst->b = col.b;
				pDst->a = col.a;
			break;
			case WG_BLENDMODE_BLEND:
				// ignore tint for now
				pDst->r = (col.a * (col.r - pDst->r) >> 8) + pDst->r;
				pDst->g = (col.a * (col.g - pDst->g) >> 8) + pDst->g;
				pDst->b = (col.a * (col.b - pDst->b) >> 8) + pDst->b;
				pDst->a = (col.a * (col.a - pDst->a) >> 8) + pDst->a;
			break;
			case WG_BLENDMODE_ADD:
				pDst->r = MIN(255, pDst->r + col.r);
				pDst->g = MIN(255, pDst->g + col.g);
				pDst->b = MIN(255, pDst->b + col.b);
				pDst->a = MIN(255, pDst->a + col.a);
			break;
			case WG_BLENDMODE_MULTIPLY:
				pDst->r = (pDst->r * col.r) >> 8;
				pDst->g = (pDst->g * col.g) >> 8;
				pDst->b = (pDst->b * col.b) >> 8;
				pDst->a = (pDst->a * col.a) >> 8;
			break;
			}

			csax++;
			sstep = (*csax >> 16);
			c00 += sstep;
			c01 += sstep;
			c10 += sstep;
			c11 += sstep;
			
			pDst++;
		}
		csay++;
		csp = (tColorRGBA *) ((Uint8 *) csp + (*csay >> 16) * src->pitch);
		
		pDst = (tColorRGBA *) ((Uint8 *) pDst + dstSkip);
	}
	delete [] dxLUT;
	delete [] dyLUT;

	SDL_SetAlpha( dst , SDL_SRCALPHA , 255 );
	
	SDL_UnlockSurface( src );
	SDL_UnlockSurface( dst );

	return 0;
}


