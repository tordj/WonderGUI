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

#ifndef WG_MODALLAYER_DOT_H
#define WG_MODALLAYER_DOT_H
#pragma once

#include <wg_layer.h>
#include <wg_childgroup.h>

namespace wg 
{

	class ModalLayer;
	typedef	StrongPtr<ModalLayer, Layer_p>	ModalLayer_p;
	typedef	WeakPtr<ModalLayer, Layer_wp>	ModalLayer_wp;

	//____ ModalSlot ___________________________________________________________

	class ModalSlot : public LayerSlot		/** @private */
	{
	public:
		ModalSlot() : origo(Origo::NorthWest) {}

		Origo		origo;
		Rect		placementGeo;		// Widgets geo relative anchor and hotspot. Setting width and height to 0 uses Widgets preferredSize() dynamically.
										// Setting just one of them to 0 uses Widgets matchingHeight() or matchingWidth() dynamically.	
		Widget_wp	pKeyFocus;			// Pointer at child that held focus when this modal was last on top.
		
	};
	
	
	class ModalChildren;
	typedef	StrongInterfacePtr<ModalChildren,Interface_p>	ModalChildren_p;
	typedef	WeakInterfacePtr<ModalChildren,Interface_wp>	ModalChildren_wp;
	
	//____ ModalChildren ________________________________________________________

	class ModalChildren : public ChildGroup<ModalSlot,ModalLayer>
	{
	public:
		ModalChildren( SlotArray<ModalSlot> * pSlotArray, ModalLayer * pHolder ) : ChildGroup<ModalSlot,ModalLayer>(pSlotArray,pHolder) {}

		inline ModalChildren_p	ptr() { return ModalChildren_p(_object(),this); }

		bool		add( Widget * pWidget, const Rect& geometry, Origo origo = Origo::NorthWest );
		bool		add( Widget * pWidget, const Coord& pos, Origo origo = Origo::NorthWest ) { return add( pWidget, Rect(pos,0,0), origo); }

		bool		moveToBack( int index );								// Put us ontop all our silbings.
		bool		moveToFront( int index );							// Put us below all our siblings.	
		bool		moveAbove( int index, int otherWidget );
		bool		moveBelow( int index, int otherWidget );

		bool		setOrigo( int index, const Origo origo );
		Origo		origo( int index ) const;
		
		bool		setGeo( int index, const Rect& geometry );
		Rect		geo( int index ) const;
	
		bool		setOfs( int index, const Coord& ofs );
		Coord		ofs( int index ) const;

		bool		setSize( int index, const Size& size );
		Size		size( int index ) const;
	
		bool		move( int index, const Coord& ofs );
	};
	
	
	

	//____ ModalLayer __________________________________________________________
	
	class ModalLayer : public Layer, protected ChildGroupHolder
	{
		friend class ModalChildren;
	
	public:

		//.____ Creation __________________________________________

		static ModalLayer_p	create() { return ModalLayer_p(new ModalLayer()); }

		//.____ Components _______________________________________

		ModalChildren	modals;

		//.____ Identification __________________________________________
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ModalLayer_p	cast( Object * pObject );
	
		
		//.____ Geometry ____________________________________________
	
		int				matchingHeight( int width ) const;
		int				matchingWidth( int height ) const;
	
		Size			preferredSize() const;
	
	protected:
		ModalLayer();
		virtual ~ModalLayer();
		virtual Widget* _newOfMyType() const { return new ModalLayer(); };
	
	
	private:
	
		ModalLayer *	_getModalLayer() const { return const_cast<ModalLayer*>(this); }
	
		void			_updateKeyboardFocus();
		bool			_refreshRealGeo( ModalSlot * pSlot );
		void			_moveSlot(int oldPos, int newPos);

		// Overloaded from Panel
	
		Widget *		_findWidget( const Coord& ofs, SearchMode mode );
	
		// Overloaded from WidgetHolder
	
		void			_childRequestResize( Slot * pSlot );

		// Methods for ModalChildren

		void			_didAddSlots(Slot * pSlot, int nb);
		void			_willRemoveSlots(Slot * pSlot, int nb);


		// Overloaded from Layer
	
		LayerSlot * 	_beginLayerSlots() const;
		LayerSlot * 	_endLayerSlots() const;
		int				_sizeOfLayerSlot() const;
	
		// Overloaded from Widget
	
		void			_cloneContent( const Widget * _pOrg );
		void			_setSize( const Size& size );
		void			_receive( Msg * pMsg );
	
	
		Widget_wp			m_pBaseKeyFocus;

		SlotArray<ModalSlot>	m_modals;
	
	};
	

} // namespace wg
#endif //WG_MODALLAYER_DOT_H
