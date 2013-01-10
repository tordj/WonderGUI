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

#include <vector>

#ifndef WG_SIZECAPSULE_DOT_H
#define WG_SIZECAPSULE_DOT_H

#ifndef WG_CAPSULE_DOT_H
#	include <wg_capsule.h>
#endif


class WgSizeCapsule : public WgCapsule
{
public:
	WgSizeCapsule();
	~WgSizeCapsule();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgSizeCapsule(); };

	void	SetSizes( WgSize min, WgSize preferred, WgSize max );

	void	SetPreferredSize( WgSize size );
	void	SetMinSize( WgSize size );
	void	SetMaxSize( WgSize size );

	WgSize	PreferredSize() const;
	WgSize	MinSize() const;
	WgSize	MaxSize() const;

	int		HeightForWidth( int width ) const;
	int		WidthForHeight( int height ) const;

private:

	WgSize	m_min;
	WgSize	m_max;
	WgSize	m_preferred;
};

#endif //WG_SIZECAPSULE_DOT_H
