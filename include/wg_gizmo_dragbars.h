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

#ifndef	WG_GIZMO_DRAGBARS_DOT_H
#define	WG_GIZMO_DRAGBARS_DOT_H

#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef	WG_TOOLTIP_DOT_H
#	include <wg_tooltip.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif


class WgSurface;

//____ Class: WgGizmoDragbar _____________________________________________________

class	WgGizmoDragbar : public WgGizmo
{
	public:
		WgGizmoDragbar();
		virtual	~WgGizmoDragbar();
		virtual const char * Type() const;
		static const char * GetMyType();

		//____ Callbacks ________________________________________

		static void cbSetSlider				(void * pGizmo, float pos, float size) { ((WgGizmoDragbar*)pGizmo)->SetSlider(pos,size); }
		static void cbSetSliderPos			(void * pGizmo, float pos) { ((WgGizmoDragbar*)pGizmo)->SetSliderPos(pos); }
		static void cbSetSliderSize			(void * pGizmo, float size) { ((WgGizmoDragbar*)pGizmo)->SetSliderSize(size); }
		static void cbSetSliderPosPxlOfs	(void * pGizmo, int x) { ((WgGizmoDragbar*)pGizmo)->SetSliderPosPxlOfs(x); }

		//____ Enums ____________________________________________

		enum ButtonLayout
		{
			NONE		= 0,
			HEADER_FWD	= 1,
			HEADER_BWD	= 2,
			FOOTER_FWD	= 4,
			FOOTER_BWD	= 8,
			WINDOWS		= HEADER_BWD | FOOTER_FWD,							// Like windows
			NEXT_VERT	= FOOTER_FWD | FOOTER_BWD,							// Like NeXT's vertical dragbar.
			NEXT_HORR	= HEADER_BWD | HEADER_FWD,							// Like NeXT's horizontal dragbar. 
			ALL			= HEADER_FWD | FOOTER_FWD | HEADER_BWD | FOOTER_BWD,


			DEFAULT		= WINDOWS
		};

		enum BgPressMode
		{
			SKIP_PAGE,
			GOTO_POS
		};
		
		//____ Methods __________________________________________
					
		float			GetSliderPos() const { return m_sliderPos; }
		float			GetSliderSize() const { return m_sliderSize; }
		WgBlockSetPtr 	GetBgSource() const { return m_pBgGfx; }
		WgBlockSetPtr 	GetBarSource() const { return m_pBarGfx; }
		WgBlockSetPtr 	GetBwdSource() const { return m_pBtnBwdGfx; }
		WgBlockSetPtr 	GetFwdSource() const { return m_pBtnFwdGfx; }
		ButtonLayout	GetButtonLayout() const { return m_btnLayout; }
		
		void	SetBgPressMode( BgPressMode mode );
		BgPressMode GetBgPressMode() const { return m_bgPressMode; }

		bool	SetSlider( float pos, float size );
		bool	SetSliderPos( float pos );
		bool	SetSliderPosPxlOfs( int x );
		bool	SetSliderSize( float size );

		bool	SetSource( WgBlockSetPtr pBgGfx, WgBlockSetPtr pBarGfx, WgBlockSetPtr pBtnBwdGfx, WgBlockSetPtr pBtnFwdGfx );
		bool	SetButtonLayout(  ButtonLayout layout );

		WgSize	DefaultSize() const;

		DECLARE_TOOLTIP_SUPPORT();

	protected:
		void	_onCloneContent( const WgGizmo * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
		void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
#ifdef WG_TNG
		void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
#endif
		void	_onRefresh();
		bool	_onAlphaTest( const WgCoord& ofs );
		void	_onEnable();
		void	_onDisable();


		enum Component
		{
			C_HEADER_FWD	= 0,
			C_HEADER_BWD	= 1,
			C_FOOTER_FWD	= 2,
			C_FOOTER_BWD	= 3,
			C_BG			= 4,
			C_BAR			= 5,

			C_NUMBER_OF_COMPONENTS	= 6,
			C_NONE			= -1,

		};
		
		WgBlockSetPtr	m_pBgGfx;
		WgBlockSetPtr	m_pBarGfx;
		WgBlockSetPtr	m_pBtnFwdGfx;
		WgBlockSetPtr	m_pBtnBwdGfx;
				
		float			m_sliderPos;
		float			m_sliderSize;
  	
		BgPressMode		m_bgPressMode;
		bool			m_bHorizontal;
		bool			m_bPressOnDragBar;
		int				m_dragBarPressOfs;
		WgCoord			m_lastCursorDownPos;

		ButtonLayout	m_btnLayout;
		Uint8			m_headerLen;
		Uint8			m_footerLen;

		WgMode			m_mode[C_NUMBER_OF_COMPONENTS];	

		WgSize			m_minSize;




	private:



		void	_viewToPosLen( int * _wpPos, int * _wpLen );
		void	_updateMinSize( void );


		Component	_findMarkedComponent( WgCoord ofs );								// -1 = None.
		void		_renderButton( WgGfxDevice * pDevice, const WgRect& _clip, WgRect& _dest, const WgBlock& _block );
		bool		_markTestButton( WgCoord ofs, WgRect& _dest, const WgBlock& _block );
		bool		_markTestSlider( WgCoord ofs );
		void		_headerFooterChanged();
		void		_unmarkReqRender();
};

//____ Class: WgGizmoHDragbar _______________________________________________________

class WgGizmoHDragbar:public WgGizmoDragbar
{
	public:
		WgGizmoHDragbar();

		virtual const char * Type() const;
		static const char * GetMyType();
		virtual WgGizmo * NewOfMyType() const { return new WgGizmoHDragbar(); };
		
};

//____ Class: WgGizmoVDragbar _______________________________________________________

class WgGizmoVDragbar:public WgGizmoDragbar
{
	public:
		WgGizmoVDragbar();

		virtual const char * Type() const;
		static const char * GetMyType();
		virtual WgGizmo * NewOfMyType() const { return new WgGizmoVDragbar(); };

};

#endif //	WG_GIZMO_DRAGBARS_DOT_H
