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
	
	
	class Font;
	
	class FpsDisplay;
	typedef	WgStrongPtr<FpsDisplay,Widget_p>		FpsDisplay_p;
	typedef	WgWeakPtr<FpsDisplay,Widget_wp>	FpsDisplay_wp;
	
	class FpsDisplay:public Widget, protected LegacyTextHolder
	{
	public:
		static FpsDisplay_p	create() { return FpsDisplay_p(new FpsDisplay()); }
	
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static FpsDisplay_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		LegacyModText		labels;
		LegacyText	values;
	
		//____ Methods __________________________________________
	
		void	setTextProperties( const Textprop_p& pProp );
		Size	preferredSize() const;
	
	protected:
		FpsDisplay();
		virtual ~FpsDisplay();
		virtual Widget* _newOfMyType() const { return new FpsDisplay(); };
	
		void		_onMsg( const Msg_p& pMsg );
		void		_onStateChanged( State oldState );
		void		_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void		_onCloneContent( const Widget * _pOrg );
		void		_onSkinChanged( const Skin_p& pOldSkin, const Skin_p& pNewSkin );
	
		Object * 	_object() { return this; };
		void		_onFieldDirty( Field * pField );
		void 		_onFieldResize( Field * pField );
	
	
	private:
	
		LegacyTextField	m_labelsText;
		LegacyTextField	m_valuesText;
		int *		m_pTickBuffer;
		int			m_tickBufferOfs;
		RouteId	m_tickRouteId;
	};
	
	
	

} // namespace wg
#endif //FpsDisplay_DOT_H
