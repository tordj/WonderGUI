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

#ifndef	WG_SCROLLPANEL_DOT_H
#define	WG_SCROLLPANEL_DOT_H


#ifndef WG_PANEL_DOT_H
#	include <wg_panel.h>
#endif

#ifndef	WG_SCROLLBAR_DOT_H
#	include <wg_scrollbar.h>
#endif

#ifndef WG_SCROLLBARTARGET_DOT_H
#	include <wg_scrollbartarget.h>
#endif

class WgScrollPanel;

//____ WgScrollHook _____________________________________________________________

class WgScrollHook : public WgPanelHook
{
	friend class WgScrollPanel;
public:
	const char *Type( void ) const;
	static const char * ClassType();

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;
	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgHook * 		Prev() const { return _prevHook(); }
	WgHook * 		Next() const { return _nextHook(); }


	bool			SetVisible( bool bVisible ) { return false; }		// This widget handles hide/show of children according to its own rules.
	WgScrollPanel * 	Parent() const { return m_pView; }

protected:
	WgScrollHook() : m_pView(0) {};				// So we can make them members and then make placement new...
	~WgScrollHook();
	void			_setParent( WgScrollPanel * pParent ) { m_pView = pParent; }

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgContainer *	_parent() const;

	WgScrollPanel *	m_pView;
	WgRect			m_windowGeo;		// Geometry of Widgets window inside parent.
	WgRect			m_canvasGeo;		// Geometry of Widgets canvas. Same as m_windowGeo for scrollbars.
};



//____ WgScrollPanel ________________________________________________________

class WgScrollPanel : public WgPanel
{
	friend class WgScrollHook;
public:
	WgScrollPanel();
	virtual ~WgScrollPanel();
	virtual const char * Type() const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgScrollPanel(); };


	//____ Methods _______________________________________________________________

	bool		StepUp();
	bool		StepDown();
	bool		StepLeft();
	bool		StepRight();

	bool		JumpUp();
	bool		JumpDown();
	bool		JumpLeft();
	bool		JumpRight();

	void		SetStepSize( int pixX, int pixY ) { m_stepSizeX = pixX; m_stepSizeY = pixY; };
	void		SetStepSizeX( int pixels ) { m_stepSizeX = pixels; };
	void		SetStepSizeY( int pixels ) { m_stepSizeY = pixels; };

	int 		StepSizeX() { return m_stepSizeX; };
	int 		StepSizeY() { return m_stepSizeY; };

	void		SetJumpSize( float viewFractionX, float viewFractionY ) { SetJumpSizeX(viewFractionX); SetJumpSizeY(viewFractionY); };
	void		SetJumpSizeX( float viewFraction );
	void		SetJumpSizeY( float viewFraction );
	float		JumpSizeX( ) const { return m_jumpSizeX; }
	float		JumpSizeY( ) const { return m_jumpSizeY; }

	int			ContentWidth() { return m_contentSize.w; };
	int			ContentHeight() { return m_contentSize.h; };
	WgSize		ContentSize() { return m_contentSize; };

	void		SetContentOrigo( WgOrigo origo );
	WgOrigo ContentOrigo() const { return m_contentOrigo; }

	void		SetContentSizePolicy( WgSizePolicy widthPolicy, WgSizePolicy heightPolicy );
	WgSizePolicy	ContentWidthPolicy() const { return m_widthPolicy; }
	WgSizePolicy	ContentHeightPolicy() const { return m_heightPolicy; }


	int			ViewPixelOfsX() { return m_viewPixOfs.x; };
	int			ViewPixelOfsY() { return m_viewPixOfs.y; };
	WgCoord		ViewPixelOfs() { return m_viewPixOfs; };

	int			ViewPixelLenX();
	int			ViewPixelLenY();

	float		ViewOfsX();
	float		ViewOfsY();
	float		ViewLenX();
	float		ViewLenY();

	bool		SetViewPixelOfs( int x, int y );
	bool		SetViewPixelOfsX( int x );
	bool		SetViewPixelOfsY( int y );

	bool		SetViewOfs( float x, float y );
	bool		SetViewOfsX( float x );
	bool		SetViewOfsY( float y );

