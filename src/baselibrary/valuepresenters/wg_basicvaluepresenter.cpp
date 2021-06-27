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

#include <wg_basicvaluepresenter.h>

namespace wg
{

	const TypeInfo BasicValuePresenter::TYPEINFO = { "BasicValuePresenter", &ValuePresenter::TYPEINFO };

	//____ constructor ________________________________________________________

	BasicValuePresenter::BasicValuePresenter()
	{
	}

	BasicValuePresenter::BasicValuePresenter( const CharSeq& format )
	{
	}

	//____ Destructor _________________________________________________________

	BasicValuePresenter::~BasicValuePresenter()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& BasicValuePresenter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPlacement() _____________________________________________________

	void BasicValuePresenter::setPlacement(Placement placement)
	{

	}

	//____ setStyle() _________________________________________________________

	void BasicValuePresenter::setStyle(TextStyle* pStyle)
	{

	}


	//____ render() ___________________________________________________________

	void BasicValuePresenter::render(double value, GfxDevice* pDevice, const RectSPX& canvas)
	{

	}

	//____ preferredSize() ____________________________________________________

	SizeSPX BasicValuePresenter::preferredSize(double value, int scale) const
	{

	}

} // namespace wg
