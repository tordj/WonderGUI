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

#ifndef WG_STACKPANEL_DOT_H
#define WG_STACKPANEL_DOT_H
#pragma once

#include <wg_panel.h>
#include <wg_paddedslot.h>
#include <wg_ipaddedslotarray.h>


namespace wg
{

	class StackPanel;
	typedef	StrongPtr<StackPanel>	StackPanel_p;
	typedef	WeakPtr<StackPanel>		StackPanel_wp;



	//____ StackSlot ____________________________________________________________

	class StackSlot : public PaddedSlot		/** @private */
	{
		friend class StackPanel;
		friend class IStackSlots;
	public:
		StackSlot() : origo(Origo::Center), SizePolicy(SizePolicy2D::Original) {}

		Origo			origo;
		SizePolicy2D	SizePolicy;
	};

	//____ StackSlotArrayHolder __________________________________________________

	class StackSlotArrayHolder : public PaddedSlotArrayHolder			/** @private */
	{
	public:
		virtual void		_childRequestRender(BasicSlot * pSlot) = 0;
	};

	//____ IStackSlots ________________________________________________________

	class IStackSlots;
	typedef	StrongInterfacePtr<IStackSlots>	IStackSlots_p;
	typedef	WeakInterfacePtr<IStackSlots>	IStackSlots_wp;

	class IStackSlots : public IPaddedSlotArray<StackSlot>
	{
	public:
		/** @private */

		IStackSlots( SlotArray<StackSlot> * pSlotArray, StackSlotArrayHolder * pHolder ) : IPaddedSlotArray<StackSlot>(pSlotArray,pHolder) {}

		//.____ Geometry ______________________________________________________

		void		setSizePolicy( int index, SizePolicy2D policy );
		void		setSizePolicy( iterator it, SizePolicy2D policy );

		SizePolicy2D SizePolicy( int index ) const;
		SizePolicy2D SizePolicy( iterator it ) const;

		void		setOrigo( int index, Origo origo );
		void		setOrigo( iterator it, Origo origo );

		Origo		origo( int index ) const;
		Origo		origo( iterator it ) const;

		//.____ Misc __________________________________________________________

		inline IStackSlots_p	ptr() { return IStackSlots_p(this); }

	protected:
		void		_setSizePolicy( StackSlot * pSlot, SizePolicy2D policy );
		void		_setOrigo( StackSlot * pSlot, Origo origo );
	};



	//____ StackPanel ___________________________________________________________

	/**
	*/

	class StackPanel : public Panel, protected StackSlotArrayHolder
	{
		friend class IStackSlots;

	public:

		//.____ Creation __________________________________________

		static StackPanel_p	create() { return StackPanel_p(new StackPanel()); }

		//.____ Interfaces _______________________________________

		IStackSlots	children;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static StackPanel_p	cast( Object * pObject );

	protected:
		StackPanel();
		virtual ~StackPanel();
		virtual Widget* _newOfMyType() const override { return new StackPanel(); };

		// Overloaded from Widget

		int			_matchingHeight(int width) const override;
		int			_matchingWidth(int height) const override;

		SizeI		_preferredSize() const override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const SizeI& size ) override;

		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		// Overloaded from PaddedChildrenHolder

		void		_didAddSlots( BasicSlot * pSlot, int nb ) override;
		void		_didMoveSlots(BasicSlot * pFrom, BasicSlot * pTo, int nb) override;
		void		_willRemoveSlots( BasicSlot * pSlot, int nb ) override;
		void		_hideSlots( BasicSlot *, int nb ) override;
		void		_unhideSlots( BasicSlot *, int nb ) override;
		void		_repadSlots( BasicSlot *, int nb, BorderI padding ) override;
		void		_repadSlots(BasicSlot *, int nb, const BorderI * pPaddings) override;
		Object *	_object() override { return this; }
		WidgetHolder *	_widgetHolder() override { return this; }


		// Overloaded from WidgetHolder

		CoordI		_childPos( BasicSlot * pSlot ) const override;
		SizeI		_childSize( BasicSlot * pSlot ) const override;

		void		_childRequestRender( BasicSlot * pSlot ) override;
		void		_childRequestRender( BasicSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( BasicSlot * pSlot ) override;

		Widget *	_prevChild( const BasicSlot * pSlot ) const override;
		Widget *	_nextChild( const BasicSlot * pSlot ) const override;

		void		_releaseChild(BasicSlot * pSlot) override;

		// Internal to StackPanel

		SizeI 	_calcPreferredSize();
		void	_adaptChildrenToSize();

		void	_hideChildren( StackSlot * pSlot, int nb );
		void	_unhideChildren( StackSlot * pSlot, int nb );


		RectI	_childGeo( const StackSlot * pSlot ) const;

		SizeI	m_preferredSize;

		SlotArray<StackSlot>	m_children;
	};


} // namespace wg
#endif //WG_STACKPANEL_DOT_H
