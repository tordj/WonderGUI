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

#ifndef	WG_SIZESCALER_DOT_H
#define	WG_SIZESCALER_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_CHAIN_DOT_H
#	include <wg_chain.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgSizeScaler;

typedef	WgStrongPtr<WgSizeScaler,WgObject_p>	WgSizeScaler_p;


class WgScalable
{
	friend class WgSizeScaler;
protected:
	WgScalable() {}
	virtual ~WgScalable();
	virtual void _onScaleChanged() = 0;
	
	WgSizeScaler_p m_pScaler;
};


class WgSizeScaler : public WgObject
{
	friend class WgScalable;
public:
	static WgSizeScaler_p create();
	static WgSizeScaler_p create( float scale );
	static WgSizeScaler_p create( float scaleX, float scaleY );

	virtual ~WgSizeScaler() {}

	void	setScale( float scale );
	void	setScale( float scaleX, float scaleY );
	void	setScaleX( float scaleX );
	void	setScaleY( float scaleY );

	inline float	scaleX() const { return m_scaleX; }
	inline float	scaleY() const { return m_scaleY; }

	void	addItem( WgScalable * pScalable );
	void	removeItem( WgScalable * pScalable );
	void	removeAllItems();

private:
	WgSizeScaler( float scaleX, float scaleY );
	void	_removeDeadItem( WgScalable * pScalable );

	class Node : public WgLink
	{
	public:
		LINK_METHODS(Node);
		WgScalable * m_pScalable;
	};

	WgChain<Node>	m_nodes;

	float	m_scaleX;
	float	m_scaleY;
};


#endif //WG_SIZESCALER_DOT_H
