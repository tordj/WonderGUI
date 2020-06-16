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

namespace wg
{

	const TypeInfo FillMeterSkin::TYPEINFO = { "FillMeterSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	FillMeterSkin_p FillMeterSkin::create()
	{
		return FillMeterSkin_p(new FillMeterSkin());
	}

	FillMeterSkin_p FillMeterSkin::create(Direction direction, Color barColorEmpty, Color barColorFull, Color backColor, const BorderI& barPadding, const BorderI& contentPadding, bool bBarStartOutside )
	{
		return FillMeterSkin_p(new FillMeterSkin(direction, barColorEmpty, barColorFull, backColor, barPadding, contentPadding, bBarStartOutside ));
	}

	//____ constructor ____________________________________________________________

	FillMeterSkin::FillMeterSkin() :
		m_direction(Direction::Right),
		m_bBarStartOutside(false),
		m_barColorEmpty(Color::DarkBlue),
		m_barColorFull(Color::LightBlue),
		m_backColor(Color::Transparent)
	{
		m_preferredSize = SizeI::max(SizeI( 50,10 ),m_contentPadding.size());
		m_bIgnoresFraction = false;
		_updateOpacity();
	}

	FillMeterSkin::FillMeterSkin(Direction direction, Color barColorEmpty, Color barColorFull, Color backColor, const BorderI& barPadding, const BorderI& contentPadding, bool bBarStartOutside ) : 
		m_direction(direction),
		m_barColorEmpty(barColorEmpty),
		m_barColorFull(barColorFull),
		m_backColor(backColor),
		m_barPadding(barPadding),
		m_bBarStartOutside(bBarStartOutside)
	{
		SizeI pref = (direction == Direction::Up || direction == Direction::Down) ? SizeI(10, 50) : SizeI(50, 10);
		m_preferredSize = SizeI::max(pref, m_contentPadding.size());
		m_bIgnoresFraction = false;
		m_contentPadding = contentPadding;
		_updateOpacity();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FillMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() ______________________________________________________________

	Size FillMeterSkin::preferredSize() const
	{
		if (!m_preferredSize.isEmpty())
			return m_preferredSize;
		else
			return minSize();
	}

	//____ setPreferredSize() _________________________________________________

	void FillMeterSkin::setPreferredSize(const SizeI& preferred)
	{
		m_preferredSize = preferred;
	}

	//____ setDirection() _____________________________________________________

	void FillMeterSkin::setDirection(Direction dir)
	{
		m_direction = dir;
	}

	//____ setGfxPadding() ____________________________________________________

	void FillMeterSkin::setGfxPadding(BorderI padding)
	{
		m_barPadding = padding;
		_updateOpacity();
	}

	//____ setBackColor() _____________________________________________________

	void FillMeterSkin::setBackColor(Color back)
	{
		m_backColor = back;
		_updateOpacity();
	}

	//____ setFillColors() ____________________________________________________

	void FillMeterSkin::setFillColors(Color empty, Color full)
	{
		m_barColorEmpty = empty;
		m_barColorFull = full;
		_updateOpacity();
	}

	//____ setFillColorEmpty() ________________________________________________

	void FillMeterSkin::setFillColorEmpty(Color empty)
	{
		m_barColorEmpty = empty;
		_updateOpacity();
	}

	//____ setFillColorFull() ________________________________________________

	void FillMeterSkin::setFillColorFull(Color full)
	{
		m_barColorFull = full;
		_updateOpacity();
	}

	//____ setFillStartOutside() ________________________________________________

	void FillMeterSkin::setFillStartOutside(bool bStartOutside)
	{
		m_bBarStartOutside = bStartOutside;
	}

	//____ render() ______________________________________________________________

	void FillMeterSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction) const
	{
		RectI barCanvas = _barFillArea(_canvas,fraction).px();
		RectI backCanvas = (_canvas - m_barPadding).px();

		switch (m_direction)
		{
			case Direction::Up:
			{
				backCanvas.h = barCanvas.y - backCanvas.y;
				break;
			}

			case Direction::Down:
			{
				backCanvas.h = backCanvas.bottom() - barCanvas.bottom();
				backCanvas.y = barCanvas.bottom();
				break;
			}

			case Direction::Left:
			{
				backCanvas.w = barCanvas.x - backCanvas.x;
				break;
			}

			case Direction::Right:
			{
				backCanvas.w = backCanvas.right() - barCanvas.right();
				backCanvas.x = barCanvas.right();
				break;
			}
		}

		Color barColor = Color::mix(m_barColorEmpty, m_barColorFull, uint8_t(255 * fraction));

		pDevice->fill(barCanvas, barColor);
		pDevice->fill(backCanvas, m_backColor);
	}

	//____ markTest() _________________________________________________________

	bool FillMeterSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		if (!canvas.contains(ofs))
			return false;

		if( _barFillArea(canvas, fraction).contains(ofs) )
			return (((int)Color::mix(m_barColorEmpty, m_barColorFull, uint8_t(255*fraction)).a) >= opacityTreshold);

		return (((int)m_backColor.a) >= opacityTreshold);
	}

