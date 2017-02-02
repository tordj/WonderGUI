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

namespace wg 
{
	
	class ScrollPanel;
	
	class ScrollPanel;
	typedef	StrongPtr<ScrollPanel,Panel_p>		ScrollPanel_p;
	typedef	WeakPtr<ScrollPanel,Panel_wp>	ScrollPanel_wp;
	
	class ScrollHook;
	typedef	HookTypePtr<ScrollHook,PanelHook_p>	ScrollHook_p;
	
	//____ ScrollHook _____________________________________________________________
	
	class ScrollHook : public PanelHook
	{
		friend class ScrollPanel;
	public:
		virtual bool			isInstanceOf( const char * pClassName ) const;
		virtual const char *	className( void ) const;
		static const char		CLASSNAME[];
		static ScrollHook_p	cast( const Hook_p& pInterface );
	
		Coord			pos() const;
		Size			size() const;
		Rect			geo() const;
		Coord			globalPos() const;
		Rect			globalGeo() const;
	
		ScrollHook_p 	prev() const { return static_cast<ScrollHook*>(_prevHook()); }
		ScrollHook_p 	next() const { return static_cast<ScrollHook*>(_nextHook()); }
	
		bool			setVisible( bool bVisible ) { return false; }		// This widget handles hide/show of children according to its own rules.
		ScrollPanel_p 	parent() const { return m_pView; }
	
	protected:
		ScrollHook() : m_pView(0) {};				// So we can make them members and then make placement new...
		~ScrollHook();
		void			_setParent( ScrollPanel * pParent ) { m_pView = pParent; }
			
		Hook *		_prevHook() const;
		Hook *		_nextHook() const;
		Container *	_parent() const;
	
		ScrollPanel *	m_pView;
		Rect			m_windowGeo;		// Geometry of Widgets window inside parent.
		Rect			m_canvasGeo;		// Geometry of Widgets canvas. Same as m_windowGeo for scrollbars.
	};
	
	
	//____ ScrollPanel ________________________________________________________
	
	class ScrollPanel : public Panel
	{
		friend class ScrollHook;
	public:
		static ScrollPanel_p	create() { return ScrollPanel_p(new ScrollPanel()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static ScrollPanel_p	cast( const Object_p& pObject );
	
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
		Size		contentSize() { return m_contentSize; };
	
		void		setContentOrigo( Origo origo );
		Origo		contentOrigo() const { return m_contentOrigo; }
	
		void		setContentSizePolicy( SizePolicy widthPolicy, SizePolicy heightPolicy );
		SizePolicy	contentWidthPolicy() const { return m_widthPolicy; }
		SizePolicy	contentHeightPolicy() const { return m_heightPolicy; }
	
	
		int			viewPixelOfsX() { return m_viewPixOfs.x; };
		int			viewPixelOfsY() { return m_viewPixOfs.y; };
		Coord		viewPixelOfs() { return m_viewPixOfs; };
	
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
	
		bool	viewInclude( Rect& rect );
		bool	viewIncludeX( Rect& rect );
		bool	viewIncludeY( Rect& rect );
	*/
	
		bool		setScrollWheel( int wheel );
		int			scrollWheel() const { return m_wheelForScroll; }
	
		bool		setAutoscroll( bool bAutoX, bool bAutoY	);
		bool		autoScrollX() const { return m_bAutoScrollX; }
		bool		autoScrollY() const { return m_bAutoScrollY; }
	
		ScrollHook_p setHorizontalScrollbar( const Scrollbar_p& pScrollbar );
		void		removeHorizontalScrollbar() {setHorizontalScrollbar(0);}
		Scrollbar_p	horizontalScrollbar() const;
	
		ScrollHook_p setVerticalScrollbar( const Scrollbar_p& pScrollbar );
		void		removeVerticalScrollbar() {setVerticalScrollbar(0);}
		Scrollbar_p	verticalScrollbar() const;
	
		ScrollHook_p setContent( const Widget_p& pContent );
		void		removeContent() {setContent(0); }
		Widget_p	content() const;
	
		bool		removeWidget( const Widget_p& pWidget );
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
	
		void		setCornerSkin( const Skin_p& pSkin );
		Skin_p		cornerSkin() const { return m_pCornerSkin; }
	
		virtual void setSkin( const Skin_p& pSkin );
	
		// Overloaded from Widget
	
		Size				preferredSize() const;				// = preferred size of dragbars in the geometry, fixed value if dragbars are missing.
	
	
	
	/*
		NEED TO BE IMPLEMENTED!!!
	
		int		matchingHeight( int width ) const;	//
		int		matchingWidth( int height ) const;
	
	*/
	
	protected:
		ScrollPanel();
		virtual ~ScrollPanel();
		virtual Widget* _newOfMyType() const { return new ScrollPanel(); };

		//

		Coord		_childPos( void * pChildRef ) const;
		Size		_childSize( void * pChildRef ) const;

		void		_childRequestRender( void * pChildRef );
		void		_childRequestRender( void * pChildRef, const Rect& rect );
		void		_childRequestResize( void * pChildRef );

		Widget *	_prevChild( void * pChildRef ) const;
		Widget *	_nextChild( void * pChildRef ) const;

		Rect		_childWindowSection( void * pChildRef ) const;

		enum ElementType
		{
			WINDOW = 0,
			XDRAG,
			YDRAG
		};
	
	
		class MyScrollbarTarget : public ScrollbarTarget
		{
		public:
			float		_stepFwd();
			float		_stepBwd();
			float		_jumpFwd();
			float		_jumpBwd();
			float		_wheelRolled( int distance );
	
			float		_setPosition( float fraction );
	
			Widget*		_getWidget();
			float		_getHandlePosition();
			float		_getHandleSize();
	
			void		_updateScrollbar( float pos, float size ) { ScrollbarTarget::_updateScrollbar(pos,size); }
	
			bool		m_bHorizontal;
			ScrollPanel *	m_pParent;
		};
	
	
		static const int	MAX_ELEMENTS = 3;
	
		virtual void _setSize( const Size& size );
	
		// Overloaded from Panel
	
		Widget *	_findWidget( const Coord& pos, SearchMode mode );
	
		//
	
		void		_receive( const Msg_p& pMsg );
		void		_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches );
		void		_collectPatches( Patches& container, const Rect& geo, const Rect& clip );
		void		_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode );
	
