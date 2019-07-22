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
#ifndef WG_BOXSKIN_DOT_H
#define WG_BOXSKIN_DOT_H
#pragma once

#include <wg_extendedskin.h>
#include <wg_color.h>

#include <initializer_list>
#include <utility>

namespace wg
{

	class BoxSkin;
	typedef	StrongPtr<BoxSkin>	BoxSkin_p;

	class BoxSkin : public ExtendedSkin
	{
	public:
		//.____ Creation __________________________________________

		static BoxSkin_p	create();
		static BoxSkin_p 	create(BorderI frame, Color fillColor, Color frameColor );
		static BoxSkin_p	create(BorderI frame, std::initializer_list< std::tuple<State,Color,Color> > stateColors );

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static BoxSkin_p	cast( Object * pObject );

		//.____ Rendering ________________________________________________

		void		render( GfxDevice * pDevice, const RectI& _canvas, State state ) const override;

		//.____ Geometry _________________________________________________

		Size		minSize() const override;
		Size		preferredSize() const override;
		Size		sizeForContent( const Size contentSize ) const override;

		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setFrame( BorderI frame );

		void						setColors(Color fill, Color frame);
		void						setColors(State state, Color fill, Color frame);
		void						setColors(std::initializer_list< std::tuple<State, Color, Color> > stateColors);
		std::tuple<Color, Color>	colors(State state) const;


		//.____ Misc ____________________________________________________

		bool		markTest( const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold ) const override;

		bool		isOpaque() const override;
		bool		isOpaque( State state ) const override;
		bool		isOpaque( const RectI& rect, const SizeI& canvasSize, State state ) const override;

		bool		isStateIdentical( State state, State comparedTo ) const override;

	private:
		BoxSkin();
		BoxSkin(BorderI frame, Color fillColor, Color frameColor  );
		~BoxSkin() {};

		void	_updateOpaqueFlag();
		void	_updateUnsetColors();

		bool		m_bOpaque;
		BorderI		m_frame;
		BlendMode	m_blendMode = BlendMode::Blend;

		Bitmask<uint32_t>	m_stateColorMask = 1;

		Color		m_fillColor[StateEnum_Nb];
		Color		m_frameColor[StateEnum_Nb];
	};


} // namespace wg
#endif //WG_BOXSKIN_DOT_H


