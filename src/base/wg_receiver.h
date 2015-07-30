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

#ifndef	WG_RECEIVER_DOT_H
#define	WG_RECEIVER_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

class WgMsg;
typedef	WgStrongPtr<WgMsg,WgObject_p>			WgMsg_p;

class WgReceiver;
typedef	WgStrongPtr<WgReceiver,WgObject_p>		WgReceiver_p;
typedef	WgWeakPtr<WgReceiver,WgObject_wp>		WgReceiver_wp;

class WgReceiver : public WgObject
{
	friend class WgMsgRouter;
public:
	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgReceiver_p	cast( const WgObject_p& pObject );

	virtual void onMsg( const WgMsg_p& pMsg ) = 0;

protected:
	WgReceiver() {};
	
	virtual void	_onRouteAdded();
	virtual void	_onRouteRemoved();
};


#endif //WG_RECEIVER_DOT_H
