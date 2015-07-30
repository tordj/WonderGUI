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

class WgScrollPanel;
typedef	WgStrongPtr<WgScrollPanel,WgPanel_p>		WgScrollPanel_p;
typedef	WgWeakPtr<WgScrollPanel,WgPanel_wp>	WgScrollPanel_wp;

class WgScrollHook;
typedef	WgHookTypePtr<WgScrollHook,WgPanelHook_p>	WgScrollHook_p;

//____ WgScrollHook _____________________________________________________________

class WgScrollHook : public WgPanelHook
{
	friend class WgScrollPanel;
public:
	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgScrollHook_p	cast( const WgHook_p& pInterface );

	WgCoord			pos() const;
	WgSize			size() const;
	WgRect			geo() const;
	WgCoord			globalPos() const;
	WgRect			globalGeo() const;

	WgScrollHook_p 	prev() const { return static_cast<WgScrollHook*>(_prevHook()); }
	WgScrollHook_p 	next() const { return static_cast<WgScrollHook*>(_nextHook()); }

	bool			setVisible( bool bVisible ) { return false; }		// This widget handles hide/show of children according to its own rules.
	WgScrollPanel_p 	parent() const { return m_pView; }

protected:
	WgScrollHook() : m_pView(0) {};				// So we can make them members and then make placement new...
	~WgScrollHook();
	void			_setParent( WgScrollPanel * pParent ) { m_pView = pParent; }

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgRect			_windowSection() const;

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
	static WgScrollPanel_p	create() { return WgScrollPanel_p(new WgScrollPanel()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgScrollPanel_p	cast( const WgObject_p& pObject );

	bool		stepUp();
	bool		stepDown();
	bool		stepLeft();
	bool		stepRight();

	bool		jumpUp();
	bool		jumpDown();
	bool		jumpLeft();
	bool		jumpRight();

	void		setStepSize( int pixX, int pixY ) { m_stepSizeX = pixX; m_stepSizeY = pixY; };
	void		setStepSizeX( int pixels ) { m_stepSizeX = pixels; };
	void		setStepSizeY( int pixels ) { m_stepSizeY = pixels; };

	int 		stepSizeX() { return m_stepSizeX; };
	int 		stepSizeY() { return m_stepSizeY; };

	void		setJumpSize( float viewFractionX, float viewFractionY ) { setJumpSizeX(viewFractionX); setJumpSizeY(viewFractionY); };
	void		setJumpSizeX( float viewFraction );
	void		setJumpSizeY( float viewFraction );
	float		jumpSizeX( ) const { return m_jumpSizeX; }
	float		jumpSizeY( ) const { return m_jumpSizeY; }

	int			contentWidth() { return m_contentSize.w; };
	int			contentHeight() { return m_contentSize.h; };
	WgSize		contentSize() { return m_contentSize; };

	void		setContentOrigo( WgOrigo origo );
	WgOrigo contentOrigo() const { return m_contentOrigo; }

	void		setContentSizePolicy( WgSizePolicy widthPolicy, WgSizePolicy heightPolicy );
	WgSizePolicy	contentWidthPolicy() const { return m_widthPolicy; }
	WgSizePolicy	contentHeightPolicy() const { return m_heightPolicy; }


	int			viewPixelOfsX() { return m_viewPixOfs.x; };
	int			viewPixelOfsY() { return m_viewPixOfs.y; };
	WgCoord		viewPixelOfs() { return m_viewPixOfs; };

	int			viewPixelLenX();
	int			viewPixelLenY();

	float		viewOfsX();
	float		viewOfsY();
	float		viewLenX();
	float		viewLenY();

	bool		setViewPixelOfs( int x, int y );
	bool		setViewPixelOfsX( int x );
	bool		setViewPixelOfsY( int y );

	bool		setViewOfs( float x, float y );
	bool		setViewOfsX( float x );
	bool		setViewOfsY( float y );

/*
	bool	viewCenterOnPixel( int x, int y );
	bool	viewCenterOnPixelX( int x );
	bool	viewCenterOnPixelY( int y );

	bool	viewInclude( WgRect& rect );
	bool	viewIncludeX( WgRect& rect );
	bool	viewIncludeY( WgRect& rect );
*/

	bool		setScrollWheels( int wheelForX, int wheelForY );
	int			scrollWheelX() const { return m_wheelForScrollX; }
	int			scrollWheelY() const { return m_wheelForScrollY; }



	bool		setAutoscroll( bool bAutoX, bool bAutoY	);
	bool		autoScrollX() const { return m_bAutoScrollX; }
	bool		autoScrollY() const { return m_bAutoScrollY; }

	WgScrollHook_p setHorizontalScrollbar( const WgScrollbar_p& pScrollbar );
	void			removeHorizontalScrollbar() {setHorizontalScrollbar(0);}
	WgScrollbar_p	horizontalScrollbar() const;

	WgScrollHook_p	setVerticalScrollbar( const WgScrollbar_p& pScrollbar );
	void			removeVerticalScrollbar() {setVerticalScrollbar(0);}
	WgScrollbar_p	verticalScrollbar() const;

	WgScrollHook_p	setContent( const WgWidget_p& pContent );
	void			removeContent() {setContent(0); }
	WgWidget_p		content() const;

	bool		removeWidget( const WgWidget_p& pWidget );
	bool		clear();

	void		setScrollbarAutoHide( bool bHideVerticalScrollbar, bool bHideHorizontalScrollbar );
	bool		getHScrollbarAutoHide() const { return m_bAutoHideScrollbarX; }
	bool		getVScrollbarAutoHide() const { return m_bAutoHideScrollbarY; }

	void		setHandlePositions( bool bBottom, bool bRight );
	bool		isScrollbarAtBottom() const { return m_bScrollbarBottom; }
	bool		isScrollbarAtRight() const { return m_bScrollbarRight; }

	bool		isVerticalScrollbarVisible();
	bool		isHorizontalScrollbarVisible();

	void		setOverlayScrollbars( bool bOverlay );
	bool		overlayScrollbars() const { return m_bOverlayScrollbars; }

	void		setCornerSkin( const WgSkin_p& pSkin );
	WgSkin_p	cornerSkin() const { return m_pCornerSkin; }

	virtual void setSkin( const WgSkin_p& pSkin );

	// Overloaded from Widget

	WgSize				preferredSize() const;				// = preferred size of dragbars in the geometry, fixed value if dragbars are missing.



/*
	NEED TO BE IMPLEMENTED!!!

	int		matchingHeight( int width ) const;	//
	int		matchingWidth( int height ) const;

*/

protected:
	WgScrollPanel();
	virtual ~WgScrollPanel();
	virtual WgWidget* _newOfMyType() const { return new WgScrollPanel(); };

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

	// Overloaded from WgPanel

	WgWidget *	_findWidget( const WgCoord& pos, WgSearchMode mode );

	//

	void		_onMsg( const WgMsg_p& pMsg );
	void		_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	void		_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void		_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );
	void		_onRequestResize( WgScrollHook * pHook );

	bool		_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
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

	int			_paddedViewPixelLenX();				// Width of view after childs window padding has been applied.
	int			_paddedViewPixelLenY();				// Height of view after childs window padding has been applied.
	float		_paddedViewLenX();
	float		_paddedViewLenY();



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
	
	WgScrollHook		m_elements[MAX_ELEMENTS];	// Content, xDrag and yDrag widgets in that order.

	WgSizePolicy	m_widthPolicy;
	WgSizePolicy	m_heightPolicy;
	WgOrigo	m_contentOrigo;		// Origo when content is smaller than window

	ScrollbarTarget	m_scrollbarTargets[2];			// Order: Vertical, horizontal

private:

	WgHook*	_firstHook() const;
	WgHook*	_lastHook() const;


	WgSkin_p		m_pCornerSkin;
	WgRect			m_cornerGeo;
};

#endif //WG_SCROLLPANEL_DOT_H

