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

#ifndef	WDG_PIXMAP_DOT_H
#define	WDG_PIXMAP_DOT_H

#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif

#ifndef WG_GIZMO_PIXMAP_DOT_H
#	include <wg_gizmo_pixmap.h>
#endif


class Wdg_Pixmap : public Wdg_Widget<WgGizmoPixmap>
{
public:
	Wdg_Pixmap( WgWidget* pParent = 0 ) : Wdg_Widget<WgGizmoPixmap>(pParent) {};
	Wdg_Pixmap ( const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoPixmap>(geometry, pParent) {};
	Wdg_Pixmap( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoPixmap>( origo, geometry, pParent ) {};
	Wdg_Pixmap( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoPixmap>(upperLeft,x1, y1, lowerRight, x2, y2, pParent) {}	
	virtual ~Wdg_Pixmap() {};

};


/*
#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class	WgSurface;

class Wdg_Pixmap:public WgWidget
{
public:
	WIDGET_CONSTRUCTORS(Wdg_Pixmap,WgWidget)
	virtual ~Wdg_Pixmap();
	virtual const char * Type();
	static const char * GetMyType();

	//____ Methods __________________________________________

	bool			SetSource( WgBlockSetPtr pBlockSet );
	WgBlockSetPtr	GetSource() { return m_pGfx; }

protected:
	WgWidget * NewWidgetOfMyType();
	void		Init();
protected:
	void		DoMyOwnRender( const WgRect& window, const WgRect& clip, Uint8 _layer );	
	void		DoMyOwnRefresh( void );
	void		DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
	bool 		DoMyOwnMarkTest( int _x, int _y );
    void 		DoMyOwnDisOrEnable( void );
private:
	WgBlockSetPtr	m_pGfx;
};
*/

#endif //	WDG_PIXMAP_DOT_H
