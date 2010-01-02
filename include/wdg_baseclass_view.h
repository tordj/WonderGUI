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

#ifndef	WDG_BASECLASS_VIEW_DOT_H
#define	WDG_BASECLASS_VIEW_DOT_H


/* PROBLEM

	1. Tableview med rubriker som man kan klicka på för att sortera, resiza etc.
		Samt ev. vänsterkolumn med nummer och resizemöjlighet.
		Ska den ligga inuti view-arean?

	2. Scrollbars som automatiskt dyker upp/försvinner efter behov.

*/




#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WDG_DRAGBARS_DOT_H
#	include <wdg_dragbars.h>
#endif


class Wdg_Baseclass_View : public WgWidget
{
public:
	virtual ~Wdg_Baseclass_View();
	virtual const char * Type() const;
	static const char * GetMyType();

	//____ Callbacks _____________________________________________________________

	static void cbStepUp	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->StepUp(); }
	static void cbStepDown	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->StepDown(); }
	static void cbStepLeft	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->StepLeft(); }
	static void cbStepRight	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->StepRight(); }

	static void cbJumpUp	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->JumpUp(); }
	static void cbJumpDown	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->JumpDown(); }
	static void cbJumpLeft	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->JumpLeft(); }
	static void cbJumpRight	(void * pWdg) { ((Wdg_Baseclass_View*)pWdg)->JumpRight(); }

	static void cbSetStepSize	(void * pWdg, Uint32 x, Uint32 y) { ((Wdg_Baseclass_View*)pWdg)->SetStepSize(x,y); }
	static void cbSetStepSizeX	(void * pWdg, Uint32 x) { ((Wdg_Baseclass_View*)pWdg)->SetStepSizeX(x); }
	static void cbSetStepSizeY	(void * pWdg, Uint32 y) { ((Wdg_Baseclass_View*)pWdg)->SetStepSizeY(y); }

	static void cbSetJumpSize	(void * pWdg, float x, float y) { ((Wdg_Baseclass_View*)pWdg)->SetJumpSize(x,y); }
	static void cbSetJumpSizeX	(void * pWdg, float x) { ((Wdg_Baseclass_View*)pWdg)->SetJumpSizeX(x); }
	static void cbSetJumpSizeY	(void * pWdg, float y) { ((Wdg_Baseclass_View*)pWdg)->SetJumpSizeY(y); }

	static void cbSetViewPixelOfs	(void * pWdg, Uint32 x, Uint32 y) { ((Wdg_Baseclass_View*)pWdg)->SetViewPixelOfs(x,y); }
	static void cbSetViewPixelOfsX	(void * pWdg, Uint32 x) { ((Wdg_Baseclass_View*)pWdg)->SetViewPixelOfsX(x); }
	static void cbSetViewPixelOfsY	(void * pWdg, Uint32 y) { ((Wdg_Baseclass_View*)pWdg)->SetViewPixelOfsY(y); }

	static void cbSetViewOfs	(void * pWdg, float x, float y) { ((Wdg_Baseclass_View*)pWdg)->SetViewOfs(x,y); }
	static void cbSetViewOfsX	(void * pWdg, float x) { ((Wdg_Baseclass_View*)pWdg)->SetViewOfsX(x); }
	static void cbSetViewOfsY	(void * pWdg, float y) { ((Wdg_Baseclass_View*)pWdg)->SetViewOfsY(y); }
	
	static void cbWheelRoll(void * pWdg, int distance) { ((Wdg_Baseclass_View*)pWdg)->WheelRoll(distance); }


	//____ Methods _______________________________________________________________

	bool	StepUp();
	bool	StepDown();
	bool	StepLeft();
	bool	StepRight();

	bool	JumpUp();
	bool	JumpDown();
	bool	JumpLeft();
	bool	JumpRight();

	bool	WheelRoll(int distance);

	inline void	SetStepSize( Uint32 pixX, Uint32 pixY ){ m_stepSizeX = pixX; m_stepSizeY = pixY; };
	inline void	SetStepSizeX( Uint32 pixels ){ m_stepSizeX = pixels; };
	inline void	SetStepSizeY( Uint32 pixels ){ m_stepSizeY = pixels; };

	inline Uint32 StepSizeX() { return m_stepSizeX; };
	inline Uint32 StepSizeY() { return m_stepSizeY; };

	inline void	SetJumpSize( float viewFractionX, float viewFractionY ){ SetJumpSizeX(viewFractionX); SetJumpSizeY(viewFractionY); };
	void	SetJumpSizeX( float viewFraction );
	void	SetJumpSizeY( float viewFraction );
	float	JumpSizeX( ) const { return m_jumpSizeX; }
	float	JumpSizeY( ) const { return m_jumpSizeY; }

	inline Uint32	ContentWidth() { return m_contentWidth; };
	inline Uint32	ContentHeight() { return m_contentHeight; };

	inline Uint32	ViewPixelOfsX() { return m_viewPixOfsX; };
	inline Uint32	ViewPixelOfsY() { return m_viewPixOfsY; };
	
	Uint32	ViewPixelLenX();
	Uint32	ViewPixelLenY();

	float	ViewOfsX();
	float	ViewOfsY();
	float	ViewLenX();
	float	ViewLenY();

	bool	SetViewPixelOfs( Uint32 x, Uint32 y );
	bool	SetViewPixelOfsX( Uint32 x );
	bool	SetViewPixelOfsY( Uint32 y );

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

	bool	SetScrollbarX( Wdg_HDrag * pScrollbar );
	bool	SetScrollbarY( Wdg_VDrag * pScrollbar );
	Wdg_HDrag *GetScrollbarX() const { return m_pScrollbarX; }
	Wdg_VDrag *GetScrollbarY() const { return m_pScrollbarY; }

	void	SetScrollbarAutoHide( bool bHideX, bool bHideY );
	bool	GetScrollbarAutoHideX() const { return m_bAutoHideScrollbarX; }
	bool	GetScrollbarAutoHideY() const { return m_bAutoHideScrollbarY; }

	void	SetScrollbarPositions( bool bBottom, bool bRight );
	bool	GetScrollbarBottom() const { return m_bScrollbarBottom; }
	bool	GetScrollbarRight() const { return m_bScrollbarRight; }

	bool	ScrollbarXVisible();
	bool	ScrollbarYVisible();

