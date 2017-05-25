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
#pragma once

#include <wg_widget.h>
#include	<wg_modtext.h>

namespace wg 
{
	
	
	class Font;
	
	class FpsDisplay;
	typedef	StrongPtr<FpsDisplay>		FpsDisplay_p;
	typedef	WeakPtr<FpsDisplay>	FpsDisplay_wp;
	
	class FpsDisplay : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static FpsDisplay_p	create() { return FpsDisplay_p(new FpsDisplay()); }
	

		//.____ Components _______________________________________

		ModText		labels;
		Text		values;


		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static FpsDisplay_p	cast( Object * pObject );
	
	
		//.____ Geometry ____________________________________________
	
		Size	preferredSize() const;
	
	protected:
		FpsDisplay();
		virtual ~FpsDisplay();
		virtual Widget* _newOfMyType() const { return new FpsDisplay(); };
	
		void		_receive( Msg * pMsg );
		void		_setState( State state );
		void		_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void		_cloneContent( const Widget * _pOrg );
				
	private:
	
	const static int c_tickBuffer = 64;
	
	
		TextItem	m_labelsText;
		TextItem	m_valuesText;
		int *		m_pTickBuffer;
		int			m_tickBufferOfs;
		RouteId		m_tickRouteId;
	};
	
	
	

} // namespace wg
#endif //FpsDisplay_DOT_H
