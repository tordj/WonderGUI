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
#ifndef WG_SKIN_DOT_H
#define WG_SKIN_DOT_H
#pragma once

#include <wg_pointers.h>
#include <wg_geo.h>
#include <wg_types.h>

namespace wg
{

	class Skin;
	class GfxDevice;

	typedef	StrongPtr<Skin>	Skin_p;


	class Skin : public Object
	{
	public:
		virtual ~Skin() {};

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Skin_p		cast( Object * pObject );

		//.____ Rendering ________________________________________________

		virtual void 	render(GfxDevice * pDevice, const Rect& canvas, State state) const = 0;

		//.____ Geometry _________________________________________________

		virtual SizeQ	minSize() const = 0;
		virtual SizeQ	preferredSize() const = 0;
		virtual SizeQ	sizeForContent( const SizeQ contentSize ) const = 0;

		virtual SizeQ	contentPadding() const = 0;
		virtual CoordQ	contentOfs( State state ) const = 0;
		virtual RectQ	contentRect( const RectQ& canvas, State state ) const = 0;


		//.____ Misc ____________________________________________________

		virtual bool	markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold ) const = 0;

		virtual bool	isOpaque() const = 0;
		virtual bool	isOpaque( State state ) const = 0;
		virtual bool	isOpaque( const Rect& rect, const Size& canvasSize, State state ) const = 0;

		virtual bool	isStateIdentical( State state, State comparedTo ) const = 0;

	protected:
		Skin() {};
	};


} // namespace wg
#endif //WG_SKIN_DOT_H
