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

#include <wg_gfxdevice_gl.h>
#include <wg_surface_gl.h>

//____ Constructor _____________________________________________________________

WgGfxDeviceGL::WgGfxDeviceGL( Uint32 width, Uint32 height )
{
	m_bRendering = false;

	SetCanvas( width, height );
}

//____ Destructor ______________________________________________________________

WgGfxDeviceGL::~WgGfxDeviceGL()
{
}

//____ SetCanvas() __________________________________________________________________

void WgGfxDeviceGL::SetCanvas( Uint32 width, Uint32 height )
{
	m_canvasHeight 	= height;
	m_canvasWidth 	= width;
}

//____ SetTintColor() __________________________________________________________

void WgGfxDeviceGL::SetTintColor( WgColor color )
{
	WgGfxDevice::SetTintColor(color);

	if( m_bRendering )
		glColor4f( m_tintColor.r/255.f, m_tintColor.g/255.f, m_tintColor.b/255.f, m_tintColor.a/255.f );
}

//____ SetBlendMode() __________________________________________________________

bool WgGfxDeviceGL::SetBlendMode( WgBlendMode blendMode )
{
	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
			glDisable(GL_BLEND);
			break;

		case WG_BLENDMODE_BLEND:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;

		case WG_BLENDMODE_ADD:
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		}
		case WG_BLENDMODE_MULTIPLY:

			// Not implemented yet.
			break;

		default:
			return false;
	}

	WgGfxDevice::SetBlendMode(blendMode);
	return true;
}

//____ BeginRender() ___________________________________________________________

bool WgGfxDeviceGL::BeginRender()
{
	if( m_bRendering == true )
		return false;

	// Remember GL states so we can restore in EndRender()

	m_glDepthTest 		= glIsEnabled(GL_DEPTH_TEST);
	m_glTexture2D 		= glIsEnabled(GL_TEXTURE_2D);
	m_glBlendEnabled  	= glIsEnabled(GL_BLEND);
	m_glFog				= glIsEnabled(GL_FOG);
	m_glLighting		= glIsEnabled(GL_LIGHTING);

	glGetIntegerv(GL_BLEND_SRC, &m_glBlendSrc);
	glGetIntegerv(GL_BLEND_DST, &m_glBlendDst);
	glGetIntegerv(GL_MATRIX_MODE, &m_glMatrixMode );

	glGetFloatv(GL_CURRENT_COLOR, m_glColor );

	//  Modify states

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
//	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glColor4ub( m_tintColor.r, m_tintColor.g, m_tintColor.b, m_tintColor.a );

	// Set correct blend mode

	switch( m_blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
			glDisable(GL_BLEND);
			break;

		case WG_BLENDMODE_BLEND:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;

		case WG_BLENDMODE_ADD:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;

		case WG_BLENDMODE_MULTIPLY:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
	}

	// Save matrixes and set Ortho mode

	glMatrixMode(GL_PROJECTION);			// Select The Projection Matrix
	glPushMatrix();							// Store The Projection Matrix

	glLoadIdentity();								// Reset The Projection Matrix
	glOrtho(0,m_canvasWidth,0,m_canvasHeight,-1,1);	// Set Up An Ortho Screen

	glMatrixMode(GL_MODELVIEW);				// Select The Modelview Matrix
	glPushMatrix();							// Store The Modelview Matrix
	glLoadIdentity();						// Reset The Modelview Matrix

	//

	m_bRendering = true;
	return true;
}

//____ EndRender() _____________________________________________________________

bool WgGfxDeviceGL::EndRender()
{
	if( m_bRendering == false )
		return false;

	glMatrixMode(GL_PROJECTION);			// Select The Projection Matrix
	glPopMatrix();							// Restore The Old Projection Matrix

	glMatrixMode(GL_MODELVIEW);				// Select The Modelview Matrix
	glPopMatrix();							// Restore The Old Projection Matrix


	if( m_glDepthTest )
		glEnable(GL_DEPTH_TEST);

	if( !m_glTexture2D )
		glDisable(GL_TEXTURE_2D);

	if( m_glFog )
		glEnable(GL_FOG);

	if( m_glLighting )
		glEnable(GL_LIGHTING);

	if( m_glBlendEnabled )
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glMatrixMode( m_glMatrixMode );
	glBlendFunc( m_glBlendSrc, m_glBlendDst );

	glColor4fv( m_glColor );

	m_bRendering = false;
	return true;
}



