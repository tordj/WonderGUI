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

#ifndef	WG_SPLITPANEL_DOT_H
#define	WG_SPLITPANEL_DOT_H
#pragma once



#include <wg_panel.h>
#include <wg_cslot.h>
#include <wg_cslotarray.h>


namespace wg
{

	//____ SplitPanel ___________________________________________________________

	class SplitPanel;
	typedef	StrongPtr<SplitPanel>	SplitPanel_p;
	typedef	WeakPtr<SplitPanel>	SplitPanel_wp;

	class SplitPanel : public Panel
	{

	public:

		//____ Slot _________________________________________________________

		class Slot : public DynamicSlot
		{
			friend class SplitPanel;
			template<class S, int X> friend class CSlotArray;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Geometry _________________________________________________

			inline Coord	pos() const { return Util::spxToPts(m_geo.pos(),_holder()->_scale()); }
			inline Size		size() const { return Util::spxToPts(m_geo.size(), _holder()->_scale()); }
			inline Rect		geo() const { return Util::spxToPts(m_geo, _holder()->_scale());
			}

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:
			Slot() : DynamicSlot(nullptr) {}

			RectSPX	m_geo;
		};


		//____ CSlots ________________________________________________________

		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CSlots_p;
		typedef	WeakComponentPtr<CSlots>	CSlots_wp;

		class CSlots : public CSlotArray<Slot, 2>
		{
			friend class SplitPanel;
		public:

			//.____ Misc __________________________________________________________

			inline CSlots_p	ptr() { return CSlots_p(this); }

		protected:

			CSlots(SlotHolder * pHolder) : CSlotArray(pHolder) {}
		};


		//.____ Creation __________________________________________

		static SplitPanel_p	create() { return SplitPanel_p(new SplitPanel()); }

		//.____ Components _______________________________________

		CSlots			slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void			setAxis(Axis orientaiton);
		Axis			axis() const { return m_bHorizontal ? Axis::X : Axis::Y; }

		//.____ Appearance _________________________________________________

		void			setHandleSkin(Skin* pSkin);
		Skin_p			handleSkin() const { return m_handleSkin.get(); }

		void			setHandleThickness(pts thickness);
		pts				handleThickness() const { return m_handleThickness;  }

		//.____ Behavior _______________________________________________________

		enum ScaleBehavior
		{
			ScaleBoth,
			ScaleFirst,
			ScaleSecond
		};

		void			setScaleBehavior(ScaleBehavior behavior);
		ScaleBehavior	scaleBehavior() const { return m_scaleBehavior; }

		void			setBrokerFunction(std::function<pts(Widget * pFirst, Widget * pSecond, pts totalLength, float splitFactor, pts handleMovement)> func);
		std::function<pts(Widget * pFirst, Widget * pSecond, pts totalLength, float splitFactor, pts handleMovement)> brokerFunction() const { return m_brokerFunc;  }

		//.____ Control ________________________________________________________

		void		setSplitFactor(float fraction);
		float		splitFactor() const { return m_splitFactor; }

		//.____ Internal _______________________________________________________

		SizeSPX			_preferredSize(int scale = -1) const override;


	protected:
		SplitPanel();
		virtual ~SplitPanel();

		spx			_handleThickness(int scale) const;					// Takes both m_handleThickness and m_pHandleSkin into account.
		void		_updatePreferredSize();
		SizeSPX		_calcPreferredSize(int scale) const;
		bool		_updateGeo(spx handleMovement=0);
		spx			_defaultBroker(Widget * pFirst, Widget * pSecond, spx totalLength, float splitFactor, spx handleMovement);

		// Overloaded from Widget

		void		_resize(const SizeSPX& size, int scale = -1) override;
		void		_setState(State state) override;
		void		_refresh() override;
		void		_receive(Msg * pMsg) override;

		void		_render(GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window) override;

		void		_collectPatches(PatchesSPX& container, const RectSPX& geo, const RectSPX& clip) override;
		void		_maskPatches(PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode) override;

		bool		_alphaTest(const CoordSPX& ofs) override;

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

		CoordSPX	_childPos( const StaticSlot * pSlot) const override;

		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const RectSPX& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		//

		State			_skinState(const SkinSlot* pSlot) const override;
		SizeSPX			_skinSize(const SkinSlot* pSlot) const override;

		void			_skinRequestRender(const SkinSlot* pSlot) override;
		void			_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect) override;


		//

		SkinSlot		m_handleSkin;
		bool			m_bHorizontal;
		SizeSPX			m_preferredSize;
		float			m_splitFactor;			// fraction of available child length that goes to first child. Measured in 1/65536.
		ScaleBehavior	m_scaleBehavior;

		pts				m_handleThickness;			// Set to 0 to use default from handleSkin.
		RectSPX			m_handleGeo;
		State			m_handleState;
		spx				m_handlePressOfs;

		std::function<pts(Widget * pFirst, Widget * pSecond, pts totalLength, float splitFactor, pts handleMovement)>	m_brokerFunc;
	};

}

#endif //WG_SPLITPANEL_DOT_H
