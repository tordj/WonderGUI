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
	
	
	class WgMsgLogger;
	typedef	WgStrongPtr<WgMsgLogger,WgReceiver_p>		WgMsgLogger_p;
	typedef	WgWeakPtr<WgMsgLogger,WgReceiver_wp>		WgMsgLogger_wp;
	
	class WgMsgLogger : public WgReceiver
	{
	public:
		static WgMsgLogger_p		create( std::ostream& stream ) { return WgMsgLogger_p( new WgMsgLogger(stream)); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static WgMsgLogger_p		cast( const WgObject_p& pObject );
	
		void onMsg( const WgMsg_p& _pMsg );
	
		void ignoreMsg( WgMsgType msg );
		void logMsg( WgMsgType msg );
	
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
	
		bool isMsgLogged( WgMsgType msg ) { return m_msgFilter[msg]; }
	
	private:
		WgMsgLogger( std::ostream& stream );
		~WgMsgLogger();
	
		void	_onRouteAdded();
		void	_onRouteRemoved();
	
		std::string	_formatTimestamp( int64_t ms ) const;
		std::string _formatSource( const WgMsg_p& _pMsg ) const;
		std::string _formatCopyTo( const WgMsg_p& _pMsg ) const;
		std::string _formatModkeys( const WgMsg_p& _pMsg ) const;
		std::string _formatPointerPos( const WgMsg_p& _pMsg ) const;
		std::string _formatPointerStyle( const WgPointerChangeMsg_p& _pMsg ) const;
		std::string _formatMouseButton( WgMouseButton button ) const;
	
		bool			m_msgFilter[WG_MSG_MAX];
	
		std::ostream&	m_out;
	
	};
	
	
	
	
	

} // namespace wg
#endif //WG_MSGLOGGER_DOT_H
