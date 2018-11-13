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
#include <wg_child.h>


namespace wg
{

	class SplitPanel;
	typedef	StrongPtr<SplitPanel>	SplitPanel_p;
	typedef	WeakPtr<SplitPanel>	SplitPanel_wp;


	class SplitPanelSlot : public Slot 	/** @private */
	{
	public:
		Rect	geo;
	};


	class SplitPanel : public Panel, protected ChildHolder
	{

	public:
		//.____ Creation __________________________________________

		static SplitPanel_p	create() { return SplitPanel_p(new SplitPanel()); }

		//.____ Interfaces _______________________________________

		Child<SplitPanelSlot,ChildHolder>	first;
		Child<SplitPanelSlot,ChildHolder>	second;

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const;
		const char *			className(void) const;
		static const char		CLASSNAME[];
		static SplitPanel_p		cast(Object * pObject);

		//.____ Geometry ____________________________________________

		void			setOrientation(Orientation orientaiton);
		Orientation		orientation() const { return m_bHorizontal ? Orientation::Horizontal : Orientation::Vertical; }

		Size			preferredSize() const;

		//.____ Appearance _________________________________________________

		void			setHandleSkin(Skin * pSkin);
		Skin_p			handleSkin() const { return m_pHandleSkin; }

		void			setHandleThickness(int thickness);
		int				handleThickness() const { return m_handleThickness;  }

		//.____ Behavior _______________________________________________________

		enum ScaleBehavior
		{
			ScaleBoth,
			ScaleFirst,
			ScaleSecond
		};

		void			setScaleBehavior(ScaleBehavior behavior);
		ScaleBehavior	scaleBehavior() const { return m_scaleBehavior; }

		void			setBrokerFunction(std::function<int(Widget * pFirst, Widget * pSecond, int totalLength, float splitFactor, int handleMovement)> func);
		std::function<int(Widget * pFirst, Widget * pSecond, int totalLength, float splitFactor, int handleMovement)> brokerFunction() const { return m_brokerFunc;  }

		//.____ Control ________________________________________________________

		void		setSplitFactor(float fraction);
		float		splitFactor() const { return m_splitFactor; }

	protected:
		SplitPanel();
		virtual ~SplitPanel();
		virtual Widget* _newOfMyType() const { return new SplitPanel(); };

		int			_handleThickness();					// Takes both m_handleThickness and m_pHandleSkin into account.
		void		_updatePreferredSize();
		bool		_updateGeo(int handleMovement=0);
		int			_defaultBroker(Widget * pFirst, Widget * pSecond, int totalLength, float splitFactor, int handleMovement);

		// Overloaded from Widget

		void		_setSize(const Size& size);
		void		_setState(State state);
		void		_refresh();
		void		_receive(Msg * pMsg);

		void		_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip);

		void		_collectPatches(Patches& container, const Rect& geo, const Rect& clip);
		void		_maskPatches(Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode);

		bool		_alphaTest(const Coord& ofs);
		void		_cloneContent(const Widget * _pOrg);

		// Overloaded from Container

		Widget *	_firstChild() const;
		Widget *	_lastChild() const;

		void		_firstSlotWithGeo(SlotWithGeo& package) const;
		void		_nextSlotWithGeo(SlotWithGeo& package) const;


		// Overloaded from ChildHolder

		void	_setWidget(Slot * pSlot, Widget * pNewWidget);
		Object * _object() { return this; }

		// Overloaded from WidgetHolder

		Coord		_childPos(Slot * pSlot) const;
		Size		_childSize(Slot * pSlot) const;

		void		_childRequestRender(Slot * pSlot);
		void		_childRequestRender(Slot * pSlot, const Rect& rect);
		void		_childRequestResize(Slot * pSlot);

		Widget *	_prevChild(Slot * pSlot) const;
		Widget *	_nextChild(Slot * pSlot) const;


		bool			m_bHorizontal;
		Size			m_preferredSize;
		float			m_splitFactor;			// fraction of available child length that goes to first child. Measured in 1/65536.
		ScaleBehavior	m_scaleBehavior;

		Skin_p			m_pHandleSkin;
		int				m_handleThickness;			// Set to 0 to use default from handleSkin.
		Rect			m_handleGeo;
		State			m_handleState;
		int				m_handlePressOfs;

		SplitPanelSlot	m_firstChild;
		SplitPanelSlot	m_secondChild;

		std::function<int(Widget * pFirst, Widget * pSecond, int totalLength, float splitFactor, int handleMovement)>	m_brokerFunc;
	};

}

#endif //WG_SPLITPANEL_DOT_H