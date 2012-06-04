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

#ifndef WG_SLIDERTARGET_DOT_H
#	include <wg_slidertarget.h>
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

protected:
	WgViewHook() : m_pView(0) {};				// So we can make them members and then make placement new...
	~WgViewHook();
	void			_setParent( WgGizmoView * pParent ) { m_pView = pParent; }

	void			_requestRender();
	void			_requestRender( const WgRect& rect );
	void			_requestResize();

	WgHook *		_prevHook() const;
	WgHook *		_nextHook() const;
	WgGizmoParent *	_parent() const;

	WgGizmoView *	m_pView;
	WgRect			m_windowGeo;		// Geometry of Gizmos window inside parent.
	WgRect			m_canvasGeo;		// Geometry of Gizmos canvas. Same as m_windowGeo for sliders.
};



//____ WgGizmoView ________________________________________________________

class WgGizmoView : public WgGizmoContainer
{
	friend class WgViewHook;
public:
	WgGizmoView();
	virtual ~WgGizmoView();
	virtual const char * Type() const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoView(); };


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

	void		SetBgColor( WgColor color );
	WgColor		BgColor() const { return m_bgColor; }

	void		SetContentOrientation( WgOrientation orientation );
	WgOrientation ContentOrientation() const { return m_contentOrientation; }

	void		SetContentSizePolicy( WgSizePolicy widthPolicy, WgSizePolicy heightPolicy );
	WgSizePolicy	ContentWidthPolicy() const { return m_widthPolicy; }
	WgSizePolicy	ContentHeightPolicy() const { return m_heightPolicy; }


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

	bool				SetHSlider( WgGizmoHSlider * pSlider );
	void				DeleteHSlider() {SetVSlider(0);}
	WgGizmoHSlider *	HSlider() const { return (WgGizmoHSlider*) m_elements[XDRAG].Gizmo(); }
	WgGizmoHSlider* 	ReleaseHSlider();

	bool				SetVSlider( WgGizmoVSlider * pSlider );
	void				DeleteVSlider() {SetVSlider(0);}
	WgGizmoVSlider *	VSlider() const { return (WgGizmoVSlider*) m_elements[YDRAG].Gizmo(); }
	WgGizmoVSlider* 	ReleaseVSlider();

	bool				SetContent( WgGizmo * pContent );
	void				DeleteContent() {SetContent(0); }
	WgGizmo*			Content() const { return m_elements[WINDOW].Gizmo(); }
	WgGizmo*			ReleaseContent();

	bool				DeleteChild( WgGizmo * pGizmo );
	WgGizmo *			ReleaseChild( WgGizmo * pGizmo );

	bool				DeleteAllChildren();
	bool				ReleaseAllChildren();

	void				SetSliderAutoHide( bool bHideVSlider, bool bHideHSlider );
	bool				GetHSliderAutoHide() const { return m_bAutoHideSliderX; }
	bool				GetVSliderAutoHide() const { return m_bAutoHideSliderY; }

	void				SetSliderPositions( bool bBottom, bool bRight );
	bool				IsSliderAtBottom() const { return m_bSliderBottom; }
	bool				IsSliderAtRight() const { return m_bSliderRight; }

	bool				IsVSliderVisible();
	bool				IsHSliderVisible();

	void				SetFillerBlocks( const WgBlockSetPtr& pBlocks );
	WgBlockSetPtr		FillerBlocks() const { return m_pFillerBlocks; }

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


	class SliderTarget : public WgSliderTarget
	{
	public:
		float		_stepFwd();
		float		_stepBwd();
		float		_jumpFwd();
		float		_jumpBwd();
		float		_wheelRolled( int distance );

		float		_setPosition( float fraction );

		WgGizmo*	_getGizmo();
		float		_getSliderPosition();
		float		_getSliderSize();

		void		_updateSlider( float pos, float size ) { WgSliderTarget::_updateSlider(pos,size); }

		bool		m_bHorizontal;
		WgGizmoView *	m_pParent;
	};


	static const int	MAX_ELEMENTS = 3;

	virtual void _onNewSize( const WgSize& size );

	//

	void		_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	void		_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer );
	void		_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );
	void		_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );

	bool		_onAlphaTest( const WgCoord& ofs );
	void		_onCloneContent( const WgGizmo * _pOrg );

	WgHook *	_firstHookWithGeo( WgRect& geo ) const;
	WgHook *	_nextHookWithGeo( WgRect& geo, WgHook * pHook ) const;

	WgHook *	_lastHookWithGeo( WgRect& geo ) const;
	WgHook *	_prevHookWithGeo( WgRect& geo, WgHook * pHook ) const;


	WgSize		_calcContentSize( WgSize mySize );
	void		_updateElementGeo( WgSize mySize );

	WgRect		_genContentCanvasGeo( const WgRect& window, WgSize contentSize, WgOrientation orientation, WgCoord viewOfs );

	bool		_wheelRollX(int distance);
	bool		_wheelRollY(int distance);


	WgSize		m_contentSize;

	int			m_stepSizeX;
	int			m_stepSizeY;
	float		m_jumpSizeX;
	float		m_jumpSizeY;

	WgCoord		m_viewPixOfs;

	bool		m_bAutoHideSliderX;		// Should X-slider autohide when not needed?
	bool		m_bAutoHideSliderY;		// Should Y-slider autohide when not needed?

	bool		m_bSliderBottom;		// True if X-slider should be at the bottom, otherwise at the top.
	bool		m_bSliderRight;			// True if Y-slider should be at the right, otherwise at the left.

	bool		m_bAutoScrollX;
	bool		m_bAutoScrollY;

//	ViewGizmoCollection	m_elementsCollection;	// WgGizmoCollection for the elements gizmos.
	WgViewHook		m_elements[MAX_ELEMENTS];	// Content, xDrag and yDrag gizmos in that order.

	WgColor			m_bgColor;
	WgSizePolicy	m_widthPolicy;
	WgSizePolicy	m_heightPolicy;
	WgOrientation	m_contentOrientation;		// Orientation when content is smaller than window

	SliderTarget	m_sliderTargets[2];			// Order: Vertical, horizontal

private:

	WgHook*	_firstHook() const;
	WgHook*	_lastHook() const;


	WgBlockSetPtr	m_pFillerBlocks;
	WgRect			m_geoFiller;
};

#endif //WG_GIZMO_VIEW_DOT_H

