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

#ifndef	WG_EVENTLISTENER_DOT_H
#define	WG_EVENTLISTENER_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

class WgEvent;
typedef	WgStrongPtr<WgEvent,WgObjectPtr>			WgEventPtr;

class WgEventListener;
typedef	WgStrongPtr<WgEventListener,WgObjectPtr>		WgEventListenerPtr;
typedef	WgWeakPtr<WgEventListener,WgObjectWeakPtr>		WgEventListenerWeakPtr;

class WgEventListener : public WgObject
{
	friend class WgEventHandler;
public:
	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgEventListenerPtr	Cast( const WgObjectPtr& pObject );

	virtual void OnEvent( const WgEventPtr& pEvent ) = 0;

protected:
	WgEventListener() {};
	
	virtual void	_onRouteAdded();
	virtual void	_onRouteRemoved();
};


#endif //WG_EVENTLISTENER_DOT_H