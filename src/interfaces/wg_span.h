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
typedef	WgIStrongPtr<WgSpan,WgInterface_p>			WgSpan_p;
typedef	WgIWeakPtr<WgSpan,WgInterface_wp>		WgSpan_wp;


class WgSpan : public WgInterface
{
public:
	WgSpan(WgSpanItem* pItem) : m_pItem(pItem) {}

	virtual bool				isInstanceOf( const char * pClassName ) const;
	virtual const char *		className( void ) const;
	static const char			CLASSNAME[];
	static WgSpan_p			cast( const WgInterface_p& pInterface );				// Provided just for completeness sake.
	inline WgSpan_p			ptr() { return WgSpan_p(_object(),this); }

	inline int		min() const { return m_pItem->min; }
	inline int		max() const { return m_pItem->max; }
	inline int 		begin() const { return m_pItem->begin; }
	inline int		length() const { return m_pItem->length; }

	inline float	relativePos() const { return m_pItem->relativePos(); }
	inline float	relativeBegin() const { return m_pItem->relativeBegin(); }
	inline float	relativeLength() const { return m_pItem->relativeLength(); };


protected:
	virtual WgObject * 		_object() const;

	WgSpanItem *		m_pItem;
};


#endif //WG_SPAN_DOT_H
