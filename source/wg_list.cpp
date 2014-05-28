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

#include <wg_list.h>

const char WgList::CLASSNAME[] = {"List"};
const char WgListHook::CLASSNAME[] = {"ListHook"};


//____ WgListHook::IsInstanceOf() __________________________________________

bool WgListHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgHook::IsInstanceOf(pClassName);
}

//____ WgListHook::ClassName() _____________________________________________

const char * WgListHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgListHook::Cast() __________________________________________________

WgListHookPtr WgListHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgListHookPtr( static_cast<WgListHook*>(pHook.GetRealPtr()) );

	return 0;
}

//____ WgListHook::Parent() ___________________________________________________

WgListPtr WgListHook::Parent() const 
{ 
	return static_cast<WgList*>(_parent()); 
}

//____ WgListHook::SetVisible() ________________________________________________

bool WgListHook::SetVisible( bool bVisible )
{
	if( m_bVisible != bVisible )
	{
		m_bVisible = bVisible;
		if( bVisible )
			static_cast<WgList*>(_parent())->_onWidgetAppeared( this );
		else
			static_cast<WgList*>(_parent())->_onWidgetDisappeared( this );
	}
	return true;
}

//____ WgListHook::SetSelected() ________________________________________________

bool WgListHook::SetSelected( bool bSelected )
{
	if( !m_bVisible )
		return false;

	return (static_cast<WgList*>(_parent()))->_onEntrySelected( this, bSelected, false );
}

//____ Constructor ____________________________________________________________

WgList::WgList()
{
	m_selectMode = WG_SELECT_SINGLE;
}

//____ Destructor _____________________________________________________________

WgList::~WgList()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgList::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgContainer::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgList::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgListPtr WgList::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgListPtr( static_cast<WgList*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetEntrySkin() _________________________________________________________

void WgList::SetEntrySkin( const WgSkinPtr& pSkin )
{
	WgSize oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->ContentPadding() : WgSize();

	m_pEntrySkin[0] = pSkin;
	m_pEntrySkin[1] = pSkin;
	m_bOpaqueEntries = pSkin ? pSkin->IsOpaque() : false;

	_onEntrySkinChanged( oldPadding, pSkin ? pSkin->ContentPadding() : WgSize() );
}

bool WgList::SetEntrySkin( const WgSkinPtr& pOddEntrySkin, const WgSkinPtr& pEvenEntrySkin )
{
	WgSize oldPadding = m_pEntrySkin[0] ? m_pEntrySkin[0]->ContentPadding() : WgSize();
	WgSize padding[2];

	if( pOddEntrySkin )
		padding[0] = pOddEntrySkin->ContentPadding();

	if( pEvenEntrySkin )
		padding[1] = pEvenEntrySkin->ContentPadding();

	if( (padding[0].w != padding[1].w) || (padding[0].h != padding[1].h) )
		return false;

	m_pEntrySkin[0] = pOddEntrySkin;
	m_pEntrySkin[1] = pEvenEntrySkin;
	m_bOpaqueEntries = (pOddEntrySkin->IsOpaque() && pEvenEntrySkin->IsOpaque());

	_onEntrySkinChanged( padding[0], pOddEntrySkin ? pOddEntrySkin->ContentPadding() : WgSize() );
	return true;
}

//____ SetSelectMode() ________________________________________________________

bool WgList::SetSelectMode( WgSelectMode mode )
{
	if( mode != m_selectMode )
	{
		m_selectMode = mode;

		//TODO: Unselect all.
	}
	return true;
}