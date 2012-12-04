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
#ifndef WG_BOXLAYOUT_DOT_H
#define WG_BOXLAYOUT_DOT_H

#ifndef	WG_GEOBROKER_DOT_H
#	include <wg_geobroker.h>
#endif

#ifndef WG_SORTABLEPANEL_DOT_H
#	include <wg_sortablepanel.h>
#endif


class WgBoxLayoutHook : public WgSortableHook
{
	friend class WgBoxLayout;

public:

	bool	SetWeight( float weight );
	float	Weight() { return m_weight; }

	WgBoxLayoutHook *	Prev() const { return _prev(); }
	WgBoxLayoutHook *	Next() const { return _next(); }
	WgBoxLayout*		Parent() const;

protected:
	PROTECTED_LINK_METHODS( WgBoxLayoutHook );

	WgBoxLayoutHook();

	int				m_length;			// Width or height, depending on orientation.
	int				m_maxLength;
	int				m_minLength;
	float			m_weight;			// Weight for space allocation.
	WgSize			m_bestSize;			// Cached best size from the child.
};




class WgBoxLayout : public WgSortablePanel
{
	friend class WgBoxLayoutHook;

public:
	void	SetGeoBroker( WgGeoBroker& broker );
	void	SetExpansionBroker( WgGeoBroker& broker );
	void	SetContractionBroker( WgGeoBroker& broker );

	WgGeoBroker& ExpansionBroker() { return m_expansionBroker; }
	WgGeoBroker& ContractionBroker() { return m_contractionBroker; }

protected:
	void	_reallocateSpace();

	virtual void	_onSpaceReallocated() = 0;

	WgGeoBroker 	m_expansionBroker;
	WgGeoBroker 	m_contractionBroker;

};


#endif //WG_BOXLAYOUT_DOT_H