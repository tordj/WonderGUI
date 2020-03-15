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
#include <wg_inputhandler.h>
#include <wg_internal.h>
#include <wg_cstaticslotvector.impl.h>

#include <algorithm>

namespace wg
{
	using namespace Util;
	template class CStaticSlotVector<PopupLayer::Slot>;

	const char PopupLayer::CLASSNAME[] = {"PopupLayer"};


	//____ pushFront() ________________________________________________

	void PopupLayer::CSlots::pushFront(const Widget_p& _pPopup, Widget * _pOpener, const Rect& _launcherGeo, Origo _attachPoint, bool _bAutoClose, Size _maxSize )
	{
		_pPopup->releaseFromParent();
		_holder()->_addSlot( _pPopup, _pOpener, qpixToRaw(_launcherGeo), _attachPoint, _bAutoClose, qpixToRaw(_maxSize));
	}

	//____ pop() ________________________________________________

	void PopupLayer::CSlots::pop(int nb)
	{
		if( nb <= 0 )
			return;

		nb = std::min(nb, size());

		_holder()->_removeSlots(0,nb);
	}

	//____ pop() ________________________________________________

	void PopupLayer::CSlots::pop(Widget * pPopup)
	{
		int i = index(pPopup);
		if (i > -1)
			_holder()->_removeSlots(0,i+1);
	}

	//____ clear() ________________________________________________

	void PopupLayer::CSlots::clear()
	{
		if( isEmpty() )
			return;

		_holder()->_removeSlots(0,size());
	}



	//____ Constructor ____________________________________________________________

	PopupLayer::PopupLayer() : popupSlots(this)
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

	//____ _updateGeo() __________________________________________________________

