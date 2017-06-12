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

#ifndef	WG_VALUEDISPLAY_DOT_H
#define	WG_VALUEDISPLAY_DOT_H
#pragma once

#	include <wg_widget.h>
#	include <wg_modvalue.h>

namespace wg 
{
	
	
	class ValueDisplay;
	typedef	StrongPtr<ValueDisplay>		ValueDisplay_p;
	typedef	WeakPtr<ValueDisplay>	ValueDisplay_wp;

	/**
	* @brief Widget that displays a formatted value.
	*/ 
	class ValueDisplay : public Widget
	{
	public:
		//.____ Creation __________________________________________

		static ValueDisplay_p	create() { return ValueDisplay_p(new ValueDisplay()); }

		//.____ Interfaces _______________________________________

		ModValue		value;

		//.____ Identification __________________________________________

		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static ValueDisplay_p	cast( Object * pObject );
		
		//.____ Geometry ____________________________________________
	
		Size	preferredSize() const;
	
	protected:
		ValueDisplay();
		virtual ~ValueDisplay();
		virtual Widget* _newOfMyType() const { return new ValueDisplay(); };
	
		void	_refresh();
		void	_cloneContent( const Widget * _pOrg );
		void	_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void	_setState( State state );
		void	_setSkin( Skin * pSkin );
	
	private:
	
		ModValueItem		m_item;
	};

} // namespace wg
#endif // WG_VALUEDISPLAY_DOT_H
