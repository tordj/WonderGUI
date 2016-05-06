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
#	include	<wg_modtext.h>
#endif

namespace wg 
{
	
	
	class Font;
	
	class FpsDisplay;
	typedef	StrongPtr<FpsDisplay,Widget_p>		FpsDisplay_p;
	typedef	WeakPtr<FpsDisplay,Widget_wp>	FpsDisplay_wp;
	
	class FpsDisplay:public Widget, protected TextHolder
	{
	public:
		static FpsDisplay_p	create() { return FpsDisplay_p(new FpsDisplay()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FpsDisplay_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		ModText		labels;
		Text		values;
	
		//____ Methods __________________________________________
	
		Size	preferredSize() const;
	
	protected:
		FpsDisplay();
		virtual ~FpsDisplay();
		virtual Widget* _newOfMyType() const { return new FpsDisplay(); };
	
		void		_receive( const Msg_p& pMsg );
		void		_setState( State state );
		void		_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void		_cloneContent( const Widget * _pOrg );
	
		Object * 	_object() { return this; };
		void		_onFieldDirty( Field * pField );
		void		_onFieldDirty( Field * pField, const Rect& rect );
		void 		_onFieldResize( Field * pField );
	
	
	private:
	
	const static int c_tickBuffer = 64;
	
	
		TextField	m_labelsText;
		TextField	m_valuesText;
		int *		m_pTickBuffer;
		int			m_tickBufferOfs;
		RouteId		m_tickRouteId;
	};
	
	
	

} // namespace wg
#endif //FpsDisplay_DOT_H
