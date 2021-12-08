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

	FillMeterSkin_p FillMeterSkin::create(Direction direction, HiColor barColorEmpty, HiColor barColorFull, HiColor backColor, const BorderI& barPadding, const BorderI& contentPadding, int minFillLength )
	{
		return FillMeterSkin_p(new FillMeterSkin(direction, barColorEmpty, barColorFull, backColor, barPadding, contentPadding, minFillLength ));
	}

	//____ constructor ________________________________________________________

	FillMeterSkin::FillMeterSkin() :
		m_direction(Direction::Right),
		m_minFillLength(0),
		m_barColorEmpty(Color::DarkBlue),
		m_barColorFull(Color::LightBlue),
		m_backColor(Color::Transparent)
	{
		m_preferredSize = SizeI::max(SizeI( 50,10 ),m_contentPadding.size());
		m_bIgnoresValue = false;
		_updateOpacity();
	}

	FillMeterSkin::FillMeterSkin(Direction direction, HiColor barColorEmpty, HiColor barColorFull, HiColor backColor, const BorderI& barPadding, const BorderI& contentPadding, int minFillLength ) :
		m_direction(direction),
		m_barColorEmpty(barColorEmpty),
		m_barColorFull(barColorFull),
		m_backColor(backColor),
		m_barPadding(barPadding),
		m_minFillLength(minFillLength)
	{
		SizeI pref = (direction == Direction::Up || direction == Direction::Down) ? SizeI(10, 50) : SizeI(50, 10);
		m_preferredSize = SizeI::max(pref, m_contentPadding.size());
		m_bIgnoresValue = false;
		m_contentPadding = contentPadding;
		_updateOpacity();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FillMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() ____________________________________________________

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

	void FillMeterSkin::setGfxPadding(BorderI padding)
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

	//____ setMinFillLength() ________________________________________________

	void FillMeterSkin::setMinFillLength(int minFillLength)
	{
		m_minFillLength = minFillLength;
	}

    //____ setCenteredBarOrigin() ____________________________________________

    void FillMeterSkin::setCenteredBarOrigin(bool bCenter)
    {
        m_bCenteredBarOrigin = bCenter;
    }



	//____ render() ______________________________________________________________

	void FillMeterSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		HiColor barColor = HiColor::mix(m_barColorEmpty, m_barColorFull, int(4096 * value));

		RectI barCanvas = _barFillArea(_canvas,value, value2).px();
		pDevice->fill(barCanvas, barColor);

		if (m_backColor.a != 0)
		{
			RectI backCanvas = (_canvas - m_barPadding).px();
			RectI backCanvas1 = backCanvas;
			RectI backCanvas2 = backCanvas;

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

	//____ markTest() _________________________________________________________

	bool FillMeterSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		if (!canvas.contains(ofs))
			return false;

		if( _barFillArea(canvas, value, value2).contains(ofs) )
			return (((int)HiColor::mix(m_barColorEmpty, m_barColorFull, int(4096*value)).a*255/4096) >= opacityTreshold);

		return (((int)m_backColor.a) >= opacityTreshold);
	}

	//____ dirtyRect() ________________________________________________________

	Rect FillMeterSkin::dirtyRect(const Rect& _canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (newValue == oldValue)
			return Rect();

		if (m_barColorFull != m_barColorEmpty)
			return _canvas;

		Rect canvas = _canvas - m_contentPadding;

		Rect result1;
		Rect result2;

		if (oldValue != newValue)
			result1 = _valueChangeRect(canvas, newState, oldValue, newValue);

		if (oldValue2 != newValue2)
			result2 = _valueChangeRect(canvas, newState, oldValue2, newValue2);

		if (result2.isEmpty())
			return result1;

		if (result1.isEmpty())
			return result2;

		return Rect::getUnion(result1, result2);
	}


	Rect FillMeterSkin::_valueChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction ) const
	{
        
        if( m_bCenteredBarOrigin )
        {
            // If we start from center we ignore value2.

            bool bHorizontal = ( m_direction == Direction::Left || m_direction == Direction::Right );
            bool bInverted = ( m_direction == Direction::Left || m_direction == Direction::Up );
            
            //
            
            MU totalLen = bHorizontal ? canvas.w : canvas.h;
            
            MU sideLen = totalLen - m_minFillLength / 2;
            MU centerLen = totalLen - sideLen * 2;
            
            // Invert value if direction is inverted
            
            if( bInverted )
            {
                oldFraction = 1.f - oldFraction;
                newFraction = 1.f - newFraction;
            }
            
            // Calculate offsets.
            
            MU ofs1, ofs2;
            
            if( oldFraction < 0.5f )
                ofs1 = oldFraction / 2.f * sideLen;
            else
                ofs1 = sideLen + centerLen + (oldFraction-0.5f) * 2.f * sideLen;

            if( newFraction < 0.5f )
                ofs2 = newFraction / 2.f * sideLen;
            else
                ofs2 = sideLen + centerLen + (newFraction-0.5f) * 2.f * sideLen;

            if (ofs1 > ofs2)
                std::swap(ofs1, ofs2);

            // Convert ofs & len to a rectangle.
            
            if( bHorizontal )
                return Rect(canvas.x + ofs1, canvas.y, ofs2 - ofs1, canvas.h).aligned();
            else
                return Rect(canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1).aligned();
        }
        else
        {
            switch (m_direction)
            {
                case Direction::Up:
                {
                    MU len = canvas.h - m_minFillLength;
                    MU ofs1 = len - len * oldFraction;
                    MU ofs2 = len - len * newFraction;
                    if (ofs1 > ofs2)
                        std::swap(ofs1, ofs2);

                    return Rect(canvas.x, canvas.y + ofs1, canvas.w, ofs2 - ofs1).aligned();
                }

                case Direction::Down:
                {
                    MU len = canvas.h - m_minFillLength;
                    MU ofs1 = len * oldFraction;
                    MU ofs2 = len * newFraction;
                    if (ofs1 > ofs2)
                        std::swap(ofs1, ofs2);

                    return Rect(canvas.x, canvas.y + m_minFillLength + ofs1, canvas.w, ofs2 - ofs1).aligned();
                }

                case Direction::Left:
                {
                    MU len = canvas.w - m_minFillLength;
                    MU ofs1 = len - len * oldFraction;
                    MU ofs2 = len - len * newFraction;
                    if (ofs1 > ofs2)
                        std::swap(ofs1, ofs2);

                    return Rect(canvas.x + ofs1, canvas.y, ofs2 - ofs1, canvas.h).aligned();
                }

                case Direction::Right:
                {
                    MU len = canvas.w - m_minFillLength;
                    MU ofs1 = len * oldFraction;
                    MU ofs2 = len * newFraction;
                    if (ofs1 > ofs2)
                        std::swap(ofs1, ofs2);

                    return Rect(canvas.x + m_minFillLength, canvas.y, ofs2 - ofs1, canvas.h).aligned();
                }

                default:
                    return Rect();			// Just to avoid compiler warnings.
            }
		}
	}



	//____ _barFillArea() _____________________________________________________

	Rect FillMeterSkin::_barFillArea(const Rect& _canvas, float value, float value2) const
	{
		Rect canvas = (_canvas - m_barPadding);

        if( m_bCenteredBarOrigin )
        {
            // If we start from center we ignore value2.

            bool bHorizontal = ( m_direction == Direction::Left || m_direction == Direction::Right );
            bool bInverted = ( m_direction == Direction::Left || m_direction == Direction::Up );
            
            //
            
            MU totalLen =  bHorizontal ? canvas.w : canvas.h;
            
            MU sideLen = totalLen - m_minFillLength / 2;
            MU centerLen = totalLen - sideLen * 2;
            
            // Invert value if direction is inverted
            
            if( bInverted )
                value = 1.f - value;
            
            // Calculate offset and length of bar.
            
            MU ofs, len;
            
            if( value < 0.5f )
            {
                ofs = value / 2.f * sideLen;
                len = sideLen + centerLen - ofs;
            }
            else
            {
                ofs = sideLen;
                len = centerLen + (value-0.5f) * 2.f * sideLen;
            }
    
            // Convert ofs & len to a rectangle.
            
            if( bHorizontal )
                return { canvas.x + ofs, canvas.y, len, canvas.h };
            else
                return { canvas.x, canvas.y + ofs, canvas.w, len };

        }
        else
        {
            float beg = value2 >= 0.f ? value : 0.f;
            float end = value2 >= 0.f ? value2 : value;

            switch (m_direction)
            {
                case Direction::Up:
                {
                    MU ofs = (canvas.h - m_minFillLength) - (canvas.h - m_minFillLength) * end;
                    MU len = (canvas.h - m_minFillLength) * (end-beg);

                    return { canvas.x, canvas.y + ofs, canvas.w, len + m_minFillLength };
                }

                case Direction::Down:
                {
                    MU ofs = (canvas.h - m_minFillLength) * beg;
                    MU len = (canvas.h - m_minFillLength) * (end - beg);

                    return { canvas.x, canvas.y + ofs, canvas.w, len + m_minFillLength };
                }

                case Direction::Left:
                {
                    MU ofs = (canvas.w - m_minFillLength) - (canvas.w - m_minFillLength) * end;
                    MU len = (canvas.w - m_minFillLength) * (end - beg);

                    return { canvas.x + ofs, canvas.y, len + m_minFillLength, canvas.h };
                }

                case Direction::Right:
                {
                    MU ofs = (canvas.w - m_minFillLength) * beg;
                    MU len = (canvas.w - m_minFillLength) * (end - beg);

                    return { canvas.x + ofs, canvas.y, len + m_minFillLength, canvas.h };
                }

                default:
                    return Rect();            // Just to avoid compiler warnings.
            }
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
