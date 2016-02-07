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

namespace wg 
{
	
	class Msg;
	typedef	StrongPtr<Msg,Object_p>			Msg_p;
	
	class Receiver;
	typedef	StrongPtr<Receiver,Object_p>		Receiver_p;
	typedef	WeakPtr<Receiver,Object_wp>		Receiver_wp;
	
	class Receiver : public Object
	{
		friend class MsgRouter;
	public:
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static Receiver_p	cast( const Object_p& pObject );
	
		virtual void onMsg( const Msg_p& pMsg ) = 0;
	
	protected:
		Receiver() {};
		
		virtual void	_onRouteAdded();
		virtual void	_onRouteRemoved();
	};
	
	

} // namespace wg
#endif //WG_RECEIVER_DOT_H
