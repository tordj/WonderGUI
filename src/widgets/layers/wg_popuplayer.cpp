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

#include <wg_popuplayer.h>
#include <wg_util.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_panel.h>
#include <wg_base.h>
#include <wg_slotarray.impl.h>


namespace wg 
{
	template class SlotArray<PopupSlot>;

	const char PopupLayer::CLASSNAME[] = {"PopupLayer"};

	//____ operator[] __________________________________________________________

	Widget& PopupChildren::operator[](int index) const
	{ 
		return * m_pHolder->m_popups.slot(index)->pWidget; 
	}

	//____ size() ______________________________________________________________
	
	int PopupChildren::size() const 
	{ 
		return m_pHolder->m_popups.size(); 
	} 

	//____ at() _______________________________________________________________
	
	Widget_p PopupChildren::at( int index) const 
	{ 
		if( index < 0 || index >= m_pHolder->m_popups.size() )
			return nullptr;

		return Widget_p(m_pHolder->m_popups.slot(index)->pWidget); 
	}
	
	//____ _object() ___________________________________________________________
	
	Object * PopupChildren::_object() const 
	{	
		return m_pHolder; 
	}


	//____ push() ________________________________________________

	void PopupChildren::push(Widget * _pPopup, Widget * _pOpener, const Rect& _launcherGeo, Origo _attachPoint, Size _maxSize )
	{
		PopupSlot * pSlot = m_pHolder->m_popups.add();
		pSlot->pOpener = _pOpener;
		pSlot->launcherGeo = _launcherGeo;
		pSlot->attachPoint = _attachPoint;
		pSlot->maxSize = _maxSize;
		
		pSlot->replaceWidget(m_pHolder, _pPopup);
		
		m_pHolder->_updateGeo(pSlot);
		m_pHolder->_stealKeyboardFocus();
	}

	//____ pop() ________________________________________________

	void PopupChildren::pop(int nb)
	{
		if( nb <= 0 )
			return;
		
		nb = min(nb, m_pHolder->m_popups.size());
		
		m_pHolder->_removeSlots(nb); 
	}
	
	//____ clear() ________________________________________________
	
	void PopupChildren::clear()
	{
		if( m_pHolder->m_popups.isEmpty() ) 
			return; 

		m_pHolder->_removeSlots(m_pHolder->m_popups.size()); 
	}


	
	//____ Constructor ____________________________________________________________
	
	PopupLayer::PopupLayer() : popups(this)
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	PopupLayer::~PopupLayer()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool PopupLayer::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Layer::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * PopupLayer::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	PopupLayer_p PopupLayer::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return PopupLayer_p( static_cast<PopupLayer*>(pObject) );
	
