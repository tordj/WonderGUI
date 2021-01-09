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

#include <wg_stateskin.h>
#include <wg_surface.h>

#include <initializer_list>
#include <utility>
#include <vector>

namespace wg
{

	class MultiBlockSkin;
	typedef	StrongPtr<MultiBlockSkin>	MultiBlockSkin_p;
	typedef	WeakPtr<MultiBlockSkin>	MultiBlockSkin_wp;



	class MultiBlockSkin : public StateSkin
	{
		//TODO: Add sanity-checking to all Set-methods.
		//TODO: Optimize rendering based on invisibleSections and opaqueSections!

	public:

		//.____ Creation __________________________________________

		static MultiBlockSkin_p create(SizeI blockSize, BorderI frame = BorderI(0) );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Appearance _________________________________________________

		int		addLayer(Surface * pSurf, CoordI ofs);
		int		addLayer(Surface * pSurf, CoordI blockStartOfs, SizeI blockPitch, std::initializer_list<State> stateBlocks);		// DEPRECATED!!!
		int		addLayer(Surface * pSurf, std::initializer_list<State> stateBlocks, Axis axis = Axis::Y, int spacing = 0, CoordI blockStartOfs = { 0,0 } );

		bool	setLayerColor(int layerIdx, HiColor tintColor);
		bool	setLayerColor(int layerIdx, std::initializer_list< std::pair<State,HiColor> > stateColors);
		bool	setLayerBlendMode(int layerIdx, BlendMode blendMode);

		//.____ Geometry _________________________________________________

		Size	minSize() const override;
		Size	preferredSize() const override;
		Size	sizeForContent(const Size& contentSize) const override;

		//.____ Misc ____________________________________________________

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		Rect	dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:

		struct LayerData
		{
			Surface *			pSurface;
			BlendMode			blendMode;
			Bitmask<uint32_t>   stateBlockMask = 0;
			Bitmask<uint32_t>   stateColorMask = 1;         // Normal state always exist for colors and is by default white.

			CoordI				blockOfs[StateEnum_Nb];		// Block for each state
			HiColor				tintColor[StateEnum_Nb];
	//		Placement			placementOrigo;
	//		CoordI			placementOfs;
	//		SizeI			dimensions;                   // Stored in pixels
	//		int				m_tiledSections;
		};


		MultiBlockSkin(SizeI blockSize, BorderI frame);
		~MultiBlockSkin() {};

		void			_updateStateOpacity( int stateIdx );

		SizeI			m_blockSizePoints;					// Stored in points

		SizeI			m_blockSize;						// Stored in pixels
		BorderI			m_frame;                        // Stored in points

		std::vector<LayerData>	m_layers;

		bool			m_bStateOpaque[StateEnum_Nb];

	};

}

#endif //WG_MULTIBLOCKSKIN_DOT_H
