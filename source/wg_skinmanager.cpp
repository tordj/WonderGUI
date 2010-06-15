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

#include <wg_skinmanager.h>
#include <wg_gizmo.h>


//____ Constructor ____________________________________________________________

WgSkinManager::WgSkinManager()
{
}

//____ Destructor _____________________________________________________________

WgSkinManager::~WgSkinManager()
{
}

//____ NewNode() ______________________________________________________________

WgSkinNode * WgSkinManager::NewNode( WgGizmo * pGizmo )
{
	if( !pGizmo )
		return 0;

	WgSkinNode * p = new WgSkinNode( this, pGizmo );
	m_nodes.push_back( p );
	return p;
}


//____ WgSkinNode::Constructor ________________________________________________

WgSkinNode::WgSkinNode( WgSkinManager * pManager, WgGizmo * pGizmo )
{
	m_pManager = pManager;
	m_pGizmo = pGizmo;

	if( pGizmo->GetSkinNode() )
		delete pGizmo->GetSkinNode();
	pGizmo->SetSkinNode(this);
}

//____ WgSkinNode::Destructor _________________________________________________

WgSkinNode::~WgSkinNode()
{
	m_pGizmo->SetSkinNode(0);
}

//____ WgSkinNode::Refresh() __________________________________________________

void WgSkinNode::Refresh()
{
	m_pGizmo->Refresh();
}
