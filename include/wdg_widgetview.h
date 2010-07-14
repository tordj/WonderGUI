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

#ifndef WDG_WIDGETVIEW_DOT_H
#define WDG_WIDGETVIEW_DOT_H

#ifndef WDG_BASECLASS_VIEW_DOT_H
#	include <wdg_baseclass_view.h>
#endif


class Wdg_WidgetView : public Wdg_Baseclass_View
{
public:
	WIDGET_CONSTRUCTORS(Wdg_WidgetView,Wdg_Baseclass_View)
	virtual ~Wdg_WidgetView();
	virtual const char * Type() const;
	static const char * GetMyType();

	void	SetContent( WgWidget * pContent );
	WgWidget * Content() { return m_pViewContent; }

protected:
	WgWidget * NewOfMyType() const;

	void	DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );
	void	DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer );	
	void	DoMyOwnRefresh( void );

	void	DoMyOwnActionRespond( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	bool	DoMyOwnMarkTest( int _x, int _y );

	void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
//	void	DoMyOwnDisOrEnable( void );
	
private:
	void 	Init();

	void RenderRecursively( WgWidget * pWidget, const WgRect& parentGeo, const WgRect& clip, Uint8 _layer );
	void TransferDirtRecursively( WgWidget * pWidget, WgDirtyRectObj * pDest, int ofsx, int ofsy );

	WgWidget *	m_pViewContent;
	WgWidget *	m_pLastMarkedWidget;
	WgWidget *	m_pPressWidget[10];
};



#endif //WDG_WIDGETVIEW_DOT_H