	//____ fractionChangeRect() ______________________________________

	Rect FillMeterSkin::fractionChangeRect(const Rect& _canvas, State state, float oldFraction, float newFraction) const
	{
		if (m_barColorFull != m_barColorEmpty)
			return _canvas;

		Rect canvas = _canvas - m_contentPadding;

		switch (m_direction)
		{
			case Direction::Up:
			{
				MU ofs1 = canvas.h - canvas.h * oldFraction;
				MU ofs2 = canvas.h - canvas.h * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return Rect( canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1 ).aligned();
			}

			case Direction::Down:
			{
				MU ofs1 = canvas.h * oldFraction;
				MU ofs2 = canvas.h * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return Rect(canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1).aligned();
			}

			case Direction::Left:
			{
				MU ofs1 = canvas.w - canvas.w * oldFraction;
				MU ofs2 = canvas.w - canvas.w * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return Rect( canvas.x +  ofs1, canvas.y, ofs2 - ofs1, canvas.h ).aligned();
			}

			case Direction::Right:
			{
				MU ofs1 = canvas.w * oldFraction;
				MU ofs2 = canvas.w * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return Rect( canvas.x + ofs1, canvas.y, ofs2 - ofs1, canvas.h ).aligned();
			}

			default:
				return Rect();			// Just to avoid compiler warnings.
		}
	}

	//____ _barFillArea() _____________________________________________________

	Rect FillMeterSkin::_barFillArea(const Rect& _canvas, float fraction) const
	{
		Rect canvas = (_canvas - m_barPadding);

		switch (m_direction)
		{
			case Direction::Up:
			{
				MU ofs = canvas.h - canvas.h * fraction;

				if (m_bBarStartOutside)
					return { canvas.x, canvas.y + ofs, canvas.w, _canvas.h - ofs };
				else
					return { canvas.x, canvas.y + ofs, canvas.w, canvas.h - ofs };
			}

			case Direction::Down:
			{
				MU ofs = canvas.h * fraction;
				if (m_bBarStartOutside)
					return { canvas.x, _canvas.y, canvas.w, ofs + (canvas.y - _canvas.y) };
				else
					return { canvas.x, canvas.y, canvas.w, ofs };
			}

			case Direction::Left:
			{
				MU ofs = canvas.w - canvas.w * fraction;
				if (m_bBarStartOutside)
					return { canvas.x + ofs, canvas.y, _canvas.w - ofs, canvas.h };
				else
					return { canvas.x + ofs, canvas.y, canvas.w - ofs, canvas.h };
			}

			case Direction::Right:
			{
				MU ofs = canvas.w * fraction;
				if (m_bBarStartOutside)
					return { _canvas.x, canvas.y, ofs + (canvas.x - _canvas.x), canvas.h };
				else
					return { canvas.x, canvas.y, ofs, canvas.h };
			}

			default:
				return Rect();			// Just to avoid compiler warnings.
		}
	}
	 
	//____ _updateOpacity() ______________________________________________________________

	void FillMeterSkin::_updateOpacity()
	{
		if (!m_barPadding.isEmpty())
			m_bOpaque = false;
		else
			m_bOpaque = int(m_barColorEmpty.a) + int(m_barColorFull.a) + int(m_backColor.a) == 255 * 3 ? true : false;
	}


} // namespace wg