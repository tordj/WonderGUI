/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_modallayer.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_base.h>

namespace wg 
{
	
	const char ModalLayer::CLASSNAME[] = {"ModalLayer"};
	
	
	//TODO: Improve ModalHook geometry handling, should be able to run on PreferredSize by default, answering to resize-requests.
	
	
	
	//_____________________________________________________________________________
	void ModalHook::top()
	{
		_moveLast();
		m_pWidget->_requestRender();
		m_pParent->_updateKeyboardFocus();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setGeo( const Rect& geometry, Origo origo )
	{
		m_placementGeo 	= geometry;
		m_origo 		= origo;
	
		if( m_placementGeo.w <= 0 )
			m_placementGeo.w = 1;
	
		if( m_placementGeo.h <= 0 )
			m_placementGeo.h = 1;
	
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setGeo( const Coord& ofs, Origo origo )
	{
		m_placementGeo.setPos(ofs);
		m_origo	= origo;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setOfs( const Coord& ofs )
	{
		m_placementGeo.setPos(ofs);
		return _refreshRealGeo();
	}
	
		
	//_____________________________________________________________________________
	bool ModalHook::setSize( Size sz )
	{
		if( sz.w < 0 || sz.h < 0 )
			return false;
	
		m_placementGeo.setSize( sz );
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setWidth( int width )
	{
		if( width < 0 )
			return false;
	
		m_placementGeo.w = width;
		return _refreshRealGeo();
	}
	
	//_____________________________________________________________________________
	bool ModalHook::setHeight( int height )
	{
		if( height < 0 )
			return false;
	
		m_placementGeo.h = height;
		return _refreshRealGeo();
	}
	


	bool ModalLayer::add( const Widget_p& pWidget, const Rect& geometry, Origo origo = Origo::NorthWest )
	bool ModalLayer::add( const Widget_p& pWidget, const Coord& pos, Origo origo = Origo::NorthWest ) { addModalWidget( pWidget, Rect(pos,0,0), origo); }

	bool ModalLayer::moveToBack( int index )
	{
			
	}
	
	bool ModalChildren::moveToFront( int index )
	bool ModalChildren::moveAbove( int index, int otherWidget );
	bool ModalChildren::moveBelow( int index, int otherWidget );

	bool ModalChildren::setOrigo( int index, const FlexPos& origo );
	Origo ModalChildren::origo( int index ) const;
	
	bool ModalChildren::setGeo( int index, const Rect& geometry );
	Rect ModalChildren::geo( int index ) const;

	bool ModalChildren::setOfs( int index, const Coord& ofs );
	Coord ModalChildren::ofs( int index ) const;

	bool ModalChildren::setSize( int index, const Size& size );


	//____ size() ______________________________________________________________

	Size ModalChildren::size( int index ) const
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return {0,0};
		
		return m_pSlotArray->slot(index)->geo.size();			
	}

	//____ move() ______________________________________________________________

	bool ModalChildren::move( int index, const Coord& ofs )
	{
		if( index < 0 || index >= m_pSlotArray->size() )
			return false;
		
		ModalSlot * pSlot = m_pSlotArray->slot(index);		
	
		pSlot->placementGeo += ofs;
		return m_pHolder->_refreshRealGeo( pSlot );		
	}


	
	//_____________________________________________________________________________
	
	bool ModalLayer::_refreshRealGeo( ModalSlot * pSlot )	// Return false if we couldn't get exactly the requested (floating) geometry.
	{
		Size sz = pSlot->placementGeo.size();
	
		if( sz.w == 0 && sz.h == 0 )
			sz = pSlot->pWidget->preferredSize();
		else if( sz.w == 0 )
			sz.w = pSlot->pWidget->matchingWidth(sz.h);
		else if( sz.h == 0 )
			sz.h = pSlot->pWidget->matchingHeight(sz.w);
	
		if( sz.w <= 0 )
			sz.w = 1;
		if( sz.h <= 0 )
			sz.h = 1;
	
		Coord ofs = Util::origoToOfs( pSlot->origo, m_size ) - Util::origoToOfs( pSlot->origo, sz );
		ofs += pSlot->placementGeo.pos();
	
		Rect newGeo( ofs, sz );
	
		if( newGeo != m_geo )
		{
			_onRequestRender(pSlot->geo, pSlot);
			pSlot->geo = Rect( ofs, sz );
			_onRequestRender(pSlot->geo, pSlot);
		}
	
		return true;
	}
	
	//_____________________________________________________________________________
	void ModalLayer::_childRequestResize( void * pChildRef )
	{
		if( pChildRef == m_baseSlot )
			_requestResize();
		else
			_refreshRealGeo( (ModalSlot *) pChildRef );
	}
	
	
	//____ Constructor ____________________________________________________________
	
	ModalLayer::ModalLayer()
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	ModalLayer::~ModalLayer()
	{
		// Children are deleted automaticallly when their hooks are deteled.
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ModalLayer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Layer::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ModalLayer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ModalLayer_p ModalLayer::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ModalLayer_p( static_cast<ModalLayer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ addModalWidget() ________________________________________________________
	
	ModalHook_p ModalLayer::addModalWidget( const Widget_p& pWidget, const Rect& geometry, Origo origo )
	{
		// Create MyHook and fill in members.
	
		ModalHook * pHook = new ModalHook( this );
		pHook->_setWidget(pWidget.rawPtr());
		pHook->m_origo = origo;
		pHook->m_placementGeo = geometry;
		m_modalHooks.pushBack(pHook);
	
		pWidget->_setHolder( this, (Hook*) pHook );

		// Refresh geometry and request render.
	
		pHook->_refreshRealGeo();
		_updateKeyboardFocus();
		return pHook;
	}
	
	//____ removeModalWidgets() ________________________________________________
	
	bool ModalLayer::removeModalWidgets()
	{
		ModalHook * pHook = m_modalHooks.first();
		while( pHook )
		{
			pHook->_widget()->_setHolder( nullptr, nullptr );
			pHook = pHook->_next();
		}

		m_modalHooks.clear();
		_requestRender();
		_updateKeyboardFocus();
		return true;
	}
	
	//____ removeChild() _________________________________________________________
	
	bool ModalLayer::removeChild( const Widget_p& pWidget )
	{
		if( !pWidget || pWidget->parent() != this )
			return false;
	
		if( pWidget == m_baseHook._widget() )
			return removeBaseWidget();
		else
		{
			ModalHook * pHook = (ModalHook *) reinterpret_cast<Hook*>(pWidget->_holdersRef());
			pWidget->_setHolder( nullptr, nullptr );
			_childRequestRender( (Hook*) pHook );
			delete pHook;
			_updateKeyboardFocus();
			return true;
		}
	
	}
	
	//____ clear() ________________________________________________________________
	
	bool ModalLayer::clear()
	{
		removeBaseWidget();
		removeModalWidgets();
		return true;
	}
	
	
	//____ matchingHeight() _______________________________________________________
	
	int ModalLayer::matchingHeight( int width ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int ModalLayer::matchingWidth( int height ) const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size ModalLayer::preferredSize() const
	{
		if( m_baseHook._widget() )
			return m_baseHook._widget()->preferredSize();
		else
			return Size(1,1);
	}
	
	//____ _findWidget() ____________________________________________________________
	
	Widget *  ModalLayer::_findWidget( const Coord& ofs, SearchMode mode )
	{
		// In search mode ACTION_TARGET we always return either the topmost non-hidden modal Widget (or its children),
		// or us.
	
		if( mode == SearchMode::ActionTarget )
		{
			ModalHook * pHook = m_modalHooks.last();
	
			if( pHook )
			{
				if( pHook->_widget()->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(pHook->_widget())->_findWidget( ofs - pHook->pos(), mode );
					if( pResult )
						return pResult;
				}
				else
				{
					if( pHook->_widget()->markTest(ofs - pHook->pos()) )
						return pHook->_widget();
					else
						return this;
				}
			}
			else if( m_baseHook._widget() )
			{
				if( m_baseHook._widget()->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(m_baseHook._widget())->_findWidget( ofs - m_baseHook.pos(), mode );
					if( pResult )
						return pResult;
				}
				else
					return m_baseHook._widget();
			}
			else
				return 0;
		}
	
		// For the rest of the modes we can rely on the default method.
	
		return Container::_findWidget( ofs, mode );
	}
	
	//____ _updateKeyboardFocus() _______________________________________________________
	
	void ModalLayer::_updateKeyboardFocus()
	{
		// Get message handler, verify that we have a root
	
		if( !_holder() || !_holder()->_root() )
			return;
		
		// Retrieve focused Widget and verify it being a descendant to us.
	
		Widget * pFocused = Base::inputHandler()->focusedWidget().rawPtr();
	
		Widget * p = pFocused;
		while( p && p->parent() && p->parent() != this )
			p = p->_parent();
	
		if( p && p->_parent() != this )
			return;								// Focus belongs to a Widget that is not a descendant to us,
												// so we can't save and shouldn't steal focus.
	
		// Save old focus so we can return it properly in the future.
		if( p )
		{
			if( p == m_baseHook._widget() )
				m_pBaseKeyFocus = pFocused;
			else
			{
				ModalHook * pHook = static_cast<ModalHook*>(reinterpret_cast<Hook*>(p->_holdersRef()));
				pHook->m_pKeyFocus = pFocused;
			}
		}
	
		// Find which child-branch to focus and switch to our previously saved focus
	
		ModalHook * pHook = m_modalHooks.last();
	
		Widget * 	pSavedFocus = 0;
		Hook *	pBranch	= 0;
	
		if( pHook )
		{
			pSavedFocus = pHook->m_pKeyFocus.rawPtr();
			pHook->m_pKeyFocus = 0;								// Needs to be cleared for the future.
			pBranch = pHook;
		}
		else if( m_baseHook._widget() )
		{
			pSavedFocus = m_pBaseKeyFocus.rawPtr();
			m_pBaseKeyFocus = 0;								// Needs to be cleared for the future.
			pBranch = &m_baseHook;
		}
	
		// Verify that saved focus still is within branch and is not hidden
	
		Widget * pW = pSavedFocus;
		while( pW && pW->_parent() != this )
			pW = pW->_parent();

		if( pW )
		{
			Hook * pHook = (Hook*) pW->_holdersRef();
			if( pHook != pBranch )
				pSavedFocus = 0;				// Previously focused Widget is no longer a child of focused branch.
		}
	
		// Switch to previously saved focus, or null if not applicable

		_holder()->_childRequestFocus( m_pHoldersRef, pSavedFocus );
	}

	//____ _beginLayerSlots() __________________________________________________
	
	LayerSlot * ModalLayer::_beginLayerSlots() const
	{
		return m_modals.begin();
	}

	//____ _endLayerSlots() ____________________________________________________

	LayerSlot *  ModalLayer::_endLayerSlots() const
	{
		return m_modals.end();
	}

	//____ _sizeOfLayerSlot() __________________________________________________

	int ModalLayer::_sizeOfLayerSlot() const
	{
		return sizeof(ModalSlot);
	}
	
	
	//____ _setSize() ___________________________________________________________
	
	void ModalLayer::_setSize( const Size& sz )
	{
		Layer::_setSize(sz);
			
		// Update size of base widget
	
		if( m_baseHook._widget() )
			m_baseHook._widget()->_setSize(sz);
	
		// Refresh modal widgets geometry, their positions might have changed.
	
		ModalHook * pHook = m_modalHooks.first();
	
		while( pHook )
		{
			pHook->_refreshRealGeo();
			pHook = pHook->_next();
		}
	}
	
	//____ _cloneContent() ______________________________________________________
	
	void ModalLayer::_cloneContent( const Widget * _pOrg )
	{
		Layer::_cloneContent( _pOrg );
	}
	
	//____ _receive() ______________________________________________________________
	
	void ModalLayer::_receive( const Msg_p& _pMsg )
	{
		Layer::_receive(_pMsg);
	
		if( _pMsg->isInstanceOf( InputMsg::CLASSNAME ) )
		{
			InputMsg_p pMsg = InputMsg::cast(_pMsg);
			
			if( !m_modalHooks.isEmpty() && _findWidget( pMsg->pointerPos(), SearchMode::ActionTarget ) == this )
			{
				switch( pMsg->type() )
				{
					case MsgType::MousePress:
					{
						MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
						Base::msgRouter()->post( new ModalBlockedPressMsg( pMsg->inputId(), pMsg->button(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;
		
					case MsgType::MouseRelease:
					{
						MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
						Base::msgRouter()->post( new ModalBlockedPressMsg( pMsg->inputId(), pMsg->button(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;
		
					case MsgType::MouseMove:
					{
						Base::msgRouter()->post( new ModalMoveOutsideMsg( pMsg->inputId(), this, pMsg->modKeys(), pMsg->pointerPos(), pMsg->timestamp()) );
					}
					break;
                    
                    default:
                        break;
				}
			}
		}
	}
	

} // namespace wg
