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

#ifndef WG_LAMBDAPANEL_DOT_H
#define WG_LAMBDAPANEL_DOT_H
#pragma once


#include <wg_panel.h>
#include <wg_hideablechildren.h>

namespace wg 
{
	
	class LambdaPanel;
	typedef	StrongPtr<LambdaPanel>		LambdaPanel_p;
	typedef	WeakPtr<LambdaPanel>		LambdaPanel_wp;
	
	//____ LambdaPanelSlot ____________________________________________________________

	class LambdaPanelSlot : public Slot		/** @private */
	{
	public:
		LambdaPanelSlot() : pFunc(nullptr), bVisible(false) {}

		const static bool safe_to_relocate = false;

		std::function<Rect(Widget * pWidget, Size parentSize)>	pFunc;
		bool			bVisible;
		Rect			geo;				// Widgets geo relative parent	
	};

	//____ LambdaChildrenHolder _________________________________________________

	class LambdaChildrenHolder : public HideableChildrenHolder		/** @private */
	{
	public:
		virtual void		_updateSlotGeo(Slot * pSlot, int nb) = 0;
	};


	class LambdaPanelChildren;
	typedef	StrongInterfacePtr<LambdaPanelChildren>	LambdaPanelChildren_p;
	typedef	WeakInterfacePtr<LambdaPanelChildren>	LambdaPanelChildren_wp;

	//____ LambdaPanelChildren ________________________________________________________

	class LambdaPanelChildren : public HideableChildren<LambdaPanelSlot,LambdaChildrenHolder>
	{
	public:
		/** @private */

		LambdaPanelChildren( SlotArray<LambdaPanelSlot> * pSlotArray, LambdaChildrenHolder * pHolder ) : HideableChildren<LambdaPanelSlot,LambdaChildrenHolder>(pSlotArray,pHolder) {}

		//.____ Content _______________________________________________________

		iterator	add( Widget * pWidget, std::function<Rect(Widget * pWidget, Size parenSize)> func );

		iterator	insert( int index, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func );
		iterator	insert( iterator pos, Widget * pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func );

		//.____ Geometry ______________________________________________________

		void		setFunction( int index, std::function<Rect(Widget * pWidget, Size parentSize)> func);
		void		setFunction( iterator pos, std::function<Rect(Widget * pWidget, Size parentSize)> func);

		std::function<Rect(Widget * pWidget, Size parentSize)> function(int index) const;
		std::function<Rect(Widget * pWidget, Size parentSize)> function(iterator pos) const;

		//.____ Misc __________________________________________________________

		inline LambdaPanelChildren_p	ptr() { return LambdaPanelChildren_p(this); }
	};


	//____ LambdaPanel _________________________________________________________
	
	class LambdaPanel : public Panel, protected LambdaChildrenHolder
	{
	
	public:

		//.____ Creation __________________________________________

		static LambdaPanel_p	create() { return LambdaPanel_p(new LambdaPanel()); }

		//.____ Interfaces _______________________________________

		LambdaPanelChildren	children;

		//.____ Identification __________________________________________
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static LambdaPanel_p	cast( Object * pObject );
		
		//.____ Geometry ____________________________________________

		bool				setMinSize(Size min);
		bool				setMaxSize(Size max);
		bool				setSizeLimits( Size min, Size max );
		bool				setPreferredSize(Size pref);
		
		Size				preferredSize() const;
	
	protected:
		LambdaPanel();
		virtual ~LambdaPanel();
		virtual Widget* _newOfMyType() const { return new LambdaPanel(); };
	

		// Overloaded from Container
		
		Widget *	_firstChild() const;
		Widget *	_lastChild() const;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const;


		// Methods for LambdaPanelChildren

		void		_didAddSlots( Slot * pSlot, int nb );
		void		_didMoveSlots(Slot * pFrom, Slot * pTo, int nb);
		void		_willRemoveSlots( Slot * pSlot, int nb );
		void		_hideSlots( Slot * pSlot, int nb );
		void		_unhideSlots( Slot * pSlot, int nb );
		void		_updateSlotGeo(Slot * pSlot, int nb);
		Object *	_object() { return this; }
		WidgetHolder *	_widgetHolder() { return this; }

		// Overloaded from WidgetHolder

		Coord		_childPos( Slot * pSlot ) const;
		Size		_childSize( Slot * pSlot ) const;

		void		_childRequestRender( Slot * pSlot );
		void		_childRequestRender( Slot * pSlot, const Rect& rect );
		void		_childRequestResize( Slot * pSlot );

		Widget *	_prevChild( Slot * pSlot ) const;
		Widget *	_nextChild( Slot * pSlot ) const;

	
	private:
	
		void		_cloneContent( const Widget * _pOrg );
		void		_setSize( const Size& size );
		void		_updateGeo(LambdaPanelSlot * pSlot);

		void		_onRequestRender( const Rect& rect, const LambdaPanelSlot * pSlot );

		
		SlotArray<LambdaPanelSlot>	m_children;
	
		Size		m_minSize;
		Size		m_maxSize;
		Size		m_preferredSize;
	};
	
	

} // namespace wg
#endif //WG_LAMBDAPANEL_DOT_H
