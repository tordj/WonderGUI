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

#include <wg_slotarray.impl.h>
#include <wg_paddedchildren.impl.h>

namespace wg 
{
	INSTANTIATE_PADDEDCHILDREN(StackPanelSlot, StackPanelChildrenHolder)

	template class SlotArray<StackPanelSlot>;

	const char StackPanel::CLASSNAME[] = {"StackPanel"};
		
	
	void StackPanelChildren::setSizePolicy( int index, SizePolicy2D policy )
	{
		//TODO: Assert!

		_setSizePolicy( m_pSlotArray->slot(index), policy );
	}

	void StackPanelChildren::setSizePolicy( iterator it, SizePolicy2D policy )
	{
		//TODO: Assert!

		_setSizePolicy( it._slot(), policy );
	}

	
	SizePolicy2D StackPanelChildren::sizePolicy( int index ) const
	{
		//TODO: Assert!

		return m_pSlotArray->slot(index)->sizePolicy;
	}

	SizePolicy2D StackPanelChildren::sizePolicy( iterator it ) const
	{
		//TODO: Assert!

		return it._slot()->sizePolicy;
	}


	void StackPanelChildren::setOrigo( int index, Origo origo )
	{
		//TODO: Assert!

		_setOrigo( m_pSlotArray->slot(index), origo );
	}

	void StackPanelChildren::setOrigo( iterator it, Origo origo )
	{
		//TODO: Assert!

		_setOrigo( it._slot(), origo );
	}

	
	Origo StackPanelChildren::origo( int index ) const
	{
		//TODO: Assert!

		return m_pSlotArray->slot(index)->origo;
	}

	Origo StackPanelChildren::origo( iterator it ) const
	{
		//TODO: Assert!

		return it._slot()->origo;		
	}


	void StackPanelChildren::_setSizePolicy( StackPanelSlot * pSlot, SizePolicy2D policy )
	{
		if( policy != pSlot->sizePolicy )
		{
			Rect oldGeo = ((StackPanel*)m_pHolder)->_childGeo(pSlot);
			pSlot->sizePolicy = policy;
			Rect newGeo = ((StackPanel*)m_pHolder)->_childGeo(pSlot);

			if (newGeo.size() != oldGeo.size())
			{
				((StackPanel*)m_pHolder)->_requestRender(oldGeo);
				((StackPanel*)m_pHolder)->_requestRender(newGeo);
				pSlot->pWidget->_setSize(newGeo.size());
			}
		};		
	}
		
	void StackPanelChildren::_setOrigo( StackPanelSlot * pSlot, Origo origo )
	{
		if( origo != pSlot->origo )
		{
			m_pHolder->_childRequestRender( pSlot );
			pSlot->origo = origo;
			m_pHolder->_childRequestRender( pSlot );
		};		
	}

	
	
	//____ Constructor ____________________________________________________________
	
	StackPanel::StackPanel() : children(&m_children,this)
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
	
		return Panel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * StackPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	StackPanel_p StackPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return StackPanel_p( static_cast<StackPanel*>(pObject) );
	
