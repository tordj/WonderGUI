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
#include <wg_islot.h>

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

		//.____ Interfaces ____________________________________

		ISlot				child;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static RootPanel_p	cast( Object * pObject );

		//.____ Geometry _________________________________________________

		bool				setGeo( const Rect& geo );
		inline Rect			geo() const { return Util::rawToQpix(_geo()); };

		//.____ State _________________________________________________

		bool				setVisible( bool bVisible );							//TODO: Implement!!!
		bool				isVisible() const { return m_bVisible; }

		inline Widget_p		focusedChild() const { return _focusedChild(); }

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


		Widget_p			findWidget( const Coord& ofs, SearchMode mode ) { return Widget_p(_findWidget( Util::qpixToRaw(ofs)-m_geo.pos(),mode)); }

		inline int			nbDirtyRects() const { return m_dirtyPatches.size(); }
		inline const Rect*	firstDirtyRect() const { return m_dirtyPatches.isEmpty() ? nullptr : reinterpret_cast<const Rect*>(m_dirtyPatches.begin()); }

		inline int			nbUpdatedRects() const { return m_updatedPatches.size(); }
		inline const Rect*	firstUpdatedRect() const { return m_updatedPatches.isEmpty() ? nullptr : reinterpret_cast<const Rect*>(m_updatedPatches.begin()); }

		inline void			addDirtyPatch( const Rect& rect ) { m_dirtyPatches.add( Util::qpixToRawAligned(rect) ); }


	protected:
		RootPanel();
		RootPanel( GfxDevice * pGfxDevice );
		~RootPanel();

		class RootSlot : public BasicSlot
		{
			friend class RootPanel;

			RootSlot(SlotHolder *pHolder) : BasicSlot(pHolder) {}
		};

		RectI			_geo() const;

		// WidgetHolder methods

		CoordI			_childPos( BasicSlot * pSlot ) const override;
		CoordI			_childGlobalPos( BasicSlot * pSlot ) const override;

		bool			_isChildVisible( BasicSlot * pSlot ) const override;
		RectI			_childWindowSection( BasicSlot * pSlot ) const override;

		Container *  	_container() override;
		RootPanel *		_root() override;

		void			_childRequestRender( BasicSlot * pSlot ) override;
		void			_childRequestRender( BasicSlot * pSlot, const RectI& rect ) override;
		void			_childRequestResize( BasicSlot * pSlot ) override;

		bool			_childRequestFocus( BasicSlot * pSlot, Widget * pWidget ) override;
		bool			_childReleaseFocus( BasicSlot * pSlot, Widget * pWidget ) override;

		void			_childRequestInView( BasicSlot * pSlot ) override;
		void			_childRequestInView( BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea ) override;

		Widget *		_prevChild( const BasicSlot * pSlot ) const override;
		Widget *		_nextChild( const BasicSlot * pSlot ) const override;

		void			_releaseChild( BasicSlot * pSlot ) override;
		void			_replaceChild(BasicSlot * pSlot, Widget * pNewChild) override;

		// ChildHolder methods

		Object *		_object() override;
//		const Object *	_object() const override;


		//

        inline void         _addPreRenderCall(Widget * pWidget) { m_preRenderCalls.push_back(pWidget); }
        
		Widget *			_findWidget( const CoordI& ofs, SearchMode mode );

//		void				_setFocusedChild( Widget * pWidget );
		Widget *			_focusedChild() const;


		Patches				m_dirtyPatches;		// Dirty patches that needs to be rendered.
		Patches				m_updatedPatches;	// Patches that were updated in last rendering session.

        std::vector<Widget_p>   m_preRenderCalls;

		bool				m_bDebugMode;
		Skin_p				m_pDebugOverlay;
		int					m_afterglowFrames;
		std::deque<Patches>	m_afterglowRects;	// Afterglow rects are placed in this queue.

		GfxDevice_p			m_pGfxDevice;
		RootSlot			m_child;
		RectI				m_geo;
		bool				m_bHasGeo;
		bool				m_bVisible;

		Widget_wp			m_pFocusedChild;
	};




} // namespace wg
#endif //WG_ROOTPANEL_DOT_H
