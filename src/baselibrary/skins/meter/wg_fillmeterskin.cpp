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

#include <wg_fillmeterskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo FillMeterSkin::TYPEINFO = { "FillMeterSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() ___________________________________________________________

	FillMeterSkin_p FillMeterSkin::create()
	{
		return FillMeterSkin_p(new FillMeterSkin());
	}

	FillMeterSkin_p FillMeterSkin::create(Direction direction, HiColor barColorEmpty, HiColor barColorFull, HiColor backColor, const Border& barPadding, const Border& contentPadding, bool bBarStartOutside )
	{
		return FillMeterSkin_p(new FillMeterSkin(direction, barColorEmpty, barColorFull, backColor, barPadding, contentPadding, bBarStartOutside ));
	}

	//____ constructor ________________________________________________________

	FillMeterSkin::FillMeterSkin() :
		m_direction(Direction::Right),
		m_bBarStartOutside(false),
		m_barColorEmpty(Color::DarkBlue),
		m_barColorFull(Color::LightBlue),
		m_backColor(Color::Transparent)
	{
		m_preferredSize = Size::max(Size( 50,10 ),m_contentPadding.size());
		m_bIgnoresValue = false;
		_updateOpacity();
	}

	FillMeterSkin::FillMeterSkin(Direction direction, HiColor barColorEmpty, HiColor barColorFull, HiColor backColor, const Border& barPadding, const Border& contentPadding, bool bBarStartOutside ) : 
		m_direction(direction),
		m_barColorEmpty(barColorEmpty),
		m_barColorFull(barColorFull),
		m_backColor(backColor),
		m_barPadding(barPadding),
		m_bBarStartOutside(bBarStartOutside)
	{
		Size pref = (direction == Direction::Up || direction == Direction::Down) ? Size(10, 50) : Size(50, 10);
		m_preferredSize = Size::max(pref, m_contentPadding.size());
		m_bIgnoresValue = false;
		m_contentPadding = contentPadding;
		_updateOpacity();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FillMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPreferredSize() _________________________________________________

	void FillMeterSkin::setPreferredSize(const Size& preferred)
	{
		m_preferredSize = preferred;
	}

	//____ setBlendMode() _____________________________________________________

	void FillMeterSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		_updateOpacity();
	}


	//____ setDirection() _____________________________________________________

	void FillMeterSkin::setDirection(Direction dir)
	{
		m_direction = dir;
	}

	//____ setGfxPadding() ____________________________________________________

	void FillMeterSkin::setGfxPadding(Border padding)
	{
		m_barPadding = padding;
		_updateOpacity();
	}

	//____ setBackColor() _____________________________________________________

	void FillMeterSkin::setBackColor(HiColor back)
	{
		m_backColor = back;
		_updateOpacity();
	}

	//____ setFillColors() ____________________________________________________

	void FillMeterSkin::setFillColors(HiColor empty, HiColor full)
	{
		m_barColorEmpty = empty;
		m_barColorFull = full;
		_updateOpacity();
	}

	//____ setFillColorEmpty() ________________________________________________

	void FillMeterSkin::setFillColorEmpty(HiColor empty)
	{
		m_barColorEmpty = empty;
		_updateOpacity();
	}

	//____ setFillColorFull() ________________________________________________

	void FillMeterSkin::setFillColorFull(HiColor full)
	{
		m_barColorFull = full;
		_updateOpacity();
	}

	//____ setFillStartOutside() ________________________________________________

	void FillMeterSkin::setFillStartOutside(bool bStartOutside)
	{
		m_bBarStartOutside = bStartOutside;
	}

	//____ _preferredSize() ____________________________________________________

	SizeSPX FillMeterSkin::_preferredSize(int scale) const
	{
		if (!m_preferredSize.isEmpty())
			return ptsToSpx(m_preferredSize, scale);
		else
			return _minSize(scale);
	}

	//____ _render() ______________________________________________________________

	void FillMeterSkin::_render(GfxDevice * pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		HiColor barColor = HiColor::mix(m_barColorEmpty, m_barColorFull, int(4096 * value));

		RectSPX barCanvas = _barFillArea(_canvas, scale, value, value2);
		pDevice->fill(barCanvas, barColor);

		BorderSPX	barPadding = align(ptsToSpx(m_barPadding, scale));

		if (m_backColor.a != 0)
		{
			RectSPX backCanvas = (_canvas - barPadding);
			RectSPX backCanvas1 = backCanvas;
			RectSPX backCanvas2 = backCanvas;

			switch (m_direction)
			{
			case Direction::Up:
			case Direction::Down:
			{
				backCanvas1.h = barCanvas.y - backCanvas.y;
	
				backCanvas2.h = backCanvas.bottom() - barCanvas.bottom();
				backCanvas2.y = barCanvas.bottom();
				break;
			}

			case Direction::Left:
			case Direction::Right:
			{
				backCanvas1.w = barCanvas.x - backCanvas.x;

				backCanvas2.w = backCanvas.right() - barCanvas.right();
				backCanvas2.x = barCanvas.right();
				break;
			}
			}

			pDevice->fill(backCanvas1, m_backColor);
			pDevice->fill(backCanvas2, m_backColor);
		}
	}

	//____ _markTest() _________________________________________________________

	bool FillMeterSkin::_markTest(const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		if (!canvas.contains(ofs))
			return false;

		if( _barFillArea(canvas, scale, value, value2).contains(ofs) )
			return (HiColor::mix(m_barColorEmpty, m_barColorFull, int(4096*value)).a >= opacityTreshold);

		return (m_backColor.a >= opacityTreshold);
	}

	//____ _dirtyRect() ________________________________________________________

	RectSPX FillMeterSkin::_dirtyRect(const RectSPX& _canvas, int scale, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (newValue == oldValue)
			return RectSPX();

		if (m_barColorFull != m_barColorEmpty)
			return _canvas;

		RectSPX canvas = _canvas - align(ptsToSpx(m_contentPadding, scale));

		RectSPX result1;
		RectSPX result2;

		if (oldValue != newValue)
			result1 = _valueChangeRect(canvas, newState, oldValue, newValue);

		if (oldValue2 != newValue2)
			result2 = _valueChangeRect(canvas, newState, oldValue2, newValue2);

		if (result2.isEmpty())
			return result1;

		if (result1.isEmpty())
			return result2;

		return RectSPX::getUnion(result1, result2);
	}


	RectSPX FillMeterSkin::_valueChangeRect(const RectSPX& canvas, State state, float oldFraction, float newFraction ) const
	{
		switch (m_direction)
		{
		case Direction::Up:
		{
			spx ofs1 = align(canvas.h - canvas.h * oldFraction);
			spx ofs2 = align(canvas.h - canvas.h * newFraction);
			if (ofs1 > ofs2)
				std::swap(ofs1, ofs2);

			return RectSPX(canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1);
		}

		case Direction::Down:
		{
			spx ofs1 = align(canvas.h * oldFraction);
			spx ofs2 = align(canvas.h * newFraction);
			if (ofs1 > ofs2)
				std::swap(ofs1, ofs2);

			return RectSPX(canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1);
		}

		case Direction::Left:
		{
			spx ofs1 = align(canvas.w - canvas.w * oldFraction);
			spx ofs2 = align(canvas.w - canvas.w * newFraction);
			if (ofs1 > ofs2)
				std::swap(ofs1, ofs2);

			return RectSPX(canvas.x + ofs1, canvas.y, ofs2 - ofs1, canvas.h);
		}

		case Direction::Right:
		{
			spx ofs1 = canvas.w * oldFraction;
			spx ofs2 = canvas.w * newFraction;
			if (ofs1 > ofs2)
				std::swap(ofs1, ofs2);

			return RectSPX(canvas.x + ofs1, canvas.y, ofs2 - ofs1, canvas.h);
		}

		default:
			return RectSPX();			// Just to avoid compiler warnings.
		}
	}



	//____ _barFillArea() _____________________________________________________

	RectSPX FillMeterSkin::_barFillArea(const RectSPX& _canvas, int scale, float value, float value2) const
	{
		RectSPX canvas = (_canvas - align(ptsToSpx(m_barPadding,scale)));

		float beg = value2 >= 0.f ? value : 0.f;
		float end = value2 >= 0.f ? value2 : value;

		bool bStartOutside = m_bBarStartOutside && beg == 0.f;

		switch (m_direction)
		{
			case Direction::Up:
			{
				spx ofs = align(canvas.h - canvas.h * end);
				spx len = align(canvas.h * (end-beg));

				if (bStartOutside)
					return { canvas.x, canvas.y + ofs, canvas.w, _canvas.h - ofs };
				else
					return { canvas.x, canvas.y + ofs, canvas.w, len };
			}

			case Direction::Down:
			{
				spx ofs = align(canvas.h * beg);
				spx len = align(canvas.h * (end - beg));

				if (bStartOutside)
					return { canvas.x, _canvas.y, canvas.w, len + (canvas.y - _canvas.y) };
				else
					return { canvas.x, canvas.y + ofs, canvas.w, len };
			}

			case Direction::Left:
			{
				spx ofs = align(canvas.w - canvas.w * end);
				spx len = align(canvas.w * (end - beg));

				if (bStartOutside)
					return { canvas.x + ofs, canvas.y, _canvas.w - ofs, canvas.h };
				else
					return { canvas.x + ofs, canvas.y, len, canvas.h };
			}

			case Direction::Right:
			{
				spx ofs = align(canvas.w * beg);
				spx len = align(canvas.w * (end - beg));

				if (bStartOutside)
					return { _canvas.x, canvas.y, len + (canvas.x - _canvas.x), canvas.h };
				else
					return { canvas.x + ofs, canvas.y, len, canvas.h };
			}

			default:
				return RectSPX();			// Just to avoid compiler warnings.
		}
	}
	 
	//____ _updateOpacity() ______________________________________________________________

	void FillMeterSkin::_updateOpacity()
	{
		if (!m_barPadding.isEmpty())
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode != BlendMode::Blend)
			m_bOpaque = false;
		else
			m_bOpaque = int(m_barColorEmpty.a) + int(m_barColorFull.a) + int(m_backColor.a) == 4096 * 3 ? true : false;
	}


} // namespace wg
