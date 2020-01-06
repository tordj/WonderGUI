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
#include <wg_cpaddedslotarray.h>


namespace wg
{

	class StackPanel;
	typedef	StrongPtr<StackPanel>	StackPanel_p;
	typedef	WeakPtr<StackPanel>		StackPanel_wp;


	//____ StackSlot ____________________________________________________________

	class StackSlot : public PaddedSlot
	{
		friend class StackPanel;
		friend class CStackSlotArray;
		friend class CDynamicSlotArray<StackSlot>;

	public:

		//.____ Properties _________________________________________________

		void				setSizePolicy(SizePolicy2D policy) { _holder()->_setSizePolicy(this, policy); }
		inline SizePolicy2D sizePolicy() const { return m_sizePolicy; }

		void				setOrigo(Origo origo) { _holder()->_setOrigo(this, origo); }
		inline Origo		origo() const { return m_origo; }

		//.____ Operators __________________________________________

		inline void operator=(Widget * pWidget) { setWidget(pWidget); }

	protected:
		class Holder : public PaddedSlot::Holder
		{
		public:
			virtual void		_setSizePolicy(StackSlot * pSlot, SizePolicy2D policy) = 0;
			virtual void		_setOrigo(StackSlot * pSlot, Origo origo) = 0;
		};

		StackSlot(Holder * pHolder) : PaddedSlot(pHolder) {}
		StackSlot(StackSlot&& o) = default;
		StackSlot& operator=(StackSlot&& o) = default;


		Holder *		_holder() { return static_cast<Holder*>(PaddedSlot::_holder()); }
		const Holder *	_holder() const { return static_cast<const Holder*>(PaddedSlot::_holder()); }

		Origo			m_origo			= Origo::Center;
		SizePolicy2D	m_sizePolicy	= SizePolicy2D::Original;
	};


	//____ CStackSlotArray ________________________________________________________

	class CStackSlotArray;
	typedef	StrongComponentPtr<CStackSlotArray>	CStackSlotArray_p;
	typedef	WeakComponentPtr<CStackSlotArray>	CStackSlotArray_wp;

	class CStackSlotArray : public CPaddedSlotArray<StackSlot>
	{
		friend class StackPanel;
	public:

		//.____ Misc __________________________________________________________

		inline CStackSlotArray_p	ptr() { return CStackSlotArray_p(this); }

	protected:

		class Holder : public CPaddedSlotArray<StackSlot>::Holder			/** @private */
		{
		};

		CStackSlotArray(Holder * pHolder) : CPaddedSlotArray<StackSlot>(pHolder) {}
	};



	//____ StackPanel ___________________________________________________________

	/**
	*/

	class StackPanel : public Panel, protected CStackSlotArray::Holder
	{
		friend class CStackSlotArray;

	public:

		//.____ Creation __________________________________________

		static StackPanel_p	create() { return StackPanel_p(new StackPanel()); }

		//.____ Components _______________________________________

		CStackSlotArray	slots;

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

		// Overloaded from StackCSlotArray::Holder

		void		_didAddSlots( StaticSlot * pSlot, int nb ) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots( StaticSlot * pSlot, int nb ) override;
		void		_hideSlots( StaticSlot *, int nb ) override;
		void		_unhideSlots( StaticSlot *, int nb ) override;
		void		_repadSlots( StaticSlot *, int nb, BorderI padding ) override;
		void		_repadSlots(StaticSlot *, int nb, const BorderI * pPaddings) override;
		void		_setSizePolicy(StackSlot * pSlot, SizePolicy2D policy) override;
		void		_setOrigo(StackSlot * pSlot, Origo origo) override;


		// Overloaded from StackSlotHolder

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
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

		// Internal to StackPanel

		SizeI 	_calcPreferredSize();
		void	_adaptChildrenToSize();

		void	_hideChildren( StackSlot * pSlot, int nb );
		void	_unhideChildren( StackSlot * pSlot, int nb );


		RectI	_childGeo( const StackSlot * pSlot ) const;

		SizeI	m_preferredSize;
	};


} // namespace wg
#endif //WG_STACKPANEL_DOT_H
