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

#ifndef	WG_FIELD_DOT_H
#define	WG_FIELD_DOT_H

class WgField;
class WgObject;

//____ WgField ___________________________________________________________

class WgFieldHolder
{
public:
	virtual void		_onFieldDirty( WgField * pField ) = 0;
	virtual void		_onFieldResize( WgField * pField ) = 0;
	virtual WgObject*	_object() = 0;
};

//____ WgField _________________________________________________________________

class WgField
{
public:
	WgField( WgFieldHolder * pHolder ) : m_pHolder(pHolder) {}

	inline WgObject* _object() { return m_pHolder->_object(); }

protected:

	inline void		_onDirty() { return m_pHolder->_onFieldDirty(this); }
	inline void		_onResize() { return m_pHolder->_onFieldResize(this); }

	WgFieldHolder * m_pHolder;
};



#endif //WG_ITEM_DOT_H