	void PopupLayer::_updateGeo(Slot* pSlot, bool bForceResize)
	{
		// Get size of parent and correct launcherGeo


		//

		RectI geo(0,0,SizeI::min(pSlot->_preferredSize(),SizeI::min(pSlot->m_maxSize,m_size)));

		switch( pSlot->m_attachPoint )
		{
			case Origo::NorthEast:					// Right side of launcherGeo, going down.
			{
				geo.x = pSlot->m_launcherGeo.right();
				geo.y = pSlot->m_launcherGeo.top();
				break;
			}

			case Origo::SouthEast:					// Right side of launcherGeo, going up.
			{
				geo.x = pSlot->m_launcherGeo.right();
				geo.y = pSlot->m_launcherGeo.bottom() - geo.h;
				break;
			}

			case Origo::NorthWest:					// Left-aligned above launcher.
			{
				geo.x = pSlot->m_launcherGeo.left();
				geo.y = pSlot->m_launcherGeo.top() - geo.h;
				break;
			}

			case Origo::SouthWest:					// Left-aligned below launcher.
			{
				geo.x = pSlot->m_launcherGeo.left();
				geo.y = pSlot->m_launcherGeo.bottom();
				break;
			}

			case Origo::West:						// Centered left of launcherGeo.
			{
				geo.x = pSlot->m_launcherGeo.left() - geo.w;
				geo.y = pSlot->m_launcherGeo.top() + pSlot->m_launcherGeo.h/2 - geo.h/2;
				break;
			}

			case Origo::North:						// Centered above launcherGeo.
			{
				geo.x = pSlot->m_launcherGeo.left() + pSlot->m_launcherGeo.w/2 + geo.w/2;
				geo.y = pSlot->m_launcherGeo.top() - geo.h;
				break;
			}

			case Origo::East:						// Centered right of launcherGeo.
			{
				geo.x = pSlot->m_launcherGeo.right();
				geo.y = pSlot->m_launcherGeo.top() + pSlot->m_launcherGeo.h/2 - geo.h/2;
				break;
			}

			case Origo::South:						// Centered below launcherGeo.
			{
				geo.x = pSlot->m_launcherGeo.left() + pSlot->m_launcherGeo.w/2 + geo.w/2;
				geo.y = pSlot->m_launcherGeo.bottom();
				break;
			}

			default:
				assert(false);
				break;

		}

		// Adjust geometry to fit inside parent.

		if( geo.right() > m_size.w )
		{
			if( geo.left() == pSlot->m_launcherGeo.right() )
			{
				if( pSlot->m_launcherGeo.left() > m_size.w - pSlot->m_launcherGeo.right() )
				{
					geo.x = pSlot->m_launcherGeo.left() - geo.w;
					if( geo.x < 0 )
					{
						geo.x = 0;
						geo.w = pSlot->m_launcherGeo.left();
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
			if( geo.right() == pSlot->m_launcherGeo.left() )
			{
				if( pSlot->m_launcherGeo.left() < m_size.w - pSlot->m_launcherGeo.right() )
				{
					geo.x = pSlot->m_launcherGeo.right();
					if( geo.right() > m_size.w )
						geo.w = m_size.w - geo.x;
				}
				else
				{
					geo.x = 0;
					geo.w = pSlot->m_launcherGeo.left();
				}

			}
			else
				geo.x = 0;
		}

		if( geo.bottom() > m_size.h )
		{
			if( geo.top() == pSlot->m_launcherGeo.bottom() )
			{
				if( pSlot->m_launcherGeo.top() > m_size.h - pSlot->m_launcherGeo.bottom() )
				{
					geo.y = pSlot->m_launcherGeo.top() - geo.h;
					if( geo.y < 0 )
					{
						geo.y = 0;
						geo.h = pSlot->m_launcherGeo.top();
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
			if( geo.bottom() == pSlot->m_launcherGeo.top() )
			{
				if( pSlot->m_launcherGeo.top() < m_size.h - pSlot->m_launcherGeo.bottom() )
				{
					geo.y = pSlot->m_launcherGeo.bottom();
					if( geo.bottom() > m_size.h )
						geo.h = m_size.h - geo.y;
				}
				else
				{
					geo.y = 0;
					geo.h = pSlot->m_launcherGeo.bottom();
				}
			}
			else
				geo.y = 0;
		}

		// Update geometry if it has changed.

		if (geo != pSlot->m_geo)
		{
			_onRequestRender(pSlot->m_geo, pSlot);
			pSlot->m_geo = geo;
			_onRequestRender(pSlot->m_geo, pSlot);
		}

		if (bForceResize || pSlot->_size() != geo.size())
			pSlot->_setSize(geo);
	}


	//____ _findWidget() ____________________________________________________________

	Widget *  PopupLayer::_findWidget( const CoordI& ofs, SearchMode mode )
	{
		// MenuPanel has its own _findWidget() method since we need special treatment of
		// searchmode ACTION_TARGET when a menu is open.

		if( mode == SearchMode::ActionTarget && !popupSlots.isEmpty() )
		{
			// In search mode ACTION_TARGET we limit our target to us, our menu-branches and the menu-opener if a menu is open.

			Slot * pSlot = popupSlots._begin();
			Widget * pResult = 0;

			while( pSlot != popupSlots._end() && !pResult )
			{
				if( pSlot->m_geo.contains( ofs ) )
				{
					if( pSlot->_widget()->isContainer() )
						pResult = static_cast<OContainer*>(pSlot->_widget())->_findWidget( ofs - pSlot->m_geo.pos(), mode );
					else if( pSlot->_markTest( ofs - pSlot->m_geo.pos() ) )
						pResult = pSlot->_widget();
				}
				pSlot++;
			}

			if( pResult == 0 )
			{
				// Check the root opener

				Slot * pSlot = popupSlots._last();
				if( pSlot->m_pOpener )
				{
					OWidget * pOpener = OO(pSlot->m_pOpener.rawPtr());

					CoordI 	absPos 		= ofs + _globalPos();
					RectI	openerGeo 	= Util::qpixToRaw(pOpener->globalGeo());

					if( openerGeo.contains(absPos) && pOpener->_markTest( absPos - openerGeo.pos() ) )
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

	void PopupLayer::_onRequestRender( const RectI& rect, const Layer::Slot * pSlot )
	{
		// Don not render anything if not visible

		if (pSlot && ((Slot*)pSlot)->m_state == Slot::State::OpeningDelay)
			return;

		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add( RectI( rect, RectI(0,0,m_size)) );

		// Remove portions of dirty rect that are covered by opaque upper siblings,
		// possibly filling list with many small dirty rects instead.

		if( !popupSlots.isEmpty() )
		{
			Slot * pCover;

			if (pSlot)
				pCover = ((Slot*)pSlot) - 1;
			else
				pCover = popupSlots._last();

			while (pCover >= popupSlots._begin())
			{
				if (pCover->m_geo.intersectsWith(rect) && pCover->m_state != Slot::State::OpeningDelay && pCover->m_state != Slot::State::Opening && pCover->m_state != Slot::State::Closing)
					OO(pCover->_widget())->_maskPatches(patches, pCover->m_geo, RectI(0, 0, INT_MAX, INT_MAX), _getBlendMode());

				pCover--;
			}
		}
		// Make request render calls

		for( const RectI * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _render() ___________________________________________________

	class WidgetRenderContext
	{
	public:
		WidgetRenderContext() : pSlot(0) {}
		WidgetRenderContext(PopupLayer::Slot * pSlot, const RectI& geo) : pSlot(pSlot), geo(geo) {}

		PopupLayer::Slot *	pSlot;
		RectI		geo;
		ClipPopData clipPop;
	};

	void PopupLayer::_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window)
	{
		// Render container itself

		if( m_pSkin )
			m_pSkin->_render(pDevice, _canvas, m_state);

		// Render children

		RectI	dirtBounds = pixelsToRaw( pDevice->clipBounds() );

		// Create WidgetRenderContext's for popups that might get dirty patches

		std::vector<WidgetRenderContext> renderList;

		for( auto pSlot = popupSlots._begin() ; pSlot != popupSlots._end() ; pSlot++ )
		{
			RectI geo = pSlot->m_geo + _canvas.pos();

			if (geo.intersectsWith(dirtBounds) && pSlot->m_state != Slot::State::OpeningDelay)
				renderList.push_back(WidgetRenderContext(pSlot, geo));
		}

		// Collect dirty patches from gfxDevice

		int nClipRects = pDevice->clipListSize();
		auto pClipRects = pDevice->clipList();
		Patches patches( nClipRects );

		for( int i = 0 ; i < nClipRects ; i++ )
			patches.push(pixelsToRaw(pClipRects[i]));

		// Go through WidgetRenderContexts, push and mask dirt

		for (unsigned int i = 0; i < renderList.size(); i++)
		{
			WidgetRenderContext * p = &renderList[i];

			p->clipPop = patchesToClipList(pDevice, p->geo, patches);
			if( p->pSlot->m_state != Slot::State::Opening && p->pSlot->m_state != Slot::State::Closing )
				OO(p->pSlot->_widget())->_maskPatches(patches, p->geo, p->geo, pDevice->blendMode());		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

			if (patches.isEmpty())
				break;
		}

		// Any dirt left in patches is for base child, lets render that first

		if ( mainSlot._widget() && !patches.isEmpty())
		{
			ClipPopData popData = patchesToClipList(pDevice, _window, patches);
			OO(mainSlot._widget())->_render(pDevice, _canvas, _window);
			popClipList(pDevice, popData);
		}

		// Go through WidgetRenderContexts and render the patches in reverse order (topmost popup rendered last).

		for (int i = int(renderList.size()) - 1; i >= 0; i--)
		{
			WidgetRenderContext * p = &renderList[i];

			Color tint = Color::White;

			if (p->pSlot->m_state == Slot::State::Opening)
				tint.a = 255 * p->pSlot->m_stateCounter / m_openingFadeMs;

			if (p->pSlot->m_state == Slot::State::Closing)
				tint.a = 255 - (255 * p->pSlot->m_stateCounter / m_closingFadeMs);

			if (tint.a == 255)
				OO(p->pSlot->_widget())->_render(pDevice, p->geo, p->geo);
			else
			{
				Color oldTint = pDevice->tintColor();
				pDevice->setTintColor(oldTint*tint);
				OO(p->pSlot->_widget())->_render(pDevice, p->geo, p->geo);
				pDevice->setTintColor(oldTint);
			}
			popClipList(pDevice,p->clipPop);
		}
	}


	//____ _maskPatches() _____________________________________________________

//	void PopupLayer::_maskPatches(Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode)
//	{
//		Need to except children that are in states OpeningDelay, Opening and Closing.
//	}

	//____ _collectPatches() ___________________________________________________

//	void PopupLayer::_collectPatches(Patches& container, const RectI& geo, const RectI& clip)
//	{
//		Need to make sure patches are not collected for children in mode "OpeningDelay"
//		This might be handled by slotWithGeo() methods, depending on how what we choose.
//	}



	//____ _resize() ___________________________________________________________

	void PopupLayer::_resize( const SizeI& sz )
	{
		Layer::_resize(sz);
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

		int ms = static_cast<TickMsg*>(_pMsg)->timediff();

		switch( _pMsg->type() )
		{
			case MsgType::Tick:

				// Update state for all open popups

				for (Slot* pSlot = popupSlots._begin() ; pSlot != popupSlots._end() ; pSlot++)
				{
					Slot& popup = *pSlot;

					switch (popup.m_state)
					{
					case Slot::State::OpeningDelay:
						if (popup.m_stateCounter + ms < m_openingDelayMs)
						{
							popup.m_stateCounter += ms;
							break;
						}
						else
						{
							popup.m_state = Slot::State::Opening;
							popup.m_stateCounter -= m_openingDelayMs;
							// No break here, let's continue down to opening...
						}
					case Slot::State::Opening:
						popup.m_stateCounter += ms;
						_requestRender(popup.m_geo);
						if (popup.m_stateCounter >= m_openingFadeMs)
						{
							popup.m_stateCounter = 0;
							popup.m_state = popup.m_bAutoClose ? Slot::State::PeekOpen : Slot::State::FixedOpen;
						}
						break;

					case Slot::State::ClosingDelay:
						if (popup.m_stateCounter + ms < m_closingDelayMs)
						{
							popup.m_stateCounter += ms;
							break;
						}
						else
						{
							popup.m_state = Slot::State::Closing;
							popup.m_stateCounter -= m_closingDelayMs;
							// No break here, let's continue down to closing...
						}
					case Slot::State::Closing:
						popup.m_stateCounter += ms;
						_requestRender(popup.m_geo);
						// Removing any closed popups is done in next loop
						break;
					default:
						break;
					}

				}

				// Close any popup that is due for closing.

				while (!popupSlots.isEmpty() && popupSlots._first()->m_state == Slot::State::Closing && popupSlots._first()->m_stateCounter >= m_closingFadeMs)
					_removeSlots(0, 1);

			break;

			case MsgType::MouseEnter:
			case MsgType::MouseMove:
			{
				if (popupSlots.isEmpty())
					break;

				CoordI 	pointerPos = static_cast<InputMsg*>(_pMsg)->pointerPosRaw() - _globalPos();

				// Top popup can be in state PeekOpen, which needs special attention.

				Slot * pSlot = popupSlots._first();
				if (pSlot && pSlot->m_state == Slot::State::PeekOpen)
				{
					// Promote popup to state WeakOpen if pointer has entered its geo,
					// otherwise begin delayed closing if pointer has left launcherGeo.

					if (pSlot->m_geo.contains(pointerPos))
						pSlot->m_state = Slot::State::WeakOpen;
					else if (!pSlot->m_launcherGeo.contains(pointerPos))
					{
						pSlot->m_state = Slot::State::ClosingDelay;
						pSlot->m_stateCounter = 0;
					}
				}

				// A popup in state ClosingDelay should be promoted to
				// state PeekOpen if pointer has entered its launcherGeo and
				// to state WeakOpen if pointer has entered its geo.
				// Promoting to WeakOpen Should also promote any ancestor also in state ClosingDelay.

				for (Slot * pSlot = popupSlots._begin() ; pSlot != popupSlots._end() ; pSlot++)
				{
					Slot& popup = *pSlot;

					if (popup.m_state == Slot::State::ClosingDelay)
					{
						if (popup.m_launcherGeo.contains(pointerPos))
						{
							popup.m_state = Slot::State::PeekOpen;
							popup.m_stateCounter = 0;
						}
						else if (popup.m_geo.contains(pointerPos))
						{
							Slot * p = &popup;
							while (p != popupSlots._end() && p->m_state == Slot::State::ClosingDelay)
							{
								p->m_state = Slot::State::WeakOpen;
								p->m_stateCounter = 0;
								p++;
							}
							break;		// Nothing more to do further down.
						}
					}
				}

				// If pointer has entered a selectable widget of a popup that isn't the top one
				// and all widgets between them have bAutoClose=true, they should all enter
				// state ClosingDelay (unless already in state Closing).


//				Widget * pTop = popupSlots._first()->_widget();
				Widget * pMarked = _findWidget(pointerPos, SearchMode::ActionTarget);

				if (pMarked != this && pMarked->isSelectable() && popupSlots._first()->m_bAutoClose)
				{
					// Trace hierarchy from marked to one of our children.

					while (OO(pMarked)->_parent() != this)
						pMarked = OO(pMarked)->_parent();

					//

					auto p = popupSlots._first();
					while (p->m_bAutoClose && p->_widget() != pMarked)
					{
						if (p->m_state != Slot::State::Closing && p->m_state != Slot::State::ClosingDelay)
						{
							p->m_state = Slot::State::ClosingDelay;
							p->m_stateCounter = 0;
						}
						p++;
					}
				}


			}
			break;

/*
			case MsgType::MouseLeave:
			{
				// Top popup can be in state PeekOpen, which should begin closing when
				// pointer has left.

				Slot * pSlot = m_popups.first();
				if (pSlot && pSlot->m_state == Slot::State::PeekOpen)
				{
					pSlot->m_state = Slot::State::ClosingDelay;
					pSlot->m_stateCounter = 0;
				}
			}
			break;
*/

			case MsgType::MouseRelease:
			{
				if (popupSlots.isEmpty())
					break;					// Popup was removed already on the press.

				// Allow us to release the mouse within opener without closing any popups

				Slot * pSlot = popupSlots._first();
				if (pSlot->m_pOpener)
				{
					OWidget * pOpener = OO(pSlot->m_pOpener.rawPtr());

					CoordI 	absPos = static_cast<MouseReleaseMsg*>(_pMsg)->pointerPosRaw();
					Rect	openerGeo = pOpener->_globalGeo();

					if (pOpener->_markTest(absPos - openerGeo.pos()))
						break;
				}

				// DON'T BREAK! Continuing down to case MousePress on purpose.
			}
			case MsgType::MousePress:
			{
				if (popupSlots.isEmpty())
					break;

				auto pMsg = static_cast<MousePressMsg*>(_pMsg);

				auto pSource = static_cast<Widget*>(_pMsg->originalSource().rawPtr());
				if (!pSource || pSource == this )
					_removeSlots(0,popupSlots.size());
				else if (pSource->isSelectable())
				{
					MsgRouter * pRouter = Base::msgRouter().rawPtr();

					if (pRouter)
						pRouter->post(SelectMsg::create(pSource));

					_removeSlots(0,popupSlots.size());
				}

				_pMsg->swallow();
			}
			break;

			case MsgType::KeyPress:
			case MsgType::KeyRepeat:
			{
				auto pMsg = static_cast<KeyMsg*>(_pMsg);

				if( pMsg->translatedKeyCode() == Key::Escape )
				{
					if( !popupSlots.isEmpty() )
					{
						_removeSlots(0,1);
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

		if( popupSlots.size() < 2 )
			m_pKeyFocus = Base::inputHandler()->focusedWidget().rawPtr();
		else
			popupSlots[1].m_pKeyFocus = Base::inputHandler()->focusedWidget().rawPtr();

		// Steal keyboard focus to top menu

		Widget * pWidget = popupSlots._first()->_widget();

		_childRequestFocus(popupSlots._first(), pWidget );
	}

	//____ _restoreKeyboardFocus() _________________________________________________

	void PopupLayer::_restoreKeyboardFocus()
	{
		// Verify that we have a root

		if( !_root() )
			return;

		//

		if( popupSlots.isEmpty() )
			_holder()->_childRequestFocus( _slot(), m_pKeyFocus.rawPtr() );
		else
			_holder()->_childRequestFocus( _slot(),  popupSlots._first()->m_pKeyFocus.rawPtr() );
	}

	//____ _childRequestResize() _______________________________________________

	void PopupLayer::_childRequestResize(StaticSlot * pSlot)
	{
		if( pSlot == &mainSlot )
			_requestResize();
		else
			_updateGeo( (Slot *) pSlot, true );
	}

	//____ _releaseChild() _____________________________________________________

	void PopupLayer::_releaseChild( StaticSlot * pSlot )
	{
		if (pSlot == &mainSlot)
			Layer::_releaseChild(pSlot);
		else
		{
			// PopupLayer is stack-based, releasing a popup forces us to also close all ontop of it

			_removeSlots(0, popupSlots._index(static_cast<Slot*>(pSlot)) + 1);
		}
	}

	//____ _addSlot() ____________________________________________________________

	void PopupLayer::_addSlot(Widget * _pPopup, Widget * _pOpener, const RectI& _launcherGeo, Origo _attachPoint, bool _bAutoClose, SizeI _maxSize)
	{
		Slot * pSlot = popupSlots._pushFrontEmpty();
		pSlot->m_pOpener = _pOpener;
		pSlot->m_launcherGeo = _launcherGeo;
		pSlot->m_attachPoint = _attachPoint;
		pSlot->m_bAutoClose = _bAutoClose;
		pSlot->m_state = Slot::State::OpeningDelay;
		pSlot->m_stateCounter = 0;
		pSlot->m_maxSize = _maxSize;

		pSlot->_setWidget(_pPopup);

		_updateGeo(pSlot);
		_stealKeyboardFocus();

		if (m_tickRouteId == 0)
			m_tickRouteId = Base::msgRouter()->addRoute(MsgType::Tick, this);
	}


	//____ _removeSlots() __________________________________________________

	void PopupLayer::_removeSlots(int ofs, int nb)
	{
		MsgRouter * pEH = Base::msgRouter().rawPtr();

		Slot * pSlot = popupSlots._slot(ofs);

		nb = min(nb, popupSlots.size());

		for(int i = 0 ; i < nb ; i++ )
		{
			if( pEH )
				pEH->post( new PopupClosedMsg( pSlot[i]._widget(), pSlot[i].m_pOpener ) );
			_requestRender(pSlot[i].m_geo);
		}

		popupSlots._erase(ofs, nb);
		_restoreKeyboardFocus();

		if (popupSlots.isEmpty())
		{
			Base::msgRouter()->deleteRoute(m_tickRouteId);
			m_tickRouteId = 0;
		}
	}

	//____ _beginLayerSlots() __________________________________________________

	const Layer::Slot * PopupLayer::_beginLayerSlots() const
	{
		return popupSlots._begin();
	}

	//____ _endLayerSlots() __________________________________________________

	const Layer::Slot * PopupLayer::_endLayerSlots() const
	{
		return popupSlots._end();
	}

	//____ _sizeOfLayerSlot() __________________________________________________

	int PopupLayer::_sizeOfLayerSlot() const
	{
		return sizeof(Slot);
	}


} // namespace wg
