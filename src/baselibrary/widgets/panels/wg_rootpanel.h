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
#include <wg_slot.h>
#include <wg_root.h>

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


	class RootPanel : public Object, public Root, protected SlotHolder, protected SkinSlot::Holder
	{
		friend class Widget;
		friend class Container;
		friend class InputHandler;

	public:

		//.____ Creation __________________________________________

		static RootPanel_p	create();
		static RootPanel_p	create(Surface* pCanvasSurface, GfxDevice* pDevice = nullptr);
		static RootPanel_p	create(CanvasRef canvasRef, GfxDevice* pDevice = nullptr);

		//.____ Components ____________________________________

		DynamicSlot			slot;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		bool				setScale(int scale);
		void				clearScale();
		inline int			scale() const;
		inline bool			isScaleSet() const;

		bool				setGeo( const Rect& geo );
		Rect				geo() const;

		//.____ State _________________________________________________

		bool				setVisible( bool bVisible );							//TODO: Implement!!!
		bool				isVisible() const { return m_bVisible; }

		inline Widget_p		focusedChild() const { return _focusedChild(); }

		//.____ Appearance ____________________________________________________

		virtual void		setSkin(Skin* pSkin);
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

		bool				setCanvas(Surface* pCanvasSurface);
		bool				setCanvas(CanvasRef canvasRef);
		inline const CanvasInfo& canvasInfo() const { return m_canvas; }
		inline SizeI		canvasSize() const { return m_canvas.size; }

		void				setCanvasLayers( CanvasLayers * pLayers );
		CanvasLayers_p		canvasLayers() const { return m_pCanvasLayers; }

		Widget_p			findWidget( const Coord& ofs, SearchMode mode ) { return Widget_p(_findWidget( Util::ptsToSpx(ofs,m_scale)-m_geo.pos(),mode)); }

		inline int			nbDirtyRects() const { return m_dirtyPatches.size(); }
		inline const RectSPX*	firstDirtyRect() const { return m_dirtyPatches.isEmpty() ? nullptr : m_dirtyPatches.begin(); }

		inline int			nbUpdatedRects() const { return m_updatedPatches.size(); }
		inline const RectSPX*	firstUpdatedRect() const { return m_updatedPatches.isEmpty() ? nullptr : m_updatedPatches.begin(); }

		inline void			addDirtyPatch( const RectSPX& rect ) override { m_dirtyPatches.add( rect ); }
		inline void         addPreRenderCall(Widget* pWidget) override { m_preRenderCalls.push_back(pWidget); }


	protected:
		RootPanel();
		RootPanel(Surface* pCanvasSurface, GfxDevice* pGfxDevice );
		RootPanel(CanvasRef canvasRef, GfxDevice* pGfxDevice);
		~RootPanel();

		// SlotHolder methods

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Container *  	_container() override;
		Root *			_root() override;
		int				_scale() const override;

		CoordSPX		_childPos( const StaticSlot * pSlot ) const override;
		CoordSPX		_childGlobalPos( const StaticSlot * pSlot ) const override;

		bool			_isChildVisible( const StaticSlot * pSlot ) const override;
		RectSPX			_childWindowSection( const StaticSlot * pSlot ) const override;

		void			_childRequestRender( StaticSlot * pSlot ) override;
		void			_childRequestRender( StaticSlot * pSlot, const RectSPX& rect ) override;
		void			_childRequestResize( StaticSlot * pSlot ) override;

		bool			_childRequestFocus( StaticSlot * pSlot, Widget * pWidget ) override;
		bool			_childReleaseFocus( StaticSlot * pSlot, Widget * pWidget ) override;

		void			_childRequestInView( StaticSlot * pSlot ) override;
		void			_childRequestInView( StaticSlot * pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea ) override;

		Widget *		_prevChild( const StaticSlot * pSlot ) const override;
		Widget *		_nextChild( const StaticSlot * pSlot ) const override;

		void			_releaseChild( StaticSlot * pSlot ) override;
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;

		// Methods for skin to access

		float			_skinValue(const SkinSlot* pSlot) const override;
		float			_skinValue2(const SkinSlot* pSlot) const override;

		State			_skinState(const SkinSlot* pSlot) const override;

		SizeSPX			_skinSize(const SkinSlot* pSlot) const override;

		void			_skinRequestRender(const SkinSlot* pSlot) override;
		void			_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect) override;


		Widget *			_findWidget( const CoordSPX& ofs, SearchMode mode );

//		void				_setFocusedChild( Widget * pWidget );
		Widget *			_focusedChild() const;

		PatchesSPX			m_dirtyPatches;		// Dirty patches that needs to be rendered.
		PatchesSPX			m_updatedPatches;	// Patches that were updated in last rendering session.

		std::vector<Widget_p>   m_preRenderCalls;

		bool				m_bDebugMode = false;
		Skin_p				m_pDebugOverlay;
		int					m_afterglowFrames;
		std::deque<PatchesSPX>	m_afterglowRects;	// Afterglow rects are placed in this queue.

		GfxDevice_p			m_pGfxDevice;
		CanvasInfo			m_canvas;			// Size of canvas in subpixels, when m_pCanvas is null.
		CanvasLayers_p		m_pCanvasLayers;

		RectSPX				m_geo;
		bool				m_bHasGeo = false;

		int					m_scale;
		bool				m_bScaleSet = false;// Set when scale is explicitly specified and not taken from canvas.

		bool				m_bVisible = true;

		SkinSlot			m_skin;				//TODO: Padding is not respected yet.


		Widget_wp			m_pFocusedChild;
	};

	//____ scale() ____________________________________________________________

	int RootPanel::scale() const
	{
		return m_scale;
	}

	//____ isScaleSet() _______________________________________________________

	bool RootPanel::isScaleSet() const
	{
		return m_bScaleSet;
	}

	//____ skin() _____________________________________________________________

	Skin_p RootPanel::skin() const
	{
		return m_skin.get();
	}

} // namespace wg
#endif //WG_ROOTPANEL_DOT_H
