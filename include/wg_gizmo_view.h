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

#ifndef	WG_GIZMO_SLIDER_DOT_H
#	include <wg_gizmo_slider.h>
#endif

class WgGizmoView;

//____ WgViewHook _____________________________________________________________

class WgViewHook : public WgHook
{
	friend class WgGizmoView;
public:

	WgCoord			Pos() const;
	WgSize			Size() const;
	WgRect			Geo() const;
	WgCoord			ScreenPos() const;
	WgRect			ScreenGeo() const;

	WgHook * 		Prev() const { return _prevHook(); }
	WgHook * 		Next() const { return _nextHook(); }


	bool			SetHidden( bool bHide ) { return false; }		// This gizmo handles hide/show of children according to its own rules.
	WgGizmoView * 	Parent() const { return m_pView; }

	WgWidget*	GetRoot();			// Should in the future not return a widget, but a gizmo.


protected:
	WgViewHook() : m_pView(0), m_bShow(false) {};				// So we can make them members and then make placement new...
	~WgViewHook();
	void			_setParent( WgGizmoView * pParent ) { m_pView = pParent; }

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgGizmoParent *	_parent() const;


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
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoView(); };


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

	bool		StepUp();
	bool		StepDown();
	bool		StepLeft();
	bool		StepRight();

	bool		JumpUp();
	bool		JumpDown();
	bool		JumpLeft();
	bool		JumpRight();

	bool		WheelRollX(int distance);
	bool		WheelRollY(int distance);

	void		SetStepSize( int pixX, int pixY ) { m_stepSizeX = pixX; m_stepSizeY = pixY; };
	void		SetStepSizeX( int pixels ) { m_stepSizeX = pixels; };
	void		SetStepSizeY( int pixels ) { m_stepSizeY = pixels; };

	Uint32 		StepSizeX() { return m_stepSizeX; };
	Uint32 		StepSizeY() { return m_stepSizeY; };

	void		SetJumpSize( float viewFractionX, float viewFractionY ) { SetJumpSizeX(viewFractionX); SetJumpSizeY(viewFractionY); };
	void		SetJumpSizeX( float viewFraction );
	void		SetJumpSizeY( float viewFraction );
	float		JumpSizeX( ) const { return m_jumpSizeX; }
	float		JumpSizeY( ) const { return m_jumpSizeY; }

	int			ContentWidth() { return m_contentSize.w; };
	int			ContentHeight() { return m_contentSize.h; };
	WgSize		ContentSize() { return m_contentSize; };

	Uint32		ViewPixelOfsX() { return m_viewPixOfs.x; };
	Uint32		ViewPixelOfsY() { return m_viewPixOfs.y; };
	WgCoord		ViewPixelOfs() { return m_viewPixOfs; };

	Uint32		ViewPixelLenX();
	Uint32		ViewPixelLenY();

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

	bool		SetAutoscroll( bool bAutoX, bool bAutoY	);
	bool		AutoScrollX() const { return m_bAutoScrollX; }
	bool		AutoScrollY() const { return m_bAutoScrollY; }

	bool				SetSliderX( WgGizmoHSlider * pSlider );
	void				DeleteSliderX() {SetSliderX(0);}
	WgGizmoHSlider *	GetSliderX() const { return (WgGizmoHSlider*) m_elements[XDRAG].Gizmo(); }
	WgGizmoHSlider* 	ReleaseSliderX();

	bool				SetSliderY( WgGizmoVSlider * pSlider );
	void				DeleteSliderY() {SetSliderY(0);}
	WgGizmoVSlider *	GetSliderY() const { return (WgGizmoVSlider*) m_elements[YDRAG].Gizmo(); }
	WgGizmoVSlider* 	ReleaseSliderY();

	bool				SetContent( WgGizmo * pContent );
	void				DeleteContent() {SetContent(0); }
	WgGizmo*			GetContent() const { return m_elements[WINDOW].Gizmo(); }
	WgGizmo*			ReleaseContent();

	bool				DeleteChild( WgGizmo * pGizmo );
	WgGizmo *			ReleaseChild( WgGizmo * pGizmo );

	bool				DeleteAllChildren();
	bool				ReleaseAllChildren();

	void				SetSliderAutoHide( bool bHideX, bool bHideY );
	bool				GetSliderAutoHideX() const { return m_bAutoHideSliderX; }
	bool				GetSliderAutoHideY() const { return m_bAutoHideSliderY; }

	void				SetSliderPositions( bool bBottom, bool bRight );
	bool				GetSliderBottom() const { return m_bSliderBottom; }
	bool				GetSliderRight() const { return m_bSliderRight; }

	bool				SliderXVisible();
	bool				SliderYVisible();

	void				SetFillerSource( const WgBlockSetPtr& pBlocks );



	WgGizmoContainer * CastToContainer() { return this; }
	const WgGizmoContainer * CastToContainer() const { return this; }

	WgGizmo* 		CastToGizmo() { return this; }


	// Overloaded from container

	WgGizmo * FindGizmo( const WgCoord& pos, WgSearchMode mode );

	// Overloaded from Gizmo

	WgSize				DefaultSize() const;				// = preferred size of dragbars in the geometry, fixed value if dragbars are missing.


