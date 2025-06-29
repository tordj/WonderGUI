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

#include <wg_simplistic.h>

#include <wg_sysfont.h>

#include <wg_togglegroup.h>
#include <wg_labelcapsule.h>

#include <wg_colorskin.h>
#include <wg_boxskin.h>

#include <wg_basictextlayout.h>

namespace wg
{

const TypeInfo Simplistic::TYPEINFO = { "Simplistic", &Theme::TYPEINFO };


//____ create() _______________________________________________________________

Simplistic_p Simplistic::create(Font * pNormal, Font * pBold, Font * pItalic, Font * pMonospace )
{
	return Simplistic_p(new Simplistic(pNormal, pBold, pItalic, pMonospace));
}

//____ typeInfo() _____________________________________________________________

const TypeInfo& Simplistic::typeInfo(void) const
{
	return TYPEINFO;
}

//____ constructor ____________________________________________________________

Simplistic::Simplistic( Font * pNormal, Font * pBold, Font * pItalic, Font * pMonospace )
{
	m_pFontNormal = pNormal;
	m_pFontBold = pBold;
	m_pFontItalic = pItalic;
	m_pFontMonospace = pMonospace;


	HiColor plateColor = Color::LightGrey;
	HiColor borderColor = Color::DarkGrey;

	HiColor titlebarColor = Color::LightGreen;
	HiColor titlebarBorderColor = Color::DarkGreen;

	m_pStrongStyle = TextStyle::create(WGBP(TextStyle, _.font = m_pFontBold, _.color = HiColor::Black, _.size = c_textSizeNormal));
	m_pEmphasisStyle = TextStyle::create(WGBP(TextStyle, _.font = m_pFontItalic, _.color = HiColor::Black, _.size = c_textSizeNormal));
	m_pCodeStyle = TextStyle::create(WGBP(TextStyle, _.font = m_pFontMonospace, _.color = HiColor::Black, _.size = c_textSizeNormal));
	m_pMonospaceStyle = TextStyle::create(WGBP(TextStyle, _.font = m_pFontMonospace, _.color = HiColor::Black, _.size = c_textSizeNormal));


	m_pBlackStyle = TextStyle::create( WGBP(TextStyle, _.font = m_pFontNormal, _.color = HiColor::Black, _.size = 12 ));
	m_pWhiteStyle = TextStyle::create( WGBP(TextStyle, _.font = m_pFontNormal, _.color = HiColor::White, _.size = 12 ));

	m_heading1Style = TextStyle::create( WGBP(TextStyle, _.font = m_pFontNormal, _.color = HiColor::Black, _.size = 20));
	m_heading2Style = TextStyle::create( WGBP(TextStyle, _.font = m_pFontBold, _.color = HiColor::Black, _.size = 20));
	m_heading3Style = TextStyle::create( WGBP(TextStyle, _.font = m_pFontNormal, _.color = HiColor::Black, _.size = 16));
	m_heading4Style = TextStyle::create( WGBP(TextStyle, _.font = m_pFontBold, _.color = HiColor::Black, _.size = 16));
	m_heading5Style = TextStyle::create( WGBP(TextStyle, _.font = m_pFontBold, _.color = HiColor::Black, _.size = 14));
	m_heading6Style = TextStyle::create( WGBP(TextStyle, _.font = m_pFontNormal, _.color = HiColor::Black, _.size = 14));

	auto pLayoutCenteredNoWrap = BasicTextLayout::create( WGBP(BasicTextLayout,
															   _.autoEllipsis = true,
															   _.placement = Placement::Center,
															   _.wrap = false ));

	auto pPlateSkin = BoxSkin::create( WGBP(BoxSkin,
											_.color = plateColor,
											_.outlineColor = borderColor,
											_.outlineThickness = 1,
											_.padding = 3 ));

	auto pTitleBarSkin = BoxSkin::create( WGBP(BoxSkin,
											_.color = titlebarColor,
											_.outlineColor = titlebarBorderColor,
											_.outlineThickness = 1,
											_.padding = 4 ));

	auto pLabelCapsuleSkin = BoxSkin::create( WGBP(BoxSkin,
										  _.color = plateColor,
										  _.outlineColor = borderColor,
										  _.outlineThickness = 1,
										  _.spacing = { 6,2,2,2},
										  _.padding = { 8, 4, 4, 4} ));

	auto pCapsuleLabelSkin = ColorSkin::create( WGBP(ColorSkin,
													 _.color = plateColor,
													 _.padding = { 2,2,2,2 }
													 ) );

	auto pCapsuleLabelSkin2 = ColorSkin::create( WGBP(ColorSkin,
													 _.color = HiColor::Transparent,
													 _.padding = { 2,2,2,2 }
													 ) );


	auto pInvisibleBoxSkin = ColorSkin::create( WGBP(ColorSkin,
										  _.color = HiColor::Transparent,
										  _.spacing = { 6,2,2,2},
										  _.padding = { 16, 4, 4, 4} ));


	m_labeledBoxBP = WGBP(LabelCapsule,
						  _.label.style = m_pBlackStyle,
						  _.labelSkin = pCapsuleLabelSkin,
						  _.labelPlacement = Placement::North,
						  _.skin = pLabelCapsuleSkin );

	m_labeledSectionBP = WGBP(LabelCapsule,
						  _.label.style = m_pBlackStyle,
						  _.labelSkin = pCapsuleLabelSkin2,
						  _.labelPlacement = Placement::North,
						  _.skin = pInvisibleBoxSkin );


	auto pScrollbarBgSkin = BoxSkin::create( WGBP(BoxSkin,
											_.color = Color::DarkGray,
											  _.outlineColor = Color::Black,
											  _.outlineThickness = 1,
											  _.padding = 2));

	auto pScrollbarSkin = BoxSkin::create( WGBP(BoxSkin,
											_.color = Color::LightGray,
											  _.outlineColor = Color::Black,
											  _.outlineThickness = 1,
											  _.padding = 6));

	m_scrollPanelYBP = WGBP(ScrollPanel,
						   _.childConstraintX = SizeConstraint::Equal,
						   _.childConstraintY = SizeConstraint::GreaterOrEqual,
						   _.scrollbarY.background = pScrollbarBgSkin,
						   _.scrollbarY.bar = pScrollbarSkin
						   );

	m_scrollPanelXBP = WGBP(ScrollPanel,
						   _.childConstraintX = SizeConstraint::GreaterOrEqual,
						   _.childConstraintY = SizeConstraint::Equal,
						   _.scrollbarX.background = pScrollbarBgSkin,
						   _.scrollbarX.bar = pScrollbarSkin
						   );

	m_scrollPanelXYBP = WGBP(ScrollPanel,
						   _.childConstraintX = SizeConstraint::GreaterOrEqual,
						   _.childConstraintY = SizeConstraint::GreaterOrEqual,
						   _.scrollbarX.background = pScrollbarBgSkin,
						   _.scrollbarX.bar = pScrollbarSkin
						   );

	m_windowTitleBar = WGBP(TextDisplay,
							_.skin = pTitleBarSkin,
							_.display.layout = pLayoutCenteredNoWrap );

	/*
	m_pushButtonBP;

	m_toggleButtonBP;
	m_radioButtonBP;
	m_checkboxBP;
*/
}


pts Simplistic::textSizeSmallest() const
{
	return c_textSizeSmallest;
}

pts Simplistic::textSizeSmall() const
{
	return c_textSizeSmall;
}

pts	Simplistic::textSizeNormal() const
{
	return c_textSizeNormal;
}

pts	Simplistic::textSizeLarge() const
{
	return c_textSizeLarge;
}

pts	Simplistic::textSizeLargest() const
{
	return c_textSizeLargest;
}

Font_p Simplistic::normalFont() const
{
	return m_pFontNormal;
}

Font_p Simplistic::monospaceFont() const
{
	return m_pFontMonospace;
}

Font_p Simplistic::boldFont() const
{
	return m_pFontBold;
}

Font_p Simplistic::italicFont() const
{
	return m_pFontItalic;
}

TextStyle_p Simplistic::heading1Style() const
{
	return m_heading1Style;
}

TextStyle_p Simplistic::heading2Style() const
{
	return m_heading2Style;
}

TextStyle_p Simplistic::heading3Style() const
{
	return m_heading3Style;
}

TextStyle_p Simplistic::heading4Style() const
{
	return m_heading4Style;
}

TextStyle_p Simplistic::heading5Style() const
{
	return m_heading5Style;
}

TextStyle_p Simplistic::heading6Style() const
{
	return m_heading6Style;
}

TextStyle_p Simplistic::subheadStyle1() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::subheadStyle2() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::subheadStyle3() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::subheadStyle4() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::subheadStyle5() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::subheadStyle6() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::paragraphStyle() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::strongStyle() const
{
	return m_pStrongStyle;
}

TextStyle_p Simplistic::emphasisStyle() const
{
	return m_pEmphasisStyle;
}

TextStyle_p Simplistic::codeStyle() const
{
	return m_pCodeStyle;
}

TextStyle_p Simplistic::linkStyle() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::monospaceStyle() const
{
	return m_pMonospaceStyle;
}

TextStyle_p Simplistic::quoteStyle() const
{
	return m_pEmphasisStyle;
}

TextStyle_p Simplistic::captionStyle() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::calloutStyle() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::defaultStyle() const
{
	return m_pBlackStyle;
}

TextStyle_p Simplistic::pressableStyle() const
{
	return m_pBlackStyle;
}

const LabelCapsule::Blueprint& Simplistic::labeledBox() const
{
	return m_labeledBoxBP;
}

const LabelCapsule::Blueprint& Simplistic::labeledSection() const
{
	return m_labeledSectionBP;
}

const Button::Blueprint& Simplistic::pushButton() const
{
	return m_pushButtonBP;
}

const ToggleButton::Blueprint& Simplistic::toggleButton() const
{
	return m_toggleButtonBP;
}

const ToggleButton::Blueprint& Simplistic::radioButton() const
{
	return m_radioButtonBP;
}

const ToggleButton::Blueprint& Simplistic::checkbox() const
{
	return m_checkboxBP;
}

const ScrollPanel::Blueprint& Simplistic::scrollPanelX() const
{
	return m_scrollPanelXBP;
}

const ScrollPanel::Blueprint& Simplistic::scrollPanelY() const
{
	return m_scrollPanelYBP;
}

const ScrollPanel::Blueprint& Simplistic::scrollPanelXY() const
{
	return m_scrollPanelXYBP;
}

const TextDisplay::Blueprint& Simplistic::windowTitleBar() const
{
	return m_windowTitleBar;
}




}
