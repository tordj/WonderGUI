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

#include <vector>
#include <wg_panel.h>
#include <wg_util.h>

#include <wg_patches.h>

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

//____ Constructor _____________________________________________________________

WgPanel::WgPanel() : m_bFocusGroup(false), m_bRadioGroup(false), m_bTooltipGroup(false), m_maskOp(WG_MASKOP_RECURSE)
{
}

//____ IsPanel() ______________________________________________________________

bool WgPanel::IsPanel() const
{
	return true;
}

//____ CastToPanel() _______________________________________________________

WgPanel * WgPanel::CastToPanel()
{
	return this;
}

const WgPanel * WgPanel::CastToPanel() const
{
	return this;
}

//____ SetMaskOp() _____________________________________________________________

void WgPanel::SetMaskOp( WgMaskOp operation )
{
	if( operation != m_maskOp )
	{
		m_maskOp = operation;
		_requestRender();
	}
}

//____ SetSkin() _______________________________________________________________

void WgPanel::SetSkin( const WgSkinPtr& pSkin )
{
	m_pSkin = pSkin;
	_requestRender();
}

//____ _onAlphaTest() _________________________________________________________

bool WgPanel::_onAlphaTest( const WgCoord& ofs )
{
	if( m_pSkin )
		return m_pSkin->MarkTest( ofs, Size(), m_bEnabled?WG_STATE_NORMAL:WG_STATE_DISABLED, m_markOpacity );
	else
		return false;		// By default cointainers have nothing to display themselves.
}

//____ _onCloneContent() _______________________________________________________

void WgPanel::_onCloneContent( const WgPanel * _pOrg )
{
	m_bFocusGroup 		= _pOrg->m_bFocusGroup;
	m_bRadioGroup 		= _pOrg->m_bRadioGroup;
	m_bTooltipGroup 	= _pOrg->m_bTooltipGroup;
	m_maskOp 			= _pOrg->m_maskOp;
	m_pSkin				= _pOrg->m_pSkin;

	WgContainer::_onCloneContent( _pOrg );
}

//____ _onCollectPatches() _______________________________________________________

void WgPanel::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	if( m_pSkin && m_pSkin->IsOpaque(m_bEnabled?WG_STATE_NORMAL:WG_STATE_DISABLED) )
		container.Add( WgRect( geo, clip ) );
	else
		WgContainer::_onCollectPatches( container, geo, clip );

}


//____ _onMaskPatches() __________________________________________________________

void WgPanel::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{

	//TODO: Don't just check IsOpaque() globally, check rect by rect.
	if( m_pSkin && m_pSkin->IsOpaque() )
	{
		patches.Sub( WgRect(geo,clip) );
		return;
	}
	
	switch( m_maskOp )
	{
		case WG_MASKOP_RECURSE:
		{
			WgRect childGeo;
			WgPanelHook * p = static_cast<WgPanelHook*>(_firstHookWithGeo( childGeo ));

			while(p)
			{
				if( p->IsVisible() )
					p->Widget()->_onMaskPatches( patches, childGeo + geo.Pos(), clip, blendMode );
				p = static_cast<WgPanelHook*>(_nextHookWithGeo( childGeo, p ));
			}
			break;
		}
		case WG_MASKOP_SKIP:
			break;
		case WG_MASKOP_MASK:
			patches.Sub( WgRect(geo,clip) );
			break;
	}
}

//____ _onRender() ___________________________________________________________________

void WgPanel::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	if( m_pSkin )
		m_pSkin->Render( pDevice, _canvas, WG_STATE_NORMAL, _clip );
}

//____ WgPanelHook::Parent() __________________________________________________

WgPanel* WgPanelHook::Parent() const 
{ 
	return static_cast<WgPanel*>(_parent()); 
}


//____ WgPanelHook::SetVisible() _____________________________________________________________

bool WgPanelHook::SetVisible( bool bVisible )
{
	if( bVisible != m_bVisible )
	{
		if( bVisible )
		{
			m_bVisible = true;
			_requestRender();
		}
		else
		{
			_requestRender();
			m_bVisible = false;
		}		
	}
	return true;
}

//____ WgPanelHook::SetPadding() ______________________________________________

bool WgPanelHook::SetPadding( WgBorders padding, WgUnit unit )
{
	if( padding != m_padding || unit != m_paddingUnit )
	{
		m_padding = padding;
        m_paddingUnit = unit;
		_requestResize();
	}
	return true;
}

