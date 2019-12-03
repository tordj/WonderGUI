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
#include <wg_islotarray.h>

namespace wg
{

	class LambdaPanel;
	typedef	StrongPtr<LambdaPanel>		LambdaPanel_p;
	typedef	WeakPtr<LambdaPanel>		LambdaPanel_wp;

	//____ LambdaSlotHolder ____________________________________________________________

	class LambdaSlotHolder : public SlotHolder
	{

	};


	//____ LambdaSlot ____________________________________________________________

	class LambdaSlot : public BasicSlot		/** @private */
	{
		friend class LambdaPanel;
		friend class ILambdaSlots;
	public:
		LambdaSlot(SlotHolder * pHolder ) : BasicSlot(pHolder), pFunc(nullptr), bVisible(false) {}

		const static bool safe_to_relocate = false;

		std::function<Rect(Widget * pWidget, Size parentSize)>	pFunc;
		bool			bVisible;
		RectI			geo;				// Widgets geo relative parent
	};

	//____ LambdaSlotArrayHolder _________________________________________________

	class LambdaSlotArrayHolder : public SlotArrayHolder		/** @private */
	{
	public:
		virtual void		_updateSlotGeo(BasicSlot * pSlot, int nb) = 0;
	};


	class ILambdaSlots;
	typedef	StrongInterfacePtr<ILambdaSlots>	ILambdaSlots_p;
	typedef	WeakInterfacePtr<ILambdaSlots>		ILambdaSlots_wp;

	//____ ILambdaSlots ________________________________________________________

	class ILambdaSlots : public ISlotArray<LambdaSlot>
	{
		friend class LambdaPanel;
	public:

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

		inline ILambdaSlots_p	ptr() { return ILambdaSlots_p(this); }

	protected:
		ILambdaSlots(SlotArray<LambdaSlot> * pSlotArray, LambdaSlotArrayHolder * pHolder) : ISlotArray<LambdaSlot>(pSlotArray, pHolder) {}

		const LambdaSlotArrayHolder *	_holder() const { return static_cast<LambdaSlotArrayHolder*>(m_pHolder); }
		LambdaSlotArrayHolder *	_holder() { return static_cast<LambdaSlotArrayHolder*>(m_pHolder); }
	};


	//____ LambdaPanel _________________________________________________________

	class LambdaPanel : public Panel, protected LambdaSlotArrayHolder, protected LambdaSlotHolder
	{

	public:

		//.____ Creation __________________________________________

		static LambdaPanel_p	create() { return LambdaPanel_p(new LambdaPanel()); }

		//.____ Interfaces _______________________________________

		ILambdaSlots	children;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static LambdaPanel_p	cast( Object * pObject );

		//.____ Geometry ____________________________________________

		bool				setMinSize(Size min);
		bool				setMaxSize(Size max);
		bool				setSizeLimits( Size min, Size max );
		bool				setPreferredSize(Size pref);

	protected:
		LambdaPanel();
		virtual ~LambdaPanel();
		virtual Widget* _newOfMyType() const override { return new LambdaPanel(); };


		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;


		// Methods for LambdaSlotArrayHolder

		void		_didAddSlots( BasicSlot * pSlot, int nb ) override;
		void		_didMoveSlots(BasicSlot * pFrom, BasicSlot * pTo, int nb) override;
		void		_willRemoveSlots( BasicSlot * pSlot, int nb ) override;
		void		_hideSlots( BasicSlot * pSlot, int nb ) override;
		void		_unhideSlots( BasicSlot * pSlot, int nb ) override;
		void		_updateSlotGeo(BasicSlot * pSlot, int nb) override;

		// Overloaded from LambdaSlotHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }

		CoordI		_childPos(const BasicSlot * pSlot) const override;
		CoordI		_childGlobalPos(const BasicSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(const BasicSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection(const BasicSlot * pSlot) const override { return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(BasicSlot * pSlot) override { return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		void		_childRequestRender( BasicSlot * pSlot ) override;
		void		_childRequestRender( BasicSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( BasicSlot * pSlot ) override;

		Widget *	_prevChild( const BasicSlot * pSlot ) const override;
		Widget *	_nextChild( const BasicSlot * pSlot ) const override;

		void		_releaseChild(BasicSlot * pSlot) override;
		void		_replaceChild(BasicSlot * pSlot, Widget * pNewChild) override;

	private:

		SizeI		_preferredSize() const override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const SizeI& size ) override;
		void		_updateGeo(LambdaSlot * pSlot, bool bForceResize = false);

		void		_onRequestRender( const RectI& rect, const LambdaSlot * pSlot );

		SlotArray<LambdaSlot>	m_children;

		SizeI		m_minSize;
		SizeI		m_maxSize;
		SizeI		m_preferredSize;
	};



} // namespace wg
#endif //WG_LAMBDAPANEL_DOT_H
