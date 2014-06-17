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

#ifndef	WG_SCROLLBAR_DOT_H
#define	WG_SCROLLBAR_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif


class WgSurface;
class WgScrollbarTarget;
class WgMenu;


class WgScrollbar;
typedef	WgStrongPtr<WgScrollbar,WgWidgetPtr>		WgScrollbarPtr;
typedef	WgWeakPtr<WgScrollbar,WgWidgetWeakPtr>		WgScrollbarWeakPtr;

//____ Class: WgScrollbar _____________________________________________________

class	WgScrollbar : public WgWidget
{
	friend class WgScrollbarTarget;
	friend class WgMenu;

	public:
		bool		IsInstanceOf( const char * pClassName ) const;
		const char *ClassName( void ) const;
		static const char	CLASSNAME[];
		static WgScrollbarPtr	Cast( const WgObjectPtr& pObject );

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
			JUMP_PAGE,
			GOTO_POS
		};

		//____ Methods __________________________________________

		float			GetHandlePos() const { return m_handlePos; }
		float			GetHandleSize() const { return m_handleSize; }
		WgSkinPtr 		GetBackgroundSkin() const { return m_pBgSkin; }
		WgSkinPtr 		GetHandleSkin() const { return m_pHandleSkin; }
		WgSkinPtr 		GetBwdButtonSkin() const { return m_pBtnBwdSkin; }
		WgSkinPtr 		GetFwdButtonSkin() const { return m_pBtnFwdSkin; }
		ButtonLayout	GetButtonLayout() const { return m_btnLayout; }

		void		SetBackgroundPressMode( BgPressMode mode );
		BgPressMode GetBackgroundPressMode() const { return m_bgPressMode; }

		bool	SetHandle( float pos, float size );
		bool	SetHandlePos( float pos );
		bool	SetHandlePosPxlOfs( int x );
		bool	SetHandleSize( float size );

		void	SetBackgroundSkin( const WgSkinPtr& pSkin );
		void	SetHandleSkin( const WgSkinPtr& pSkin );
		void	SetBwdButtonSkin( const WgSkinPtr& pSkin );
		void	SetFwdButtonSkin( const WgSkinPtr& pSkin );

		void	SetSkins( const WgSkinPtr& pBackgroundSkin, const WgSkinPtr& pHandleSkin, const WgSkinPtr& pBwdButtonSkin, const WgSkinPtr& pFwdButtonSkin );
		void	SetButtonLayout(  ButtonLayout layout );

		bool	SetScrollbarTarget( WgScrollbarTarget * pTarget );

		WgSize	PreferredSize() const;

	protected:
		WgScrollbar();
		virtual	~WgScrollbar();

		void	_onCloneContent( const WgWidget * _pOrg );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
		void	_onRefresh();
		bool	_onAlphaTest( const WgCoord& ofs );
		void	_onStateChanged( WgState oldState );

		bool	_setHandle( float pos, float size );		// Set scrollbar pos/size without notifying target (but should post events).

		enum Component
		{
			C_HEADER_FWD	= 0,
			C_HEADER_BWD	= 1,
			C_FOOTER_FWD	= 2,
			C_FOOTER_BWD	= 3,
			C_BG			= 4,
			C_HANDLE		= 5,

			C_NUMBER_OF_COMPONENTS	= 6,
			C_NONE			= -1,

		};

		WgSkinPtr		m_pBgSkin;
		WgSkinPtr		m_pHandleSkin;
		WgSkinPtr		m_pBtnFwdSkin;
		WgSkinPtr		m_pBtnBwdSkin;

		float			m_handlePos;
		float			m_handleSize;

		BgPressMode		m_bgPressMode;
		bool			m_bHorizontal;
		bool			m_bPressOnHandle;
		int				m_handlePressOfs;
		WgCoord			m_lastCursorDownPos;

		ButtonLayout	m_btnLayout;
		Uint8			m_headerLen;
		Uint8			m_footerLen;

		WgState			m_states[C_NUMBER_OF_COMPONENTS];

		WgSize			m_minSize;

		WgScrollbarTarget *m_pScrollbarTargetInterface;			// So we can access our target.
		WgWidgetWeakPtr	m_pScrollbarTargetWidget;				// So we can check if target has been deleted.


	private:



		void	_viewToPosLen( int * _wpPos, int * _wpLen );
		void	_updateMinSize( void );


		Component	_findMarkedComponent( WgCoord ofs );								// -1 = None.
		void		_renderButton( WgGfxDevice * pDevice, const WgRect& _clip, WgRect& _dest, const WgSkinPtr& pSkin, WgState state );
		bool		_markTestButton( WgCoord ofs, WgRect& _dest, const WgSkinPtr& pSkin, WgState state );
		bool		_markTestHandle( WgCoord ofs );
		void		_headerFooterChanged();
		void		_unhoverReqRender();
};

//____ Class: WgHScrollbar _______________________________________________________

class WgHScrollbar;
typedef	WgStrongPtr<WgHScrollbar,WgScrollbarPtr>		WgHScrollbarPtr;
typedef	WgWeakPtr<WgHScrollbar,WgScrollbarWeakPtr>		WgHScrollbarWeakPtr;

class WgHScrollbar:public WgScrollbar
{
public:
	static WgHScrollbarPtr	Create() { return WgHScrollbarPtr(new WgHScrollbar()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgHScrollbarPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgHScrollbar();
	virtual ~WgHScrollbar() {}
	virtual WgWidget* _newOfMyType() const { return new WgHScrollbar(); };

};

//____ Class: WgVScrollbar _______________________________________________________

class WgVScrollbar;
typedef	WgStrongPtr<WgVScrollbar,WgScrollbarPtr>		WgVScrollbarPtr;
typedef	WgWeakPtr<WgVScrollbar,WgScrollbarWeakPtr>		WgVScrollbarWeakPtr;

class WgVScrollbar:public WgScrollbar
{
	friend class WgMenu;
public:
	static WgVScrollbarPtr	Create() { return WgVScrollbarPtr(new WgVScrollbar()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgVScrollbarPtr	Cast( const WgObjectPtr& pObject );

protected:
	WgVScrollbar();
	virtual ~WgVScrollbar() {}
	virtual WgWidget* _newOfMyType() const { return new WgVScrollbar(); };

};

#endif //	WG_SCROLLBAR_DOT_H
