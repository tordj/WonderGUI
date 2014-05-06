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

#include <wg_textmanager.h>

#include <wg_textfield.h>
#include <wg_widget.h>

const char WgTextManager::CLASSNAME[] = {"TextManager"};


//____ Constructor ____________________________________________________________

WgTextManager::WgTextManager()
{
	m_scale = 1.f;

	m_scaleMultiplier = 1.f;

	m_pAllowedSizes = 0;

	m_growTreshold = 1.f;
	m_growRatio = 1.f;
	m_growLimit = 1024.f;

	m_shrinkTreshold = 1.f;
	m_shrinkRatio = 1.f;
	m_shrinkLimit = 0.f;

	m_sizeStepping = 0.f;

	m_sizeRounding = ROUND_NEAREST;

}

//____ Destructor _____________________________________________________________

WgTextManager::~WgTextManager()
{
	if( m_pAllowedSizes )
		delete [] m_pAllowedSizes;
}

//____ IsInstanceOf() _________________________________________________________

bool WgTextManager::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgTextManager::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgTextManagerPtr WgTextManager::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTextManagerPtr( static_cast<WgTextManager*>(pObject.GetRealPtr()) );

	return 0;
}


//____ _newNode() ______________________________________________________________

WgTextNode * WgTextManager::_newNode( WgText * pText )
{
	if( !pText )
		return 0;

	WgTextNode * p = new WgTextNode( this, pText );
	m_nodes.PushBack( p );
	p->Refresh();
	return p;
}

//____ _refreshAll() ___________________________________________________________

void WgTextManager::_refreshAll()
{
	WgTextNode * p = m_nodes.First();
	while( p )
	{
		p->Refresh();
		p = p->Next();
	}
}

//____ SetScaleValue() _________________________________________________________

bool WgTextManager::SetScaleValue( float scale )
{
	if( scale <= 0.f )
		return false;

	scale *= m_scaleMultiplier;

	if( scale != m_scale )
	{
		m_scale = scale;
		_refreshAll();
	}

	return true;
}

//____ SetScaleMultiplier() ___________________________________________________

bool WgTextManager::SetScaleMultiplier( float multi )
{
	if( multi <= 0.f )
		return false;

	if( m_scaleMultiplier != multi )
	{
		m_scaleMultiplier = multi;
		_refreshAll();
	}
	return true;
}

//____ SetAllowedSizes() ______________________________________________________

bool WgTextManager::SetAllowedSizes( int nSizes, float sizes[] )
{
	if( nSizes < 0 || (nSizes > 0 && sizes == 0) )
		return false;

	if( m_pAllowedSizes )
		delete [] m_pAllowedSizes;

	if( nSizes == 0 )
	{
		m_pAllowedSizes = 0;
	}
	else
	{
		float prevSize = 0.f;
		for( int i = 0 ; i < nSizes ; i++ )
		{
			if( sizes[i] <= prevSize )
				return false;
			prevSize = sizes[i];
		}


		m_pAllowedSizes = new float[nSizes+1];
		for( int i = 0 ; i < nSizes ; i++ )
			m_pAllowedSizes[i] = sizes[i];

		m_pAllowedSizes[nSizes] = 0;
	}
	_refreshAll();
	return true;
}

//____ SetGrowFormula() _______________________________________________________

bool WgTextManager::SetGrowFormula( float treshold, float ratio, float limit )
{
	if( treshold < 1.f || ratio < 0.f || limit < 0.f )
		return false;

	if( treshold != m_growTreshold || ratio != m_growRatio || limit != m_growLimit )
	{
		m_growTreshold = treshold;
		m_growRatio = ratio;
		m_growLimit = limit;
		_refreshAll();
	}
	return true;
}

//____ SetShrinkFormula() _____________________________________________________

bool WgTextManager::SetShrinkFormula( float treshold, float ratio, float limit )
{
	if( treshold < 0.f || ratio < 0.f || limit < 0.f )
		return false;

	if( treshold != m_shrinkTreshold || ratio != m_shrinkRatio || limit != m_shrinkLimit )
	{
		m_shrinkTreshold = treshold;
		m_shrinkRatio = ratio;
		m_shrinkLimit = limit;
		_refreshAll();
	}
	return true;
}

//____ SetSizeStepping() ______________________________________________________

bool WgTextManager::SetSizeStepping( float stepping )
{
	if( stepping < 0.f )
		return false;

	if( stepping != m_sizeStepping )
	{
		m_sizeStepping = stepping;
		_refreshAll();
	}
	return true;
}

//____ SetSizeRounding() ______________________________________________________

void WgTextManager::SetSizeRounding( Rounding rounding )
{
	if( rounding != m_sizeRounding )
	{
		m_sizeRounding = rounding;
		_refreshAll();
	}
}
//____ _getSize() __________________________________________________

float WgTextManager::_getSize( WgTextNode * pNode, const WgFontPtr& pFont, WgFontStyle style, int size )
{
	float scale = m_scale;

	// Modify scale according to formulas

	if( scale > m_growTreshold )
	{
		scale = 1.f + (scale - m_growTreshold)*m_growRatio;
		if( m_growLimit != 0.f && scale > m_growLimit )
			scale = m_growLimit;
	}
	else if( scale < m_shrinkTreshold )
	{
		scale = 1.f - (m_shrinkTreshold-scale)*m_shrinkRatio;
		if( scale < m_shrinkLimit )
			scale = m_shrinkLimit;

	}

	// Modify scale according to size stepping (with m_sizeRounding)

	if( m_sizeStepping != 0.f && scale != 1.f )
	{
		float stepping = m_sizeStepping + 1.f;

		float low = 1.f;
		float high = 1.f;

		if( scale > 1.f )
		{
			while( high < scale )
			{
				low = high;
				high *= stepping;
			}
		}
		else
		{
			while( low > scale )
			{
				high = low;
				low /= stepping;
			}
		}

		switch( m_sizeRounding )
		{
			case ROUND_UP:
				scale = high;
				break;
			case ROUND_DOWN:
				scale = low;
				break;
			case ROUND_NEAREST:
				if( scale - low > high - scale )
					scale = high;
				else
					scale = low;
			break;
		}
	}

	// Calc new size from scale

	float newSize = scale*size;


	// Modify scale to allowed size, if specified

	if( m_pAllowedSizes )
	{

		float * pSize = m_pAllowedSizes;
		float high = * pSize++;
		float low = high;


		while( high < newSize )
		{
			low = high;
			high = * pSize++;

			if( high == 0.f )
				return low;
		}

		switch( m_sizeRounding )
		{
			case ROUND_UP:
				newSize = high;
				break;
			case ROUND_DOWN:
				newSize = low;
				break;
			case ROUND_NEAREST:
				if( newSize - low > high - newSize )
					newSize = high;
				else
					newSize = low;
			break;
		}
	}

	return  newSize;
}



//____ WgTextNode::Constructor ________________________________________________

WgTextNode::WgTextNode( WgTextManager * pManager, WgText * pText )
{
	m_pManager = pManager;
	m_pText = pText;

	if( pText->m_pManagerNode )
		delete pText->m_pManagerNode;
	pText->m_pManagerNode = this;
}

//____ WgTextNode::Destructor _________________________________________________

WgTextNode::~WgTextNode()
{
	m_pText->m_pManagerNode = 0;
}

//____ WgTextNode::Refresh() __________________________________________________

void WgTextNode::Refresh()
{
	m_pText->refresh();
}

