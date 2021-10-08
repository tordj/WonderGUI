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
#include <wg_skin.impl.h>

namespace wg
{

	using namespace Util;

	const TypeInfo TileSkin::TYPEINFO = { "TileSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________


	TileSkin_p TileSkin::create(Surface * pSurface, HiColor color, int layer )
	{
		if (pSurface == nullptr || !pSurface->isTiling() )
			return nullptr;

		return TileSkin_p( new TileSkin(pSurface, color, layer) );
	}

	TileSkin_p TileSkin::create(const Blueprint& blueprint)
	{
		if (blueprint.surface == nullptr || !blueprint.surface->isTiling())
			return nullptr;


		return TileSkin_p(new TileSkin(blueprint));
	}


	//____ constructor ____________________________________________________________

	TileSkin::TileSkin(Surface * pSurface, HiColor color, int layer)
	{
		m_bOpaque			= pSurface->isOpaque();
		
		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			m_stateSurfaces[i] = pSurface;
			m_stateColors[i] = color;
		}

		m_layer = layer;
	}

	TileSkin::TileSkin(const Blueprint& blueprint)
	{
		int ofs = _stateToIndex(StateEnum::Normal);


		m_layer = blueprint.layer;
		m_blendMode = blueprint.blendMode;
		m_gradient = blueprint.gradient;
		m_contentPadding = blueprint.contentPadding;


		m_stateSurfaces[ofs] = blueprint.surface;
		m_stateColors[ofs] = blueprint.color;


		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (blueprint.states[i].state != StateEnum::Normal)
			{
				ofs = _stateToIndex(blueprint.states[i].state);

				m_stateSurfaces[ofs] = blueprint.states[i].data.surface;
				m_stateColors[ofs] = blueprint.states[i].data.color;
				m_contentShift[ofs] = blueprint.states[i].data.contentShift;

				if (m_stateSurfaces[ofs])
					m_stateSurfaceMask.setBit(ofs);

				if( m_stateColors[ofs] != HiColor::Undefined )
					m_stateColorMask.setBit(ofs);
			}
		}

		_updateOpaqueFlags();
		_updateUnsetStateSurfaces();
		_updateUnsetStateColors();
	}


	//____ typeInfo() _________________________________________________________

	const TypeInfo& TileSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}


	//____ _render() _______________________________________________________________

	void TileSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float * pStateFractions) const
	{
		int idx = _stateToIndex(state);

		Surface * pSurf = m_stateSurfaces[idx];

		if( !pSurf )
			return;

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_stateColors[idx], canvas, m_gradient);

		pDevice->setBlitSource(pSurf);
		pDevice->scaleTile(canvas,scale/64.f);
	}

	//____ _preferredSize() ________________________________________________________

	SizeSPX TileSkin::_preferredSize(int scale) const
	{
		SizeSPX content = align(ptsToSpx(m_contentPadding,scale));
		SizeSPX surface;

		Surface * pSurface = m_stateSurfaces[0];
		if (pSurface)
			surface = align(ptsToSpx(pSurface->pointSize(),scale));

		return SizeSPX::max(content, surface);
	}

	//____ _markTest() _____________________________________________________________

	bool TileSkin::_markTest( const CoordSPX& _ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		//TODO: Take gradient and tintColor into account.

		Surface * pSurf = m_stateSurfaces[_stateToIndex(state)];

		return markTestTileRect(_ofs, pSurf, canvas, scale, opacityTreshold);
	}

	//____ _isOpaque() _____________________________________________________________

	bool TileSkin::_isOpaque( State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	bool TileSkin::_isOpaque( const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state ) const
	{
		return m_bStateOpaque[_stateToIndex(state)];
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX TileSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (oldState == newState)
			return RectSPX();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);

		if(m_stateSurfaces[i1] != m_stateSurfaces[i2])
			return canvas;
		
		return StateSkin::_dirtyRect(canvas, scale, newState, oldState, newValue, oldValue, newValue2, oldValue2, 
									newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
	}

	//____ _updateOpaqueFlags() ________________________________________________

	void TileSkin::_updateOpaqueFlags()
	{

		bool bTintDecides = false;

		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_gradient.isValid && !m_gradient.isOpaque())
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Blend)
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
				m_bStateOpaque[i] = m_stateColors[i].a == 4096;
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
