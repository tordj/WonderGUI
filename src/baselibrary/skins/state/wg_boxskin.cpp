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

#include <wg_boxskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	using namespace Util;

	const TypeInfo BoxSkin::TYPEINFO = { "BoxSkin", &StateSkin::TYPEINFO };

	//____ create() _______________________________________________________________

	BoxSkin_p BoxSkin::create( const Blueprint& blueprint )
	{
		return BoxSkin_p(new BoxSkin(blueprint));
	}

	BoxSkin_p BoxSkin::create(Border frame, HiColor fillColor, HiColor frameColor, Border contentPadding )
	{
		Blueprint bp;
		bp.frame = frame;
		bp.color = fillColor;
		bp.frameColor = frameColor;
		bp.contentPadding = contentPadding;

		return BoxSkin_p(new BoxSkin(bp));
	}


	//____ constructor ____________________________________________________________

	BoxSkin::BoxSkin( const Blueprint& blueprint )
	{
		m_frame = blueprint.frame;
		m_blendMode = blueprint.blendMode;
		m_contentPadding = blueprint.contentPadding;
		m_layer = blueprint.layer;

		m_fillColor[0] = blueprint.color;
		m_frameColor[0] = blueprint.frameColor;


		for (auto& stateInfo : blueprint.states)
		{
			if (stateInfo.state != StateEnum::Normal)
			{
				int index = _stateToIndex(stateInfo.state);

				if (stateInfo.data.contentShift.x != 0 || stateInfo.data.contentShift.y != 0)
				{
					m_contentShiftStateMask.setBit(index);
					m_contentShift[index] = stateInfo.data.contentShift;
					m_bContentShifting = true;
				}

				if (stateInfo.data.color != HiColor::Undefined)
				{
					m_stateColorMask.setBit(index);
					m_fillColor[index] = stateInfo.data.color;
				}

				if (stateInfo.data.frameColor != HiColor::Undefined)
				{
					m_stateFrameColorMask.setBit(index);
					m_frameColor[index] = stateInfo.data.frameColor;
				}
			}
		}

		_updateContentShift();
		_updateOpaqueFlag();
		_updateUnsetColors();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BoxSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() _______________________________________________________________

	void BoxSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		//TODO: Optimize! Clip patches against canvas first.

		RenderSettings settings(pDevice, m_layer, m_blendMode);

		int i = _stateToIndex(state);

		BorderSPX frame = align(ptsToSpx(m_frame, scale));

		if( frame.width() + frame.height() == 0 || m_frameColor[i] == m_fillColor[i] )
		{
			pDevice->fill( canvas, m_fillColor[i] );
		}
		else
		{
			if (frame.width() >= canvas.w || frame.height() >= canvas.h)
			{
				pDevice->fill(canvas, m_frameColor[i]);
			}
			else
			{
				RectSPX top( canvas.x, canvas.y, canvas.w, frame.top );
				RectSPX left( canvas.x, canvas.y+frame.top, frame.left, canvas.h - frame.height() );
				RectSPX right( canvas.x + canvas.w - frame.right, canvas.y+frame.top, frame.right, canvas.h - frame.height() );
				RectSPX bottom( canvas.x, canvas.y + canvas.h - frame.bottom, canvas.w, frame.bottom );
				RectSPX center( canvas - frame );

				pDevice->fill( top, m_frameColor[i] );
				pDevice->fill( left, m_frameColor[i] );
				pDevice->fill( right, m_frameColor[i] );
				pDevice->fill( bottom, m_frameColor[i] );

				if( center.w > 0 || center.h > 0 )
					pDevice->fill( center, m_fillColor[i] );
			}
		}

	}

	//____ _minSize() ______________________________________________________________

	SizeSPX BoxSkin::_minSize(int scale) const
	{
		SizeSPX content = StateSkin::_minSize(scale);
		SizeSPX frame = align(ptsToSpx(m_frame,scale));

		return SizeSPX::max(content,frame);
	}

	//____ _preferredSize() ________________________________________________________

	SizeSPX BoxSkin::_preferredSize(int scale) const
	{
		SizeSPX content = StateSkin::_minSize(scale);
		SizeSPX frame = align(ptsToSpx(m_frame, scale));

		return SizeSPX::max(content, frame);
	}

	//____ _sizeForContent() _______________________________________________________

	SizeSPX BoxSkin::_sizeForContent( const SizeSPX& contentSize, int scale ) const
	{
		SizeSPX content = StateSkin::_sizeForContent(contentSize,scale);
		SizeSPX frame = align(ptsToSpx(m_frame, scale));

		return SizeSPX::max(content, frame);
	}

	//____ _markTest() _____________________________________________________________

	bool BoxSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		if( !canvas.contains(ofs) )
			return false;

		int opacity;

		if( m_bOpaque )
			opacity = 255;
		else
		{
			int i = _stateToIndex(state);

			RectSPX center = canvas - align(ptsToSpx(m_frame,scale));
			if( center.contains(ofs) )
				opacity = m_fillColor[i].a;
			else
				opacity = m_frameColor[i].a;
		}

		return ( opacity >= opacityTreshold );
	}

	//____ _isOpaque() _____________________________________________________________

	bool BoxSkin::_isOpaque( State state ) const
	{
		int i = _stateToIndex(state);
		if( m_bOpaque || (m_fillColor[i].a == 4096 && (m_frameColor[i].a == 4096 || (m_frame.width() + m_frame.height() == 0))) )
			return true;

		return false;
	}

	bool BoxSkin::_isOpaque( const RectSPX& rect, const SizeSPX& canvasSize, int scale, State state ) const
	{
		if( m_bOpaque )
			return true;

		RectSPX center = RectSPX(canvasSize) - align(ptsToSpx(m_frame,scale));
		int i = _stateToIndex(state);
		if( center.contains(rect) )
			return m_fillColor[i].a == 4096;
		else if( !center.intersectsWith(rect) )
			return m_frameColor[i].a == 4096;

		return m_fillColor[i].a == 4096 && m_frameColor[i].a == 4096;
	}

	//____ _dirtyRect() ______________________________________________________

	RectSPX BoxSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (oldState == newState)
			return RectSPX();

		int i1 = _stateToIndex(newState);
		int i2 = _stateToIndex(oldState);

		if (m_fillColor[i1] != m_fillColor[i2] || (!m_frame.isEmpty() && m_frameColor[i1] != m_frameColor[i2]))
			return canvas;

		return StateSkin::_dirtyRect(canvas, scale, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
	}

	//____ _updateOpaqueFlag() ____________________________________________________

	void BoxSkin::_updateOpaqueFlag()
	{
		switch (m_blendMode)
		{
			case BlendMode::Replace:
				m_bOpaque = true;
				break;

			case BlendMode::Blend:
			{
				int alpha = 0;
				int frameAlpha = 0;

				for (int i = 0; i < StateEnum_Nb; i++)
				{
					alpha += (int)m_fillColor[i].a;
					frameAlpha += (int)m_frameColor[i].a;
				}

				bool hasFrame = (m_frame.width() + m_frame.height() > 0);

				if (alpha == 4096 * StateEnum_Nb && (!hasFrame || frameAlpha == 4096 * StateEnum_Nb))
					m_bOpaque = true;
				else
					m_bOpaque = false;

				break;
			}

			default:
				m_bOpaque = false;
		}
	}

	//____ _updateUnsetColors() _______________________________________________

	void BoxSkin::_updateUnsetColors()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (!m_stateColorMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateColorMask);
				m_fillColor[i] = m_fillColor[bestAlternative];
			}

			if (!m_stateFrameColorMask.bit(i))
			{
				int bestAlternative = bestStateIndexMatch(i, m_stateFrameColorMask);
				m_frameColor[i] = m_frameColor[bestAlternative];
			}
		}
	}


} // namespace wg
