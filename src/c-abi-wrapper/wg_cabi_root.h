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
#ifndef WG_CABIROOT_DOT_H
#define WG_CABIROOT_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_slotholder.h>
#include <wg_geo.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_gfxdevice.h>
#include <wg_slot.h>
#include <wg_root.h>

#include <wg_cabi_root_interface.h>

namespace wg
{

	class Widget;


	class CABIRoot;
	typedef	StrongPtr<CABIRoot>	CABIRoot_p;
	typedef	WeakPtr<CABIRoot>	CABIRoot_wp;




	//____ CABIRoot ____________________________________________________________

	/**
	 * @brief	Special widget, being the root of all widgets on screen.
	 *
	 * Special widget, being the root of all widgets on screen.
	 *
	 *
	 */


	class CABIRoot : public Object, public Root, protected SlotHolder
	{
		friend class Widget;
		friend class Container;
		friend class InputHandler;

	public:

		//.____ Creation __________________________________________

		static CABIRoot_p	create();
		static CABIRoot_p	create( wg_cabi_root_outcalls * pBridge );

		//.____ Components ____________________________________

		DynamicSlot			slot;

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		inline int			scale() const;

		//.____ State _________________________________________________

		inline Widget_p		focusedChild() const { return _focusedChild(); }


		//.____ Misc ___________________________________________________________

		Widget_p			findWidget(const Coord& ofs, SearchMode mode);

		void				addDirtyPatch(const RectSPX& rect) override;
		void				addPreRenderCall(Widget* pWidget) override;

		//.____ Internal ________________________________________________________

		spx			_matchingHeight(spx width, int scale = -1) const;
		spx			_matchingWidth(spx height, int scale = -1) const;

		SizeSPX		_defaultSize(int scale = -1) const;
		SizeSPX		_minSize(int scale = -1) const;
		SizeSPX		_maxSize(int scale = -1) const;

		bool		_markTest(const CoordSPX& ofs);

		void    	_preRender();
		void		_render(wg_obj device, const RectSPX& _canvas, const RectSPX& _window);

		void		_resize(const SizeSPX& size, int scale = -1);
		void		_setState(State state);

		void		_receive(Msg* pMsg);

	protected:
		CABIRoot();
		CABIRoot(wg_cabi_root_outcalls* pBridge);
		~CABIRoot();

		// SlotHolder methods

		const TypeInfo& _slotTypeInfo(const StaticSlot* pSlot) const override;

		Container*		_container() override;
		Root*			_root() override;
		int				_scale() const override;

		CoordSPX		_childPos(const StaticSlot* pSlot) const override;
		CoordSPX		_childGlobalPos(const StaticSlot* pSlot) const override;

		bool			_isChildVisible(const StaticSlot* pSlot) const override;
		RectSPX			_childWindowSection(const StaticSlot* pSlot) const override;

		void			_childRequestRender(StaticSlot* pSlot) override;
		void			_childRequestRender(StaticSlot* pSlot, const RectSPX& rect) override;
		void			_childRequestResize(StaticSlot* pSlot) override;

		bool			_childRequestFocus(StaticSlot* pSlot, Widget* pWidget) override;
		bool			_childReleaseFocus(StaticSlot* pSlot, Widget* pWidget) override;

		void			_childRequestInView(StaticSlot* pSlot) override;
		void			_childRequestInView(StaticSlot* pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea) override;

		Widget*			_prevChild(const StaticSlot* pSlot) const override;
		Widget*			_nextChild(const StaticSlot* pSlot) const override;

		void			_releaseChild(StaticSlot* pSlot) override;
		void			_replaceChild(StaticSlot* pSlot, Widget* pNewChild) override;

		void			_didAddSlots(StaticSlot* pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot* pFrom, StaticSlot* pTo, int nb) override;
		void			_willEraseSlots(StaticSlot* pSlot, int nb) override;





		Widget*			_findWidget(const CoordSPX& ofs, SearchMode mode);

		//		void				_setFocusedChild( Widget * pWidget );
		Widget*			_focusedChild() const;


		wg_cabi_root_outcalls	m_cabi;

		std::vector<Widget_p>   m_preRenderCalls;


		GfxDevice_p			m_pGfxDevice;
		CanvasLayers_p		m_pCanvasLayers;

		RectSPX				m_size;
		int					m_scale;


		Widget_wp			m_pFocusedChild;
	};

	//____ scale() ____________________________________________________________

	int CABIRoot::scale() const
	{
		return m_scale;
	}


} // namespace wg
#endif //WG_ROOTPANEL_DOT_H