//____ Fill() __________________________________________________________________

void WgGfxDeviceGL::Fill( const WgRect& _rect, const WgColor& _col )
{
	if( _col.a == 0 || _rect.w < 1 || _rect.h < 1 )
		return;

	glDisable(GL_TEXTURE_2D);
	if( !_col.a )
		glDisable(GL_BLEND);
	else
		glEnable(GL_BLEND);

	int	dx1 = _rect.x;
	int	dy1 = m_canvasHeight - _rect.y;
	int dx2 = _rect.x + _rect.w;
	int dy2 = m_canvasHeight - (_rect.y + _rect.h);

	glColor4ub( _col.r, _col.g, _col.b, _col.a );
	glBegin(GL_QUADS);
		glVertex2i( dx1, dy1 );
		glVertex2i( dx2, dy1 );
		glVertex2i( dx2, dy2 );
		glVertex2i( dx1, dy2 );
	glEnd();
	glColor4ub( m_tintColor.r, m_tintColor.g, m_tintColor.b, m_tintColor.a );
	glEnable(GL_TEXTURE_2D);

	return;
}

//____ Blit() __________________________________________________________________

void WgGfxDeviceGL::Blit( const WgSurface* _pSrc, const WgRect& _src, Sint32 _dx, Sint32 _dy  )
{
	if( !_pSrc )
		return;

	float sw = (float) _pSrc->GetWidth();
	float sh = (float) _pSrc->GetHeight();

	float	sx1 = _src.x/sw;
	float	sx2 = (_src.x+_src.w)/sw;
	float	sy1 = _src.y/sh;
	float	sy2 = (_src.y+_src.h)/sh;

	int		dx1 = _dx;
	int		dx2 = _dx + _src.w;
	int		dy1 = m_canvasHeight - _dy;
	int		dy2 = dy1 - _src.h;

	glBindTexture(GL_TEXTURE_2D, ((WgSurfaceGL*) _pSrc)->GetTexture() );
	glBegin(GL_QUADS);
		glTexCoord2f( sx1, sy1 );
		glVertex2i( dx1, dy1 );

		glTexCoord2f( sx2, sy1 );
		glVertex2i( dx2, dy1 );

		glTexCoord2f( sx2, sy2 );
		glVertex2i( dx2, dy2 );

		glTexCoord2f( sx1, sy2 );
		glVertex2i( dx1, dy2 );
	glEnd();

}

//____ StretchBlitSubPixel() ___________________________________________________

void WgGfxDeviceGL::StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy,
										 float sw, float sh,
								   		 float dx, float dy, float dw, float dh )
{
	if( !pSrc )
		return;

	float tw = (float) pSrc->GetWidth();
	float th = (float) pSrc->GetHeight();

	float	sx1 = sx/tw;
	float	sx2 = (sx+sw)/tw;
	float	sy1 = sy/th;
	float	sy2 = (sy+sh)/th;

	float	dx1 = dx;
	float	dx2 = dx + dw;
	float	dy1 = m_canvasHeight - dy;
	float	dy2 = dy1 - dh;

	glBindTexture(GL_TEXTURE_2D, ((WgSurfaceGL*)pSrc)->GetTexture() );
	glBegin(GL_QUADS);
		glTexCoord2f( sx1, sy1 );
		glVertex2f( dx1, dy1 );

		glTexCoord2f( sx2, sy1 );
		glVertex2f( dx2, dy1 );

		glTexCoord2f( sx2, sy2 );
		glVertex2f( dx2, dy2 );

		glTexCoord2f( sx1, sy2 );
		glVertex2f( dx1, dy2 );
	glEnd();
}

