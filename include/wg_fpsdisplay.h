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

#ifndef	WG_FPSDISPLAY_DOT_H
#define WG_FPSDISPLAY_DOT_H

#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include	<wg_textprop.h>
#endif

class WgFont;


class WgFpsDisplay:public WgGizmo
{
	public:
		WgFpsDisplay();
		virtual ~WgFpsDisplay();
		virtual const char * Type() const;
		static const char * GetClass();
		virtual WgGizmo * NewOfMyType() const { return new WgFpsDisplay(); };
		

		//____ Methods __________________________________________

		void	SetTextProperties( const WgTextpropPtr& pProp );
		WgSize	DefaultSize() const;

	protected:
		void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
		void	_onCloneContent( const WgGizmo * _pOrg );

	private:

		WgTextpropPtr	m_pProp;


		Uint32 *		m_pTickBuffer;
		Uint32			m_tickBufferOfs;

};



#endif //WgFpsDisplay_DOT_H