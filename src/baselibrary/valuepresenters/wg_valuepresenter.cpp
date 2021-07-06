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

#include <wg_valuepresenter.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

namespace wg
{

	const TypeInfo ValuePresenter::TYPEINFO = { "ValuePresenter", &Object::TYPEINFO };

	using namespace Util;


	//____ typeInfo() _________________________________________________________

	const TypeInfo& ValuePresenter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _renderString() ____________________________________________________

	void ValuePresenter::_renderString(GfxDevice* pDevice, const Char* pChar, TextStyle* pStyle, CoordSPX pos, int scale, State state)
	{
		TextAttr		baseAttr;
		pStyle->exportAttr(state, &baseAttr, scale);

		TextAttr		attr;
		Font_p 			pFont;
		TextStyle_h		hStyle = 0xFFFF;

		HiColor	baseTint = pDevice->tintColor();
		HiColor	localTint = Color::White;

		BlendMode renderMode = pDevice->blendMode();

		// Render back colors

//		_renderBack(pText, pDevice, canvas);

		//

		const RectSPX& clip = pDevice->clipBounds();

		Glyph_p	pGlyph;
		Glyph_p	pPrevGlyph = 0;

		bool bRecalcColor = false;
		bool bInSelection = false;

		while( !pChar->isEndOfText() )
		{
			// TODO: Include handling of special characters
			// TODO: Support char textcolor and background color and effects.

			if (pChar->styleHandle() != hStyle)
			{
				spx oldFontSize = attr.size;
				HiColor oldBgColor = attr.bgColor;

				attr = baseAttr;

				if (pChar->styleHandle() != 0)
					pChar->stylePtr()->addToAttr(state, &attr, scale);

				if (pFont != attr.pFont || attr.size != oldFontSize)
				{

					if (!pFont || (pFont->isMonochrome() != attr.pFont->isMonochrome()))
						bRecalcColor = true;		// Font tint-color is changed.

					pFont = attr.pFont;
					pFont->setSize(attr.size);
					pPrevGlyph = 0;								// No kerning across different fonts or character of different size.
				}

				if (attr.color != localTint)
				{
					localTint = attr.color;
					bRecalcColor = true;
				}
			}

			//
/*
			if (pChar == pSelBeg)
			{
				bInSelection = true;
				bRecalcColor = true;
			}
			else if (pChar == pSelEnd)
			{
				bInSelection = false;
				bRecalcColor = true;
			}
*/
			if (bRecalcColor)
			{
//				if (bInSelection)
//					pDevice->setTintColor(baseTint * HiColor::blend(localTint, m_selectionCharColor, m_selectionCharBlend));
//				else
					pDevice->setTintColor(baseTint * localTint);

				bRecalcColor = false;
			}

			//

			uint16_t charCode = pChar->code();
			Glyph_p pGlyph = pFont->getGlyph(charCode);

			if (pGlyph && charCode > 32)
			{
				if (pPrevGlyph)
					pos.x += pFont->kerning(pPrevGlyph, pGlyph);

				const GlyphBitmap* pBitmap = pGlyph->getBitmap();
				pDevice->setBlitSource(pBitmap->pSurface);
				pDevice->blit(CoordSPX(pos.x + pBitmap->bearingX, pos.y + pBitmap->bearingY), pBitmap->rect);
				pos.x += pGlyph->advance();
			}
			else if (pChar->code() == 32)
				pos.x += pFont->whitespaceAdvance();

			pPrevGlyph = pGlyph;
			pChar++;
		}

		pDevice->setTintColor(baseTint);


		// Render caret (if there is any)
/*
		if (_caretVisible(pText))
		{
			Caret* pCaret = m_pCaret ? m_pCaret : Base::defaultCaret();

			if (pCaret)
			{
				pCaret->render(pDevice, charRect(pText, _caretOfs(pText)) + canvas.pos(), _scale(pText));
			}
		}
*/
	}


} // namespace wg
