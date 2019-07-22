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
#ifndef WG_MULTIBLOCKSKIN_DOT_H
#define WG_MULTIBLOCKSKIN_DOT_H
#pragma once

#include <wg_extendedskin.h>
#include <wg_surface.h>

#include <initializer_list>
#include <utility>
#include <vector>

namespace wg
{

	class MultiBlockSkin;
	typedef	StrongPtr<MultiBlockSkin>	MultiBlockSkin_p;



	class MultiBlockSkin : public ExtendedSkin
	{
		//TODO: Add sanity-checking to all Set-methods.
		//TODO: Optimize rendering based on invisibleSections and opaqueSections!

	public:

		//.____ Creation __________________________________________

		static MultiBlockSkin_p create(SizeI blockSize, BorderI frame = BorderI(0) );

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const override;
		const char *			className(void) const override;
		static const char		CLASSNAME[];
		static MultiBlockSkin_p	cast(Object * pObject);

		//.____ Rendering ________________________________________________

		void	render( GfxDevice * pDevice, const RectI& _canvas, State state ) const override;

		//.____ Geometry _________________________________________________

		Size	minSize() const override;
		Size	preferredSize() const override;

		Size	sizeForContent( const Size contentSize ) const override;

		//.____ Appearance _________________________________________________

		int		addLayer(Surface * pSurf, CoordI ofs);
		int		addLayer(Surface * pSurf, CoordI blockStartOfs, CoordI blockPitch, const std::initializer_list<State>& stateBlocks);		// DEPRECATED!!!
		int		addLayer(Surface * pSurf, const std::initializer_list<State>& stateBlocks, Orientation orientation = Orientation::Vertical, int spacing = 0, CoordI blockStartOfs = { 0,0 } );

		bool	setLayerTint(int layerIdx, Color tintColor);
		bool	setLayerTint(int layerIdx, const std::initializer_list< std::pair<State,Color> >& stateColors);
		bool	setLayerBlendMode(int layerIdx, BlendMode blendMode);


		//.____ Misc ____________________________________________________

		bool	markTest( const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold ) const override;

		bool	isOpaque() const override;
		bool	isOpaque( State state ) const override;
		bool	isOpaque( const RectI& rect, const SizeI& canvasSize, State state ) const override;

		bool	isStateIdentical( State state, State comparedTo ) const override;

	private:

		struct LayerData
		{
			Surface *			pSurface;
			BlendMode			blendMode;
			Bitmask<uint32_t>   stateBlockMask = 0;
			Bitmask<uint32_t>   stateColorMask = 1;         // Normal state always exist for colors and is by default white.

			CoordI				blockOfs[StateEnum_Nb];		// Block for each state
			Color				tintColor[StateEnum_Nb];
	//		Origo			placementOrigo;
	//		CoordI			placementOfs;
	//		SizeI			dimensions;                   // Stored in pixels
	//		int				m_tiledSections;
		};


		MultiBlockSkin(SizeI blockSize, BorderI frame);
		~MultiBlockSkin() {};

		void			_updateStateOpacity( int stateIdx );

		SizeI			m_blockSizePoints;					// Stored in points

		SizeI			m_blockSize;						// Stored in pixels
		bool			m_bIsOpaque;
		BorderI			m_frame;                        // Stored in points

		std::vector<LayerData>	m_layers;

		bool			m_bStateOpaque[StateEnum_Nb];

	};

}

#endif //WG_MULTIBLOCKSKIN_DOT_H
