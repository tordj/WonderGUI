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

#ifndef WG_SPAN_DOT_H
#define WG_SPAN_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_SPANITEM_DOT_H
#	include <wg_spanitem.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

class WgSpan;
typedef	WgIStrongPtr<WgSpan,WgInterfacePtr>			WgSpanPtr;
typedef	WgIWeakPtr<WgSpan,WgInterfaceWeakPtr>		WgSpanWeakPtr;


class WgSpan : public WgInterface
{
public:
	WgSpan(WgSpanItem* pItem) : m_pItem(pItem) {}

	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgSpanPtr			Cast( const WgInterfacePtr& pInterface );				// Provided just for completeness sake.
	inline WgSpanPtr			Ptr() { return WgSpanPtr(_object(),this); }

	inline int		Min() const { return m_pItem->min; }
	inline int		Max() const { return m_pItem->max; }
	inline int 		Begin() const { return m_pItem->begin; }
	inline int		Length() const { return m_pItem->length; }

	inline float	RelativePos() const { return m_pItem->RelativePos(); }
	inline float	RelativeBegin() const { return m_pItem->RelativeBegin(); }
	inline float	RelativeLength() const { return m_pItem->RelativeLength(); };


protected:
	virtual WgObject * 		_object() const;

	WgSpanItem *		m_pItem;
};


#endif //WG_SPAN_DOT_H
