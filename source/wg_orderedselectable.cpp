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

#include <wg_orderedselectable.h>

void WgOrdSelHook::SetSelected( bool bSelected )
{
}

void WgOrdSelHook::SetSelectable( bool bSelectable )
{
}

WgOrdSelHook::WgOrdSelHook( WgGizmo * pGizmo )
{
}

WgOrdSelHook::~WgOrdSelHook()
{
}

WgOrdSelHook *	WgOrdSelHook::_prevSelectedHook() const
{
}

WgOrdSelHook *	WgOrdSelHook::_nextSelectedHook() const
{
}

int WgOrdSelLayout::SelectAll()
{
}

void WgOrdSelLayout::ClearSelection()
{
}

int WgOrdSelLayout::GetNbSelected()
{
}

void WgOrdSelLayout::SetSelectMode( WgSelectMode mode )
{
}

void WgOrdSelLayout::SetChildBgBlocks( const WgBlockSetPtr& pOddBg, const WgBlockSetPtr& pEvenBg )
{
}

void WgOrdSelLayout::SetChildFgBlocks( const WgBlockSetPtr& pFg )
{
}

void WgOrdSelLayout::SetChildBgColors( const WgColorSetPtr& pOddBg, const WgColorSetPtr& pEvenBg )
{
}

void WgOrdSelLayout::_onCloneContent( const WgGizmo * _pOrg )
{
}

WgOrdSelHook * WgOrdSelLayout::_firstSelectedHook() const
{
}

WgOrdSelHook * WgOrdSelLayout::_lastSelectedHook() const
{
}
