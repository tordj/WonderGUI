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

#ifndef	WG_GIZMO_VIEW_DOT_H
#define	WG_GIZMO_VIEW_DOT_H


#ifndef WG_GIZMO_CONTAINER_DOT_H
#	include <wg_gizmo_container.h>
#endif

#ifndef	WG_GIZMO_DRAGBARS_DOT_H
#	include <wg_gizmo_dragbars.h>
#endif

class WgGizmoView;

//____ WgViewHook _____________________________________________________________

class WgViewHook : public WgGizmoHook
{
	friend class WgGizmoView;
public:

	WgCord	Pos() const;
	WgSize	Size() const;
	WgRect	Geo() const;
	WgCord	ScreenPos() const;
	WgRect	ScreenGeo() const;

	inline WgGizmoHook * PrevHook() const { return _prevHook(); }
	inline WgGizmoHook * NextHook() const { return _nextHook(); }

	WgGizmoContainer * Parent() const;

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.

	void	RequestRender();
	void	RequestRender( const WgRect& rect );
	void	RequestResize();

protected:
	WgViewHook() : WgGizmoHook( 0 ) {};				// So we can make them members and then make placement new...
	WgViewHook( WgGizmoHDragbar * pHDragbar, WgGizmoView * pView );
	WgViewHook( WgGizmoVDragbar * pHDragbar, WgGizmoView * pView );
	WgViewHook( WgGizmo * pContent, WgGizmoView * pView );
	~WgViewHook();

	WgGizmoHook *	_prevHook() const;
	WgGizmoHook *	_nextHook() const;


//	ElementType		m_type;
	WgGizmoView *	m_pView;
	WgRect			m_geo;
	bool			m_bShow;
};



//____ WgGizmoView ________________________________________________________

class WgGizmoView : public WgGizmo, public WgGizmoContainer
{
	friend class WgViewHook;
public:
	virtual ~WgGizmoView();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Callbacks _____________________________________________________________

	static void cbStepUp	(void * pGizmo) { ((WgGizmoView*)pGizmo)->StepUp(); }
	static void cbStepDown	(void * pGizmo) { ((WgGizmoView*)pGizmo)->StepDown(); }
	static void cbStepLeft	(void * pGizmo) { ((WgGizmoView*)pGizmo)->StepLeft(); }
	static void cbStepRight	(void * pGizmo) { ((WgGizmoView*)pGizmo)->StepRight(); }

	static void cbJumpUp	(void * pGizmo) { ((WgGizmoView*)pGizmo)->JumpUp(); }
	static void cbJumpDown	(void * pGizmo) { ((WgGizmoView*)pGizmo)->JumpDown(); }
	static void cbJumpLeft	(void * pGizmo) { ((WgGizmoView*)pGizmo)->JumpLeft(); }
	static void cbJumpRight	(void * pGizmo) { ((WgGizmoView*)pGizmo)->JumpRight(); }

	static void cbSetStepSize	(void * pGizmo, int x, int y) { ((WgGizmoView*)pGizmo)->SetStepSize(x,y); }
	static void cbSetStepSizeX	(void * pGizmo, int x) { ((WgGizmoView*)pGizmo)->SetStepSizeX(x); }
	static void cbSetStepSizeY	(void * pGizmo, int y) { ((WgGizmoView*)pGizmo)->SetStepSizeY(y); }

	static void cbSetJumpSize	(void * pGizmo, float x, float y) { ((WgGizmoView*)pGizmo)->SetJumpSize(x,y); }
	static void cbSetJumpSizeX	(void * pGizmo, float x) { ((WgGizmoView*)pGizmo)->SetJumpSizeX(x); }
	static void cbSetJumpSizeY	(void * pGizmo, float y) { ((WgGizmoView*)pGizmo)->SetJumpSizeY(y); }

	static void cbSetViewPixelOfs	(void * pGizmo, int x, int y) { ((WgGizmoView*)pGizmo)->SetViewPixelOfs(x,y); }
	static void cbSetViewPixelOfsX	(void * pGizmo, int x) { ((WgGizmoView*)pGizmo)->SetViewPixelOfsX(x); }
	static void cbSetViewPixelOfsY	(void * pGizmo, int y) { ((WgGizmoView*)pGizmo)->SetViewPixelOfsY(y); }

