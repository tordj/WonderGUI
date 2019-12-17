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
#ifndef WG_PACKPANEL_DOT_H
#define WG_PACKPANEL_DOT_H
#pragma once

#include <wg_sizebroker.h>
#include <wg_panel.h>
#include <wg_paddedslot.h>
#include <wg_cpaddedslotarray.h>

namespace wg
{

	class PackPanel;
	typedef	StrongPtr<PackPanel>		PackPanel_p;
	typedef	WeakPtr<PackPanel>			PackPanel_wp;


	//____ PackSlot ____________________________________________________________

	class PackSlot : public PaddedSlot		/** @private */
	{
		friend class PackPanel;
		friend class CPackSlotArray;
		template<class S> friend class SlotArray;
		friend class CPaddedSlotArray<PackSlot>::Holder;

	public:

		//____ Holder ___________________________________________________________

		class Holder : public PaddedSlot::Holder
		{
		};

		PackSlot(Holder *pHolder) : PaddedSlot(pHolder) {}

	protected:

		bool			bResizeRequired = false;
		float			weight = 1.f;				// Weight for space allocation.
		RectI			geo;						// Real geo of child (no padding included).
		SizeI			preferredSize;				// Cached padded preferred size from the child.
	};


	//____ CPackSlotArray ________________________________________________________

	class CPackSlotArray;
	typedef	StrongComponentPtr<CPackSlotArray>	CPackSlotArray_p;
	typedef	WeakComponentPtr<CPackSlotArray>	CPackSlotArray_wp;

	class CPackSlotArray : public CPaddedSlotArray<PackSlot>
	{
		friend class PackPanel;
	public:

		//____ Holder _________________________________________________________

		class Holder : public CPaddedSlotArray<PackSlot>::Holder 	/** @private */
		{
		public:
			virtual void	_reweightSlots(PackSlot * pSlot, int nb, float weight) = 0;
			virtual void	_reweightSlots(PackSlot * pSlot, int nb, const float * pWeights) = 0;
			virtual void	_refreshChildGeo() = 0;
		};

		//.____ Geometry _______________________________________________________

		bool		setWeight(int index, float weight);
		bool		setWeight(const SlotIterator& it, float weight);
		bool		setWeight(int index, int amount, float weight);
		bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, float weight);
		bool		setWeight(int index, int amount, const std::initializer_list<float> weights);
		bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, const std::initializer_list<float> weights);

		float		weight(int index) const;
		float		weight(const SlotIterator&  it) const;

		//.____ Misc __________________________________________________________

		inline CPackSlotArray_p	ptr() { return CPackSlotArray_p(this); }

	protected:
		CPackSlotArray(Holder * pHolder) : CPaddedSlotArray<PackSlot>(pHolder) {}

		Holder *		_holder() { return static_cast<Holder*>(CPaddedSlotArray<PackSlot>::_holder()); }
		const Holder *	_holder() const { return static_cast<const Holder*>(CPaddedSlotArray<PackSlot>::_holder()); }
	};

	//____ PackPanel ________________________________________________________________
	/**
	 * @brief	A widget for arranging children horizontally or vertically.
	 *
	 * A widget for arranging children horizontally or vertically.
	 */

	class PackPanel : public Panel, protected CPackSlotArray::Holder
	{
		friend class CPackSlotArray;

	public:

		//.____ Creation __________________________________________

		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }

		//.____ Components _______________________________________

		CPackSlotArray		slots;

		//.____ Identification __________________________________________

		bool		isInstanceOf( const char * pClassName ) const override;
		const char *className( void ) const override;
		static const char	CLASSNAME[];
		static PackPanel_p	cast( Object * pObject );

		//.____ Geometry ____________________________________________

		void			setOrientation( Orientation orientaiton );
		Orientation		orientation() const { return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical; }

		//.____ Behavior ________________________________________________________

		void			setSizeBroker( SizeBroker * pBroker );
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }


	protected:
		PackPanel();
		virtual ~PackPanel();
		virtual Widget* _newOfMyType() const  override { return new PackPanel(); };

		// Overloaded from Widget

		void			_resize( const SizeI& size ) override;

		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		SizeI			_preferredSize() const override;


		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;


		// Overloaded from CPaddedSlotArray::Holder

		void		_didAddSlots( BasicSlot * pSlot, int nb ) override;
		void		_didMoveSlots(BasicSlot * pFrom, BasicSlot * pTo, int nb) override;
		void		_willRemoveSlots( BasicSlot * pSlot, int nb ) override;
		void		_hideSlots( BasicSlot *, int nb ) override;
		void		_unhideSlots( BasicSlot *, int nb ) override;
		void		_repadSlots( BasicSlot *, int nb, BorderI padding ) override;
		void		_repadSlots(BasicSlot *, int nb, const BorderI * pPaddings) override;

		// Needed by CPackSlotArray

		void		_reweightSlots(PackSlot * pSlot, int nb, float weight);
		void		_reweightSlots(PackSlot * pSlot, int nb, const float * pWeights);
		void		_refreshChildGeo() { _refreshChildGeo(true); }

		// Overloaded from PackSlotHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }
		const Object *	_object() const override { return this; }

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
		void		_replaceChild(BasicSlot * pSlot, Widget * pNewWidget ) override;

		//

		void		_refreshChildGeo(bool bRequestRender);

		void		_hideChildren(PackSlot * pSlot, int nb);
		void		_unhideChildren(PackSlot * pSlot, int nb);

		void		_refreshGeometries();
		SizeI		_calcPreferredSize();
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const;

		int			_setItemLengths(SizeBrokerItem * pItems, int nItems, int totalLength) const;
		int			_setPreferredLengths(SizeBrokerItem * pItems, int nItems) const;

		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		SizeI			m_preferredContentSize;

	};



} // namespace wg
#endif //WG_PACKPANEL_DOT_H
