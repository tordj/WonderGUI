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

#include <wg_widget.h>
#include <wg_valuefield.h>


//____ Constructor _____________________________________________________________

WgValueField::WgValueField(WgValueHolder * pHolder) : WgField(pHolder)
{
	m_value = 0;
	m_alignment = WG_EAST;
	m_bAutoEllipsis = true;
}

//____ Destructor ______________________________________________________________

WgValueField::~WgValueField()
{
	
}

//____ SetFormat() _____________________________________________________________

void WgValueField::SetFormat( const WgValueFormatPtr& pFormat )
{
	if( m_pFormat != pFormat )
	{
		m_pFormat = pFormat;
		_regenText();
	}
}

//____ SetProperties() _________________________________________________________

void WgValueField::SetProperties( const WgTextpropPtr& pProp )
{
	if( m_pProp != pProp )
	{
		m_pProp = pProp;
		_recalcSize();
		_onDirty();
	}	
}

//____ ClearProperties() _______________________________________________________

void WgValueField::ClearProperties()
{
		SetProperties( 0 );
}

//____ SetAlignment() __________________________________________________________

void WgValueField::SetAlignment( WgOrigo alignment )
{
	if( alignment != m_alignment )
	{
		m_alignment = alignment;
		_onDirty();
	}
}

//____ SetAutoEllipsis() _______________________________________________________

void WgValueField::SetAutoEllipsis(bool bAutoEllipsis)
{
	if( bAutoEllipsis != m_bAutoEllipsis )
	{
		m_bAutoEllipsis = bAutoEllipsis;
		_onDirty();
	}
}



void WgValueField::SetValue( Sint64 value )
{
	if( value != m_value )
	{
		m_value = value;
		_regenText();
	}
}

void WgValueField::_recalcSize()
{
	
}

void WgValueField::_regenText()
{
	_recalcSize();
	_onDirty();
}
