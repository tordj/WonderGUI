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

#include <wg_glgfxdevice.h>
#include <wg_glsurface.h>

const char WgGLGfxDevice::CLASSNAME[] = {"GLGfxDevice"};

//____ Constructor _____________________________________________________________

WgGLGfxDevice::WgGLGfxDevice( WgSize canvas ) : WgGfxDevice(canvas)
{
	m_bRendering = false;
}

//____ Destructor ______________________________________________________________

WgGLGfxDevice::~WgGLGfxDevice()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgGLGfxDevice::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgGfxDevice::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgGLGfxDevice::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgGLGfxDevicePtr WgGLGfxDevice::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgGLGfxDevicePtr( static_cast<WgGLGfxDevice*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetCanvas() __________________________________________________________________

void WgGLGfxDevice::SetCanvas( WgSize size )
{
	m_canvasSize 	= size;
}

//____ SetTintColor() __________________________________________________________

void WgGLGfxDevice::SetTintColor( WgColor color )
{
	WgGfxDevice::SetTintColor(color);

	if( m_bRendering )
		glColor4f( m_tintColor.r/255.f, m_tintColor.g/255.f, m_tintColor.b/255.f, m_tintColor.a/255.f );
}

//____ SetBlendMode() __________________________________________________________

bool WgGLGfxDevice::SetBlendMode( WgBlendMode blendMode )
{
	if( blendMode != WG_BLENDMODE_BLEND && blendMode != WG_BLENDMODE_OPAQUE && 
		blendMode != WG_BLENDMODE_ADD && blendMode != WG_BLENDMODE_MULTIPLY &&
		blendMode != WG_BLENDMODE_INVERT )
			return false;
 
	WgGfxDevice::SetBlendMode(blendMode);
	if( m_bRendering )
		_setBlendMode(blendMode);

	return true;
}

//____ BeginRender() ___________________________________________________________

bool WgGLGfxDevice::BeginRender()
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

	_setBlendMode(m_blendMode);

	// Save matrixes and set Ortho mode

	glMatrixMode(GL_PROJECTION);			// Select The Projection Matrix
	glPushMatrix();							// Store The Projection Matrix

	glLoadIdentity();								// Reset The Projection Matrix
	glOrtho(0,m_canvasSize.w,0,m_canvasSize.h,-1,1);	// Set Up An Ortho Screen

	glMatrixMode(GL_MODELVIEW);				// Select The Modelview Matrix
	glPushMatrix();							// Store The Modelview Matrix
	glLoadIdentity();						// Reset The Modelview Matrix

	//

	m_bRendering = true;
	return true;
}

//____ EndRender() _____________________________________________________________

bool WgGLGfxDevice::EndRender()
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

void WgGLGfxDevice::Fill( const WgRect& _rect, const WgColor& _col )
{
	if( _col.a == 0 || _rect.w < 1 || _rect.h < 1 )
		return;

	glDisable(GL_TEXTURE_2D);
	if( !_col.a )
		glDisable(GL_BLEND);
	else
		glEnable(GL_BLEND);

	int	dx1 = _rect.x;
	int	dy1 = m_canvasSize.h - _rect.y;
	int dx2 = _rect.x + _rect.w;
	int dy2 = m_canvasSize.h - (_rect.y + _rect.h);

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

void WgGLGfxDevice::Blit( const WgSurfacePtr& _pSrc, const WgRect& _src, int _dx, int _dy  )
{
	if( !_pSrc )
		return;

	float sw = (float) _pSrc->Width();
	float sh = (float) _pSrc->Height();

	float	sx1 = _src.x/sw;
	float	sx2 = (_src.x+_src.w)/sw;
	float	sy1 = _src.y/sh;
	float	sy2 = (_src.y+_src.h)/sh;

	int		dx1 = _dx;
	int		dx2 = _dx + _src.w;
	int		dy1 = m_canvasSize.h - _dy;
	int		dy2 = dy1 - _src.h;

	glBindTexture(GL_TEXTURE_2D, ((WgGLSurface*) _pSrc.GetRealPtr())->GetTexture() );
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

void WgGLGfxDevice::StretchBlitSubPixel( const WgSurfacePtr& pSrc, float sx, float sy,
										 float sw, float sh,
								   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias )
{
	if( !pSrc )
		return;

	float tw = (float) pSrc->Width();
	float th = (float) pSrc->Height();

	float	sx1 = sx/tw;
	float	sx2 = (sx+sw)/tw;
	float	sy1 = sy/th;
	float	sy2 = (sy+sh)/th;

	float	dx1 = dx;
	float	dx2 = dx + dw;
	float	dy1 = m_canvasSize.h - dy;
	float	dy2 = dy1 - dh;

	glBindTexture(GL_TEXTURE_2D, ((WgGLSurface*)pSrc.GetRealPtr())->GetTexture() );
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

//____ _setBlendMode() _________________________________________________________

void WgGLGfxDevice::_setBlendMode( WgBlendMode blendMode )
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
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;

		case WG_BLENDMODE_MULTIPLY:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;

		case WG_BLENDMODE_INVERT:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			break;

		default:
			break;
	}
}