	static void cbSetViewOfs	(void * pGizmo, float x, float y) { ((WgGizmoView*)pGizmo)->SetViewOfs(x,y); }
	static void cbSetViewOfsX	(void * pGizmo, float x) { ((WgGizmoView*)pGizmo)->SetViewOfsX(x); }
	static void cbSetViewOfsY	(void * pGizmo, float y) { ((WgGizmoView*)pGizmo)->SetViewOfsY(y); }

	static void cbWheelRollX(void * pGizmo, int distance) { ((WgGizmoView*)pGizmo)->WheelRollX(distance); }
	static void cbWheelRollY(void * pGizmo, int distance) { ((WgGizmoView*)pGizmo)->WheelRollY(distance); }


	//____ Methods _______________________________________________________________

	bool	StepUp();
	bool	StepDown();
	bool	StepLeft();
	bool	StepRight();

	bool	JumpUp();
	bool	JumpDown();
	bool	JumpLeft();
	bool	JumpRight();

	bool	WheelRollX(int distance);
	bool	WheelRollY(int distance);

	inline void	SetStepSize( int pixX, int pixY ){ m_stepSizeX = pixX; m_stepSizeY = pixY; };
	inline void	SetStepSizeX( int pixels ){ m_stepSizeX = pixels; };
	inline void	SetStepSizeY( int pixels ){ m_stepSizeY = pixels; };

	inline Uint32 StepSizeX() { return m_stepSizeX; };
	inline Uint32 StepSizeY() { return m_stepSizeY; };

	inline void	SetJumpSize( float viewFractionX, float viewFractionY ){ SetJumpSizeX(viewFractionX); SetJumpSizeY(viewFractionY); };
	void	SetJumpSizeX( float viewFraction );
	void	SetJumpSizeY( float viewFraction );
	float	JumpSizeX( ) const { return m_jumpSizeX; }
	float	JumpSizeY( ) const { return m_jumpSizeY; }

	inline int	ContentWidth() { return m_contentSize.w; };
	inline int	ContentHeight() { return m_contentSize.h; };
	inline WgSize	ContentSize() { return m_contentSize; };

	inline Uint32	ViewPixelOfsX() { return m_viewPixOfs.x; };
	inline Uint32	ViewPixelOfsY() { return m_viewPixOfs.y; };
	inline WgCord		ViewPixelOfs() { return m_viewPixOfs; };

	Uint32	ViewPixelLenX();
	Uint32	ViewPixelLenY();

	float	ViewOfsX();
	float	ViewOfsY();
	float	ViewLenX();
	float	ViewLenY();

	bool	SetViewPixelOfs( int x, int y );
	bool	SetViewPixelOfsX( int x );
	bool	SetViewPixelOfsY( int y );

	bool	SetViewOfs( float x, float y );
	bool	SetViewOfsX( float x );
	bool	SetViewOfsY( float y );

/*
	bool	ViewCenterOnPixel( Sint32 x, Sint32 y );
	bool	ViewCenterOnPixelX( Sint32 x );
	bool	ViewCenterOnPixelY( Sint32 y );

	bool	ViewInclude( WgRect& rect );
	bool	ViewIncludeX( WgRect& rect );
	bool	ViewIncludeY( WgRect& rect );
*/

	bool	SetAutoscroll( bool bAutoX, bool bAutoY	);
	bool	AutoScrollX() const { return m_bAutoScrollX; }
	bool	AutoScrollY() const { return m_bAutoScrollY; }

	bool				SetScrollbarX( WgGizmoHDragbar * pScrollbar );
	inline void			DeleteScrollbarX() {SetScrollbarX(0);}
	WgGizmoHDragbar *	GetScrollbarX() const { return (WgGizmoHDragbar*) m_elements[XDRAG].Gizmo(); }
	WgGizmoHDragbar* 	ReleaseScrollbarX();

	bool				SetScrollbarY( WgGizmoVDragbar * pScrollbar );
	inline void			DeleteScrollbarY() {SetScrollbarY(0);}
	WgGizmoVDragbar *	GetScrollbarY() const { return (WgGizmoVDragbar*) m_elements[YDRAG].Gizmo(); }
	WgGizmoVDragbar* 	ReleaseScrollbarY();

