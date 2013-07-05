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

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include	<wg_text.h>
#endif

class WgFont;


class WgFpsDisplay:public WgWidget
{
	public:
		WgFpsDisplay();
		virtual ~WgFpsDisplay();
		virtual const char * Type() const;
		static const char * GetClass();
		virtual WgWidget * NewOfMyType() const { return new WgFpsDisplay(); };
		

		//____ Methods __________________________________________

		void	SetTextProperties( const WgTextpropPtr& pProp );
		WgSize	PreferredSize() const;

	protected:
		void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
		void	_onStateChanged( WgState oldState, WgState newState );
		void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void	_onCloneContent( const WgWidget * _pOrg );
		void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );

	private:

		WgText		m_labelsText;
		WgText		m_valuesText;
		int *		m_pTickBuffer;
		int			m_tickBufferOfs;

};



#endif //WgFpsDisplay_DOT_H
