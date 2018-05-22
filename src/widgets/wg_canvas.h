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
#ifndef WG_CANVAS_DOT_H
#define WG_CANVAS_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_gfxdevice.h>
#include <wg_modbitmap.h>


namespace wg 
{
	
	class Canvas;
	typedef	StrongPtr<Canvas>	Canvas_p;
	typedef	WeakPtr<Canvas>		Canvas_wp;
	




	//____ Canvas ____________________________________________________________
	/**
	 * @brief Simplest kind of widget.
	 *
	 * Canvas is the simplest kind of widget that displays nothing but its skin
	 * and does nothing but posting the normal mouse messages that every widget does.
	 *
	 * It is typically used as backgrounds and for filling out empty spaces in
	 * organized layouts.
	 *
	 **/
	
	
	class Canvas : public Widget
	{
		friend class ModBitmap;

	public:

		//.____ Creation __________________________________________

		static Canvas_p	create() { return Canvas_p(new Canvas()); }

		//.____ Interfaces _______________________________________

		ModBitmap		canvas;


		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Canvas_p		cast( Object * pObject );
	
	
	protected:
		Canvas();
		virtual ~Canvas();
		virtual Widget* _newOfMyType() const { return new Canvas(); };
	
		void			_cloneContent( const Widget * _pOrg );
		void			_setSize(const Size& size);
		virtual void	_setSkin(Skin * pSkin);
		virtual void	_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip);
		virtual	bool	_alphaTest(const Coord& ofs);


		ModBitmapItem	m_canvas;
	};
	
	

} // namespace wg
#endif //WG_CANVAS_DOT_H