		return 0;
	}

	//____ _updateGeo() __________________________________________________________
	
	bool PopupLayer::_updateGeo(PopupSlot* pSlot)
	{
		// Get size of parent and correct launcherGeo
	
	
		//
	
		Rect geo(0,0,Size::min(pSlot->pWidget->preferredSize(),Size::min(pSlot->maxSize,m_size)));
	
		switch( pSlot->attachPoint )
		{
			case Origo::NorthEast:					// Right side of launcherGeo, going down.
			{
				geo.x = pSlot->launcherGeo.right();
				geo.y = pSlot->launcherGeo.top();
				break;
			}
	
			case Origo::SouthEast:					// Right side of launcherGeo, going up.
			{
				geo.x = pSlot->launcherGeo.right();
				geo.y = pSlot->launcherGeo.bottom() - geo.h;
				break;
			}
	
			case Origo::NorthWest:					// Left-aligned above launcher.
			{
				geo.x = pSlot->launcherGeo.left();
				geo.y = pSlot->launcherGeo.top() - geo.h;
				break;
			}
	
			case Origo::SouthWest:					// Left-aligned below launcher.
			{
				geo.x = pSlot->launcherGeo.left();
				geo.y = pSlot->launcherGeo.bottom();
				break;
			}
	
			case Origo::West:						// Centered left of launcherGeo.
			{
				geo.x = pSlot->launcherGeo.left() - geo.w;
				geo.y = pSlot->launcherGeo.top() + pSlot->launcherGeo.h/2 - geo.h/2;
				break;
			}
	
			case Origo::North:						// Centered above launcherGeo.
			{
				geo.x = pSlot->launcherGeo.left() + pSlot->launcherGeo.w/2 + geo.w/2;
				geo.y = pSlot->launcherGeo.top() - geo.h;
				break;
			}
	
			case Origo::East:						// Centered right of launcherGeo.
			{
				geo.x = pSlot->launcherGeo.right();
				geo.y = pSlot->launcherGeo.top() + pSlot->launcherGeo.h/2 - geo.h/2;
				break;
			}
	
			case Origo::South:						// Centered below launcherGeo.
			{
				geo.x = pSlot->launcherGeo.left() + pSlot->launcherGeo.w/2 + geo.w/2;
				geo.y = pSlot->launcherGeo.bottom();
				break;
			}
	
		}
	
		// Adjust geometry to fit inside parent.
	
		if( geo.right() > m_size.w )
		{
			if( geo.left() == pSlot->launcherGeo.right() )
			{
				if( pSlot->launcherGeo.left() > m_size.w - pSlot->launcherGeo.right() )
				{
					geo.x = pSlot->launcherGeo.left() - geo.w;
					if( geo.x < 0 )
					{
						geo.x = 0;
						geo.w = pSlot->launcherGeo.left();
					}
				}
				else
					geo.w = m_size.w - geo.x;
			}
			else
				geo.x = m_size.w - geo.w;
		}
	
		if( geo.left() < 0 )
		{
			if( geo.right() == pSlot->launcherGeo.left() )
			{
				if( pSlot->launcherGeo.left() < m_size.w - pSlot->launcherGeo.right() )
				{
					geo.x = pSlot->launcherGeo.right();
					if( geo.right() > m_size.w )
						geo.w = m_size.w - geo.x;
				}
				else
				{
					geo.x = 0;
					geo.w = pSlot->launcherGeo.left();
				}
	
			}
			else
				geo.x = 0;
		}
	
		if( geo.bottom() > m_size.h )
		{
			if( geo.top() == pSlot->launcherGeo.bottom() )
			{
				if( pSlot->launcherGeo.top() > m_size.h - pSlot->launcherGeo.bottom() )
				{
					geo.y = pSlot->launcherGeo.top() - geo.h;
					if( geo.y < 0 )
					{
						geo.y = 0;
						geo.h = pSlot->launcherGeo.top();
					}
				}
				else
					geo.h = m_size.h - geo.y;
			}
			else
				geo.y = m_size.h - geo.h;
		}
	
		if( geo.top() < 0 )
		{
			if( geo.bottom() == pSlot->launcherGeo.top() )
			{
				if( pSlot->launcherGeo.top() < m_size.h - pSlot->launcherGeo.bottom() )
				{
					geo.y = pSlot->launcherGeo.bottom();
					if( geo.bottom() > m_size.h )
						geo.h = m_size.h - geo.y;
				}
				else
				{
					geo.y = 0;
					geo.h = pSlot->launcherGeo.bottom();
				}
			}
			else
				geo.y = 0;
		}
	
		// Update geometry if it has changed.
	
		if( geo != pSlot->geo )
		{
			_onRequestRender(pSlot->geo,pSlot);
			pSlot->geo = geo;
			_onRequestRender(pSlot->geo,pSlot);	
			return true;
		}
		else
			return false;
	}


	//____ _findWidget() ____________________________________________________________
	
	Widget *  PopupLayer::_findWidget( const Coord& ofs, SearchMode mode )
	{
		// MenuPanel has its own _findWidget() method since we need special treatment of
		// searchmode ACTION_TARGET when a menu is open.
	
		if( mode == SearchMode::ActionTarget && !m_popups.isEmpty() )
		{
			// In search mode ACTION_TARGET we limit our target to us, our menu-branches and the menu-opener if a menu is open.
	
			PopupSlot * pSlot = m_popups.last();
			Widget * pResult = 0;
	
			while( pSlot >= m_popups.begin() && !pResult )
			{
				if( pSlot->geo.contains( ofs ) )
				{
					if( pSlot->pWidget->isContainer() )
						pResult = static_cast<Container*>(pSlot->pWidget)->_findWidget( ofs - pSlot->geo.pos(), mode );
					else if( pSlot->pWidget->markTest( ofs - pSlot->geo.pos() ) )
						pResult = pSlot->pWidget;
				}
				pSlot--;
			}
	
			if( pResult == 0 )
			{
				// Check the first opener
				
				PopupSlot * pSlot = m_popups.first();
				if( pSlot->pOpener )
				{
					Widget * pOpener = pSlot->pOpener.rawPtr();
	
					Coord 	absPos 		= ofs + globalPos();
					Rect	openerGeo 	= pOpener->globalGeo();
	
					if( openerGeo.contains(absPos) && pOpener->markTest( absPos - openerGeo.pos() ) )
						pResult = pOpener;
				}
				
				// Fall back to us.
				
				if( pResult == 0 )
					pResult = this;
			}
			return pResult;
		}
		else
		{
			// For the rest of the modes we can rely on the default method.
	
			return Container::_findWidget( ofs, mode );
		}
	}
	
	//____ _onRequestRender() _____________________________________________________
	
	void PopupLayer::_onRequestRender( const Rect& rect, const LayerSlot * pSlot )
	{
		// Clip our geometry and put it in a dirtyrect-list
	
		Patches patches;
		patches.add( Rect( rect, Rect(0,0,m_size)) );
	
		// Remove portions of dirty rect that are covered by opaque upper siblings,
		// possibly filling list with many small dirty rects instead.
	
		PopupSlot * pCover;
	
		if( pSlot )
			pCover = ((PopupSlot*)pSlot)+1;
		else
			pCover = m_popups.first();
	
		while( pCover < m_popups.end() )
		{
			if( pCover->geo.intersectsWith( rect ) )
				pCover->pWidget->_maskPatches( patches, pCover->geo, Rect(0,0,INT_MAX,INT_MAX ), _getBlendMode() );
	
			pCover++;
		}
	
		// Make request render calls
	
		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}
	
	//____ _setSize() ___________________________________________________________
	
	void PopupLayer::_setSize( const Size& sz )
	{
		Layer::_setSize(sz);
	}
	
	//____ _cloneContent() ______________________________________________________
	
	void PopupLayer::_cloneContent( const Widget * _pOrg )
	{
		Layer::_cloneContent( _pOrg );
	}
	
	//____ _receive() ______________________________________________________________
	
	void PopupLayer::_receive( Msg * _pMsg )
	{
		Layer::_receive(_pMsg);
		
	
		switch( _pMsg->type() )
		{
			case MsgType::MouseRelease:
			{
				if (m_popups.isEmpty())
					break;					// Popup was removed already on the press.

				// Allow us to release the mouse within opener without closing any popups

				PopupSlot * pSlot = m_popups.last();
				if (pSlot->pOpener)
				{
					Widget * pOpener = pSlot->pOpener.rawPtr();

					Coord 	absPos = MouseReleaseMsg::cast(_pMsg)->pointerPos();
					Rect	openerGeo = pOpener->globalGeo();

					if (pOpener->markTest(absPos - openerGeo.pos()))
						break;
				}
						
				_removeSlots(m_popups.size());
				_pMsg->swallow();
			}
			break;

			case MsgType::MousePress:
			{
				_removeSlots(m_popups.size());
				_pMsg->swallow();
			}
			break;
	
			case MsgType::KeyPress:
			case MsgType::KeyRepeat:
			{
				KeyMsg_p pMsg = KeyMsg::cast(_pMsg);
	
				if( pMsg->translatedKeyCode() == Key::Escape )
				{
					if( !m_popups.isEmpty() )
					{
						_removeSlots(1);
						_pMsg->swallow();
					}
				}
			}
			break;
            
            default:
                break;
		}
	}
	
	//____ _stealKeyboardFocus() _________________________________________________
	
	void PopupLayer::_stealKeyboardFocus()
	{
		// Verify that we have a root
	
		if( !_root() )
			return;
	
		// Save old keyboard focus, which we assume belonged to previous menu in hierarchy.
	
		if( m_popups.size() < 2 )
			m_pKeyFocus = Base::inputHandler()->focusedWidget().rawPtr();
		else
			m_popups[m_popups.size()-2].pKeyFocus = Base::inputHandler()->focusedWidget().rawPtr();
	
		// Steal keyboard focus to top menu
	
		Widget * pWidget = m_popups.last()->pWidget;

		_childRequestFocus( pWidget->_slot(), pWidget );
	}
	
	//____ _restoreKeyboardFocus() _________________________________________________
	
	void PopupLayer::_restoreKeyboardFocus()
	{
		// Verify that we have a root
	
		if( !_root() )
			return;
	
		//
	
		if( m_popups.isEmpty() )
			_holder()->_childRequestFocus( _slot(), m_pKeyFocus.rawPtr() );
		else
			_holder()->_childRequestFocus( _slot(),  m_popups.last()->pKeyFocus.rawPtr() );
	}

	//____ _childRequestResize() _______________________________________________

	void PopupLayer::_childRequestResize(Slot * pSlot)
	{
		if( pSlot == &m_baseSlot )
			_requestResize();
		else
			_updateGeo( (PopupSlot *) pSlot );
	}

	//____ _removeSlots() __________________________________________________

	void PopupLayer::_removeSlots(int nb)
	{
		MsgRouter * pEH = Base::msgRouter().rawPtr();
	
		PopupSlot * pSlot = m_popups.end() - nb;

		for(int i = 0 ; i < nb ; i++ )
		{
			if( pEH )
				pEH->post( new PopupClosedMsg( pSlot[i].pWidget, pSlot[i].pOpener ) );
			_requestRender(pSlot[i].geo);
		}
			
			
		m_popups.remove(m_popups.size()-nb, nb);
		_restoreKeyboardFocus();
	}

	//____ _beginLayerSlots() __________________________________________________

	LayerSlot * PopupLayer::_beginLayerSlots() const
	{
		return m_popups.begin();
	}

	//____ _endLayerSlots() __________________________________________________

	LayerSlot * PopupLayer::_endLayerSlots() const
	{
		return m_popups.end();
	}

	//____ _sizeOfLayerSlot() __________________________________________________

	int PopupLayer::_sizeOfLayerSlot() const
	{
		return sizeof(PopupSlot);
	}




} // namespace wg
