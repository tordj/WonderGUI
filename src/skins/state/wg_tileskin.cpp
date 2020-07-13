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

#include <wg_tileskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	using namespace Util;

	const TypeInfo TileSkin::TYPEINFO = { "TileSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	TileSkin_p TileSkin::create()
	{
		return TileSkin_p(new TileSkin());
	}


	TileSkin_p TileSkin::create(Surface * pSurface)
	{
		if (pSurface == nullptr || pSurface->isTiling() )
			return nullptr;

		return TileSkin_p( new TileSkin(pSurface) );
	}

	TileSkin_p TileSkin::create(std::initializer_list<std::tuple<State, Surface_p>>& stateSurfaces)
	{
		if (stateSurfaces.size() < 1 )
			return nullptr;

		// Create the skin

		TileSkin * p = new TileSkin();

		p->setSurfaces(stateSurfaces);
		return TileSkin_p(p);
	}


	//____ constructor ____________________________________________________________

	TileSkin::TileSkin()
	{
		m_bOpaque = false;

		for (int i = 0; i < StateEnum_Nb; i++)
			m_stateColors[i] = Color::White;
	}

	TileSkin::TileSkin(Surface * pSurface)
	{
		m_bOpaque			= pSurface->isOpaque();
		
		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			m_stateSurfaces[i] = pSurface;
			m_stateColors[i] = Color::White;
		}
	}



	//____ typeInfo() _________________________________________________________

	const TypeInfo& TileSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlock() _____________________________________________________________

	void TileSkin::setSurface(Surface * pSurface)
	{
		m_stateSurfaces[0] = pSurface;
		m_stateSurfaceMask = 1;

		_updateUnsetStateSurfaces();
	}

	void TileSkin::setSurface(State state, Surface * pSurface)
	{
		int i = _stateToIndex(state);

		m_stateSurfaces[i] = pSurface;
		m_stateSurfaceMask.setBit(i);
		_updateUnsetStateSurfaces();
	}

	//____ setSurfaces() ________________________________________________________

	void TileSkin::setSurfaces(std::initializer_list<std::tuple<State, Surface_p>>& stateSurfaces)
	{
		for (auto& state : stateSurfaces)
		{
			int index = _stateToIndex(std::get<0>(state));
			m_stateSurfaceMask.setBit(index);
			m_stateSurfaces[index] = std::get<1>(state);
		}
		_updateUnsetStateSurfaces();
	}

	//____ surface() ____________________________________________________________

	Surface_p TileSkin::surface(State state) const
	{
		return m_stateSurfaces[_stateToIndex(state)];
	}

	//____ setTint() __________________________________________________________

	void TileSkin::setTint(Color tint)
	{
		m_stateColors[0] = tint;
		m_stateColorMask = 1;

		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void TileSkin::setTint(State state, Color tint)
	{
		int i = _stateToIndex(state);

		m_stateColors[i] = tint;
		m_stateColorMask.setBit(i);
		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void TileSkin::setTint(const std::initializer_list< std::tuple<State, Color> >& stateTints)
	{
		for (auto& state : stateTints)
		{
			int i = _stateToIndex(std::get<0>(state));
			m_stateColorMask.setBit(i);
			m_stateColors[i] = std::get<1>(state);
		}

		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	//____ tint() _____________________________________________________________

	Color TileSkin::tint(State state) const
	{
		return m_stateColors[_stateToIndex(state)];
	}



	//____ setBlendMode() _____________________________________________________

	void TileSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlags();
	}

	//____ render() _______________________________________________________________

	void TileSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, float fraction, float fraction2) const
	{
		Surface * pSurf = m_stateSurfaces[_stateToIndex(state)];

		if( !pSurf )
			return;

		BlendMode savedBlendMode = BlendMode::Undefined;
		if (m_blendMode != BlendMode::Undefined)
		{
			savedBlendMode = pDevice->blendMode();
			pDevice->setBlendMode(m_blendMode);
		}

		pDevice->setBlitSource(pSurf);
		pDevice->scaleTile(canvas.px(),MU::scale());

		if (m_blendMode != BlendMode::Undefined)
			pDevice->setBlendMode(savedBlendMode);
	}

	//____ preferredSize() ________________________________________________________

	Size TileSkin::preferredSize() const
	{
		Size content = Border(m_contentPadding).aligned();
		Size surface;

		Surface * pSurface = m_stateSurfaces[0];
		if (pSurface)
			surface = pSurface->size();

		return Size::max(content, surface);
	}

	//____ markTest() _____________________________________________________________

	bool TileSkin::markTest( const Coord& _ofs, const Rect& canvas, State state, int opacityTreshold, float fraction, float fraction2) const
	{
		return true; //TODO: Implement!

//		CoordI srcOfs = m_stateBlocks[_stateToIndex(state)];
//		return markTestNinePatch(_ofs, m_pSurface, { srcOfs,m_dimensions }, canvas, opacityTreshold, m_frame);
	}

	//____ isOpaque() _____________________________________________________________

	bool TileSkin::isOpaque( State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	bool TileSkin::isOpaque( const Rect& rect, const Size& canvasSize, State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	//____ isStateIdentical() _____________________________________________________

	bool TileSkin::isStateIdentical( State state, State comparedTo, float fraction, float fraction2) const
	{
		int i1 = _stateToIndex(state);
		int i2 = _stateToIndex(comparedTo);

		return ( m_stateSurfaces[i1] == m_stateSurfaces[i2] && StateSkin::isStateIdentical(state,comparedTo) );
	}

	//____ _updateOpaqueFlags() ________________________________________________

	void TileSkin::_updateOpaqueFlags()
	{
		bool bTintDecides = false;

		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Undefined )		// Assumes that incoming BlendMide is Blend.
		{
			bool bOpaque = true;

			for (int i = 0; i < StateEnum_Nb; i++)
			{
				if (!m_stateSurfaces[i] || !m_stateSurfaces[i]->isOpaque())
				{
					bOpaque = false;
					break;
				}
			}

			m_bOpaque = bOpaque;
			bTintDecides = m_bOpaque;
		}
		else
			m_bOpaque = false;

		if (bTintDecides)
		{
			for (int i = 0; i < StateEnum_Nb; i++)
				m_bStateOpaque[i] = m_stateColors[i].a == 255;
		}
		else
		{
			for (int i = 0; i < StateEnum_Nb; i++)
				m_bStateOpaque[i] = m_bOpaque;
		}
	}

	//____ _updateUnsetStateSurfaces() _______________________________________________

	void TileSkin::_updateUnsetStateSurfaces()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateSurfaceMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateSurfaceMask);
				m_stateSurfaces[i] = m_stateSurfaces[bestAlternative];
			}
		}
	}

	//____ _updateUnsetStateColors() _______________________________________________

	void TileSkin::_updateUnsetStateColors()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateColorMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateColorMask);
				m_stateColors[i] = m_stateColors[bestAlternative];
			}
		}
	}


} // namespace wg
