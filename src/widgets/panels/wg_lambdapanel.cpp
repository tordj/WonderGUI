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

	iterator LambdaPanelChildren::add( Widget * pWidget, std::function<Rect(const Rect& parentGeo)> func )
	{
		
	}

	iterator LambdaPanelChildren::insert( int index, Widget * pWidget, std::function<Rect(const Rect& parentGeo)> func )
	{
		
	}
	
	iterator LambdaPanelChildren::insert( iterator pos, Widget * pWidget, std::function<Rect(const Rect& parentGeo)> func )
	{
		
	}

	void LambdaPanelChildren::moveToBack( int index )
	{
		
	}
	
	iterator LambdaPanelChildren::moveToBack( iterator it )
	{
		
	}

	void LambdaPanelChildren::moveToFront( int index )
	{
		
	}
	
	iterator LambdaPanelChildren::moveToFront( iterator it )
	{
		
	}

	void LambdaPanelChildren::moveBefore( int index, int sibling )
	{
		
	}
	
	iterator LambdaPanelChildren::moveBefore( iterator it, iterator sibling )
	{
		
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
	
	bool LambdaPanel::setMaxSize(Size sz)
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

	void LambdaPanel::_didAddSlots( Slot * pSlot, int nb )
	{
		
	}
	
	void LambdaPanel::_willRemoveSlots( Slot * pSlot, int nb )
	{
		
	}
	
	void LambdaPanel::_hideSlots( LambdaPanelSlot *, int nb )
	{
		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == true )
			{
				pSlot[i].bVisible = false;
				_onRequestRender(pSlot[i].realGeo, pSlot);
			}
		}		
	}
	
	void LambdaPanel::_unhideSlots( LambdaPanelSlot *, int nb )
	{
		for( int i = 0 ; i < nb ; i++ )
		{
			if( pSlot[i].bVisible == false )
			{
				pSlot[i].bVisible = true;
				_refreshRealGeo(&pSlot[i]);
				_onRequestRender(pSlot[i].realGeo, pSlot);
			}
		}
		
	}

	Coord LambdaPanel::_childPos( Slot * pSlot ) const
	{
		return ((LambdaPanelSlot*)pSlot)->geo.pos();		
	}
	
	Size LambdaPanel::_childSize( Slot * pSlot ) const
	{
		return ((LambdaPanelSlot*)pSlot)->geo.size();
	}

	void LambdaPanel::_childRequestRender( Slot * pSlot )
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);
		_onRequestRender( pSlot->realGeo, pSlot );		
	}
	
	void LambdaPanel::_childRequestRender( Slot * pSlot, const Rect& rect )
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);
		_onRequestRender( rect + pSlot->realGeo.pos(), pSlot );		
	}
	
	void LambdaPanel::_childRequestResize( Slot * pSlot )
	{
		
	}

	Widget * LambdaPanel::_prevChild( Slot * pSlot ) const
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

		if (pSlot > m_children.begin())
			return pSlot[-1].pWidget;

		return nullptr;
		
	}
	
	Widget * LambdaPanel::_nextChild( Slot * pSlot ) const
	{
		LambdaPanelSlot * pSlot = static_cast<LambdaPanelSlot*>(_pSlot);

		if (pSlot < m_children.last())
			return pSlot[1].pWidget;

		return nullptr;		
	}

	void LambdaPanel::_cloneContent( const Widget * _pOrg )
	{
		
	}
	
	void LambdaPanel::_setSize( const Size& size )
	{
		
	}

	void LambdaPanel::_onRequestRender( const Rect& rect, const LambdaPanelSlot * pSlot )
	{
		
	}

	void LambdaPanel::_moveSlot(FlexPanelSlot * pFrom, FlexPanelSlot * pTo)
	{
		
	}



} // namespace wg