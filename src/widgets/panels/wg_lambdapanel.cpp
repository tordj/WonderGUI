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

	LambdaPanel::LambdaPanel() : children(&m_children,this)
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

	void LambdaPanel::setMinSize(Size sz)
	{
		
	}
	
	void LambdaPanel::setMaxSize(Size sz)
	{
		
	}
	
	void LambdaPanel::setPreferredSize(Size sz)
	{
		
	}

	
	Size LambdaPanel::preferredSize() const
	{
		
	}
	
	Widget * LambdaPanel::_firstChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.first()->pWidget;		
	}
	
	Widget * LambdaPanel::_lastChild() const
	{
		if (m_children.isEmpty())
			return nullptr;

		return m_children.last()->pWidget;		
	}

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