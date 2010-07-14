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

#include <wg_gfxdevice_mig.h>
#include <wg_surface_mig.h>

#include	"EObjects/ERenderer_w32.h"


//____ Constructor _____________________________________________________________

WgGfxDeviceMIG::WgGfxDeviceMIG()
{
	m_blendModeMIG		= ERenderer::eBlendModeBlend;
	m_tintColorMIG		= EColor(1.f,1.f,1.f);
	m_orient			= 0;
}

//____ Destructor ______________________________________________________________

WgGfxDeviceMIG::~WgGfxDeviceMIG()
{
}

//____ SetTintColor() __________________________________________________________

void WgGfxDeviceMIG::SetTintColor( WgColor color )
{
	m_tintColorMIG.a = color.a/255.f;
	m_tintColorMIG.r = color.r/255.f;
	m_tintColorMIG.g = color.g/255.f;
	m_tintColorMIG.b = color.b/255.f;

	WgGfxDevice::SetTintColor(color);
}

//____ SetBlendMode() __________________________________________________________

bool WgGfxDeviceMIG::SetBlendMode( WgBlendMode blendMode )
{
	switch( blendMode )
	{
		case WG_BLENDMODE_OPAQUE:
			m_blendModeMIG = ERenderer::eBlendModeNone;
			break;

		case WG_BLENDMODE_BLEND:
			m_blendModeMIG = ERenderer::eBlendModeBlend;
			break;

		case WG_BLENDMODE_ADD:
			m_blendModeMIG = ERenderer::eBlendModeAlphaAdd;
			break;

		case WG_BLENDMODE_MULTIPLY:
			m_blendModeMIG = ERenderer::eBlendModeMultiply;
			break;

		default:
			return false;
	}
	
	WgGfxDevice::SetBlendMode(blendMode);
	return true;
}

//____ Fill() __________________________________________________________________
Uint32 WgGfxDeviceMIG::SetRenderFlags( Uint32 flags )
{
	Uint32 orient = flags & WG_ORIENT_MASK;
	m_orient = 0;
	switch( orient )
	{
	case WG_ORIENT_NORMAL:					m_orient = ERenderer::eOrientNormal; break;
	case WG_ORIENT_ROTATE_CW90:				m_orient = ERenderer::eOrientRotateCW90; break;
	case WG_ORIENT_ROTATE_180:				m_orient = ERenderer::eOrientRotate180; break;
	case WG_ORIENT_ROTATE_CCW90:			m_orient = ERenderer::eOrientRotateCCW90; break;
	case WG_ORIENT_MIRROR_X:				m_orient = ERenderer::eOrientMirrorX; break;
	case WG_ORIENT_MIRROR_X_ROTATE_CW90:	m_orient = ERenderer::eOrientMirrorXRotateCW90; break;
	case WG_ORIENT_MIRROR_X_ROTATE_180:		m_orient = ERenderer::eOrientMirrorXRotate180; break;
	case WG_ORIENT_MIRROR_X_ROTATE_CCW90:	m_orient = ERenderer::eOrientMirrorXRotateCCW90; break;
	}

	return WgGfxDevice::SetRenderFlags( flags );
}

//____ Fill() __________________________________________________________________

void WgGfxDeviceMIG::Fill( const WgRect& _rect, const WgColor& _col )
{
	if( _col.a == 0 || _rect.w < 1 || _rect.h < 1 )
		return;	

	EColor c(_col.r/255.f * m_tintColorMIG.r, _col.g/255.f * m_tintColorMIG.g, _col.b/255.f * m_tintColorMIG.b, _col.a/255.f * m_tintColorMIG.a);
	ERenderer::GetCurrent()->Blit( NULL, 0, 0, 0, 0, float(_rect.x), float(_rect.y), float(_rect.w), float(_rect.h), m_blendModeMIG, c );
	return;
}

//____ Blit() __________________________________________________________________

void WgGfxDeviceMIG::Blit( const WgSurface* pSrc, const WgRect& src, Sint32 dx, Sint32 dy )
{
	if( !pSrc )
		return;

	ERenderer::GetCurrent()->Blit(((WgSurfaceMIG*)pSrc)->MIG_Surf(), src.x, src.y, dx, dy, src.w, src.h, m_blendModeMIG, m_tintColorMIG, m_orient );
}

//____ StretchBlitSubPixel() ___________________________________________________

void WgGfxDeviceMIG::StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias )
{
	ERenderer::GetCurrent()->Blit(((WgSurfaceMIG*)pSrc)->MIG_Surf(), sx, sy, sw, sh, dx, dy, dw, dh, m_blendModeMIG, m_tintColorMIG, m_orient, bTriLinear, mipBias );
}