		bool		_alphaTest( const Coord& ofs );
		void		_cloneContent( const Widget * _pOrg );
	
		Hook *		_firstChildWithGeo( Rect& geo ) const;
		Hook *		_nextChildWithGeo( Rect& geo, Hook * pHook ) const;	
	
		Size		_calcContentSize( Size mySize );
		void		_updateElementGeo( Size mySize );
	
		Rect		_genContentCanvasGeo( const Rect& window, Size contentSize, Origo origo, Coord viewOfs );
	
		bool		_wheelRollX(int distance);
		bool		_wheelRollY(int distance);
	
		int			_paddedViewPixelLenX();				// Width of view after childs window padding has been applied.
		int			_paddedViewPixelLenY();				// Height of view after childs window padding has been applied.
		float		_paddedViewLenX();
		float		_paddedViewLenY();
	
	
	
		Size		m_contentSize;
	
		int			m_stepSizeX;
		int			m_stepSizeY;
		float		m_jumpSizeX;
		float		m_jumpSizeY;
	
		Coord		m_viewPixOfs;
	
		bool		m_bAutoHideScrollbarX;		// Should X-scrollbar autohide when not needed?
		bool		m_bAutoHideScrollbarY;		// Should Y-scrollbar autohide when not needed?
	
		bool		m_bScrollbarBottom;		// True if X-scrollbar should be at the bottom, otherwise at the top.
		bool		m_bScrollbarRight;			// True if Y-scrollbar should be at the right, otherwise at the left.
	
		bool		m_bAutoScrollX;
		bool		m_bAutoScrollY;
	
		int			m_wheelForScroll;		// What wheel should be used for scrolling. (0=none)
	
		bool		m_bOverlayScrollbars;
		
		ScrollHook	m_elements[MAX_ELEMENTS];	// Content, xDrag and yDrag widgets in that order.
	
		SizePolicy	m_widthPolicy;
		SizePolicy	m_heightPolicy;
		Origo		m_contentOrigo;		// Origo when content is smaller than window
	
		MyScrollbarTarget	m_scrollbarTargets[2];			// Order: Vertical, horizontal
	
	private:
	
		Hook*	_firstChild() const;
		Hook*	_lastChild() const;
	
	
		Skin_p		m_pCornerSkin;
		Rect			m_cornerGeo;
	};
	

} // namespace wg
#endif //WG_SCROLLPANEL_DOT_H

