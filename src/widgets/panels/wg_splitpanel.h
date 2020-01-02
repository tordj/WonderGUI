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
#include <wg_cstandardslot.h>


namespace wg
{

	class SplitPanel;
	typedef	StrongPtr<SplitPanel>	SplitPanel_p;
	typedef	WeakPtr<SplitPanel>	SplitPanel_wp;


	class SplitSlot : public StaticSlot
	{
		friend class SplitPanel;
	public:
		
		//.____ Geometry _________________________________________________

		inline Coord	pos() const { return Util::rawToQpix(m_geo.pos()); }
		inline Size		size() const { return Util::rawToQpix(m_geo.size()); }
		inline Rect		geo() const { return Util::rawToQpix(m_geo); }

	protected:
		class Holder : public StaticSlot::Holder
		{
		};

		RectI	m_geo;
	};



	class SplitPanel : public Panel, protected CStandardSlot::Holder
	{

	public:

		//.____ Creation __________________________________________

		static SplitPanel_p	create() { return SplitPanel_p(new SplitPanel()); }

		//.____ Components _______________________________________

		CStandardSlot		slotOne;
		CStandardSlot		slotTwo;

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const override;
		const char *			className(void) const override;
		static const char		CLASSNAME[];
		static SplitPanel_p		cast(Object * pObject);

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

		// Overloaded from SlotHolder

		Container *	_container() override	{ return this; }
		RootPanel *	_root() override		{ return Container::_root(); }
		Object *	_object() override		{ return this; }
		const Object *	_object() const override { return this; }

		CoordI		_childPos( const StaticSlot * pSlot) const override;
		CoordI		_childGlobalPos( const StaticSlot * pSlot) const override		{ return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible( const StaticSlot * pSlot) const override		{ return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection( const StaticSlot * pSlot) const override	{ return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(StaticSlot * pSlot) override			{ return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

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

		RectI			m_slotGeo[2];

		class SlotAccess : public CStandardSlot { friend class SplitPanel; };
		SlotAccess * _slotOne() { return static_cast<SlotAccess*>(&slotOne); }
		SlotAccess * _slotTwo() { return static_cast<SlotAccess*>(&slotTwo); }
		const SlotAccess * _slotOne() const { return static_cast<const SlotAccess*>(&slotOne); }
		const SlotAccess * _slotTwo() const { return static_cast<const SlotAccess*>(&slotTwo); }
	};

}

#endif //WG_SPLITPANEL_DOT_H
