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

#ifndef	WDG_FPS_DOT_H
#define WDG_FPS_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_FPS_DOT_H
#	include <wg_gizmo_fps.h>
#endif

class Wdg_Fps : public Wdg_Widget<WgGizmoFps>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_Fps,WgGizmoFps);
	virtual ~Wdg_Fps() {};

};

/*
#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include	<wg_textprop.h>
#endif

class WgFont;


class Wdg_Fps:public WgWidget
{
	public:
		WIDGET_CONSTRUCTORS(Wdg_Fps,WgWidget)
		virtual ~Wdg_Fps();
		virtual const char * Type() const;
		static const char * GetMyType();

		//____ Methods __________________________________________

		void SetTextProperties( const WgTextPropPtr& pProp );

	protected:
		WgWidget * NewWidgetOfMyType() const;
	private:
		void	Init();
		void	DoMyOwnRender( const WgRect& _window, const WgRect& _clip, Uint8 _layer );
		void	DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot );
		void	DoMyOwnUpdate( const WgUpdateInfo& _updateInfo );
		bool	DoMyOwnMarkTest( int _x, int _y );

		WgTextPropPtr	m_pProp;


		Uint32 *		m_pTickBuffer;
		Uint32			m_tickBufferOfs;

};
*/


#endif //WDG_FPS_DOT_H
