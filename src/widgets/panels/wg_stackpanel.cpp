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

#include <wg_stackpanel.h>
#include <wg_util.h>
#include <wg_patches.h>

namespace wg 
{
	
	const char StackPanel::CLASSNAME[] = {"StackPanel"};
	const char StackHook::CLASSNAME[] = {"StackHook"};
	
	
	
	void StackHook::setSizePolicy( SizePolicy policy )
	{
		if( policy != m_sizePolicy )
		{
			m_pParent->_renderRequested(this);
			m_sizePolicy = policy;
			m_pParent->_renderRequested(this);
		};
	}
	
	void StackHook::setOrigo( Origo origo )
	{
		if( origo != m_origo )
		{
			m_pParent->_renderRequested(this);
			m_origo = origo;
			m_pParent->_renderRequested(this);
		}
	}
	
	StackHook::StackHook( StackPanel * pParent ): m_pParent(pParent), m_origo( Origo::NorthWest ), m_sizePolicy( STRETCH )
	{
	}
	
	//____ StackHook::isInstanceOf() __________________________________________
	
	bool StackHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return VectorHook::isInstanceOf(pClassName);
	}
	
	//____ StackHook::className() _____________________________________________
	
	const char * StackHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ StackHook::cast() __________________________________________________
	
	StackHook_p StackHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return StackHook_p( static_cast<StackHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	Container * StackHook::_parent() const
	{
		return m_pParent;
	}
	
	
	
	Rect StackHook::_getGeo( const Rect& parentGeo ) const
	{
		Rect base = parentGeo - m_padding;
	
		if( base.w <= 0 || base.h <= 0 )
			return Rect(0,0,0,0);
	
		switch( m_sizePolicy )
		{
			default:
			case DEFAULT:
			{
				Size	size = m_pWidget->preferredSize();
				Rect geo = Util::origoToRect( m_origo, base, size );
	
				if( geo.w > base.w )
				{
					geo.x = 0;
					geo.w = base.w;
				}
	
				if( geo.h > base.h )
				{
					geo.y = 0;
					geo.h = base.h;
				}
				return geo;
			}
			case STRETCH:
			{
				return base;
			}
			case SCALE:
			{
				Size	orgSize = m_pWidget->preferredSize();
				Size	size;
	
				float	fracX = orgSize.w / (float) base.w;
				float	fracY = orgSize.h / (float) base.h;
	
				if( fracX > fracY )
				{
					size.w = base.w;
					size.h = int (orgSize.h / fracX);
				}
				else
				{
					size.h = base.h;
					size.w = (int) (orgSize.w / fracY);
				}
	
				return Util::origoToRect( m_origo, base, size );
			}
		}
	}
	
	
	//____ Constructor ____________________________________________________________
	
	StackPanel::StackPanel()
	{
		m_bSiblingsOverlap = true;
	}
	
	//____ Destructor _____________________________________________________________
	
	StackPanel::~StackPanel()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool StackPanel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return LegacyVectorPanel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * StackPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	StackPanel_p StackPanel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StackPanel_p( static_cast<StackPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int StackPanel::matchingHeight( int width ) const
	{
		int height = 0;
	
		StackHook * pHook = static_cast<StackHook*>(m_hooks.first());
		while( pHook )
		{
			int h = pHook->_widget()->matchingHeight(width);
			if( h > height )
				height = h;
			pHook = pHook->_next();
		}
	
		return height;
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int StackPanel::matchingWidth( int height ) const
	{
		int width = 0;
	
		StackHook * pHook = static_cast<StackHook*>(m_hooks.first());
		while( pHook )
		{
			int w = pHook->_widget()->matchingWidth(height);
			if( w > width )
				width = w;
			pHook = pHook->_next();
		}
	
		return width;
	}
	
	
	//____ preferredSize() _____________________________________________________________
	
	Size StackPanel::preferredSize() const
	{
		return m_preferredSize;
	}
	
	//____ _setSize() ___________________________________________________________
	
	void StackPanel::_setSize( const Size& size )
	{
		Panel::_setSize(size);
		_adaptChildrenToSize();
	}
	
	
	//____ _hookGeo() _____________________________________________________________
	
	Rect StackPanel::_hookGeo( const VectorHook * pHook )
	{
		return ((StackHook*)pHook)->_getGeo(m_size);
	}
	
	//____ _requestResizeRequested() ____________________________________________________
	
	void StackPanel::_requestResizeRequested( VectorHook * _pHook )
	{
		_refreshPreferredSize();
	}
	
	//____ _renderRequested() ____________________________________________________
	
	void StackPanel::_renderRequested( VectorHook * _pHook )
	{
		StackHook * pHook = static_cast<StackHook*>(_pHook);
	
		_renderRequested(pHook, pHook->_getGeo(Rect(0,0,m_size)));
	}
	
	void StackPanel::_renderRequested( VectorHook * _pHook, const Rect& _rect )
	{
		StackHook * pHook = static_cast<StackHook*>(_pHook);
	
		if( !pHook->isVisible() )
			return;
	
		// Put our rectangle into patches
	
		Rect rect = _rect + pHook->_getGeo(Rect(0,0,m_size)).pos();
	
	
		Patches patches;
		patches.add( rect );
	
		// Remove portions of patches that are covered by opaque upper siblings
	
		StackHook * pCover = ((StackHook*)pHook)->_next();
		while( pCover )
		{
			Rect geo = pCover->_getGeo(m_size);
			if( pCover->isVisible() && geo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, geo, Rect(0,0,65536,65536 ), _getBlendMode() );
	
			pCover = pCover->_next();
		}
	
		// Make request render calls
	
		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}
	
	//____ _onWidgetAppeared() _____________________________________________________
	
	void StackPanel::_onWidgetAppeared( VectorHook * _pInserted )
	{
		StackHook * pInserted = (StackHook*) _pInserted;
	
		bool	bRequestResize = false;
	
		// Check if we need to resize to fit Widget in current width
	
		int height = pInserted->_widget()->matchingHeight(m_size.w);
		if( height > m_size.h )
			bRequestResize = true;
	
		// Update bestSize
	
		Size preferred = pInserted->_widget()->preferredSize();
	
		if( preferred.w > m_preferredSize.w )
		{
			m_preferredSize.w = preferred.w;
			bRequestResize = true;
		}
		if( preferred.h > m_preferredSize.h )
		{
			m_preferredSize.h = preferred.h;
			bRequestResize = true;
		}
	
		if( bRequestResize )
			_requestResize();
	
		// Adapt inserted Widget to our size
	
		pInserted->_widget()->_setSize(m_size);
	
		// Force a render.
	
		_renderRequested( pInserted );
	}
	
	//____ _onWidgetDisappeared() __________________________________________________
	
	void StackPanel::_onWidgetDisappeared( VectorHook * _pToBeRemoved )
	{
		bool	bRequestResize = false;
		StackHook * pToBeRemoved = (StackHook*) _pToBeRemoved;
	
		// Get dirty rectangles for all visible sections of pToBeRemoved.
	
		_renderRequested( pToBeRemoved );
	
		// Update m_preferredSize, skiping pToBeRemoved
	
		Size	preferredSize;
		StackHook * pHook = static_cast<StackHook*>(m_hooks.first());
		while( pHook )
		{
			if( pHook != pToBeRemoved )
			{
				Size sz = pHook->_widget()->preferredSize();
				if( sz.w > preferredSize.w )
					preferredSize.w = sz.w;
				if( sz.h > preferredSize.h )
					preferredSize.h = sz.h;
			}
			pHook = pHook->_next();
		}
	
		if( preferredSize != m_preferredSize )
			bRequestResize = true;
	
		m_preferredSize = preferredSize;
	
		// Check if removal might affect height for current width
	
		int height = pToBeRemoved->_widget()->matchingHeight(m_size.w);
		if( height >= m_size.h )
			bRequestResize = true;
	
		//
	
		if( bRequestResize )
			_requestResize();
	}
	
	//____ _onWidgetsReordered() ___________________________________________________
	
	void StackPanel::_onWidgetsReordered()
	{
		_requestRender();
	}
	
	//____ _refreshAllWidgets() ____________________________________________________
	
	void StackPanel::_refreshAllWidgets()
	{
		_refreshPreferredSize();
		_adaptChildrenToSize();
		_requestRender();
	}
	
	//____ _newHook() _____________________________________________________________
	
	VectorHook * StackPanel::_newHook()
	{
		return new StackHook(this);
	}
	
	//____ _refreshPreferredSize() _____________________________________________________
	
	void StackPanel::_refreshPreferredSize()
	{
		Size	preferredSize;
	
		StackHook * pHook = static_cast<StackHook*>(m_hooks.first());
		while( pHook )
		{
			Size sz = pHook->_paddedPreferredSize();
			if( sz.w > preferredSize.w )
				preferredSize.w = sz.w;
			if( sz.h > preferredSize.h )
				preferredSize.h = sz.h;
			pHook = pHook->_next();
		}
	
		if( m_preferredSize != preferredSize)
		{
			m_preferredSize = preferredSize;
			_requestResize();
		}
	}
	
	//____ _adaptChildrenToSize() ___________________________________________________________
	
	void StackPanel::_adaptChildrenToSize()
	{
		StackHook * pHook = static_cast<StackHook*>(m_hooks.first());
		while( pHook )
		{
			pHook->_widget()->_setSize( pHook->_getGeo(m_size) );
			pHook = pHook->_next();
		}
	}
	
	//____ _firstChildWithGeo() _____________________________________________________
	
	void StackPanel::_firstChildWithGeo( WidgetWithGeo& package ) const
	{
		StackHook * p = static_cast<StackHook*>(m_hooks.first());
		if( p )
		{
			package.pMagic = p;
			package.pWidget = p->_widget();
			package.geo = p->_getGeo(m_size);
		}
		else
			package.pWidget = nullptr;
	}
	
	//____ _nextChildWithGeo() ______________________________________________________
	
	void StackPanel::_nextChildWithGeo( WidgetWithGeo& package ) const
	{
		StackHook * p = static_cast<StackHook*>(package.pMagic)->_next();
		if( p )
		{
			package.pMagic = p;
			package.pWidget = p->_widget();
			package.geo = p->_getGeo(m_size);
		}
		else
			package.pWidget = nullptr;
	}

	//____ _cloneContent() ______________________________________________________
	
	void StackPanel::_cloneContent( const Widget * _pOrg )
	{
		Panel::_cloneContent( _pOrg );

		//TODO: Implement		
	}
	

} // namespace wg