//____ WgPanelHook::_sizeFromPolicy() ________________________________________________________

WgSize WgPanelHook::_sizeFromPolicy( WgSize specifiedSize, WgSizePolicy widthPolicy, WgSizePolicy heightPolicy ) const
{
	WgSize	defaultSize = _paddedPreferredSize();

	WgSize	sz;

	switch( widthPolicy )
	{
		case WG_DEFAULT:
		{
			sz.h = WgUtil::SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
			sz.w = _paddedWidthForHeight(sz.h);
			break;
		case WG_BOUND:
			sz.w = specifiedSize.w;
			sz.h = WgUtil::SizeFromPolicy( _paddedHeightForWidth(sz.w), specifiedSize.h, heightPolicy );
			break;
		case WG_CONFINED:
			if( defaultSize.w > specifiedSize.w )
			{
				sz.w = specifiedSize.w;
				sz.h = WgUtil::SizeFromPolicy( _paddedHeightForWidth(sz.w), specifiedSize.h, heightPolicy );
			}
			else
			{
				sz.h = WgUtil::SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
				sz.w = _paddedWidthForHeight(sz.h);
				if( sz.w > specifiedSize.w )
					sz.w = specifiedSize.w;
			}
			break;
		case WG_EXPANDED:
			if( defaultSize.w < specifiedSize.w )
			{
				sz.w = specifiedSize.w;
				sz.h = WgUtil::SizeFromPolicy( _paddedHeightForWidth(sz.w), specifiedSize.h, heightPolicy );
			}
			else
			{
				sz.h = WgUtil::SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
				sz.w = _paddedWidthForHeight(sz.h);
				if( sz.w < specifiedSize.w )
					sz.w = specifiedSize.w;
			}
			break;
		}
	}

	return sz;
}

WgSize WgPanelHook::_paddedPreferredSize() const
{
    switch( m_paddingUnit )
    {
        case WG_PIXELS:
            return m_pWidget->PreferredSize() + m_padding;
        case WG_FRACTION:
        {
            WgSize sz = m_pWidget->PreferredSize();
            sz.w += ((sz.w * m_padding.left) >> 8) + ((sz.w * m_padding.right) >> 8);
            sz.h += ((sz.h * m_padding.top) >> 8) + ((sz.h * m_padding.bottom) >> 8);
            return sz;
        }
            
    }
}

WgSize WgPanelHook::_paddedMinSize() const
{
    switch( m_paddingUnit )
    {
        case WG_PIXELS:
            return m_pWidget->MinSize() + m_padding;
        case WG_FRACTION:
        {
            WgSize sz = m_pWidget->MinSize();
            sz.w += ((sz.w * m_padding.left) >> 8) + ((sz.w * m_padding.right) >> 8);
            sz.h += ((sz.h * m_padding.top) >> 8) + ((sz.h * m_padding.bottom) >> 8);
            return sz;
        }
            
    }
}

WgSize WgPanelHook::_paddedMaxSize() const
{
    switch( m_paddingUnit )
    {
        case WG_PIXELS:
            return m_pWidget->MaxSize() + m_padding;
        case WG_FRACTION:
        {
            WgSize sz = m_pWidget->MaxSize();
            sz.w += ((sz.w * m_padding.left) >> 8) + ((sz.w * m_padding.right) >> 8);
            sz.h += ((sz.h * m_padding.top) >> 8) + ((sz.h * m_padding.bottom) >> 8);
            return sz;
        }
            
    }
}

int WgPanelHook::_paddedWidthForHeight( int paddedHeight ) const
{
    switch( m_paddingUnit )
    {
        case WG_PIXELS:
            return m_pWidget->WidthForHeight( paddedHeight - m_padding.Height() ) + m_padding.Width();
        case WG_FRACTION:
        {
        }
            
    }
}

int WgPanelHook::_paddedHeightForWidth( int paddedWidth ) const
{
    switch( m_paddingUnit )
    {
        case WG_PIXELS:
            return m_pWidget->HeightForWidth( paddedWidth - m_padding.Width() ) + m_padding.Height();
        case WG_FRACTION:
        {
            float fOrgWidth = paddedWidth / 1.f + (m_padding.left + m_padding.right)/256.f;
            
            float leftPadding = fOrgWidth * m_padding.left/256.f;
            float rightPadding = fOrgWidth * m_padding.right/256.f;
            
            int orgWidth = paddedWidth - ((int)leftPadding) - ((int)rightPadding);
            
        }
            
    }
}
