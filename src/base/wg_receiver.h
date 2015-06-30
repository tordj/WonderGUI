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
typedef	WgStrongPtr<WgMsg,WgObjectPtr>			WgMsgPtr;

class WgReceiver;
typedef	WgStrongPtr<WgReceiver,WgObjectPtr>		WgReceiverPtr;
typedef	WgWeakPtr<WgReceiver,WgObjectWeakPtr>		WgReceiverWeakPtr;

class WgReceiver : public WgObject
{
	friend class WgMsgRouter;
public:
	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgReceiverPtr	Cast( const WgObjectPtr& pObject );

	virtual void OnMsg( const WgMsgPtr& pMsg ) = 0;

protected:
	WgReceiver() {};
	
	virtual void	_onRouteAdded();
	virtual void	_onRouteRemoved();
};


#endif //WG_RECEIVER_DOT_H