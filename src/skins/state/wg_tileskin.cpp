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

	TileSkin_p TileSkin::create()
	{
		return TileSkin_p(new TileSkin());
	}


	TileSkin_p TileSkin::create(Surface * pSurface)
	{
		if (pSurface == nullptr || !pSurface->isTiling() )
			return nullptr;

		return TileSkin_p( new TileSkin(pSurface) );
	}

	TileSkin_p TileSkin::create(std::initializer_list<std::tuple<State, Surface_p>> stateSurfaces)
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
			m_stateColors[i] = HiColor::White;
	}

	TileSkin::TileSkin(Surface * pSurface)
	{
		m_bOpaque			= pSurface->isOpaque();
		
		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			m_stateSurfaces[i] = pSurface;
			m_stateColors[i] = HiColor::White;
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

	void TileSkin::setSurfaces(std::initializer_list<std::tuple<State, Surface_p>> stateSurfaces)
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

	//____ setColor() __________________________________________________________

	void TileSkin::setColor(HiColor tint)
	{
		m_stateColors[0] = tint;
		m_stateColorMask = 1;

		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void TileSkin::setColor(State state, HiColor tint)
	{
		int i = _stateToIndex(state);

		m_stateColors[i] = tint;
		m_stateColorMask.setBit(i);
		_updateUnsetStateColors();
		_updateOpaqueFlags();
	}

	void TileSkin::setColor(std::initializer_list< std::tuple<State, HiColor> > stateTints)
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

	//____ color() _____________________________________________________________

	HiColor TileSkin::color(State state) const
	{
		return m_stateColors[_stateToIndex(state)];
	}

	//____ setGradient() ______________________________________________________

	void TileSkin::setGradient(const Gradient& gradient)
	{
		m_gradient = gradient;
		m_bGradient = true;
		_updateOpaqueFlags();
	}


	//____ setBlendMode() _____________________________________________________

	void TileSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpaqueFlags();
	}

	//____ render() _______________________________________________________________

	void TileSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, float value, float value2, int animPos, float * pStateFractions) const
	{
		int idx = _stateToIndex(state);

		Surface * pSurf = m_stateSurfaces[idx];

		if( !pSurf )
			return;

		RenderSettingsWithGradient settings(pDevice, m_layer, m_blendMode, m_stateColors[idx], canvas, m_gradient, m_bGradient);

		pDevice->setBlitSource(pSurf);
		pDevice->scaleTile(canvas.px(),MU::scale());
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

	bool TileSkin::markTest( const Coord& _ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		//TODO: Take gradient and tintColor into account.

		Surface * pSurf = m_stateSurfaces[_stateToIndex(state)];

		return markTestTileRect(_ofs, pSurf, canvas, opacityTreshold);
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

	//____ dirtyRect() ______________________________________________________

	Rect TileSkin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (oldState == newState)
			return Rect();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);

		if(m_stateSurfaces[i1] != m_stateSurfaces[i2])
			return canvas;
		
		return StateSkin::dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2, 
									newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
	}

	//____ _updateOpaqueFlags() ________________________________________________

	void TileSkin::_updateOpaqueFlags()
	{

		bool bTintDecides = false;

		if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_bGradient && !m_gradient.isOpaque())
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
