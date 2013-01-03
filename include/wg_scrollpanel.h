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

#ifndef	WG_WIDGET_SLIDER_DOT_H
#	include <wg_slider.h>
#endif

#ifndef WG_SLIDERTARGET_DOT_H
#	include <wg_slidertarget.h>
#endif

class WgScrollPanel;

//____ WgScrollHook _____________________________________________________________

class WgScrollHook : public WgHook
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
	WgWidgetHolder *	_parent() const;

	WgScrollPanel *	m_pView;
	WgRect			m_windowGeo;		// Geometry of Widgets window inside parent.
	WgRect			m_canvasGeo;		// Geometry of Widgets canvas. Same as m_windowGeo for sliders.
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

	bool		SetScrollWheels( int wheelForX, int wheelForY );
	int			ScrollWheelX() const { return m_wheelForScrollX; }
	int			ScrollWheelY() const { return m_wheelForScrollY; }



	bool		SetAutoscroll( bool bAutoX, bool bAutoY	);
	bool		AutoScrollX() const { return m_bAutoScrollX; }
	bool		AutoScrollY() const { return m_bAutoScrollY; }

	bool				SetHSlider( WgHSlider * pSlider );
	void				DeleteHSlider() {SetVSlider(0);}
	WgHSlider *	HSlider() const { return (WgHSlider*) m_elements[XDRAG].Widget(); }
	WgHSlider* 	ReleaseHSlider();

	bool				SetVSlider( WgVSlider * pSlider );
	void				DeleteVSlider() {SetVSlider(0);}
	WgVSlider *	VSlider() const { return (WgVSlider*) m_elements[YDRAG].Widget(); }
	WgVSlider* 	ReleaseVSlider();

	bool				SetContent( WgWidget * pContent );
	void				DeleteContent() {SetContent(0); }
	WgWidget*			Content() const { return m_elements[WINDOW].Widget(); }
	WgWidget*			ReleaseContent();

	bool				DeleteChild( WgWidget * pWidget );
	WgWidget *			ReleaseChild( WgWidget * pWidget );

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

	void				SetFillerBlocks( const WgBlocksetPtr& pBlocks );
	WgBlocksetPtr		FillerBlocks() const { return m_pFillerBlocks; }

	// Overloaded from WgPanel

	WgWidget * FindWidget( const WgCoord& pos, WgSearchMode mode );

	// Overloaded from Widget

	WgSize				DefaultSize() const;				// = preferred size of dragbars in the geometry, fixed value if dragbars are missing.



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


	class SliderTarget : public WgSliderTarget
	{
	public:
		float		_stepFwd();
		float		_stepBwd();
		float		_jumpFwd();
		float		_jumpBwd();
		float		_wheelRolled( int distance );

		float		_setPosition( float fraction );

		WgWidget*	_getWidget();
		float		_getSliderPosition();
		float		_getSliderSize();

		void		_updateSlider( float pos, float size ) { WgSliderTarget::_updateSlider(pos,size); }

		bool		m_bHorizontal;
		WgScrollPanel *	m_pParent;
	};


	static const int	MAX_ELEMENTS = 3;

	virtual void _onNewSize( const WgSize& size );

	//

	void		_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	void		_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer );
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

	int			m_wheelForScrollX;		// What wheel should be used for horizontal scrolling. (0=none)
	int			m_wheelForScrollY;		// What wheel should be used for vertical scrolling. (0=none)
	

//	ViewWidgetCollection	m_elementsCollection;	// WgWidgetCollection for the elements widgets.
	WgScrollHook		m_elements[MAX_ELEMENTS];	// Content, xDrag and yDrag widgets in that order.

	WgColor			m_bgColor;
	WgSizePolicy	m_widthPolicy;
	WgSizePolicy	m_heightPolicy;
	WgOrientation	m_contentOrientation;		// Orientation when content is smaller than window

	SliderTarget	m_sliderTargets[2];			// Order: Vertical, horizontal

private:

	WgHook*	_firstHook() const;
	WgHook*	_lastHook() const;


	WgBlocksetPtr	m_pFillerBlocks;
	WgRect			m_geoFiller;
};

#endif //WG_SCROLLPANEL_DOT_H

