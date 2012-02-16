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

#ifndef	WG_SKINMANAGER_DOT_H
#define	WG_SKINMANAGER_DOT_H

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgSkinNode;
class WgGizmo;

class WgSkinManager
{

friend class WgSkinNode;
friend class WgGizmo;

public:
	WgSkinManager();
	virtual ~WgSkinManager();


private:
	int		_getAlternative( WgSkinNode * pNode, WgSize size );

	virtual WgSkinNode *	_newNode( WgGizmo * pGizmo );
	WgChain<WgSkinNode>		m_nodes;



};

class WgSkinNode : public WgLink
{

public:
	WgSkinNode( WgSkinManager * pManager, WgGizmo * pGizmo );
	virtual ~WgSkinNode();

	LINK_METHODS( WgSkinNode );

	inline int	GetAlternative( WgSize size ) { return m_pManager->_getAlternative( this, size ); }
	void		Refresh();

	inline WgSkinManager *	GetManager() const { return m_pManager; }
	inline WgGizmo *		GetGizmo() const { return m_pGizmo; }

private:
	WgSkinManager	* m_pManager;
	WgGizmo			* m_pGizmo;
};


#endif //WG_SKINMANAGER_DOT_H