		return 0;
	}
	
	//____ matchingHeight() _______________________________________________________
	
	int StackPanel::matchingHeight( int width ) const
	{
		int height = 0;

		StackPanelSlot * pSlot = m_children.begin();
		StackPanelSlot * pEnd = m_children.end();
	
		while( pSlot != pEnd )
		{
			int h = pSlot->pWidget->matchingHeight(width);
			if( h > height )
				height = h;
			pSlot++;
		}
	
		return height;
	}
	
	//____ matchingWidth() _______________________________________________________
	
	int StackPanel::matchingWidth( int height ) const
	{
		int width = 0;
	
		StackPanelSlot * pSlot = m_children.begin();
		StackPanelSlot * pEnd = m_children.end();
	
		while( pSlot != pEnd )
		{
			int h = pSlot->pWidget->matchingWidth(height);
			if( h > height )
				height = h;
			pSlot++;
		}
	
		return width;
	}
	
	
	//____ preferredSize() _____________________________________________________________
	
	Size StackPanel::preferredSize() const
	{
		return m_preferredSize;
	}
	
	//____ _cloneContent() ______________________________________________________
	
	void StackPanel::_cloneContent( const Widget * _pOrg )
	{
		Panel::_cloneContent( _pOrg );

		//TODO: Implement		
	}

	//____ _setSize() ___________________________________________________________
	
	void StackPanel::_setSize( const Size& size )
	{
		Panel::_setSize(size);
		_adaptChildrenToSize();
	}

	//____ _firstChild() _______________________________________________________

	Widget * StackPanel::_firstChild() const
	{
		if( m_children.isEmpty() )
			return nullptr;
			
		return m_children.first()->pWidget;
	}

	//____ _lastChild() ________________________________________________________
	
	Widget * StackPanel::_lastChild() const
	{
		if( m_children.isEmpty() )
			return nullptr;
			
		return m_children.last()->pWidget;
		
	}
	
	
	//____ _firstSlotWithGeo() _____________________________________________________
	
	void StackPanel::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		if( m_children.isEmpty() )
			package.pSlot = nullptr;
		else
		{
			StackPanelSlot * pSlot = m_children.first();
			package.pSlot = pSlot;
			package.geo = _childGeo(pSlot);			
		}			
	}

	//____ _nextSlotWithGeo() ______________________________________________________
	
	void StackPanel::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		StackPanelSlot * pSlot = (StackPanelSlot*) package.pSlot;
		
		if( pSlot == m_children.last() )
			package.pSlot = nullptr;
		else
		{
			pSlot++;
			package.pSlot = pSlot;
			package.geo = _childGeo(pSlot);			
		}
	}

	//____ _didAddSlots() ________________________________________________________

	void StackPanel::_didAddSlots( Slot * pSlot, int nb )
	{
		_unhideChildren( (StackPanelSlot*) pSlot, nb );
	}

	//____ _willRemoveSlots() ____________________________________________________

	void StackPanel::_willRemoveSlots( Slot * pSlot, int nb )
	{
		_hideChildren( (StackPanelSlot*) pSlot, nb );
	}

	//____ _hideSlots() _______________________________________________________

	void StackPanel::_hideSlots( Slot * pSlot, int nb )
	{
		_hideChildren( (StackPanelSlot*) pSlot, nb );
	}

	//____ _unhideSlots() _____________________________________________________

	void StackPanel::_unhideSlots( Slot * pSlot, int nb )
	{
		_unhideChildren( (StackPanelSlot*) pSlot, nb );
	}

	//____ _repadSlots() _________________________________________________________

	void StackPanel::_repadSlots( Slot * pSlot, int nb, Border padding )
	{
		for( int i = 0 ; i < nb ; i++ )
			((StackPanelSlot*)pSlot)[i].padding = padding;

		_refreshPreferredSize();
		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.
	}

	//____ _didMoveSlots() ________________________________________________________

	void StackPanel::_didMoveSlots(Slot * pFrom, Slot * pTo, int nb )
	{
		_requestRender();	//TODO: Optimize! Only re-render what might have changed.
	}

	//____ _childPos() ________________________________________________________

	Coord StackPanel::_childPos( Slot * pSlot ) const
	{
		return _childGeo((StackPanelSlot *)pSlot).pos();	
	}
	
	//____ _childSize() _______________________________________________________

	Size StackPanel::_childSize( Slot * pSlot ) const
	{
		return ((StackPanelSlot *) pSlot)->pWidget->size();
	}

	//____ _childRequestRender() ______________________________________________

	void StackPanel::_childRequestRender( Slot * pSlot )
	{
		_childRequestRender( pSlot, _childGeo((StackPanelSlot*) pSlot) );
	}
	
	void StackPanel::_childRequestRender( Slot * _pSlot, const Rect& _rect )
	{
		StackPanelSlot * pSlot = (StackPanelSlot *) _pSlot;

		if( !pSlot->bVisible )
			return;

		// Put our rectangle into patches

		Rect rect = _rect + _childPos(pSlot);


		Patches patches;
		patches.add( rect );

		// Remove portions of patches that are covered by opaque upper siblings

		for( auto pCover = m_children.begin() ; pCover < pSlot ; pCover++ )
		{
			Rect geo = _childGeo(pCover);
			if( pCover->bVisible && geo.intersectsWith( rect ) )
				pCover->pWidget->_maskPatches( patches, geo, Rect(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const Rect * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );		
	}

	//____ _childRequestResize() ______________________________________________

	void StackPanel::_childRequestResize( Slot * pSlot )
	{
		_refreshPreferredSize();		
	}

	//____ _prevChild() __________________________________________________________

	Widget * StackPanel::_prevChild( Slot * pSlot ) const
	{
		StackPanelSlot * p = (StackPanelSlot *) pSlot;
		
		if( p > m_children.begin() )
			return p[-1].pWidget;

		return nullptr;
	}
	
	//____ _nextChild() __________________________________________________________

	Widget * StackPanel::_nextChild( Slot * pSlot ) const
	{
		StackPanelSlot * p = (StackPanelSlot *) pSlot;
		
		if( p < m_children.last() )
			return p[1].pWidget;		

		return nullptr;
	}
	
	//____ _unhideChildren() _____________________________________________________
	
	void StackPanel::_unhideChildren( StackPanelSlot * pSlot, int nb )
	{
		bool	bRequestResize = false;
		
		// Update m_preferredSize

		for( int i = 0 ; i < nb ; i++ )
		{
			if( !pSlot[i].bVisible )
			{
				Size preferred = pSlot[i].paddedPreferredSize();
	
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
			}
		}

		if( bRequestResize )
			_requestResize();
	
		// Make unhidden Widgets visible, adapt them to our size and force a render.
	
		for( int i = 0 ; i < nb ; i++ )
		{
			if( !pSlot[i].bVisible )
			{
				pSlot[i].bVisible = true;
				pSlot[i].pWidget->_setSize(_childGeo(pSlot).size() );
				_childRequestRender( pSlot + i );
			}
		}	
	}

	//____ _hideChildren() __________________________________________________
	
	void StackPanel::_hideChildren( StackPanelSlot * pRemove, int nb )
	{
		bool	bRequestResize = false;
	
		// Get dirty rectangles for all visible sections of widgets to be removed.
	
		for( int i = 0 ; i < nb ; i++ )
		{
			if( pRemove[i].bVisible )
			{
				_childRequestRender( pRemove + i );
				pRemove[i].bVisible = false;
			}
		}
		// Update m_preferredSize
	
		Size	preferredSize;
		StackPanelSlot * p = m_children.begin();
		
		while( p != m_children.end() )
		{
			if( p->bVisible )
			{
				Size sz = p->paddedPreferredSize();
				if( sz.w > preferredSize.w )
					preferredSize.w = sz.w;
				if( sz.h > preferredSize.h )
					preferredSize.h = sz.h;
				p++;
			}
		}
	
		if( preferredSize != m_preferredSize )
			bRequestResize = true;
	
		m_preferredSize = preferredSize;
	
		// Check if removal might affect height for current width
/*	
		int height = pToBeRemoved->_widget()->matchingHeight(m_size.w);
		if( height >= m_size.h )
			bRequestResize = true;
*/	
		//
	
		if( bRequestResize )
			_requestResize();
	}
	
	
	//____ _refreshAllWidgets() ____________________________________________________
/*	
	void StackPanel::_refreshAllWidgets()
	{
		_refreshPreferredSize();
		_adaptChildrenToSize();
		_requestRender();
	}
*/	
	
	//____ _refreshPreferredSize() _____________________________________________________
	
	void StackPanel::_refreshPreferredSize()
	{
		Size	preferredSize;

		auto * pSlot = m_children.begin();
		auto * pEnd = m_children.end();
		
		while( pSlot != pEnd )
		{
			if( pSlot->bVisible )
			{
				Size sz = pSlot->paddedPreferredSize();
				if( sz.w > preferredSize.w )
					preferredSize.w = sz.w;
				if( sz.h > preferredSize.h )
					preferredSize.h = sz.h;
			}
			pSlot++;
		}

//TODO: We can't trust that same preferredSize results in same matchingHeight. We need to find a more robust optimization.
//		if( m_preferredSize != preferredSize)
		{
			m_preferredSize = preferredSize;
			_requestResize();
		}
	}
	
	//____ _adaptChildrenToSize() ___________________________________________________________
	
	void StackPanel::_adaptChildrenToSize()
	{
		auto * pSlot = m_children.begin();
		auto * pEnd = m_children.end();
		
		while( pSlot != pEnd )
		{
			if( pSlot->bVisible )
				pSlot->pWidget->_setSize( _childGeo(pSlot) );
			pSlot++;
		}
	}

	//____ _childGeo() ___________________________________________________________

	Rect StackPanel::_childGeo( const StackPanelSlot * pSlot ) const
	{
		Rect base = Rect( m_size ) - pSlot->padding;
	
		if( base.w <= 0 || base.h <= 0 )
			return Rect(0,0,0,0);
	
		switch( pSlot->sizePolicy )
		{
			default:
		case SizePolicy2D::Original:
			{
				Size	size = pSlot->pWidget->preferredSize();
				Rect geo = Util::origoToRect( pSlot->origo, base, size );
	
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
			case SizePolicy2D::Stretch:
			{
				return base;
			}
			case SizePolicy2D::Scale:
			{
				Size	orgSize = pSlot->pWidget->preferredSize();
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
	
				return Util::origoToRect( pSlot->origo, base, size );
			}
		}
	}




} // namespace wg
