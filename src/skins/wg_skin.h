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

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size			minSize() const;
		Size			preferredSize() const;

		//.____ Misc ____________________________________________________

		virtual bool	isOpaque() const = 0;
		virtual bool	isOpaque( State state ) const = 0;
		bool			isOpaque( const Rect& rect, const Size& canvasSize, State state ) const;

		virtual bool	isStateIdentical( State state, State comparedTo ) const = 0;

		//.____ Internal ______________________________________________________

		virtual bool	_markTest(const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold) const = 0;
		virtual bool	_isOpaque(const RectI& rect, const SizeI& canvasSize, State state) const = 0;

		virtual SizeI	_minSize() const = 0;
		virtual SizeI	_preferredSize() const = 0;
		virtual SizeI	_sizeForContent(const SizeI contentSize) const = 0;

		virtual BorderI	_contentPadding() const = 0;
		virtual SizeI	_contentPaddingSize() const = 0;
		virtual CoordI	_contentOfs(State state) const = 0;
		virtual RectI	_contentRect(const RectI& canvas, State state) const = 0;
		 

		virtual void 	_render(GfxDevice * pDevice, const RectI& canvas, State state) const = 0;


	protected:
		Skin() {};
	};




} // namespace wg
#endif //WG_SKIN_DOT_H
