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
#include <wg_cdynamicslotvector.h>

namespace wg
{

	class LambdaPanel;
	typedef	StrongPtr<LambdaPanel>		LambdaPanel_p;
	typedef	WeakPtr<LambdaPanel>		LambdaPanel_wp;


	//____ LambdaSlot ____________________________________________________________

	class LambdaSlot : public DynamicSlot
	{
		friend class LambdaPanel;
		friend class CLambdaSlotVector;
		friend class CDynamicSlotVector<LambdaSlot>;

	public:

		//.____ Properties _________________________________________________

		inline void	setFunction(const std::function<Rect(Widget * pWidget, Size parentSize)>& func) { m_func = func; static_cast<Holder*>(_holder())->_updateSlotGeo(this, 1); }
		inline const std::function<Rect(Widget * pWidget, Size parentSize)>& function() const { return m_func; }

		//.____ Geometry _________________________________________________

		inline Coord	pos() const { return Util::rawToQpix(m_geo.pos()); }
		inline Size		size() const { return Util::rawToQpix(m_geo.size()); }
		inline Rect		geo() const { return Util::rawToQpix(m_geo); }

		//.____ Operators __________________________________________

		inline void operator=(Widget * pWidget) { setWidget(pWidget); }

	protected:
		class Holder : public StaticSlot::Holder
		{
		public:
			virtual void		_updateSlotGeo(StaticSlot * pSlot, int nb) = 0;
		};

		LambdaSlot(Holder * pHolder) : DynamicSlot(pHolder) {}
		LambdaSlot(LambdaSlot&& o) = default;
		LambdaSlot& operator=(LambdaSlot&& o) = default;

		const static bool safe_to_relocate = false;


		std::function<Rect(Widget * pWidget, Size parentSize)>	m_func = nullptr;
		bool			m_bVisible = false;
		RectI			m_geo;				// Widgets geo relative parent
	};


	class CLambdaSlotVector;
	typedef	StrongComponentPtr<CLambdaSlotVector>	CLambdaSlotVector_p;
	typedef	WeakComponentPtr<CLambdaSlotVector>		CLambdaSlotVector_wp;

	//____ CLambdaSlotVector ________________________________________________________

	class CLambdaSlotVector : public CDynamicSlotVector<LambdaSlot>
	{
		friend class LambdaPanel;
	public:


		//.____ Content _______________________________________________________

		iterator	pushFront(const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parenSize)> func );
		iterator	pushBack(const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parenSize)> func);

		iterator	insert( int index, const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func );
		iterator	insert( iterator pos, const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func );

		//.____ Misc __________________________________________________________

		inline CLambdaSlotVector_p	ptr() { return CLambdaSlotVector_p(this); }

	protected:

		//____ Holder _________________________________________________________

		class Holder : public CDynamicSlotVector<LambdaSlot>::Holder
		{
		};

		CLambdaSlotVector(CLambdaSlotVector::Holder * pHolder) : CDynamicSlotVector<LambdaSlot>(pHolder) {}

		Holder *		_holder() { return static_cast<Holder*>(CDynamicSlotVector<LambdaSlot>::_holder()); }
		const Holder *	_holder() const { return static_cast<const Holder*>(CDynamicSlotVector<LambdaSlot>::_holder()); }
	};


	//____ LambdaPanel _________________________________________________________

	class LambdaPanel : public Panel, protected CLambdaSlotVector::Holder
	{

	public:

		//.____ Creation __________________________________________

		static LambdaPanel_p	create() { return LambdaPanel_p(new LambdaPanel()); }

		//.____ Components _______________________________________

		CLambdaSlotVector	slots;

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


		// Methods for CLambdaSlotVector::Holder

		void		_didAddSlots( StaticSlot * pSlot, int nb ) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots( StaticSlot * pSlot, int nb ) override;
		void		_hideSlots( StaticSlot * pSlot, int nb ) override;
		void		_unhideSlots( StaticSlot * pSlot, int nb ) override;
		void		_updateSlotGeo(StaticSlot * pSlot, int nb) override;

		// Overloaded from LambdaSlotHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }
		const Object *	_object() const override { return this; }

		CoordI		_childPos(const StaticSlot * pSlot) const override;
		CoordI		_childGlobalPos(const StaticSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(const StaticSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection(const StaticSlot * pSlot) const override { return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(StaticSlot * pSlot) override { return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

	private:

		SizeI		_preferredSize() const override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const SizeI& size ) override;
		void		_updateGeo(LambdaSlot * pSlot, bool bForceResize = false);

		void		_onRequestRender( const RectI& rect, const LambdaSlot * pSlot );

		SizeI		m_minSize;
		SizeI		m_maxSize;
		SizeI		m_preferredSize;
	};



} // namespace wg
#endif //WG_LAMBDAPANEL_DOT_H
