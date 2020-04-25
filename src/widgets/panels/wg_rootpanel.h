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
#ifndef WG_ROOTPANEL_DOT_H
#define WG_ROOTPANEL_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_slotholder.h>
#include <wg_geo.h>
#include <wg_patches.h>
#include <wg_msgrouter.h>
#include <wg_gfxdevice.h>
#include <wg_cstandardslot.h>

namespace wg
{

	class Widget;


	class RootPanel;
	typedef	StrongPtr<RootPanel>	RootPanel_p;
	typedef	WeakPtr<RootPanel>	RootPanel_wp;


	//____ RootPanel ____________________________________________________________

	/**
	 * @brief	Special widget, being the root of all widgets on screen.
	 *
	 * Special widget, being the root of all widgets on screen.
	 *
	 *
	 */


	class RootPanel : public Object, protected SlotHolder
	{
		friend class Widget;
		friend class Container;
		friend class InputHandler;

	public:

		//.____ Creation __________________________________________

		static RootPanel_p	create() { return RootPanel_p(new RootPanel()); }
		static RootPanel_p	create( GfxDevice * pDevice ) { return RootPanel_p(new RootPanel(pDevice)); }

		//.____ Components ____________________________________

		CStandardSlot		slot;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		bool				setGeo( const Rect& geo );
		inline Rect			geo() const;

		//.____ State _________________________________________________

		bool				setVisible( bool bVisible );							//TODO: Implement!!!
		bool				isVisible() const { return m_bVisible; }

		inline Widget_p		focusedChild() const { return _focusedChild(); }

		//.____ Appearance _________________________________________________

		void				setSkin(Skin * pSkin);
		inline Skin_p		skin() const;

		//.____ Rendering ________________________________________________


		bool				render();
		bool				render( const Rect& clip );

		bool				beginRender();
		bool				renderSection( const Rect& clip );
		bool				endRender();


		//.____ Debug __________________________________________________________

		void				setDebugMode( bool onOff );
		bool				isDebugMode() const { return m_bDebugMode; }
		void 				setDebugOverlay(Skin * pOverlaySkin );
		void				setDebugAfterglow(int frames);
		Skin_p				debugOverlay() const { return m_pDebugOverlay;  }
		int					debugAfterglow() const { return m_afterglowFrames;  }

		//.____ Misc ___________________________________________________________

		bool				setGfxDevice( GfxDevice * pDevice );
		inline GfxDevice_p 	gfxDevice() const { return m_pGfxDevice; }


		Widget_p			findWidget( const Coord& ofs, SearchMode mode ) { return Widget_p(_findWidget( ofs-m_geo.pos(),mode)); }

		inline int			nbDirtyRects() const { return m_dirtyPatches.size(); }
		inline const Rect*	firstDirtyRect() const { return m_dirtyPatches.isEmpty() ? nullptr : reinterpret_cast<const Rect*>(m_dirtyPatches.begin()); }

		inline int			nbUpdatedRects() const { return m_updatedPatches.size(); }
		inline const Rect*	firstUpdatedRect() const { return m_updatedPatches.isEmpty() ? nullptr : reinterpret_cast<const Rect*>(m_updatedPatches.begin()); }

		inline void			addDirtyPatch( const Rect& rect ) { m_dirtyPatches.add( rect ); }


	protected:
		RootPanel();
		RootPanel( GfxDevice * pGfxDevice );
		~RootPanel();

		// SlotHolder methods

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Container *  	_container() override;
		RootPanel *		_root() override;
		Object *		_object() override;
		const Object *	_object() const override;

		Coord			_childPos( const StaticSlot * pSlot ) const override;
		Coord			_childGlobalPos( const StaticSlot * pSlot ) const override;

		bool			_isChildVisible( const StaticSlot * pSlot ) const override;
		Rect			_childWindowSection( const StaticSlot * pSlot ) const override;

		void			_childRequestRender( StaticSlot * pSlot ) override;
		void			_childRequestRender( StaticSlot * pSlot, const Rect& rect ) override;
		void			_childRequestResize( StaticSlot * pSlot ) override;

		bool			_childRequestFocus( StaticSlot * pSlot, Widget * pWidget ) override;
		bool			_childReleaseFocus( StaticSlot * pSlot, Widget * pWidget ) override;

		void			_childRequestInView( StaticSlot * pSlot ) override;
		void			_childRequestInView( StaticSlot * pSlot, const Rect& mustHaveArea, const Rect& niceToHaveArea ) override;

		Widget *		_prevChild( const StaticSlot * pSlot ) const override;
		Widget *		_nextChild( const StaticSlot * pSlot ) const override;

		void			_releaseChild( StaticSlot * pSlot ) override;
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		void			_selectSlots(StaticSlot * pSlot, int nb) override;
		void			_unselectSlots(StaticSlot * pSlot, int nb) override;

		void			_repadSlots(StaticSlot * pSlot, int nb, Border padding) override;
		void			_repadSlots(StaticSlot * pSlot, int nb, const Border * pPadding) override;

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;

		void			_hideSlots(StaticSlot * pSlot, int nb) override;
		void			_unhideSlots(StaticSlot * pSlot, int nb) override;

		//

		inline void         _addPreRenderCall(Widget * pWidget) { m_preRenderCalls.push_back(pWidget); }

		Widget *			_findWidget( const Coord& ofs, SearchMode mode );

//		void				_setFocusedChild( Widget * pWidget );
		Widget *			_focusedChild() const;

		Patches				m_dirtyPatches;		// Dirty patches that needs to be rendered.
		Patches				m_updatedPatches;	// Patches that were updated in last rendering session.

		std::vector<Widget_p>   m_preRenderCalls;

		Skin_p				m_pSkin;				//TODO: Padding is not respected yet.

		bool				m_bDebugMode;
		Skin_p				m_pDebugOverlay;
		int					m_afterglowFrames;
		std::deque<Patches>	m_afterglowRects;	// Afterglow rects are placed in this queue.

		GfxDevice_p			m_pGfxDevice;
		Rect				m_geo;
		bool				m_bHasGeo;
		bool				m_bVisible;

		Widget_wp			m_pFocusedChild;
	};


	//____ skin() _____________________________________________________________
	/**
	 * @brief Get the skin used as background.
	 *
	 * Get the skin used as background for the whole RootPanel.
	 *
	 * @return Pointer to the skin.
	 */

	Skin_p RootPanel::skin() const
	{
		return m_pSkin;
	}

} // namespace wg
#endif //WG_ROOTPANEL_DOT_H
