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

#include <wg_lambdapanel.h>
#include <wg_patches.h>
#include <wg_util.h>

namespace wg
{

	const char LambdaPanel::CLASSNAME[] = {"LambdaPanel"};

	LambdaPanelChildren::iterator LambdaPanelChildren::add( Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		LambdaPanelSlot * pSlot = m_pSlotArray->add();
		pSlot->replaceWidget(m_pHolder, pWidget);
		pSlot->pFunc = func;
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	LambdaPanelChildren::iterator LambdaPanelChildren::insert( int index, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		LambdaPanelSlot * pSlot = m_pSlotArray->insert(index);
		pSlot->replaceWidget(m_pHolder, pWidget);
		pSlot->pFunc = func;
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}
	
	LambdaPanelChildren::iterator LambdaPanelChildren::insert( iterator pos, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func )
	{
		//TODO: Assert

		LambdaPanelSlot * pSlot = m_pSlotArray->insert(pos._slot());
		pSlot->replaceWidget(m_pHolder, pWidget);
		pSlot->pFunc = func;
		m_pHolder->_didAddSlots(pSlot, 1);
		return iterator(pSlot);
	}

	void LambdaPanelChildren::moveToBack( int index )
	{
		//TODO: Assert

		m_pHolder->_moveSlot(m_pSlotArray->slot(index), m_pSlotArray->end() );
	}
	
	LambdaPanelChildren::iterator LambdaPanelChildren::moveToBack( iterator it )
	{
		//TODO: Assert

		m_pHolder->_moveSlot(it._slot(), m_pSlotArray->end());
		return iterator(m_pSlotArray->last());
	}

	void LambdaPanelChildren::moveToFront( int index )
	{
		//TODO: Assert

		m_pHolder->_moveSlot(m_pSlotArray->slot(index), m_pSlotArray->begin());
	}
	
	LambdaPanelChildren::iterator LambdaPanelChildren::moveToFront( iterator it )
	{
		//TODO: Assert

		m_pHolder->_moveSlot(it._slot(), m_pSlotArray->begin());
		return iterator(m_pSlotArray->begin());
	}

	void LambdaPanelChildren::moveBefore( int index, int sibling )
	{
		//TODO: Assert

		if (index < sibling)
			sibling--;

		if( index != sibling )
			m_pHolder->_moveSlot(m_pSlotArray->slot(index), m_pSlotArray->slot(sibling));
	}
	
	LambdaPanelChildren::iterator LambdaPanelChildren::moveBefore( iterator it, iterator sibling )
	{
		//TODO: Assert

		if(it < sibling)
			sibling--;

		if (it != sibling)
			m_pHolder->_moveSlot(it._slot(), sibling._slot());

		return sibling;
	}


	//____ Constructor ____________________________________________________________

	LambdaPanel::LambdaPanel() : children(&m_children,this), m_minSize(0,0), m_preferredSize(512,512), m_maxSize(INT_MAX,INT_MAX)
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	LambdaPanel::~LambdaPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool LambdaPanel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Panel::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * LambdaPanel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	LambdaPanel_p LambdaPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return LambdaPanel_p( static_cast<LambdaPanel*>(pObject) );

		return 0;
	}

	//____ setMinSize() _________________________________________________________________

	bool LambdaPanel::setMinSize(Size min)
	{
		//TODO: Assert >= 0.
		
		if( m_minSize != min )
		{
			if( min.w > m_maxSize.w || min.h > m_maxSize.h )
				return false;
			
			m_minSize = min;
			if( m_preferredSize.w < min.w )
				m_preferredSize.w = min.w;
			if( m_preferredSize.h < min.h )
				m_preferredSize.h = min.h;
				
			_requestResize();
		}
		return true;
	}
	
	//____ setMaxSize() ________________________________________________________
	
	bool LambdaPanel::setMaxSize(Size max)
	{
		//TODO: Assert >= 0.

 		if( m_maxSize != max )
		{
			if( max.w < m_minSize.w || max.h < m_minSize.h )
				return false;
			
			m_maxSize = max;
			if( m_preferredSize.w > max.w )
				m_preferredSize.w = max.w;
			if( m_preferredSize.h > max.h )
				m_preferredSize.h = max.h;
				
			_requestResize();
		}
		return true;		
	}

