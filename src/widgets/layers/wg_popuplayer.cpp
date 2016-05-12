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

namespace wg 
{
	
	const char PopupLayer::CLASSNAME[] = {"PopupLayer"};
	const char PopupHook::CLASSNAME[] = {"PopupHook"};
	
	
	
	//_____________________________________________________________________________
	PopupLayer_p PopupHook::parent() const
	{
		return m_pParent;
	}
	
	//_____________________________________________________________________________
	PopupHook::PopupHook( PopupLayer * pParent, Widget * pOpener, const Rect& launcherGeo, Origo attachPoint, Size maxSize )
	{
		m_pParent 		= pParent;
		m_pOpener		= pOpener;
		m_launcherGeo	= launcherGeo;
		m_attachPoint 	= attachPoint;
		m_maxSize 		= maxSize;
	}
	
	//____ PopupHook::isInstanceOf() __________________________________________
	
	bool PopupHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return LayerHook::isInstanceOf(pClassName);
	}
	
	//____ PopupHook::className() _____________________________________________
	
	const char * PopupHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ PopupHook::cast() __________________________________________________
	
	PopupHook_p PopupHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return PopupHook_p( static_cast<PopupHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	//_____________________________________________________________________________
	void PopupHook::_requestRender()
	{
		m_pParent->_onRequestRender( m_geo, this );
	}
	
	//_____________________________________________________________________________
	void PopupHook::_requestRender( const Rect& rect )
	{
		m_pParent->_onRequestRender( rect + m_geo.pos(), this );
	}
	
	//_____________________________________________________________________________
	void PopupHook::_requestResize()
	{
		_updateGeo();					// Just update this menus geo, don't recursively update children, their position should remain where it is.
	}
	
	//_____________________________________________________________________________
	LayerHook * PopupHook::_prevLayerHook() const
	{
		PopupHook * p = _prev();
	
		// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.
	
		if( p )
			return p;
		else
			return 0;
	}
	
	//_____________________________________________________________________________
	LayerHook * PopupHook::_nextLayerHook() const
	{
		PopupHook * p = _next();
	
		// We have multiple inheritance, so lets make the cast in a safe way, preserving NULL-pointer as NULL.
	
		if( p )
			return p;
		else
			return 0;
	}
	
	//_____________________________________________________________________________
	Container * PopupHook::_parent() const
	{
		return m_pParent;
	}
	
	//____ _updateGeo() __________________________________________________________
	
	bool PopupHook::_updateGeo()
	{
		// Get size of parent and correct launcherGeo
	
		Rect 	parentSize = m_pParent->size();
	
		//
	
		Rect geo(0,0,Size::min(_widget()->preferredSize(),Size::min(m_maxSize,parentSize)));
	
		switch( m_attachPoint )
		{
			case Origo::NorthEast:					// Right side of launcherGeo, going down.
			{
				geo.x = m_launcherGeo.right();
				geo.y = m_launcherGeo.top();
				break;
			}
	
			case Origo::SouthEast:					// Right side of launcherGeo, going up.
			{
				geo.x = m_launcherGeo.right();
				geo.y = m_launcherGeo.bottom() - geo.h;
				break;
			}
	
			case Origo::NorthWest:					// Left-aligned above launcher.
			{
				geo.x = m_launcherGeo.left();
				geo.y = m_launcherGeo.top() - geo.h;
				break;
			}
	
			case Origo::SouthWest:					// Left-aligned below launcher.
			{
				geo.x = m_launcherGeo.left();
				geo.y = m_launcherGeo.bottom();
				break;
			}
	
			case Origo::East:						// Centered left of launcherGeo.
			{
				geo.x = m_launcherGeo.left() - geo.w;
				geo.y = m_launcherGeo.top() + m_launcherGeo.h/2 - geo.h/2;
				break;
			}
	
			case Origo::North:						// Centered above launcherGeo.
			{
				geo.x = m_launcherGeo.left() + m_launcherGeo.w/2 + geo.w/2;
				geo.y = m_launcherGeo.top() - geo.h;
				break;
			}
	
			case Origo::West:						// Centered right of launcherGeo.
			{
				geo.x = m_launcherGeo.right();
				geo.y = m_launcherGeo.top() + m_launcherGeo.h/2 - geo.h/2;
				break;
			}
	
			case Origo::South:						// Centered below launcherGeo.
			{
				geo.x = m_launcherGeo.left() + m_launcherGeo.w/2 + geo.w/2;
				geo.y = m_launcherGeo.bottom();
				break;
			}
	
		}
	
		// Adjust geometry to fit inside parent.
	
		if( geo.right() > parentSize.w )
		{
			if( geo.left() == m_launcherGeo.right() )
			{
				if( m_launcherGeo.left() > parentSize.w - m_launcherGeo.right() )
				{
					geo.x = m_launcherGeo.left() - geo.w;
					if( geo.x < 0 )
					{
						geo.x = 0;
						geo.w = m_launcherGeo.left();
					}
				}
				else
					geo.w = parentSize.w - geo.x;
			}
			else
				geo.x = parentSize.w - geo.w;
		}
	
		if( geo.left() < 0 )
		{
			if( geo.right() == m_launcherGeo.left() )
			{
				if( m_launcherGeo.left() < parentSize.w - m_launcherGeo.right() )
				{
					geo.x = m_launcherGeo.right();
					if( geo.right() > parentSize.w )
						geo.w = parentSize.w - geo.x;
				}
				else
				{
					geo.x = 0;
					geo.w = m_launcherGeo.left();
				}
	
			}
			else
				geo.x = 0;
		}
	
		if( geo.bottom() > parentSize.h )
		{
			if( geo.top() == m_launcherGeo.bottom() )
			{
				if( m_launcherGeo.top() > parentSize.h - m_launcherGeo.bottom() )
				{
					geo.y = m_launcherGeo.top() - geo.h;
					if( geo.y < 0 )
					{
						geo.y = 0;
						geo.h = m_launcherGeo.top();
					}
				}
				else
					geo.h = parentSize.h - geo.y;
			}
			else
				geo.y = parentSize.h - geo.h;
		}
	
		if( geo.top() < 0 )
		{
			if( geo.bottom() == m_launcherGeo.top() )
			{
				if( m_launcherGeo.top() < parentSize.h - m_launcherGeo.bottom() )
				{
					geo.y = m_launcherGeo.bottom();
					if( geo.bottom() > parentSize.h )
						geo.h = parentSize.h - geo.y;
				}
				else
				{
					geo.y = 0;
					geo.h = m_launcherGeo.bottom();
				}
			}
			else
				geo.y = 0;
		}
	
		// Update geometry if it has changed.
	
		if( geo != m_geo )
		{
			_requestRender();
			m_geo = geo;
			_requestRender();
	
			return true;
		}
		else
			return false;
	}
	
	//____ Constructor ____________________________________________________________
	
	PopupLayer::PopupLayer()
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
	
	PopupLayer_p PopupLayer::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return PopupLayer_p( static_cast<PopupLayer*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ openPopup() _______________________________________________________________
	
	PopupHook_p PopupLayer::openPopup( const Widget_p& pPopup, const Widget_p& pOpener, const Rect& launcherGeo, Origo attachPoint, Size maxSize )
	{
		// Create MyHook and fill in members.
	
		PopupHook * pHook = new PopupHook( this, pOpener.rawPtr(), launcherGeo, attachPoint, maxSize );
		pHook->_setWidget(pPopup.rawPtr());
		m_popupHooks.pushBack(pHook);
		pHook->_updateGeo();
		_stealKeyboardFocus();
		return pHook;
	}
	
	
	//____ closeAllPopups() ________________________________________________
	
	bool PopupLayer::closeAllPopups()
	{
		PopupHook * pHook = m_popupHooks.first();
		if( pHook )
			closePopup( pHook->_widget() );
	
		return true;
	}
	
	
	//____ closePopup() _________________________________________________________
	
	bool PopupLayer::closePopup( const Widget_p& pWidget )
	{
		if( !pWidget || pWidget->parent() != this || pWidget == m_baseHook.widget() )
			return false;
	
		MsgRouter * pEH = Base::msgRouter().rawPtr();
	
		PopupHook * pHook = (PopupHook *) pWidget->_hook();
	
		while( pHook )
		{
			PopupHook * p = pHook;
			pHook = pHook->_next();
	
			if( pEH )
				pEH->post( new PopupClosedMsg( p->_widget(), p->m_pOpener ) );
	
			p->_requestRender();
			delete p;
		}
		_restoreKeyboardFocus();
		return true;
	}
	
	//____ firstPopupHook() ______________________________________________________
	
	PopupHook_p PopupLayer::firstPopupHook()
	{
		return m_popupHooks.first();
	}
	
	//____ lastPopupHook() _______________________________________________________
	
	PopupHook_p PopupLayer::lastPopupHook()
	{
		return m_popupHooks.last();
	}
	
	//____ _findWidget() ____________________________________________________________
	
	Widget *  PopupLayer::_findWidget( const Coord& ofs, SearchMode mode )
	{
		// MenuPanel has its own _findWidget() method since we need special treatment of
		// searchmode ACTION_TARGET when a menu is open.
	
		if( mode == SearchMode::ActionTarget && !m_popupHooks.isEmpty() )
		{
			// In search mode ACTION_TARGET we limit our target to us, our menu-branches and the menu-opener if a menu is open.
	
			PopupHook * pHook = m_popupHooks.last();
			Widget * pResult = 0;
	
			while( pHook && !pResult )
			{
				if( pHook->m_geo.contains( ofs ) )
				{
					if( pHook->_widget()->isContainer() )
						pResult = static_cast<Container*>(pHook->_widget())->_findWidget( ofs - pHook->m_geo.pos(), mode );
					else if( pHook->_widget()->markTest( ofs - pHook->m_geo.pos() ) )
						pResult = pHook->_widget();
				}
				pHook = pHook->_prev();
			}
	
			if( pResult == 0 )
			{
				// Check the first opener
				
				PopupHook * pHook = m_popupHooks.first();
				if( pHook && pHook->m_pOpener )
				{
					Widget * pOpener = pHook->m_pOpener.rawPtr();
	
					Coord absPos 		= ofs + globalPos();
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
	
	void PopupLayer::_onRequestRender( const Rect& rect, const PopupHook * pHook )
	{
		// Clip our geometry and put it in a dirtyrect-list
	
		Patches patches;
		patches.add( Rect( rect, Rect(0,0,m_size)) );
	
		// Remove portions of dirty rect that are covered by opaque upper siblings,
		// possibly filling list with many small dirty rects instead.
	
		PopupHook * pCover;
	
		if( pHook )
			pCover = pHook->_next();
		else
			pCover = m_popupHooks.first();
	
		while( pCover )
		{
			if( pCover->m_geo.intersectsWith( rect ) )
				pCover->_widget()->_maskPatches( patches, pCover->m_geo, Rect(0,0,65536,65536 ), _getBlendMode() );
	
			pCover = pCover->_next();
		}
	
		// Make request render calls
	
		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}
	
	//____ _setSize() ___________________________________________________________
	
	void PopupLayer::_setSize( const Size& sz )
	{
		Layer::_setSize(sz);
		
		// Update size of base widget
	
		if( m_baseHook._widget() )
			m_baseHook._widget()->_setSize(sz);
	}
	
	//____ _cloneContent() ______________________________________________________
	
	void PopupLayer::_cloneContent( const Widget * _pOrg )
	{
		Layer::_cloneContent( _pOrg );
	}
	
	//____ _receive() ______________________________________________________________
	
	void PopupLayer::_receive( const Msg_p& _pMsg )
	{
		Layer::_receive(_pMsg);
	
		Widget * pOpener = 0;
	
		// Try to find an opener
	
		Object * pSource = _pMsg->sourceRawPtr();
		if( pSource && pSource != this )
		{
			PopupHook * pHook = m_popupHooks.first();
			while( pHook && pHook->_widget() != pSource )
				pHook = pHook->_next();
				
			if( pHook && pHook->m_pOpener )
				pOpener = pHook->m_pOpener.rawPtr();
		}
		
		// First we try to repost message to opener (if any)
	
		if( pOpener )
		{
			_pMsg->setRepost( _pMsg->source().rawPtr(), pOpener );
			return;
		}	
	
		// Secondly we take care of message ourselves if it is addressed to one of our menus or us.
	
		switch( _pMsg->type() )
		{
	/*
			case WG_MSG_MOUSE_POSITION:
	
				if( !m_popupHooks.isEmpty() )							// Process only if we have at least one open menu.
				{
					Coord ofs = _pMsg->pointerPos();
					Widget * p = _findWidget( ofs, SearchMode::ActionTarget );
					if( p != this )
					{
						while( p->parent() != this )
							p = p->parent();
							
						if( p != m_popupHooks.	
					}	
				}
			break;
	*/		
			case MsgType::MouseRelease:
			case MsgType::MousePress:
			{
				MouseButtonMsg_p pMsg = MouseButtonMsg::cast(_pMsg);
	
				Coord ofs = pMsg->pointerPos() - globalPos();
				Widget * p = _findWidget( ofs, SearchMode::ActionTarget );
				if( p == this )
				{
					closeAllPopups();
					_pMsg->swallow();
					return;
				}
			}
			break;
	
			case MsgType::KeyPress:
			case MsgType::KeyRepeat:
			{
				KeyMsg_p pMsg = KeyMsg::cast(_pMsg);
	
				if( pMsg->translatedKeyCode() == Key::Escape )
				{
					if( !m_popupHooks.isEmpty() )
					{
						closePopup( m_popupHooks.last()->_widget() );
						_pMsg->swallow();
						return;
					}
				}
	
			}
			break;
		}
	}
	
	//____ _stealKeyboardFocus() _________________________________________________
	
	void PopupLayer::_stealKeyboardFocus()
	{
		// Verify that we have a root
	
		if( !hook() || !hook()->root() )
			return;
	
		// Save old keyboard focus, which we assume belonged to previous menu in hierarchy.
	
		if( m_popupHooks.size() < 2 )
			m_pKeyFocus = Base::inputHandler()->focusedWidget().rawPtr();
		else
			m_popupHooks.last()->prev()->m_pKeyFocus = Base::inputHandler()->focusedWidget().rawPtr();
	
		// Steal keyboard focus to top menu
	
		Widget * pWidget = m_popupHooks.last()->_widget();

		_hook()->parent()->_focusRequested(_hook(), pWidget);
	}
	
	//____ _restoreKeyboardFocus() _________________________________________________
	
	void PopupLayer::_restoreKeyboardFocus()
	{
		// Verify that we have a root
	
		if( !hook() || !hook()->root() )
			return;
	
		//
	
		if( m_popupHooks.isEmpty() )
			_hook()->parent()->_focusRequested(_hook(), m_pKeyFocus.rawPtr());
		else
			_hook()->parent()->_focusRequested(_hook(), m_popupHooks.last()->m_pKeyFocus.rawPtr());
	}
	

} // namespace wg