/*
	bool	ViewCenterOnPixel( int x, int y );
	bool	ViewCenterOnPixelX( int x );
	bool	ViewCenterOnPixelY( int y );

	bool	ViewInclude( WgRect& rect );
	bool	ViewIncludeX( WgRect& rect );
	bool	ViewIncludeY( WgRect& rect );
*/

	bool		SetScrollWheels( int wheelForX, int wheelForY );
	int			ScrollWheelX() const { return m_wheelForScrollX; }
	int			ScrollWheelY() const { return m_wheelForScrollY; }



	bool		SetAutoscroll( bool bAutoX, bool bAutoY	);
	bool		AutoScrollX() const { return m_bAutoScrollX; }
	bool		AutoScrollY() const { return m_bAutoScrollY; }

	WgPanelHook*SetHScrollbar( WgHScrollbar * pScrollbar );
	void		DeleteHScrollbar() {SetVScrollbar(0);}
	WgHScrollbar *	HScrollbar() const { return (WgHScrollbar*) m_elements[XDRAG].Widget(); }
	WgHScrollbar* 	ReleaseHScrollbar();

	WgPanelHook*SetVScrollbar( WgVScrollbar * pScrollbar );
	void		DeleteVScrollbar() {SetVScrollbar(0);}
	WgVScrollbar *	VScrollbar() const { return (WgVScrollbar*) m_elements[YDRAG].Widget(); }
	WgVScrollbar* 	ReleaseVScrollbar();

	WgPanelHook*SetContent( WgWidget * pContent );
	void		DeleteContent() {SetContent(0); }
	WgWidget*	Content() const { return m_elements[WINDOW].Widget(); }
	WgWidget*	ReleaseContent();

	bool		DeleteChild( WgWidget * pWidget );
	WgWidget *	ReleaseChild( WgWidget * pWidget );

	bool		DeleteAllChildren();
	bool		ReleaseAllChildren();

	void		SetScrollbarAutoHide( bool bHideVScrollbar, bool bHideHScrollbar );
	bool		GetHScrollbarAutoHide() const { return m_bAutoHideScrollbarX; }
	bool		GetVScrollbarAutoHide() const { return m_bAutoHideScrollbarY; }

	void		SetHandlePositions( bool bBottom, bool bRight );
	bool		IsScrollbarAtBottom() const { return m_bScrollbarBottom; }
	bool		IsScrollbarAtRight() const { return m_bScrollbarRight; }

	bool		IsVScrollbarVisible();
	bool		IsHScrollbarVisible();

	void		SetOverlayScrollbars( bool bOverlay );
	bool		OverlayScrollbars() const { return m_bOverlayScrollbars; }

	void				SetFillerSkin( const WgSkinPtr& pSkin );
	WgSkinPtr			FillerSkin() const { return m_pFillerSkin; }

	virtual void SetSkin( const WgSkinPtr& pSkin );

	// Overloaded from WgPanel

	WgWidget * FindWidget( const WgCoord& pos, WgSearchMode mode );

	// Overloaded from Widget

	WgSize				PreferredSize() const;				// = preferred size of dragbars in the geometry, fixed value if dragbars are missing.



/*
	NEED TO BE IMPLEMENTED!!!

	int		HeightForWidth( int width ) const;	//
	int		WidthForHeight( int height ) const;

*/

protected:

	enum ElementType
	{
		WINDOW = 0,
		XDRAG,
		YDRAG
	};


	class ScrollbarTarget : public WgScrollbarTarget
	{
	public:
		float		_stepFwd();
		float		_stepBwd();
		float		_jumpFwd();
		float		_jumpBwd();
		float		_wheelRolled( int distance );

		float		_setPosition( float fraction );

		WgWidget*	_getWidget();
		float		_getHandlePosition();
		float		_getHandleSize();

		void		_updateScrollbar( float pos, float size ) { WgScrollbarTarget::_updateScrollbar(pos,size); }

		bool		m_bHorizontal;
		WgScrollPanel *	m_pParent;
	};


	static const int	MAX_ELEMENTS = 3;

	virtual void _onNewSize( const WgSize& size );

	//

	void		_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	void		_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	void		_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void		_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );

	bool		_onAlphaTest( const WgCoord& ofs );
	void		_onCloneContent( const WgWidget * _pOrg );

	WgHook *	_firstHookWithGeo( WgRect& geo ) const;
	WgHook *	_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& geo ) const;
	WgHook *	_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;


	WgSize		_calcContentSize( WgSize mySize );
	void		_updateElementGeo( WgSize mySize );

	WgRect		_genContentCanvasGeo( const WgRect& window, WgSize contentSize, WgOrigo origo, WgCoord viewOfs );

	bool		_wheelRollX(int distance);
	bool		_wheelRollY(int distance);


	WgSize		m_contentSize;

	int			m_stepSizeX;
	int			m_stepSizeY;
	float		m_jumpSizeX;
	float		m_jumpSizeY;

	WgCoord		m_viewPixOfs;

	bool		m_bAutoHideScrollbarX;		// Should X-scrollbar autohide when not needed?
	bool		m_bAutoHideScrollbarY;		// Should Y-scrollbar autohide when not needed?

	bool		m_bScrollbarBottom;		// True if X-scrollbar should be at the bottom, otherwise at the top.
	bool		m_bScrollbarRight;			// True if Y-scrollbar should be at the right, otherwise at the left.

	bool		m_bAutoScrollX;
	bool		m_bAutoScrollY;

	int			m_wheelForScrollX;		// What wheel should be used for horizontal scrolling. (0=none)
	int			m_wheelForScrollY;		// What wheel should be used for vertical scrolling. (0=none)

	bool		m_bOverlayScrollbars;
	

//	ViewWidgetCollection	m_elementsCollection;	// WgWidgetCollection for the elements widgets.
	WgScrollHook		m_elements[MAX_ELEMENTS];	// Content, xDrag and yDrag widgets in that order.

	WgSizePolicy	m_widthPolicy;
	WgSizePolicy	m_heightPolicy;
	WgOrigo	m_contentOrigo;		// Origo when content is smaller than window

	ScrollbarTarget	m_scrollbarTargets[2];			// Order: Vertical, horizontal

private:

	WgHook*	_firstHook() const;
	WgHook*	_lastHook() const;


	WgSkinPtr		m_pFillerSkin;
	WgRect			m_fillerGeo;
};

#endif //WG_SCROLLPANEL_DOT_H

