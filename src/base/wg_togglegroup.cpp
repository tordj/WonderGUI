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

#include <wg_togglegroup.h>

const char WgToggleGroup::CLASSNAME[] = {"ToggleGroup"};


//____ Constructor _____________________________________________________________

WgToggleGroup::WgToggleGroup()
{
	m_bRequireSelected = true;
	m_pSelected = 0;
}

//____ Destructor ______________________________________________________________

WgToggleGroup::~WgToggleGroup()
{
	for( int i = 0 ; i < m_entries.size() ; i++ )
		m_entries[i]->_setToggleGroup(0);}

//____ isInstanceOf() _________________________________________________________

bool WgToggleGroup::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgToggleGroup::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgToggleGroup_p WgToggleGroup::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgToggleGroup_p( static_cast<WgToggleGroup*>(pObject.rawPtr()) );

	return 0;
}

//____ setRequireSelected() _______________________________________________________
/**
 * Sets the requireSelected flag of this group, which means that there will always
 * be a selected widget in the group as long as the group is not empty. The default state
 * of this flag is TRUE.
 *
 * @param bRequire 	Set to TRUE to require a member to always be selected. Set to
 * 					false to allow all members to be deselected.
 *
 * Setting this flag in an empty ToggleGroup will result in the first widget that is added
 * to automatically be selected. If the ToggleGroup is not empty but has no selected
 * member, the first member of the group will be selected.
 *
 * It will not be possible to deselect a widget when requireSelected is TRUE, the only way
 * to deselect a member is to select another one. If the selected member of
 * the group is removed, the first member will be selected.
 *
 **/

void WgToggleGroup::setRequireSelected(bool bRequire)
{
	m_bRequireSelected = bRequire;
	
	if( bRequire && !m_pSelected && !m_entries.empty() )
		m_entries[0]->setSelected(true);
}

//____ add() ___________________________________________________________________
/**
 * Add specified WgToggleButton to this group, making it behave like a RadioButton
 * against the rest of the members of this group.
 *
 * @pToggleButton	ToggleButton to be added to this ToggleGroup.
 *
 * A ToggleButton can only belong to one ToggleGroup at the same time. If added
 * ToggleButton is member of another group, it will automatically be removed from
 * that group.
 *
 * If the group is empty and it is set to require a member to be selected (default
 * behavior) the added widget will automatically be selected. If the added member
 * is in a selected state but the group already has a selected member, the added
 * member is unselected.
 *
 **/

void WgToggleGroup::add( const WgToggleButton_p& pToggleButton )
{
	WgToggleButton * p = pToggleButton.rawPtr();
	if( p )
	{
		if( p->isSelected() )
		{
			if( m_pSelected )
				p->setSelected(false);
			else
				m_pSelected = p;
		}

		p->_setToggleGroup(this);
		m_entries.push_back( p );
	}

	// Select first entry if none selected and we require one to be
	
	if( m_bRequireSelected && !m_pSelected  )
		pToggleButton->setSelected(true);
}

//____ remove() ________________________________________________________________
/**
 * Removes specified ToggleButton from this group.
 *
 * If the removed ToggleButton was selected, it will keep its selected state when removed.
 * If the group requires a member to be selected (default behavior), the first member
 * of the group will be selected.
 *
 **/


bool WgToggleGroup::remove( const WgToggleButton_p& pToggleButton )
{
	if( pToggleButton && pToggleButton->_toggleGroup() == this )
	{
		pToggleButton->_setToggleGroup(0);
		_remove( pToggleButton.rawPtr() );
		return true;
	}

	return false;
}

//____ clear() _________________________________________________________________
/**
 * Removes all members from this group.
 *
 * The selected member (if any) will keep its selected state upon removal.
 *
 **/

void WgToggleGroup::clear()
{
	for( int i = 0 ; i < m_entries.size() ; i++ )
		m_entries[i]->_setToggleGroup(0);

	m_pSelected = 0;
	m_entries.clear();
}

//____ get() ___________________________________________________________________
/**
 * Get a specific member of the group
 *
 * @param index		Index (0+) of the member to get.
 *
 *
 * @return Pointer to member at index or null if index was out of bounds.
 **/

WgToggleButton_p WgToggleGroup::get( int index )
{
	if( index >= 0 && index < m_entries.size() )
		return WgToggleButton_p(m_entries[index]);

	return WgToggleButton_p();
}

//____ size() __________________________________________________________________
/**
 * Get the number of members in the group.
 *
 * @return Number of members in this ToggleGroup.
 **/

int WgToggleGroup::size() const
{
	return m_entries.size();
}

//____ selected() ______________________________________________________________
/**
 * Get the selected widget.
 *
 * @return A pointer to the widget of the group that is selected or null if none.
 *
 **/

WgToggleButton_p WgToggleGroup::selected() const
{
	return WgToggleButton_p(m_pSelected);
}

//____ _remove() _______________________________________________________________

void WgToggleGroup::_remove( WgToggleButton * pButton )
{
	for( std::vector<WgToggleButton*>::iterator it = m_entries.begin() ; it != m_entries.end() ; ++it )
	{
		if( * it == pButton )
		{
			m_entries.erase( it );
			break;
		}
	}
	
	if( pButton == m_pSelected )
	{
		m_pSelected = 0;
		if( m_bRequireSelected && !m_entries.empty() )
			m_entries[0]->setSelected(true);			
	}
}

//____ _select() _______________________________________________________________

void WgToggleGroup::_select( WgToggleButton * pButton )
{
	WgToggleButton * pOldSelected = m_pSelected;

	m_pSelected = pButton;
	
	if( pOldSelected && pOldSelected != pButton )
		pOldSelected->setSelected(false);

}

//____ _unselect() _____________________________________________________________

bool WgToggleGroup::_unselect( WgToggleButton * pButton )
{
	if( pButton ==  m_pSelected )
	{
		if( m_bRequireSelected )
			return false;

		m_pSelected = 0;
	}
	return true;
}
