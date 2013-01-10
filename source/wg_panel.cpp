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


//____ _onAlphaTest() _________________________________________________________

bool WgPanel::_onAlphaTest( const WgCoord& ofs )
{
	return false;		// By default cointainers have nothing to display themselves.
}

//____ _onCloneContent() _______________________________________________________

void WgPanel::_onCloneContent( const WgPanel * _pOrg )
{
	m_bFocusGroup 		= _pOrg->m_bFocusGroup;
	m_bRadioGroup 		= _pOrg->m_bRadioGroup;
	m_bTooltipGroup 	= _pOrg->m_bTooltipGroup;
	m_maskOp 			= _pOrg->m_maskOp;

	WgContainer::_onCloneContent( _pOrg );
}

//____ _onMaskPatches() __________________________________________________________

void WgPanel::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
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

bool WgPanelHook::SetPadding( WgBorders padding )
{
	if( padding != m_padding )
	{
		m_padding = padding;
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
