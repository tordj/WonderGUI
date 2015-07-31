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

#ifndef WG_LEGACYMODTEXT_DOT_H
#	include	<wg_legacymodtext.h>
#endif

namespace wg 
{
	
	
	class WgFont;
	
	class WgFpsDisplay;
	typedef	WgStrongPtr<WgFpsDisplay,WgWidget_p>		WgFpsDisplay_p;
	typedef	WgWeakPtr<WgFpsDisplay,WgWidget_wp>	WgFpsDisplay_wp;
	
	class WgFpsDisplay:public WgWidget, protected WgLegacyTextHolder
	{
	public:
		static WgFpsDisplay_p	create() { return WgFpsDisplay_p(new WgFpsDisplay()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static WgFpsDisplay_p	cast( const WgObject_p& pObject );
	
		//____ Interfaces ______________________________________
	
		WgLegacyModText		labels;
		WgLegacyText	values;
	
		//____ Methods __________________________________________
	
		void	setTextProperties( const WgTextprop_p& pProp );
		WgSize	preferredSize() const;
	
	protected:
		WgFpsDisplay();
		virtual ~WgFpsDisplay();
		virtual WgWidget* _newOfMyType() const { return new WgFpsDisplay(); };
	
		void		_onMsg( const WgMsg_p& pMsg );
		void		_onStateChanged( WgState oldState );
		void		_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void		_onCloneContent( const WgWidget * _pOrg );
		void		_onSkinChanged( const WgSkin_p& pOldSkin, const WgSkin_p& pNewSkin );
	
		WgObject * 	_object() { return this; };
		void		_onFieldDirty( WgField * pField );
		void 		_onFieldResize( WgField * pField );
	
	
	private:
	
		WgLegacyTextField	m_labelsText;
		WgLegacyTextField	m_valuesText;
		int *		m_pTickBuffer;
		int			m_tickBufferOfs;
		WgRouteId	m_tickRouteId;
	};
	
	
	

} // namespace wg
#endif //WgFpsDisplay_DOT_H
