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

#ifndef	WG_TEXTMANAGER_DOT_H
#define	WG_TEXTMANAGER_DOT_H

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgTextManager;
class WgText;
class WgFont;


class WgTextNode : public WgLink
{

public:
	WgTextNode( WgTextManager * pManager, WgText * pText );
	virtual ~WgTextNode();

	LINK_METHODS( WgTextNode );		

	float	GetSize( const WgFont * m_pFont, WgFontStyle style, int size );
	void	Refresh();

	inline WgTextManager *	GetManager() const { return m_pManager; }
	inline WgText *			GetText() const { return m_pText; }

private:
	WgTextManager	* m_pManager;
	WgText			* m_pText;
};




class WgTextManager
{
friend class WgText;
friend class WgTextNode;

public:
	WgTextManager();
	virtual ~WgTextManager();

	void SetTextScale( float scale );


private:
	virtual WgTextNode *	NewNode( WgText * pText );
	WgChain<WgTextNode>	m_nodes;

	float	m_scale;

};




#endif //WG_TEXTMANAGER_DOT_H
