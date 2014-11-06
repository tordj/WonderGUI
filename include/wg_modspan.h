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
#ifndef WG_MODSPAN_DOT_H
#define WG_MODSPAN_DOT_H

#ifndef WG_SLIDESPAN_DOT_H
#	include <wg_slidespan.h>
#endif


class WgModSpan;
typedef	WgIStrongPtr<WgModSpan,WgSlideSpanPtr>		WgModSpanPtr;
typedef	WgIWeakPtr<WgModSpan,WgSlideSpanWeakPtr>	WgModSpanWeakPtr;


class WgModSpan : public WgSlideSpan
{
public:
	WgModSpan(WgSpanItem* pItem) : WgSlideSpan(pItem) {}

	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgModSpanPtr		Cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.
	inline WgModSpanPtr		Ptr() { return WgModSpanPtr(_object(),this); }


	inline bool	SetMin( int min ) { return m_pItem->SetMin(min); }
	inline bool	SetMax( int max ) { return m_pItem->SetMax(max); }
	inline bool	SetRange( int min, int max ) { return m_pItem->SetRange(min,max); }

	inline void	SetSpan( int begin, int length ) { m_pItem->SetSpan(begin,length); }
	inline void	SetLength( int length ) { m_pItem->SetLength(length); }

	inline void	SetRelativeSpan( float begin, float length ) { m_pItem->SetRelativeSpan(begin,length); }
	inline void	SetRelativeLength( float length ) { m_pItem->SetRelativeLength(length); }

};


#endif //WG_MODSPAN_DOT_H
