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

const char WgStackPanel::CLASSNAME[] = {"StackPanel"};
const char WgStackHook::CLASSNAME[] = {"StackHook"};



void WgStackHook::setSizePolicy( SizePolicy policy )
{
	if( policy != m_sizePolicy )
	{
		m_pParent->_onRenderRequested(this);
		m_sizePolicy = policy;
		m_pParent->_onRenderRequested(this);
	};
}

void WgStackHook::setOrigo( WgOrigo origo )
{
	if( origo != m_origo )
	{
		m_pParent->_onRenderRequested(this);
		m_origo = origo;
		m_pParent->_onRenderRequested(this);
	}
}

WgStackHook::WgStackHook( WgStackPanel * pParent ): m_pParent(pParent), m_origo( WG_NORTHWEST ), m_sizePolicy( STRETCH )
{
}

//____ WgStackHook::isInstanceOf() __________________________________________

bool WgStackHook::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgVectorHook::isInstanceOf(pClassName);
}

//____ WgStackHook::className() _____________________________________________

const char * WgStackHook::className( void ) const
{ 
	return CLASSNAME; 
}

//____ WgStackHook::cast() __________________________________________________

WgStackHookPtr WgStackHook::cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->isInstanceOf(CLASSNAME) )
		return WgStackHookPtr( static_cast<WgStackHook*>(pHook.rawPtr()) );

	return 0;
}

WgContainer * WgStackHook::_parent() const
{
	return m_pParent;
}



WgRect WgStackHook::_getGeo( const WgRect& parentGeo ) const
{
	WgRect base = parentGeo - m_padding;

	if( base.w <= 0 || base.h <= 0 )
		return WgRect(0,0,0,0);

	switch( m_sizePolicy )
	{
		default:
		case DEFAULT:
		{
			WgSize	size = m_pWidget->preferredSize();
			WgRect geo = WgUtil::origoToRect( m_origo, base, size );

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
			WgSize	orgSize = m_pWidget->preferredSize();
			WgSize	size;

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

			return WgUtil::origoToRect( m_origo, base, size );
		}
	}
}


//____ Constructor ____________________________________________________________

WgStackPanel::WgStackPanel()
{
	m_bSiblingsOverlap = true;
}

//____ Destructor _____________________________________________________________

WgStackPanel::~WgStackPanel()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgStackPanel::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgVectorPanel::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgStackPanel::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgStackPanelPtr WgStackPanel::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgStackPanelPtr( static_cast<WgStackPanel*>(pObject.rawPtr()) );

	return 0;
}

//____ matchingHeight() _______________________________________________________