	bool				SetContent( WgGizmo * pContent );
	inline void			DeleteContent() {SetContent(0); }
	WgGizmo*			GetContent() const { return m_elements[WINDOW].Gizmo(); }
	WgGizmo*			ReleaseContent();


	void	SetScrollbarAutoHide( bool bHideX, bool bHideY );
	bool	GetScrollbarAutoHideX() const { return m_bAutoHideScrollbarX; }
	bool	GetScrollbarAutoHideY() const { return m_bAutoHideScrollbarY; }

	void	SetScrollbarPositions( bool bBottom, bool bRight );
	bool	GetScrollbarBottom() const { return m_bScrollbarBottom; }
	bool	GetScrollbarRight() const { return m_bScrollbarRight; }

	bool	ScrollbarXVisible();
	bool	ScrollbarYVisible();

	void	SetFillerSource( const WgBlockSetPtr& pBlocks );



	WgGizmoContainer * CastToContainer() { return this; }
	const WgGizmoContainer * CastToContainer() const { return this; }

	// Overloaded from container

	WgGizmo * FindGizmo( const WgCord& pos, WgSearchMode mode );

/*
	NEED TO BE IMPLEMENTED!!!

	int		HeightForWidth( int width ) const;	//
	int		WidthForHeight( int height ) const;

	WgSize	MinSize() const;				// Defined by dragbars minsize...
	WgSize	BestSize() const;				// = size of content if dragbars autohide, otherwise content+dragbars
	WgSize	MaxSize() const;				// Unlimited

	bool	IsView() const { return true; }
*/

protected:

	enum ElementType
	{
		WINDOW = 0,
		XDRAG,
		YDRAG
	};


	WgGizmoView();
	virtual void OnNewSize( const WgSize& size );

	// Following method should be overridden by subclasses instead of OnNewSize()!
	// Takes into account that scrollbars might decrease the visible area of the subclass.

	void		OnNewViewSize( const WgSize& size ) {};
	void		OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	bool		OnAlphaTest( const WgCord& ofs );
	void		OnCloneContent( const WgGizmo * _pOrg );

	inline void		OnEnable() { WgGizmoContainer::OnEnable(); }		// Needed until WgGizmoContainer inerits from WgGizmo
	inline void		OnDisable() { WgGizmoContainer::OnDisable(); }		// Needed until WgGizmoContainer inerits from WgGizmo


	void		SetContentSize( const WgSize& size );
	void		UpdateElementGeometry( const WgSize& mySize, const WgSize& newContentSize );

	WgSize		m_contentSize;

	int			m_stepSizeX;
	int			m_stepSizeY;
	float		m_jumpSizeX;
	float		m_jumpSizeY;

	WgCord		m_viewPixOfs;

//	WgGizmoHDragbar *	m_pScrollbarX;
//	WgGizmoVDragbar *	m_pScrollbarY;

	bool		m_bAutoHideScrollbarX;		// Should X-scrollbar autohide when not needed?
	bool		m_bAutoHideScrollbarY;		// Should Y-scrollbar autohide when not needed?

	bool		m_bScrollbarBottom;		// True if X-scrollbar should be at the bottom, otherwise at the top.
	bool		m_bScrollbarRight;		// True if Y-scrollbar should be at the right, otherwise at the left.

	bool		m_bAutoScrollX;
	bool		m_bAutoScrollY;

//	ViewGizmoCollection	m_elementsCollection;	// WgGizmoCollection for the elements gizmos.
	WgViewHook		m_elements[3];			// Content, xDrag and yDrag gizmos in that order.

private:
	WgGizmo* 		_castToGizmo() { return this; }

	WgGizmoHook*	_firstHook() const { return const_cast<WgViewHook*>(&m_elements[0]); }
	WgGizmoHook*	_lastHook() const { return const_cast<WgViewHook*>(&m_elements[2]); }


	WgBlockSetPtr	m_pFillerBlocks;
	WgRect			m_geoFiller;
};

#endif //WG_GIZMO_VIEW_DOT_H