protected:
	WIDGET_CONSTRUCTORS(Wdg_Baseclass_View,WgWidget)
	virtual void DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo );

	// Following method should be overridden by subclasses instead of DoMyOwnGeometryChange()!
	// Takes into account that scrollbars might decrease the visible geometry of the subclass.

	virtual void DoMyOwnGeometryChangeSubclass( WgRect& oldGeo, WgRect& newGeo ) {};
	virtual void DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );

	void		SetContentSize( Uint32 width, Uint32 height );

	Uint32		m_contentWidth;
	Uint32		m_contentHeight;
	Uint32		m_stepSizeX;
	Uint32		m_stepSizeY;
	float		m_jumpSizeX;
	float		m_jumpSizeY;

	Uint32		m_viewPixOfsX;
	Uint32		m_viewPixOfsY;

	Wdg_HDrag *	m_pScrollbarX;
	Wdg_VDrag *	m_pScrollbarY;

	bool		m_bAutoHideScrollbarX;		// Should X-scrollbar autohide when not needed?
	bool		m_bAutoHideScrollbarY;		// Should Y-scrollbar autohide when not needed?

	bool		m_bScrollbarBottom;		// True if X-scrollbar should be at the bottom, otherwise at the top.
	bool		m_bScrollbarRight;		// True if Y-scrollbar should be at the right, otherwise at the left.

	bool		m_bAutoScrollX;
	bool		m_bAutoScrollY;


private:
		void	Init();

		WgRect	m_subclassGeo;
};

#endif //WDG_BASECLASS_VIEW_DOT_H 