	//____ setSizeLimits() _____________________________________________________
	
	bool LambdaPanel::setSizeLimits( Size min, Size max )
	{
		//TODO: Assert >= 0.

		if( min.w > max.w || min.h > max.h )
			return false;
			
		m_minSize = min;
		m_maxSize = max;
		limit( m_preferredSize.w, m_minSize.w, m_maxSize.w );
		limit( m_preferredSize.h, m_minSize.h, m_maxSize.h );

		_requestResize();
		return true;
	}
	
	//____ setPreferredSize() __________________________________________________
	
	bool LambdaPanel::setPreferredSize(Size pref)
	{
		//TODO: Assert >= 0.

		if( pref.w > m_maxSize.w || pref.h > m_maxSize.h || pref.w < m_minSize.w || pref.h < m_minSize.h )
			return false;
			
		m_preferredSize = pref;
		_requestResize();
		return true;
	}

	//____ preferredSize() _____________________________________________________
	
	Size LambdaPanel::preferredSize() const
	{
		return m_preferredSize;
	}

	//____ _firstChild() _______________________________________________________
	
	Widget * LambdaPanel::_firstChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.first()->pWidget;		
	}

	//____ _lastChild() ________________________________________________________
	
	Widget * LambdaPanel::_lastChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.last()->pWidget;		
	}

	//____ _firstSlotWithGeo() _________________________________________________

	void LambdaPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			LambdaPanelSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}
		
	}
	
	//____ _nextSlotWithGeo() _________________________________________________

	void LambdaPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		LambdaPanelSlot * pSlot = (LambdaPanelSlot*) package.pSlot;

		if( pSlot == m_children.last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = pSlot->geo;
		}		
	}

	//____ _didAddSlots() ________________________________________________________

	void LambdaPanel::_didAddSlots( Slot * pSlot, int nb )
	{
		_unhideSlots(static_cast<LambdaPanelSlot*>(pSlot), nb);
	}

	//____ _willRemoveSlots() _________________________________________________

	void LambdaPanel::_willRemoveSlots( Slot * pSlot, int nb )
	{
		_hideSlots(static_cast<LambdaPanelSlot*>(pSlot), nb);
	}
	
	//____ _hideSlots() __________________________________________________________

	void LambdaPanel::_hideSlots( LambdaPanelSlot * pSlot, int nb )
	{
		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == true )
			{
				pSlot[i].bVisible = false;
				_onRequestRender(pSlot[i].geo, pSlot);
			}
		}		
	}
	
	//____ _unhideSlots() ________________________________________________________

	void LambdaPanel::_unhideSlots( LambdaPanelSlot * pSlot, int nb )
	{
		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == false )
			{
				pSlot[i].bVisible = true;
				_updateGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].geo, pSlot);
			}
		}
		
	}

	//____ _childPos() __________________________________________________________

	Coord LambdaPanel::_childPos( Slot * pSlot ) const
	{
		return ((LambdaPanelSlot*)pSlot)->geo.pos();		
	}

	//____ _childSize() __________________________________________________________

	Size LambdaPanel::_childSize( Slot * pSlot ) const
	{
		return ((LambdaPanelSlot*)pSlot)->geo.size();
	}

	//____ _childRequestRender() _________________________________________________

	void LambdaPanel::_childRequestRender( Slot * _pSlot )
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);
		_onRequestRender( pSlot->geo, pSlot );		
	}
	
	void LambdaPanel::_childRequestRender( Slot * _pSlot, const Rect& rect )
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->geo.pos(), pSlot );		
	}

	//____ _childRequestResize() ______________________________________________

	void LambdaPanel::_childRequestResize( Slot * pSlot )
	{
		_updateGeo(static_cast<LambdaPanelSlot*>(pSlot));
	}

	//____ _prevChild() __________________________________________________________

	Widget * LambdaPanel::_prevChild( Slot * _pSlot ) const
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

		if (pSlot > m_children.begin())
			return pSlot[-1].pWidget;

		return nullptr;
		
	}

	//____ _nextChild() __________________________________________________________

	Widget * LambdaPanel::_nextChild( Slot * _pSlot ) const
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

		if (pSlot < m_children.last())
			return pSlot[1].pWidget;

		return nullptr;		
	}

	//____ _cloneContent() _______________________________________________________

	void LambdaPanel::_cloneContent( const Widget * _pOrg )
	{
		//TODO: Implement
	}

	//____ _setSize() ____________________________________________________________

	void LambdaPanel::_setSize( const Size& size )
	{
		Panel::_setSize( size );

		for (auto& slot : m_children)
			_updateGeo(&slot);
	}

	//____ _updateGeo() _______________________________________________________

	void LambdaPanel::_updateGeo(LambdaPanelSlot * pSlot)
	{
		Rect geo = pSlot->pFunc(pSlot->pWidget, m_size );


		if (geo != pSlot->geo)
		{
			if (pSlot->bVisible)
			{
				// Clip our geometry and put it in a dirtyrect-list

				Patches patches;
				patches.add(Rect(pSlot->geo, Rect(0, 0, size())));
				patches.add(Rect(geo, Rect(0, 0, size())));

				// Remove portions of patches that are covered by opaque upper siblings

				const LambdaPanelSlot * pCover = pSlot + 1;
				while (pCover < m_children.end())
				{
					if (pCover->bVisible && (pCover->geo.intersectsWith(pSlot->geo) || pCover->geo.intersectsWith(geo)) )
						pCover->pWidget->_maskPatches(patches, pCover->geo, Rect(0, 0, 65536, 65536), _getBlendMode());

					pCover++;
				}

				// Make request render calls

				for (const Rect * pRect = patches.begin(); pRect < patches.end(); pRect++)
					_requestRender(*pRect);

			}
		}

		pSlot->geo = geo;
		
		pSlot->pWidget->_setSize(geo.size());

	}

	//____ _onRequestRender() ____________________________________________________

	void LambdaPanel::_onRequestRender( const Rect& rect, const LambdaPanelSlot * pSlot )
	{
		if (!pSlot->bVisible)
			return;

		// Clip our geometry and put it in a dirtyrect-list

		Patches patches;
		patches.add(Rect(rect, Rect(0, 0, size())));

		// Remove portions of patches that are covered by opaque upper siblings

		for (LambdaPanelSlot * pCover = m_children.begin(); pCover < pSlot ; pCover++)
		{
			if (pCover->bVisible && pCover->geo.intersectsWith(rect))
				pCover->pWidget->_maskPatches(patches, pCover->geo, Rect(0, 0, 65536, 65536), _getBlendMode());
		}

		// Make request render calls

		for (const Rect * pRect = patches.begin(); pRect < patches.end(); pRect++)
			_requestRender(*pRect);

	}

	//____ _moveSlot() ________________________________________________________

	void LambdaPanel::_moveSlot(LambdaPanelSlot * pFrom, LambdaPanelSlot * pTo)
	{
		m_children.move(pFrom, pTo);

		if (pTo->bVisible)		// This is correct, we have already switched places...
		{
			if (pTo > pFrom)			// We were moved forward
			{
				// Request render on all areas covered by siblings we have skipped in front of.

				LambdaPanelSlot * p = pFrom;
				while (p < pTo)
				{
					Rect cover(pTo->geo, p->geo);

					if (p->bVisible && !cover.isEmpty())
						_onRequestRender(cover, pTo);
					p++;
				}
			}
			else							// Move us backward
			{
				// Request render on our siblings for the area we previously have covered.

				LambdaPanelSlot * p = pTo + 1;
				while (p <= pFrom)
				{
					Rect cover(pTo->geo, p->geo);

					if (p->bVisible && !cover.isEmpty())
						_onRequestRender(cover, p);
					p++;
				}
			}
		}
	}



} // namespace wg