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

#ifndef WG_MSGLOGGER_DOT_H
#define WG_MSGLOGGER_DOT_H

#include <iostream>
#include <wg_msgrouter.h>

namespace wg 
{
	
	
	class MsgLogger;
	typedef	WgStrongPtr<MsgLogger,Receiver_p>		MsgLogger_p;
	typedef	WgWeakPtr<MsgLogger,Receiver_wp>		MsgLogger_wp;
	
	class MsgLogger : public Receiver
	{
	public:
		static MsgLogger_p		create( std::ostream& stream ) { return MsgLogger_p( new MsgLogger(stream)); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static MsgLogger_p		cast( const Object_p& pObject );
	
		void onMsg( const Msg_p& _pMsg );
	
		void ignoreMsg( MsgType msg );
		void logMsg( MsgType msg );
	
		void logPointerMsgs();
		void ignorePointerMsgs();
	
		void logMouseButtonMsgs();
		void ignoreMouseButtonMsgs();
	
		void logKeyboardMsgs();
		void ignoreKeyboardMsgs();
	
		void logMouseMsgs();
		void ignoreMouseMsgs();
	
		void logInputMsgs();
		void ignoreInputMsgs();
	
		void logAllMsgs();
		void ignoreAllMsgs();
	
		bool isMsgLogged( MsgType msg ) { return m_msgFilter[msg]; }
	
	private:
		MsgLogger( std::ostream& stream );
		~MsgLogger();
	
		void	_onRouteAdded();
		void	_onRouteRemoved();
	
		std::string	_formatTimestamp( int64_t ms ) const;
		std::string _formatSource( const Msg_p& _pMsg ) const;
		std::string _formatCopyTo( const Msg_p& _pMsg ) const;
		std::string _formatModkeys( const Msg_p& _pMsg ) const;
		std::string _formatPointerPos( const Msg_p& _pMsg ) const;
		std::string _formatPointerStyle( const PointerChangeMsg_p& _pMsg ) const;
		std::string _formatMouseButton( WgMouseButton button ) const;
	
		bool			m_msgFilter[WG_MSG_MAX];
	
		std::ostream&	m_out;
	
	};
	
	
	
	
	

} // namespace wg
#endif //WG_MSGLOGGER_DOT_H
