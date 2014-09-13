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

#ifndef	WG_TOGGLEGROUP_DOT_H
#	include <wg_togglegroup.h>
#endif

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

//____ IsInstanceOf() _________________________________________________________

bool WgToggleGroup::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgToggleGroup::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgToggleGroupPtr WgToggleGroup::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgToggleGroupPtr( static_cast<WgToggleGroup*>(pObject.GetRealPtr()) );

	return 0;
}

//____ SetRequireSelected() _______________________________________________________

void WgToggleGroup::SetRequireSelected(bool bRequire)
{
	m_bRequireSelected = bRequire;
	
	if( bRequire && !m_pSelected && !m_entries.empty() )
		m_entries[0]->SetSelected(true);
}

//____ Add() ___________________________________________________________________

void WgToggleGroup::Add( const WgToggleButtonPtr& pToggleButton )
{
	WgToggleButton * p = pToggleButton.GetRealPtr();
	if( p )
	{
		p->_setToggleGroup(this);
		m_entries.push_back( p );
	}
	
	// Select first entry if none selected and we require one to be
	
	if( m_bRequireSelected && !m_pSelected  )
		pToggleButton->SetSelected(true);
}

//____ Remove() ________________________________________________________________

bool WgToggleGroup::Remove( const WgToggleButtonPtr& pToggleButton )
{
	if( pToggleButton && pToggleButton->_toggleGroup() == this )
	{
		pToggleButton->_setToggleGroup(0);
		_remove( pToggleButton.GetRealPtr() );
		return true;
	}

	return false;
}

//____ Clear() _________________________________________________________________

void WgToggleGroup::Clear()
{
	for( int i = 0 ; i < m_entries.size() ; i++ )
		m_entries[i]->_setToggleGroup(0);

	m_pSelected = 0;
	m_entries.clear();
}

//____ Get() ___________________________________________________________________

WgToggleButtonPtr WgToggleGroup::Get( int index )
{
	if( index >= 0 && index < m_entries.size() )
		return WgToggleButtonPtr(m_entries[index]);

	return WgToggleButtonPtr();
}

//____ Size() __________________________________________________________________

int WgToggleGroup::Size() const
{
	return m_entries.size();
}

//____ Selected() ______________________________________________________________

WgToggleButtonPtr WgToggleGroup::Selected() const
{
	return WgToggleButtonPtr(m_pSelected);
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
			m_entries[0]->SetSelected(true);			
	}
}

//____ _select() _______________________________________________________________

void WgToggleGroup::_select( WgToggleButton * pButton )
{
	WgToggleButton * pOldSelected = m_pSelected;

	m_pSelected = pButton;
	
	if( pOldSelected && pOldSelected != pButton )
		pOldSelected->SetSelected(false);

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
