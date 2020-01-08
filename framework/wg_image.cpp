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



#include <wg_image.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>


static const char	c_widgetType[] = {"Image"};

//____ Constructor ____________________________________________________________

WgImage::WgImage()
{
}

//____ Destructor _____________________________________________________________

WgImage::~WgImage()
{
    if( m_bDeleteImageWhenDone )
        delete m_pImage;
}

//____ Type() _________________________________________________________________

const char * WgImage::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgImage::GetClass()
{
	return c_widgetType;
}

//____ SetSkin() ______________________________________________________________

void WgImage::SetSkin(const WgSkinPtr& pSkin)
{
	if (pSkin != m_pSkin)
	{
		m_pSkin = pSkin;
		_requestResize();
		_requestRender();
	}
}


//____ SetSource() _____________________________________________________________

void WgImage::SetSource( const WgBlocksetPtr& pBlockset )
{
	if( m_pGfx != pBlockset )
	{
		m_pGfx = pBlockset;

		if( m_pGfx && m_pGfx->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

        _requestResize();
        _requestRender();
	}
}

//____ SetImage() ______________________________________________________________

void WgImage::SetImage( WgSurface * pSurface, bool bDeleteWhenDone )
{
    if( m_pImage == pSurface )
    {
        m_bDeleteImageWhenDone = bDeleteWhenDone;
        return;
    }
    
    if( m_bDeleteImageWhenDone )
        delete m_pImage;
    
    m_pImage = pSurface;
    m_bDeleteImageWhenDone = bDeleteWhenDone;
    
    _requestResize();
    _requestRender();
}


//____ PreferredPixelSize() _____________________________________________________________

WgSize WgImage::PreferredPixelSize() const
{
    if( m_pImage )
    {
        WgSize sz = (m_pImage->PixelSize() * m_scale) / m_pImage->ScaleFactor();

        if( m_pSkin )
            sz += m_pSkin->ContentPadding(m_scale);
        
        return sz;
    }
	else if( m_pGfx )
		return m_pGfx->Size(m_scale);
    else if( m_pSkin )
        return m_pSkin->PreferredSize(m_scale);

	return WgSize(1,1);
}

//____ MatchingPixelHeight() _____________________________________________________________

int  WgImage::MatchingPixelHeight(int pixelWidth) const
{
    WgSize imageSize;
    WgSize paddingSize;

    if( m_pImage )
    {
        imageSize = (m_pImage->PixelSize() * m_scale) / m_pImage->ScaleFactor();
        if( m_pSkin )
            paddingSize = m_pSkin->ContentPadding(m_scale);
    }
	else if (m_pGfx)
		imageSize = m_pGfx->Size(WG_SCALE_BASE);
    else if(m_pSkin)
        imageSize = m_pSkin->PreferredSize(WG_SCALE_BASE);
    else
        return 1;

    if( imageSize.w == 0 )
       return 0;
    
	return ((pixelWidth-paddingSize.w) * imageSize.h / imageSize.w) + paddingSize.h;
}

//____ MatchingPixelWeight() _____________________________________________________________

int  WgImage::MatchingPixelWidth(int pixelHeight) const
{
    WgSize imageSize;
    WgSize paddingSize;
    
    if( m_pImage )
    {
        imageSize = (m_pImage->PixelSize() * m_scale) / m_pImage->ScaleFactor();
        if( m_pSkin )
            paddingSize = m_pSkin->ContentPadding(m_scale);
    }
    else if (m_pGfx)
        imageSize = m_pGfx->Size(WG_SCALE_BASE);
    else if(m_pSkin)
        imageSize = m_pSkin->PreferredSize(WG_SCALE_BASE);
    else
        return 1;

    if( imageSize.h == 0 )
        return 0;

	return ((pixelHeight-paddingSize.h) * imageSize.w / imageSize.h) + paddingSize.w;
}




//____ _onCloneContent() _______________________________________________________

void WgImage::_onCloneContent( const WgWidget * _pOrg )
{
	WgImage * pOrg = (WgImage*) _pOrg;

	m_pGfx = pOrg->m_pGfx;
}

//____ _onRender() _____________________________________________________________

void WgImage::_onRender( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window )
{
	WgWidget::_onRender(pDevice, _canvas, _window);

	WgRect canvas = m_pSkin ? m_pSkin->ContentRect(_canvas, WgStateEnum::Normal, m_scale) : _canvas;

    if( m_pImage )
    {
        pDevice->setBlitSource(m_pImage->RealSurface());
        pDevice->stretchBlit(canvas);
    }
	else if( m_pGfx )
    {
        WgBlock    block;
        if( m_bEnabled )
            block = m_pGfx->GetBlock(WG_MODE_NORMAL, m_scale);
        else
            block = m_pGfx->GetBlock(WG_MODE_DISABLED, m_scale);
        
        WgGfxDevice::BlitBlock( pDevice, block, canvas);
    }
}

//____ _onAlphaTest() ___________________________________________________________

bool WgImage::_onAlphaTest( const WgCoord& ofs )
{
    WgSize sz = PixelSize();
    
    if( m_pImage )
    {
        if( m_pSkin && m_pSkin->MarkTest(ofs, sz, m_state, m_markOpacity, m_scale) )
            return true;

        WgRect canvas = m_pSkin ? m_pSkin->ContentRect(sz, m_state, m_scale) : WgRect(sz);       
        if( canvas.contains(ofs) )
        {
            WgSize imgSize = m_pImage->PixelSize();
        
            WgCoord surfOfs = { (ofs.x - canvas.x)*imgSize.w/canvas.w, (ofs.y - canvas.y)*imgSize.h/canvas.h };
            m_pImage->GetOpacity(surfOfs);
        }
    }
	else if( m_pGfx )
    {
        WgMode mode = WG_MODE_NORMAL;
        if( !m_bEnabled )
            mode = WG_MODE_DISABLED;
        
        return WgUtil::MarkTestBlock( ofs, m_pGfx->GetBlock(mode,m_scale), WgRect(0,0,sz.w,sz.h), m_markOpacity );
    }
    else if( m_pSkin )
    {
        return m_pSkin->MarkTest(ofs, sz, m_state, m_markOpacity, m_scale);
    }

    return false;
}

//____ _onEnable() _____________________________________________________________

void WgImage::_onEnable()
{
    _requestRender();
}

//____ _onDisable() ____________________________________________________________

void WgImage::_onDisable()
{
    _requestRender();
}

//____ _setScale() ____________________________________________________________

void WgImage::_setScale( int scale )
{
    WgWidget::_setScale(scale);

    if( m_pGfx || m_pSkin || m_pImage )
        _requestResize();
}



