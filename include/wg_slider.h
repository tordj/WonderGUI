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

#ifndef	WG_SLIDER_DOT_H
#define	WG_SLIDER_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif


class WgSurface;
class WgSliderTarget;
class WgMenu;

//____ Class: WgWidgetSlider _____________________________________________________

class	WgWidgetSlider : public WgWidget
{
	friend class WgSliderTarget;
	friend class WgMenu;

	public:
		WgWidgetSlider();
		virtual	~WgWidgetSlider();
		virtual const char * Type() const;
		static const char * GetClass();

		//____ Callbacks ________________________________________

		static void cbSetSlider				(void * pWidget, float pos, float size) { ((WgWidgetSlider*)pWidget)->SetSlider(pos,size); }
		static void cbSetSliderPos			(void * pWidget, float pos) { ((WgWidgetSlider*)pWidget)->SetSliderPos(pos); }
		static void cbSetSliderSize			(void * pWidget, float size) { ((WgWidgetSlider*)pWidget)->SetSliderSize(size); }
		static void cbSetSliderPosPxlOfs	(void * pWidget, int x) { ((WgWidgetSlider*)pWidget)->SetSliderPosPxlOfs(x); }

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
		WgBlocksetPtr 	GetBgSource() const { return m_pBgGfx; }
		WgBlocksetPtr 	GetBarSource() const { return m_pBarGfx; }
		WgBlocksetPtr 	GetBwdSource() const { return m_pBtnBwdGfx; }
		WgBlocksetPtr 	GetFwdSource() const { return m_pBtnFwdGfx; }
		ButtonLayout	GetButtonLayout() const { return m_btnLayout; }

		void	SetBgPressMode( BgPressMode mode );
		BgPressMode GetBgPressMode() const { return m_bgPressMode; }

		bool	SetSlider( float pos, float size );
		bool	SetSliderPos( float pos );
		bool	SetSliderPosPxlOfs( int x );
		bool	SetSliderSize( float size );

		bool	SetSource( WgBlocksetPtr pBgGfx, WgBlocksetPtr pBarGfx, WgBlocksetPtr pBtnBwdGfx, WgBlocksetPtr pBtnFwdGfx );
		bool	SetButtonLayout(  ButtonLayout layout );

		bool	SetSliderTarget( WgSliderTarget * pTarget );

		WgSize	DefaultSize() const;

	protected:
		void	_onCloneContent( const WgWidget * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
		void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
		void	_onRefresh();
		bool	_onAlphaTest( const WgCoord& ofs );
		void	_onEnable();
		void	_onDisable();

		bool	_setSlider( float pos, float size );		// Set slider pos/size without notifying target (but should post events).

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

		WgBlocksetPtr	m_pBgGfx;
		WgBlocksetPtr	m_pBarGfx;
		WgBlocksetPtr	m_pBtnFwdGfx;
		WgBlocksetPtr	m_pBtnBwdGfx;

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

		WgSliderTarget *m_pSliderTargetInterface;			// So we can access our target.
		WgWidgetWeakPtr	m_pSliderTargetWidget;				// So we can check if target has been deleted.


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

//____ Class: WgHSlider _______________________________________________________

class WgHSlider:public WgWidgetSlider
{
	public:
		WgHSlider();

		virtual const char * Type() const;
		static const char * GetClass();
		virtual WgWidget * NewOfMyType() const { return new WgHSlider(); };

};

//____ Class: WgVSlider _______________________________________________________

class WgVSlider:public WgWidgetSlider
{
	public:
		WgVSlider();

		virtual const char * Type() const;
		static const char * GetClass();
		virtual WgWidget * NewOfMyType() const { return new WgVSlider(); };

};

#endif //	WG_SLIDER_DOT_H
