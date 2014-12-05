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

const char WgPanel::CLASSNAME[] = {"Panel"};
const char WgPanelHook::CLASSNAME[] = {"PanelHook"};

//____ Constructor _____________________________________________________________

WgPanel::WgPanel() : m_bFocusGroup(false), m_bTooltipGroup(false), m_maskOp(WG_MASKOP_RECURSE)
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgPanel::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgContainer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgPanel::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgPanelPtr WgPanel::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgPanelPtr( static_cast<WgPanel*>(pObject.RawPtr()) );

	return 0;
}


//____ _isPanel() ______________________________________________________________

bool WgPanel::_isPanel() const
{
	return true;
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

//____ _onCloneContent() _______________________________________________________

void WgPanel::_onCloneContent( const WgWidget * _pOrg )
{
	const WgPanel * pOrg = static_cast<const WgPanel*>(_pOrg);

	m_bFocusGroup 		= pOrg->m_bFocusGroup;
	m_bTooltipGroup 	= pOrg->m_bTooltipGroup;
	m_maskOp 			= pOrg->m_maskOp;

	WgContainer::_onCloneContent( pOrg );
}


//____ _onMaskPatches() __________________________________________________________

void WgPanel::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	//TODO: Don't just check IsOpaque() globally, check rect by rect.
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE )
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
					p->_widget()->_onMaskPatches( patches, childGeo + geo.Pos(), clip, blendMode );
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

//____ WgPanelHook::IsInstanceOf() __________________________________________

bool WgPanelHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::IsInstanceOf(pClassName);
}

//____ WgPanelHook::ClassName() _____________________________________________

const char * WgPanelHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgPanelHook::Cast() __________________________________________________

WgPanelHookPtr WgPanelHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgPanelHookPtr( static_cast<WgPanelHook*>(pHook.RawPtr()) );

	return 0;
}

//____ WgPanelHook::Parent() __________________________________________________

WgPanelPtr WgPanelHook::Parent() const 
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

bool WgPanelHook::SetPadding( WgBorder padding )
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
			sz.w = _paddedMatchingWidth(sz.h);
			break;
		case WG_BOUND:
			sz.w = specifiedSize.w;
			sz.h = WgUtil::SizeFromPolicy( _paddedMatchingHeight(sz.w), specifiedSize.h, heightPolicy );
			break;
		case WG_CONFINED:
			if( defaultSize.w > specifiedSize.w )
			{
				sz.w = specifiedSize.w;
				sz.h = WgUtil::SizeFromPolicy( _paddedMatchingHeight(sz.w), specifiedSize.h, heightPolicy );
			}
			else
			{
				sz.h = WgUtil::SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
				sz.w = _paddedMatchingWidth(sz.h);
				if( sz.w > specifiedSize.w )
					sz.w = specifiedSize.w;
			}
			break;
		case WG_EXPANDED:
			if( defaultSize.w < specifiedSize.w )
			{
				sz.w = specifiedSize.w;
				sz.h = WgUtil::SizeFromPolicy( _paddedMatchingHeight(sz.w), specifiedSize.h, heightPolicy );
			}
			else
			{
				sz.h = WgUtil::SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
				sz.w = _paddedMatchingWidth(sz.h);
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
	return m_pWidget->PreferredSize() + m_padding;
}

WgSize WgPanelHook::_paddedMinSize() const
{
	return m_pWidget->MinSize() + m_padding;
}

WgSize WgPanelHook::_paddedMaxSize() const
{
	return m_pWidget->MaxSize() + m_padding;
}

int WgPanelHook::_paddedMatchingWidth( int paddedHeight ) const
{
	return m_pWidget->MatchingWidth( paddedHeight - m_padding.Height() ) + m_padding.Width();
}

int WgPanelHook::_paddedMatchingHeight( int paddedWidth ) const
{
	return m_pWidget->MatchingHeight( paddedWidth - m_padding.Width() ) + m_padding.Height();
}
