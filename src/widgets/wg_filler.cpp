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



#include <wg_filler.h>
#include <wg_gfxdevice.h>

namespace wg
{

	const char Filler::CLASSNAME[] = {"Filler"};

	//____ Constructor ____________________________________________________________

	Filler::Filler()
	{
		m_preferredSize = SizeI(-1,-1);
	}

	//____ Destructor _____________________________________________________________

	Filler::~Filler()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool Filler::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * Filler::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	Filler_p Filler::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Filler_p( static_cast<Filler*>(pObject) );

		return 0;
	}


	//____ setPreferredSize() _______________________________________________________

	void Filler::setPreferredSize( const SizeI& size )
	{
		if( size != m_preferredSize )
		{
			m_preferredSize = size;
			_requestResize();
		}
	}

	//____ _preferredSize() __________________________________________________________

	SizeI Filler::_preferredSize() const
	{
		if( m_preferredSize.w >= 0 && m_preferredSize.h >= 0 )
			return m_preferredSize;
		else
			return Widget::_preferredSize();
	}


	//____ _cloneContent() _______________________________________________________

	void Filler::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		Filler * pOrg = (Filler*) _pOrg;

		m_preferredSize = pOrg->m_preferredSize;
	}

} // namespace wg