int WgStackPanel::matchingHeight( int width ) const
{
	int height = 0;

	WgStackHook * pHook = _firstHook();
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

int WgStackPanel::matchingWidth( int height ) const
{
	int width = 0;

	WgStackHook * pHook = _firstHook();
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

WgSize WgStackPanel::preferredSize() const
{
	return m_preferredSize;
}

//____ _onNewSize() ___________________________________________________________

void WgStackPanel::_onNewSize( const WgSize& size )
{
	m_size = size;
	_adaptChildrenToSize();
}


//____ _hookGeo() _____________________________________________________________

WgRect WgStackPanel::_hookGeo( const WgVectorHook * pHook )
{
	return ((WgStackHook*)pHook)->_getGeo(m_size);
}

//____ _onResizeRequested() ____________________________________________________

void WgStackPanel::_onResizeRequested( WgVectorHook * _pHook )
{
	_refreshPreferredSize();
}

//____ _onRenderRequested() ____________________________________________________

void WgStackPanel::_onRenderRequested( WgVectorHook * _pHook )
{
	WgStackHook * pHook = static_cast<WgStackHook*>(_pHook);

	_onRenderRequested(pHook, pHook->_getGeo(WgRect(0,0,m_size)));
}

void WgStackPanel::_onRenderRequested( WgVectorHook * _pHook, const WgRect& _rect )
{
	WgStackHook * pHook = static_cast<WgStackHook*>(_pHook);

	if( !pHook->isVisible() )
		return;

	// Put our rectangle into patches

	WgRect rect = _rect + pHook->_getGeo(WgRect(0,0,m_size)).pos();


	WgPatches patches;
	patches.add( rect );

	// Remove portions of patches that are covered by opaque upper siblings

	WgStackHook * pCover = ((WgStackHook*)pHook)->_next();
	while( pCover )
	{
		WgRect geo = pCover->_getGeo(m_size);
		if( pCover->isVisible() && geo.intersectsWith( rect ) )
			pCover->_widget()->_onMaskPatches( patches, geo, WgRect(0,0,65536,65536 ), _getBlendMode() );

		pCover = pCover->_next();
	}

	// Make request render calls

	for( const WgRect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
		_requestRender( * pRect );
}

//____ _onWidgetAppeared() _____________________________________________________

void WgStackPanel::_onWidgetAppeared( WgVectorHook * _pInserted )
{
	WgStackHook * pInserted = (WgStackHook*) _pInserted;

	bool	bRequestResize = false;

	// Check if we need to resize to fit Widget in current width

	int height = pInserted->_widget()->matchingHeight(m_size.w);
	if( height > m_size.h )
		bRequestResize = true;

	// Update bestSize

	WgSize preferred = pInserted->_widget()->preferredSize();

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

	pInserted->_widget()->_onNewSize(m_size);

	// Force a render.

	_onRenderRequested( pInserted );
}

//____ _onWidgetDisappeared() __________________________________________________

void WgStackPanel::_onWidgetDisappeared( WgVectorHook * _pToBeRemoved )
{
	bool	bRequestResize = false;
	WgStackHook * pToBeRemoved = (WgStackHook*) _pToBeRemoved;

	// Get dirty rectangles for all visible sections of pToBeRemoved.

	_onRenderRequested( pToBeRemoved );

	// Update m_preferredSize, skiping pToBeRemoved

	WgSize	preferredSize;
	WgStackHook * pHook = _firstHook();
	while( pHook )
	{
		if( pHook != pToBeRemoved )
		{
			WgSize sz = pHook->_widget()->preferredSize();
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

void WgStackPanel::_onWidgetsReordered()
{
	_requestRender();
}

//____ _refreshAllWidgets() ____________________________________________________

void WgStackPanel::_refreshAllWidgets()
{
	_refreshPreferredSize();
	_adaptChildrenToSize();
	_requestRender();
}

//____ _newHook() _____________________________________________________________

WgVectorHook * WgStackPanel::_newHook()
{
	return new WgStackHook(this);
}

//____ _refreshPreferredSize() _____________________________________________________

void WgStackPanel::_refreshPreferredSize()
{
	WgSize	preferredSize;

	WgStackHook * pHook = _firstHook();
	while( pHook )
	{
		WgSize sz = pHook->_paddedPreferredSize();
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

void WgStackPanel::_adaptChildrenToSize()
{
	WgStackHook * pHook = _firstHook();
	while( pHook )
	{
		pHook->_widget()->_onNewSize( pHook->_getGeo(m_size) );
		pHook = pHook->_next();
	}
}

//____ _firstHookWithGeo() _____________________________________________________

WgHook * WgStackPanel::_firstHookWithGeo( WgRect& writeGeo ) const
{
	WgStackHook * p = _firstHook();
	if( p )
		writeGeo = p->_getGeo(m_size);

	return p;
}

//____ _nextHookWithGeo() ______________________________________________________

WgHook * WgStackPanel::_nextHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const
{
	WgStackHook * p = ((WgStackHook*)pHook)->_next();
	if( p )
		writeGeo = p->_getGeo(m_size);

	return p;
}

//____ _lastHookWithGeo() ______________________________________________________

WgHook * WgStackPanel::_lastHookWithGeo( WgRect& writeGeo ) const
{
	WgStackHook * p = _lastHook();
	if( p )
		writeGeo = p->_getGeo(m_size);

	return p;
}

//____ _prevHookWithGeo() ______________________________________________________

WgHook * WgStackPanel::_prevHookWithGeo( WgRect& writeGeo, WgHook * pHook ) const
{
	WgStackHook * p = ((WgStackHook*)pHook)->_prev();
	if( p )
		writeGeo = p->_getGeo(m_size);

	return p;
}