/*
	NEED TO BE IMPLEMENTED!!!

	int		HeightForWidth( int width ) const;	//
	int		WidthForHeight( int height ) const;

	bool	IsView() const { return true; }
*/

protected:

	enum ElementType
	{
		WINDOW = 0,
		XDRAG,
		YDRAG
	};

	static const int	MAX_ELEMENTS = 3;

	WgGizmoView();
	virtual void _onNewSize( const WgSize& size );


	// These are needed until WgGizmoContainer inherits from WgGizmo

	void			_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
									{ WgGizmoContainer::_renderPatches( pDevice, _canvas, _window, _pPatches, _layer ); }
	void			_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
									{ WgGizmoContainer::_onCollectPatches(container, geo, clip); }
	void			_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
									{ WgGizmoContainer::_onMaskPatches(patches, geo, clip, blendMode); }
	void			_onEnable() { WgGizmoContainer::_onEnable(); }
	void			_onDisable() { WgGizmoContainer::_onDisable(); }

	//

	// Following method should be overridden by subclasses instead of _onNewSize()!
	// Takes into account that sliders might decrease the visible area of the subclass.

	void		_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	bool		_onAlphaTest( const WgCoord& ofs );
	void		_onCloneContent( const WgGizmo * _pOrg );

	WgHook *	_firstHookWithGeo( WgRect& geo ) const;
	WgHook *	_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& geo ) const;
	WgHook *	_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;


	void		_setContentSize( const WgSize& size );
	void		_updateElementGeo( const WgSize& mySize, const WgSize& newContentSize );

	WgSize		m_contentSize;

	int			m_stepSizeX;
	int			m_stepSizeY;
	float		m_jumpSizeX;
	float		m_jumpSizeY;

	WgCoord		m_viewPixOfs;

//	WgGizmoHSlider *	m_pScrollbarX;
//	WgGizmoVSlider *	m_pScrollbarY;

	bool		m_bAutoHideSliderX;		// Should X-slider autohide when not needed?
	bool		m_bAutoHideSliderY;		// Should Y-slider autohide when not needed?

	bool		m_bSliderBottom;		// True if X-slider should be at the bottom, otherwise at the top.
	bool		m_bSliderRight;			// True if Y-slider should be at the right, otherwise at the left.

	bool		m_bAutoScrollX;
	bool		m_bAutoScrollY;

//	ViewGizmoCollection	m_elementsCollection;	// WgGizmoCollection for the elements gizmos.
	WgViewHook		m_elements[MAX_ELEMENTS];	// Content, xDrag and yDrag gizmos in that order.

private:

	WgHook*	_firstHook() const;
	WgHook*	_lastHook() const;


	WgBlockSetPtr	m_pFillerBlocks;
	WgRect			m_geoFiller;
};

#endif //WG_GIZMO_VIEW_DOT_H

