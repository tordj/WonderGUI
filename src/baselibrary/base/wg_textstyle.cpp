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

#include <wg_textstyle.h>
#include <wg_textstylemanager.h>
#include <wg_base.h>

namespace wg
{
	using namespace Util;

	const TypeInfo TextStyle::TYPEINFO = { "TextStyle", &Object::TYPEINFO };


    //____ create() ________________________________________________________________

    TextStyle_p TextStyle::create()
    {
        return TextStyle_p(new TextStyle());
    }


	//____ constructor _____________________________________________________________

	TextStyle::TextStyle()
	{
		m_handle = TextStyleManager::_reserveHandle(this);

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			m_size[i]			= 0;
			m_color[i]			= Color::Black;
			m_bgColor[i]		= Color::Transparent;
			m_decoration[i]		= TextDecoration::Undefined;
		}
	}


	//____ Destructor ______________________________________________________________

	TextStyle::~TextStyle()
	{
		TextStyleManager::_releaseHandle(m_handle);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& TextStyle::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setFont() _______________________________________________________________

	void TextStyle::setFont( Font * pFont )
	{
		m_pFont = pFont;
	}

	//____ setLink() _______________________________________________________________

	void TextStyle::setLink( TextLink * pLink )
	{
		m_pLink = pLink;
	}

	//____ setRenderMode() _________________________________________________________

	void TextStyle::setRenderMode(BlendMode mode)
	{
		m_renderMode = mode;
	}

	//____ setBgRenderMode() _________________________________________________________

	void TextStyle::setBgRenderMode(BlendMode mode)
	{
		m_bgRenderMode = mode;
	}

	//____ setColor() ______________________________________________________________

	void TextStyle::setColor( HiColor color )
	{
		for (int i = 0; i < StateEnum_Nb; i++)
			m_color[i] = color;

		m_colorSetMask = 1;
        m_colorDefinedMask = 0xFFFFFFFF;
		m_bStaticColor = true;
	}

	void TextStyle::setColor( HiColor color, State state )
	{
		int i = Util::_stateToIndex(state);

		m_color[i] = color;
		m_colorSetMask.setBit(i);

		_refreshColor();
	}


	//____ setBgColor() ______________________________________________________________

	void TextStyle::setBgColor( HiColor color )
	{
		for (int i = 0; i < StateEnum_Nb; i++)
			m_bgColor[i] = color;

		m_bgColorSetMask = 1;
        m_bgColorDefinedMask = 0xFFFFFFFF;
		m_bStaticBgColor = true;
	}

	void TextStyle::setBgColor( HiColor color, State state )
	{
		int i = Util::_stateToIndex(state);

		m_bgColor[i] = color;
		m_bgColorSetMask.setBit(i);

		_refreshBgColor();
	}

	//____ setSize() _______________________________________________________________

	void TextStyle::setSize( pts size )
	{
		for (int i = 0; i < StateEnum_Nb; i++)
			m_size[i] = size;

		m_sizeSetMask = size == 0 ? 0 : 1;
		m_bStaticSize = true;
	}

	void TextStyle::setSize( pts size, State state )
	{
		int i = Util::_stateToIndex(state);

		m_size[i] = size;
		m_sizeSetMask.setBit(i, size != 0);

		_refreshSize();
	}


	//____ setDecoration() _________________________________________________________

	void TextStyle::setDecoration( TextDecoration decoration )
	{
		for (int i = 1; i < StateEnum_Nb; i++)
			m_decoration[i] = decoration;

		m_decorationSetMask = decoration == TextDecoration::Undefined ? 0 : 1;
		m_bStaticDecoration = true;
	}

	void TextStyle::setDecoration( TextDecoration decoration, State state )
	{
		int i = Util::_stateToIndex(state);

		m_decoration[i] = decoration;
		m_decorationSetMask.setBit(i,decoration != TextDecoration::Undefined);

		_refreshDecoration();
	}

	//____ clearColor() __________________________________________________________

	void TextStyle::clearColor()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
			m_color[i] = Color::Black;

		m_colorSetMask = 0;
        m_colorDefinedMask = 0;
		m_bStaticColor = true;
	}

	void TextStyle::clearColor( State state)
	{
		int i = Util::_stateToIndex(state);

		m_color[i] = Color::Black;
		m_colorSetMask.clearBit(i);

		_refreshColor();
	}

	//____ clearBgColor() __________________________________________________________

	void TextStyle::clearBgColor()
	{
		for (int i = 0; i < StateEnum_Nb; i++)
			m_bgColor[i] = Color::Transparent;

		m_bgColorSetMask = 0;
        m_bgColorDefinedMask = 0;
		m_bStaticBgColor = true;
	}

	void TextStyle::clearBgColor(State state)
	{
		int i = Util::_stateToIndex(state);

		m_bgColor[i] = Color::Transparent;
		m_bgColorSetMask.clearBit(i);

		_refreshBgColor();
	}


	//____ exportAttr() ____________________________________________________________

	void TextStyle::exportAttr( State state, TextAttr * pDest, int scale ) const
	{
		int idx = Util::_stateToIndex(state);

		pDest->pFont 		= m_pFont;
		pDest->pLink 		= m_pLink;
		pDest->renderMode	= m_renderMode;
		pDest->bgRenderMode = m_bgRenderMode;

		pDest->size 		= ptsToSpx(m_size[idx],scale);
		pDest->color		= m_color[idx];
		pDest->bgColor		= m_bgColor[idx];
		pDest->decoration	= m_decoration[idx];

		if( pDest->size == 0 )
			pDest->size = ptsToSpx(12,scale);				// Default to size 12.

		if( pDest->decoration == TextDecoration::Undefined )
			pDest->decoration = TextDecoration::None;

		if (pDest->renderMode == BlendMode::Undefined)
			pDest->renderMode = BlendMode::Blend;			// Default to Blend.
	}

	//____ addToAttr() _____________________________________________________________

	void TextStyle::addToAttr( State state, TextAttr * pDest, int scale ) const
	{
		int idx = Util::_stateToIndex(state);

		if( m_pFont )
			pDest->pFont = m_pFont;
		if( m_pLink )
			pDest->pLink = m_pLink;
		if (m_renderMode != BlendMode::Undefined)
			pDest->renderMode = m_renderMode;
		if (m_bgRenderMode != BlendMode::Undefined)
			pDest->bgRenderMode = m_bgRenderMode;

		if( m_size[idx] != 0 )
			pDest->size	= ptsToSpx(m_size[idx],scale);
		if( m_decoration[idx] != TextDecoration::Undefined )
			pDest->decoration = m_decoration[idx];

		if( m_colorDefinedMask.bit(idx) )
			pDest->color = m_color[idx];

		if (m_bgColorDefinedMask.bit(idx))
			pDest->bgColor = m_bgColor[idx];
	}

    //____ isStateIdentical() ________________________________________________

    bool TextStyle::isStateIdentical( State state1, State state2 ) const
    {
        int idx1 = Util::_stateToIndex(state1);
        int idx2 = Util::_stateToIndex(state2);

        return ((m_size[idx1] == m_size[idx2]) &&
                (m_color[idx1] == m_color[idx2]) &&
                (m_bgColor[idx1] == m_bgColor[idx2]) &&
                (m_decoration[idx1] == m_decoration[idx2]));
    }

	//____ isIdentical() _____________________________________________________

	bool TextStyle::isIdentical( TextStyle * pOther )
	{
		if (m_pFont != pOther->m_pFont || m_pLink != pOther->m_pLink ||
			m_renderMode != pOther->m_renderMode || m_bgRenderMode != pOther->m_bgRenderMode)
			return false;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			if (m_size[i] != pOther->m_size[i] ||
				m_decoration[i] != pOther->m_decoration[i] ||
				m_color[i] != pOther->m_color[i] ||
				m_bgColor[i] != pOther->m_bgColor[i])
				return false;
		}

		return true;
	}

	//____ isIdenticalForState() _____________________________________________

	bool TextStyle::isIdenticalForState( TextStyle * pOther, State state )
	{
		if (m_pFont != pOther->m_pFont || m_pLink != pOther->m_pLink ||
			m_renderMode != pOther->m_renderMode || m_bgRenderMode != pOther->m_bgRenderMode)
			return false;

		int i = Util::_stateToIndex(state);

		if (m_size[i] != pOther->m_size[i] ||
			m_decoration[i] != pOther->m_decoration[i] ||
			m_color[i] != pOther->m_color[i] ||
			m_bgColor[i] != pOther->m_bgColor[i])
			return false;

		return true;
	}

	//____ clone() ____________________________________________________________

	TextStyle_p TextStyle::clone() const
	{
		auto p = create();

		p->m_pFont = m_pFont;
		p->m_pLink = m_pLink;
		p->m_renderMode = m_renderMode;
		p->m_bgRenderMode = m_bgRenderMode;

		for (int i = 0; i < StateEnum_Nb; i++)
		{
			p->m_size[i] = m_size[i];
			p->m_color[i] = m_color[i];
			p->m_bgColor[i] = m_bgColor[i];
			p->m_decoration[i] = m_decoration[i];
		}

		p->m_sizeSetMask = m_sizeSetMask;
		p->m_colorSetMask = m_colorSetMask;
		p->m_bgColorSetMask = m_bgColorSetMask;
		p->m_decorationSetMask = m_decorationSetMask;

		p->m_colorDefinedMask = m_colorDefinedMask;
		p->m_bgColorDefinedMask = m_bgColorDefinedMask;

		p->m_bStaticColor = m_bStaticColor;
		p->m_bStaticBgColor = m_bStaticBgColor;
		p->m_bStaticSize = m_bStaticSize;
		p->m_bStaticDecoration = m_bStaticDecoration;

		return p;
	}

	//____ _refreshSize() _____________________________________________________

	void TextStyle::_refreshSize()
	{
		Bitmask<uint32_t> mask = m_sizeSetMask;
		mask.setBit(0);
		for (int i = 1; i < StateEnum_Nb; i++)
		{
			if (!mask.bit(i))
				m_size[i] = m_size[bestStateIndexMatch(i, mask)];
		}

		//

		auto x = m_size[0];
		for (int i = 1; i < StateEnum_Nb; i++)
			if (m_size[i] != x)
			{
				m_bStaticSize = false;
				return;
			}

		m_bStaticSize = true;
		return;
	}

	//____ _refreshColor() _____________________________________________________

	void TextStyle::_refreshColor()
	{
		Bitmask<uint32_t> defined = m_colorSetMask;
		Bitmask<uint32_t> mask = m_colorSetMask;
		mask.setBit(0);

		for (int i = 1; i < StateEnum_Nb; i++)
		{
			if (!mask.bit(i))
			{
				int idx = bestStateIndexMatch(i, mask);
				m_color[i] = m_color[idx];
				defined.setBit(i, m_colorSetMask.bit(idx));		// Set defined if we copy from a defined color.
			}
		}

		m_colorDefinedMask = defined;

		//

		auto x = m_color[0];
		for (int i = 1; i < StateEnum_Nb; i++)
			if (m_color[i] != x)
			{
				m_bStaticColor = false;
				return;
			}

		m_bStaticColor = true;
		return;
	}

	//____ _refreshBgColor() _____________________________________________________

	void TextStyle::_refreshBgColor()
	{
		Bitmask<uint32_t> defined = m_bgColorSetMask;
		Bitmask<uint32_t> mask = m_bgColorSetMask;
		mask.setBit(0);

		for (int i = 1; i < StateEnum_Nb; i++)
		{
			if (!mask.bit(i))
			{
				int idx = bestStateIndexMatch(i, mask);
				m_bgColor[i] = m_bgColor[idx];
				defined.setBit(i, m_bgColorSetMask.bit(idx));		// Set defined if we copy from a defined color.
			}
		}

		m_bgColorDefinedMask = defined;

		//

		auto x = m_bgColor[0];
		for (int i = 1; i < StateEnum_Nb; i++)
			if (m_bgColor[i] != x)
			{
				m_bStaticBgColor = false;
				return;
			}

		m_bStaticBgColor = true;
		return;

	}

	//____ _refreshDecoration() _____________________________________________________

	void TextStyle::_refreshDecoration()
	{
		Bitmask<uint32_t> mask = m_decorationSetMask;
		mask.setBit(0);
		for (int i = 1; i < StateEnum_Nb; i++)
		{
			if (!mask.bit(i))
				m_decoration[i] = m_decoration[bestStateIndexMatch(i, mask)];
		}

		//

		auto x = m_decoration[0];
		for (int i = 1; i < StateEnum_Nb; i++)
			if (m_decoration[i] != x)
			{
				m_bStaticDecoration = false;
				return;
			}

		m_bStaticDecoration = true;
		return;
	}

} // namespace wg
