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

#include <wg_fillbarskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	const TypeInfo FillBarSkin::TYPEINFO = { "FillBarSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	FillBarSkin_p FillBarSkin::create(Direction direction, Color barColorEmpty, Color barColorFull, Color backColor, const BorderI& barPadding, const BorderI& contentPadding, bool bBarStartOutside )
	{
		return FillBarSkin_p(new FillBarSkin(direction, barColorEmpty, barColorFull, backColor, barPadding, contentPadding, bBarStartOutside ));
	}

	//____ constructor ____________________________________________________________

	FillBarSkin::FillBarSkin(Direction direction, Color barColorEmpty, Color barColorFull, Color backColor, const BorderI& barPadding, const BorderI& contentPadding, bool bBarStartOutside )
	{
		m_direction = direction;
		m_barColorEmpty = barColorEmpty;
		m_barColorFull = barColorFull;
		m_backColor = backColor;
		m_barPadding = barPadding;
		m_contentPadding = contentPadding;
		m_bBarStartOutside = bBarStartOutside;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FillBarSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isStateIdentical() _________________________________________________

	bool FillBarSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		return true;
	}

	//____ isOpaque() ______________________________________________________________

	bool FillBarSkin::isOpaque() const
	{
		if (!m_barPadding.isEmpty())
			return false;

		return int(m_barColorEmpty.a) + int(m_barColorFull.a) + int(m_backColor.a) == 255*3 ? true : false;
	}

	bool FillBarSkin::isOpaque(State state) const
	{
		if (!m_barPadding.isEmpty())
			return false;

		return int(m_barColorEmpty.a) + int(m_barColorFull.a) + int(m_backColor.a) == 255 * 3 ? true : false;
	}

	//____ isOpaque() ______________________________________________________________

	bool FillBarSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		if (!m_barPadding.isEmpty())
			return false;

		return int(m_barColorEmpty.a) + int(m_barColorFull.a) + int(m_backColor.a) == 255 * 3 ? true : false;
	}

	//____ render() ______________________________________________________________

	void FillBarSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction) const
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

	//____ minSize() ______________________________________________________________

	Size FillBarSkin::minSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ preferredSize() ______________________________________________________________

	Size FillBarSkin::preferredSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ contentPadding() ______________________________________________________________

	Border FillBarSkin::contentPadding(State state) const
	{
		return Border(m_contentPadding).aligned();
	}


	//____ contentPaddingSize() ______________________________________________________________

	Size FillBarSkin::contentPaddingSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ _contentOfs() ______________________________________________________________

	Coord FillBarSkin::contentOfs(State state) const
	{
		return Coord(m_contentPadding.left, m_contentPadding.top).aligned();
	}

	//____ _sizeForContent() ___________________________________________________

	Size FillBarSkin::sizeForContent(const Size& contentSize) const
	{
		return contentSize + Size(Border(m_contentPadding).aligned());
	}

	//____ contentRect() ______________________________________________________

	Rect FillBarSkin::contentRect(const Rect& canvas, State state) const
	{
		return (canvas - Border(m_contentPadding).aligned()).aligned();
	}

	//____ markTest() _________________________________________________________

	bool FillBarSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		if (!canvas.contains(ofs))
			return false;

		if( _barFillArea(canvas, fraction).contains(ofs) )
			return (((int)Color::mix(m_barColorEmpty, m_barColorFull, uint8_t(255*fraction)).a) >= opacityTreshold);

		return (((int)m_backColor.a) >= opacityTreshold);
	}

	//____ ignoresFraction() __________________________________________________

	bool FillBarSkin::ignoresFraction() const
	{
		return false;
	}

	//____ fractionChangeRect() ______________________________________

	Rect FillBarSkin::fractionChangeRect(const Rect& _canvas, State state, float oldFraction, float newFraction) const
	{
		Rect canvas = _canvas - m_contentPadding;

		switch (m_direction)
		{
			case Direction::Up:
			{
				MU ofs1 = canvas.h - canvas.h * oldFraction;
				MU ofs2 = canvas.h - canvas.h * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return { canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1 };
			}

			case Direction::Down:
			{
				MU ofs1 = canvas.h * oldFraction;
				MU ofs2 = canvas.h * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return { canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1 };
			}

			case Direction::Left:
			{
				MU ofs1 = canvas.w - canvas.w * oldFraction;
				MU ofs2 = canvas.w - canvas.w * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return { canvas.x +  ofs1, canvas.y, ofs2 - ofs1, canvas.h };
			}

			case Direction::Right:
			{
				MU ofs1 = canvas.w * oldFraction;
				MU ofs2 = canvas.w * newFraction;
				if (ofs1 > ofs2)
					std::swap(ofs1, ofs2);

				return { canvas.x + ofs1, canvas.y, ofs2 - ofs1, canvas.h };
			}

			default:
				return Rect();			// Just to avoid compiler warnings.
		}
	}

	//____ _barFillArea() _____________________________________________________

	Rect FillBarSkin::_barFillArea(const Rect& _canvas, float fraction) const
	{
		Rect canvas = (_canvas - m_barPadding);

		switch (m_direction)
		{
			case Direction::Up:
			{
				int ofs = canvas.h - canvas.h * fraction;

				if (m_bBarStartOutside)
					return { canvas.x, canvas.y + ofs, canvas.w, _canvas.h - ofs };
				else
					return { canvas.x, canvas.y + ofs, canvas.w, canvas.h - ofs };
			}

			case Direction::Down:
			{
				int ofs = canvas.h * fraction;
				if (m_bBarStartOutside)
					return { canvas.x, _canvas.y, canvas.w, ofs + (canvas.y - _canvas.y) };
				else
					return { canvas.x, canvas.y, canvas.w, ofs };
			}

			case Direction::Left:
			{
				int ofs = canvas.w - canvas.w * fraction;
				if (m_bBarStartOutside)
					return { canvas.x + ofs, canvas.y, _canvas.w - ofs, canvas.h };
				else
					return { canvas.x + ofs, canvas.y, canvas.w - ofs, canvas.h };
			}

			case Direction::Right:
			{
				int ofs = canvas.w * fraction;
				if (m_bBarStartOutside)
					return { _canvas.x, canvas.y, ofs + (canvas.x - _canvas.x), canvas.h };
				else
					return { canvas.x, canvas.y, ofs, canvas.h };
			}

			default:
				return Rect();			// Just to avoid compiler warnings.
		}
	}
	 


} // namespace wg
