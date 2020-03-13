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

			//.____ Geometry _________________________________________________

			inline Coord	pos() const { return Util::rawToQpix(m_geo.pos()); }
			inline Size		size() const { return Util::rawToQpix(m_geo.size()); }
			inline Rect		geo() const { return Util::rawToQpix(m_geo); }

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:
			Slot() : DynamicSlot(nullptr) {}

			RectI	m_geo;
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

		bool					isInstanceOf(const char * pClassName) const override;
		const char *			className(void) const override;
		static const char		CLASSNAME[];

		//.____ Geometry ____________________________________________

		void			setOrientation(Orientation orientaiton);
		Orientation		orientation() const { return m_bHorizontal ? Orientation::Horizontal : Orientation::Vertical; }

		//.____ Appearance _________________________________________________

		void			setHandleSkin(Skin * pSkin);
		Skin_p			handleSkin() const { return m_pHandleSkin; }

		void			setHandleThickness(QPix thickness);
		QPix			handleThickness() const { return QPix::fromRaw(m_handleThickness);  }

		//.____ Behavior _______________________________________________________

		enum ScaleBehavior
		{
			ScaleBoth,
			ScaleFirst,
			ScaleSecond
		};

		void			setScaleBehavior(ScaleBehavior behavior);
		ScaleBehavior	scaleBehavior() const { return m_scaleBehavior; }

		void			setBrokerFunction(std::function<QPix(Widget * pFirst, Widget * pSecond, QPix totalLength, float splitFactor, QPix handleMovement)> func);
		std::function<QPix(Widget * pFirst, Widget * pSecond, QPix totalLength, float splitFactor, QPix handleMovement)> brokerFunction() const { return m_brokerFunc;  }

		//.____ Control ________________________________________________________

		void		setSplitFactor(float fraction);
		float		splitFactor() const { return m_splitFactor; }

	protected:
		SplitPanel();
		virtual ~SplitPanel();
		virtual Widget* _newOfMyType() const override { return new SplitPanel(); };

		int			_handleThickness();					// Takes both m_handleThickness and m_pHandleSkin into account.
		void		_updatePreferredSize();
		bool		_updateGeo(int handleMovement=0);
		QPix		_defaultBroker(Widget * pFirst, Widget * pSecond, QPix totalLength, float splitFactor, QPix handleMovement);

		// Overloaded from Widget

		SizeI		_preferredSize() const override;

		void		_resize(const SizeI& size) override;
		void		_setState(State state) override;
		void		_refresh() override;
		void		_receive(Msg * pMsg) override;

		void		_render(GfxDevice * pDevice, const RectI& _canvas, const RectI& _window) override;

		void		_collectPatches(Patches& container, const RectI& geo, const RectI& clip) override;
		void		_maskPatches(Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode) override;

		bool		_alphaTest(const CoordI& ofs) override;
		void		_cloneContent(const Widget * _pOrg) override;

		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo(SlotWithGeo& package) const override;
		void		_nextSlotWithGeo(SlotWithGeo& package) const override;

		CoordI		_childPos( const StaticSlot * pSlot) const override;

		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const RectI& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		//

		bool			m_bHorizontal;
		SizeI			m_preferredSize;
		float			m_splitFactor;			// fraction of available child length that goes to first child. Measured in 1/65536.
		ScaleBehavior	m_scaleBehavior;

		Skin_p			m_pHandleSkin;
		int				m_handleThickness;			// Set to 0 to use default from handleSkin.
		RectI			m_handleGeo;
		State			m_handleState;
		int				m_handlePressOfs;

		std::function<QPix(Widget * pFirst, Widget * pSecond, QPix totalLength, float splitFactor, QPix handleMovement)>	m_brokerFunc;
	};

}

#endif //WG_SPLITPANEL_DOT_H
