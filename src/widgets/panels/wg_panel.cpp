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

namespace wg 
{
	
	const char Panel::CLASSNAME[] = {"Panel"};
	const char PanelHook::CLASSNAME[] = {"PanelHook"};
	
	//____ Constructor _____________________________________________________________
	
	Panel::Panel() : m_bTooltipGroup(false), m_maskOp(MaskOp::Recurse)
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Panel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Container::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Panel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Panel_p Panel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Panel_p( static_cast<Panel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ _isPanel() ______________________________________________________________
	
	bool Panel::_isPanel() const
	{
		return true;
	}
	
	//____ setMaskOp() _____________________________________________________________
	
	void Panel::setMaskOp( MaskOp operation )
	{
		if( operation != m_maskOp )
		{
			m_maskOp = operation;
			_requestRender();
		}
	}
	
	//____ _cloneContent() _______________________________________________________
	
	void Panel::_cloneContent( const Widget * _pOrg )
	{
		const Panel * pOrg = static_cast<const Panel*>(_pOrg);
	
		m_bTooltipGroup 	= pOrg->m_bTooltipGroup;
		m_maskOp 			= pOrg->m_maskOp;
	
		Container::_cloneContent( pOrg );
	}
	
	
	//____ _maskPatches() __________________________________________________________
	
	void Panel::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace )
		{
			patches.sub( Rect(geo,clip) );
			return;
		}
		
		switch( m_maskOp )
		{
			case MaskOp::Recurse:
			{
				Rect childGeo;
				PanelHook * p = static_cast<PanelHook*>(_firstHookWithGeo( childGeo ));
	
				while(p)
				{
					if( p->isVisible() )
						p->_widget()->_maskPatches( patches, childGeo + geo.pos(), clip, blendMode );
					p = static_cast<PanelHook*>(_nextHookWithGeo( childGeo, p ));
				}
				break;
			}
			case MaskOp::Skip:
				break;
			case MaskOp::Mask:
				patches.sub( Rect(geo,clip) );
				break;
		}
	}
	
	//____ PanelHook::isInstanceOf() __________________________________________
	
	bool PanelHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Hook::isInstanceOf(pClassName);
	}
	
	//____ PanelHook::className() _____________________________________________
	
	const char * PanelHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ PanelHook::cast() __________________________________________________
	
	PanelHook_p PanelHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return PanelHook_p( static_cast<PanelHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	//____ PanelHook::parent() __________________________________________________
	
	Panel_p PanelHook::parent() const 
	{ 
		return static_cast<Panel*>(_parent()); 
	}
	
	
	//____ PanelHook::setVisible() _____________________________________________________________
	
	bool PanelHook::setVisible( bool bVisible )
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
	
	//____ PanelHook::setPadding() ______________________________________________
	
	bool PanelHook::setPadding( Border padding )
	{
		if( padding != m_padding )
		{
			m_padding = padding;
			_requestResize();
		}
		return true;
	}
	
	//____ PanelHook::_sizeFromPolicy() ________________________________________________________
	
	Size PanelHook::_sizeFromPolicy( Size specifiedSize, SizePolicy widthPolicy, SizePolicy heightPolicy ) const
	{
		Size	defaultSize = _paddedPreferredSize();
	
		Size	sz;
	
		switch( widthPolicy )
		{
			case SizePolicy::Default:
			{
				sz.h = Util::sizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
				sz.w = _paddedMatchingWidth(sz.h);
				break;
			case SizePolicy::Bound:
				sz.w = specifiedSize.w;
				sz.h = Util::sizeFromPolicy( _paddedMatchingHeight(sz.w), specifiedSize.h, heightPolicy );
				break;
			case SizePolicy::Confined:
				if( defaultSize.w > specifiedSize.w )
				{
					sz.w = specifiedSize.w;
					sz.h = Util::sizeFromPolicy( _paddedMatchingHeight(sz.w), specifiedSize.h, heightPolicy );
				}
				else
				{
					sz.h = Util::sizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
					sz.w = _paddedMatchingWidth(sz.h);
					if( sz.w > specifiedSize.w )
						sz.w = specifiedSize.w;
				}
				break;
			case SizePolicy::Expanded:
				if( defaultSize.w < specifiedSize.w )
				{
					sz.w = specifiedSize.w;
					sz.h = Util::sizeFromPolicy( _paddedMatchingHeight(sz.w), specifiedSize.h, heightPolicy );
				}
				else
				{
					sz.h = Util::sizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
					sz.w = _paddedMatchingWidth(sz.h);
					if( sz.w < specifiedSize.w )
						sz.w = specifiedSize.w;
				}
				break;
			}
		}
	
		return sz;
	}
	
	Size PanelHook::_paddedPreferredSize() const
	{
		return m_pWidget->preferredSize() + m_padding;
	}
	
	Size PanelHook::_paddedMinSize() const
	{
		return m_pWidget->minSize() + m_padding;
	}
	
	Size PanelHook::_paddedMaxSize() const
	{
		return m_pWidget->maxSize() + m_padding;
	}
	
	int PanelHook::_paddedMatchingWidth( int paddedHeight ) const
	{
		return m_pWidget->matchingWidth( paddedHeight - m_padding.height() ) + m_padding.width();
	}
	
	int PanelHook::_paddedMatchingHeight( int paddedWidth ) const
	{
		return m_pWidget->matchingHeight( paddedWidth - m_padding.width() ) + m_padding.height();
	}

} // namespace wg
