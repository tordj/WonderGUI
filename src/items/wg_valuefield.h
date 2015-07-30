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
#ifndef	WG_VALUEFIELD_DOT_H
#define WG_VALUEFIELD_DOT_H

#ifndef WG_PRESENTABLEFIELD_DOT_H
#	include <wg_presentablefield.h>
#endif

#ifndef WG_VALUEFORMATTER_DOT_H
#	include <wg_valueformatter.h>
#endif


//____ WgValueHolder ___________________________________________________________

class WgValueHolder : public WgPresentableHolder
{
public:
};

//____ WgValueField ____________________________________________________________

class WgValueField : public WgPresentableField
{
public:
	WgValueField(WgValueHolder * pHolder);

	void				setFormatter( const WgValueFormatterPtr& pFormatter );
	void				clearFormatter();
	inline WgValueFormatterPtr	formatter() const { return m_pFormatter; }

	virtual void		clear();
	virtual bool		set( Sint64 value, int scale );
	
	void				onRefresh();
	inline Sint64		value() const { return m_value; }
	inline int			scale() const { return m_scale; }

protected:
	void				_regenText();

	Sint64					m_value;
	int						m_scale;
	
	WgValueFormatterPtr		m_pFormatter;
};


#endif //WG_VALUEFIELD_DOT_H
