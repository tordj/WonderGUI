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
#ifndef WG_PACKPANEL_DOT_H
#define WG_PACKPANEL_DOT_H

#ifndef	WG_SIZEBROKER_DOT_H
#	include <wg_sizebroker.h>
#endif

#ifndef WG_VECTORPANEL_DOT_H
#	include <wg_vectorpanel.h>
#endif


class WgPackHook : public WgVectorHook
{
	friend class WgPackPanel;

public:

	bool	SetWeight( float weight );
	float	Weight() { return m_weight; }

	WgPackHook *	Prev() const { return _prev(); }
	WgPackHook *	Next() const { return _next(); }
	WgPackPanel*		Parent() const;

protected:
	PROTECTED_LINK_METHODS( WgPackHook );

	WgPackHook();

	int				m_length;			// Width or height, depending on orientation.
	float			m_weight;			// Weight for space allocation.
	WgSize			m_preferredSize;	// Cached best size from the child.
};




class WgPackPanel : public WgVectorPanel
{
	friend class WgPackHook;

public:
	void	SetSizeBroker( WgSizeBroker& broker );
	void	SetExpansionBroker( WgSizeBroker& broker );
	void	SetContractionBroker( WgSizeBroker& broker );

	WgSizeBroker& ExpansionBroker() { return m_expansionBroker; }
	WgSizeBroker& ContractionBroker() { return m_contractionBroker; }

protected:
	void	_reallocateSpace();

	virtual void	_onSpaceReallocated() = 0;

	WgSizeBroker 	m_expansionBroker;
	WgSizeBroker 	m_contractionBroker;

};


#endif //WG_PACKPANEL_DOT_H