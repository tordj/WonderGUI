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


class WgMsgLogger;
typedef	WgStrongPtr<WgMsgLogger,WgReceiverPtr>		WgMsgLoggerPtr;
typedef	WgWeakPtr<WgMsgLogger,WgReceiverWeakPtr>		WgMsgLoggerWeakPtr;

class WgMsgLogger : public WgReceiver
{
public:
	static WgMsgLoggerPtr		create( std::ostream& stream ) { return WgMsgLoggerPtr( new WgMsgLogger(stream)); }

	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgMsgLoggerPtr		cast( const WgObjectPtr& pObject );

	void onMsg( const WgMsgPtr& _pMsg );

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
	std::string _formatSource( const WgMsgPtr& _pMsg ) const;
	std::string _formatCopyTo( const WgMsgPtr& _pMsg ) const;
	std::string _formatModkeys( const WgMsgPtr& _pMsg ) const;
	std::string _formatPointerPos( const WgMsgPtr& _pMsg ) const;
	std::string _formatPointerStyle( const WgPointerChangeMsgPtr& _pMsg ) const;
	std::string _formatMouseButton( WgMouseButton button ) const;

	bool			m_msgFilter[WG_MSG_MAX];

	std::ostream&	m_out;

};





#endif //WG_MSGLOGGER_DOT_H
