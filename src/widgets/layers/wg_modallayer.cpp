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

	//____ add() _________________________________________________________________

	bool ModalChildren::add(const Widget_p& pWidget, const Rect& geometry, Origo origo)
	{
		ModalSlot * pSlot = m_pSlotArray->add();
		pSlot->geo = geometry;
		pSlot->origo = origo;

		pSlot->replaceWidget(m_pHolder, pWidget.rawPtr());
		m_pHolder->_didAddSlots(pSlot, 1);
		return true;
	}

	//____ moveToBack() ______________________________________________________________

	bool ModalChildren::moveToBack( int index )
	{
		return moveBelow(index, 0);
	}
	
	//____ moveToFront() ______________________________________________________________

	bool ModalChildren::moveToFront(int index)
	{
		return moveAbove(index, m_pSlotArray->size() - 1);
	}

	//____ moveAbove() ______________________________________________________________

	bool ModalChildren::moveAbove(int index, int sibling)
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return false;

		if (sibling < 0 || sibling >= m_pSlotArray->size())
			return false;

		if (index > sibling)
			sibling++;

		if (index != sibling)
			m_pHolder->_moveSlot(index, sibling);

		return true;
	}

	//____ moveBelow() ______________________________________________________________

	bool ModalChildren::moveBelow(int index, int sibling)
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return false;

		if (sibling < 0 || sibling >= m_pSlotArray->size())
			return false;

		if (index < sibling)
			sibling--;

		if (index != sibling)
			m_pHolder->_moveSlot(index, sibling);

		return true;
	}

	//____ setOrigo() ______________________________________________________________

	bool ModalChildren::setOrigo(int index, const Origo origo)
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return false;

		ModalSlot * pSlot = m_pSlotArray->slot(index);

		pSlot->origo = origo;
		return m_pHolder->_refreshRealGeo(pSlot);
	}

	//____ origo() ______________________________________________________________

	Origo ModalChildren::origo(int index) const
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return { Origo::NorthWest };

		return m_pSlotArray->slot(index)->origo;
	}

	//____ setGeo() ______________________________________________________________

	bool ModalChildren::setGeo(int index, const Rect& geometry)
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return false;

		ModalSlot * pSlot = m_pSlotArray->slot(index);

		pSlot->placementGeo = geometry;
		return m_pHolder->_refreshRealGeo(pSlot);
	}

	//____ geo() _________________________________________________________________

	Rect ModalChildren::geo(int index) const
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return { 0,0,0,0 };

		return m_pSlotArray->slot(index)->geo;
	}

	//____ setOfs() ______________________________________________________________

	bool ModalChildren::setOfs(int index, const Coord& ofs)
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return false;

		ModalSlot * pSlot = m_pSlotArray->slot(index);

		pSlot->placementGeo.setPos(ofs);
		return m_pHolder->_refreshRealGeo(pSlot);
	}

	//____ ofs() _________________________________________________________________

	Coord ModalChildren::ofs(int index) const
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return { 0,0 };

		return m_pSlotArray->slot(index)->geo.pos();
	}

	//____ setSize() __________________________________________________________

	bool ModalChildren::setSize(int index, const Size& size)
	{
		if (index < 0 || index >= m_pSlotArray->size())
			return false;

		ModalSlot * pSlot = m_pSlotArray->slot(index);

		pSlot->placementGeo.setSize(size);
		return m_pHolder->_refreshRealGeo(pSlot);
	}


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


	
	//____ _refreshRealGeo() __________________________________________________
	
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
	
		if( newGeo != pSlot->geo )
		{
			_onRequestRender(pSlot->geo, pSlot);
			pSlot->geo = Rect( ofs, sz );
			_onRequestRender(pSlot->geo, pSlot);
		}
	
		return true;
	}
	
	//____ _childRequestResize() ______________________________________________
	void ModalLayer::_childRequestResize( void * pChildRef )
	{
		if( pChildRef == &m_baseSlot )
			_requestResize();
		else
			_refreshRealGeo( (ModalSlot *) pChildRef );
	}
	
	
	//____ Constructor ____________________________________________________________
	
	ModalLayer::ModalLayer() : modals(&m_modals, this)
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
		
	//____ matchingHeight() _______________________________________________________
	
	int ModalLayer::matchingHeight( int width ) const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int ModalLayer::matchingWidth( int height ) const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}
	
	//____ preferredSize() _____________________________________________________________
	
	Size ModalLayer::preferredSize() const
	{
		if( m_baseSlot.pWidget )
			return m_baseSlot.pWidget->preferredSize();
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
			ModalSlot * pSlot = m_modals.last();
	
			if( pSlot )
			{
				if( pSlot->pWidget->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(pSlot->pWidget)->_findWidget( ofs - pSlot->geo.pos(), mode );
					if( pResult )
						return pResult;
				}
				else
				{
					if( pSlot->pWidget->markTest(ofs - pSlot->geo.pos()) )
						return pSlot->pWidget;
					else
						return this;
				}
			}
			else if( m_baseSlot.pWidget )
			{
				if( m_baseSlot.pWidget->isContainer() )
				{
					Widget * pResult = static_cast<Container*>(m_baseSlot.pWidget)->_findWidget( ofs, mode );
					if( pResult )
						return pResult;
				}
				else
					return m_baseSlot.pWidget;
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
			if( p == m_baseSlot.pWidget )
				m_pBaseKeyFocus = pFocused;
			else
			{
				ModalSlot * pSlot = static_cast<ModalSlot*>(reinterpret_cast<Slot*>(p->_holdersRef()));
				pSlot->pKeyFocus = pFocused;
			}
		}
	
		// Find which child-branch to focus and switch to our previously saved focus
	
		ModalSlot * pSlot = m_modals.last();
	
		Widget * 	pSavedFocus = nullptr;
		Slot *		pBranch	= nullptr;

		while( pSlot > m_modals.first() && pSlot->geo.isEmpty() )
			pSlot--;
	
		if( pSlot > m_modals.first() )
		{
			pSavedFocus = pSlot->pKeyFocus.rawPtr();
			pSlot->pKeyFocus = nullptr;								// Needs to be cleared for the future.
			pBranch = pSlot;
		}
		else if( m_baseSlot.pWidget )
		{
			pSavedFocus = m_pBaseKeyFocus.rawPtr();
			m_pBaseKeyFocus = 0;								// Needs to be cleared for the future.
			pBranch = &m_baseSlot;
		}
	
		// Verify that saved focus still is within branch and is not hidden
	
		Widget * pW = pSavedFocus;
		while( pW && pW->_parent() != this )
			pW = pW->_parent();

		if( pW )
		{
			Slot * pSlot = (Slot*) pW->_holdersRef();
			if( pSlot != pBranch )
				pSavedFocus = 0;				// Previously focused Widget is no longer a child of focused branch.
		}
	
		// Switch to previously saved focus, or null if not applicable

		_holder()->_childRequestFocus( m_pHoldersRef, pSavedFocus );
	}

	//____ _moveSlot() ___________________________________________________________

	void ModalLayer::_moveSlot(int oldPos, int newPos)
	{
		m_modals.move(oldPos, newPos);

		ModalSlot * pOther = m_modals.slot(oldPos);				// This is correct, we have already switched places...
		ModalSlot * pIndex = m_modals.slot(newPos);

		if (pIndex > pOther)			// We were moved forward
		{
			// Request render on all areas covered by siblings we have skipped in front of.

			ModalSlot * p = pOther;
			while (p < pIndex)
			{
				Rect cover(pIndex->geo, p->geo);

				if (!cover.isEmpty())
					_onRequestRender(cover, pIndex);
				p++;
			}
		}
		else							// Move us backward
		{
			// Request render on our siblings for the area we previously have covered.

			ModalSlot * p = pIndex + 1;
			while (p <= pOther)
			{
				Rect cover(pIndex->geo, p->geo);

				if (!cover.isEmpty())
					_onRequestRender(cover, p);
				p++;
			}
		}
	}


	//____ _didAddSlots() __________________________________________________

	void ModalLayer::_didAddSlots(Slot * _pSlot, int nb)
	{
		ModalSlot * pSlot = (ModalSlot*) _pSlot;
		for( int i = 0 ; i < nb ; i++ )
			_refreshRealGeo(&pSlot[i]);

		_updateKeyboardFocus();
	}

	//____ _willRemoveSlots() __________________________________________________

	void ModalLayer::_willRemoveSlots(Slot * _pSlot, int nb)
	{
		ModalSlot * pSlot = (ModalSlot*) _pSlot;
		for( int i = 0 ; i < nb ; i++ )
		{
			_childRequestRender(pSlot+i);
			pSlot[i].geo = { 0,0,0,0 };			
		}

		_updateKeyboardFocus();
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
				
		// Refresh modal widgets geometry, their positions might have changed.
	
		for( auto pSlot = m_modals.begin() ; pSlot != m_modals.end() ; pSlot++ )
			_refreshRealGeo( pSlot );
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
			
			if( !m_modals.isEmpty() && _findWidget( pMsg->pointerPos(), SearchMode::ActionTarget ) == this )
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
