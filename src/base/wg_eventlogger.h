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

#ifndef WG_EVENTLOGGER_DOT_H
#define WG_EVENTLOGGER_DOT_H

#include <iostream>
#include <wg_eventhandler.h>


class WgEventLogger;
typedef	WgStrongPtr<WgEventLogger,WgEventListenerPtr>		WgEventLoggerPtr;
typedef	WgWeakPtr<WgEventLogger,WgEventListenerWeakPtr>		WgEventLoggerWeakPtr;

class WgEventLogger : public WgEventListener
{
public:
	static WgEventLoggerPtr		Create( std::ostream& stream ) { return WgEventLoggerPtr( new WgEventLogger(stream)); }

	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgEventLoggerPtr		Cast( const WgObjectPtr& pObject );

	void OnEvent( const WgEventPtr& _pEvent );

	void IgnoreEvent( WgEventType event );
	void LogEvent( WgEventType event );

	void LogPointerEvents();
	void IgnorePointerEvents();

	void LogMouseButtonEvents();
	void IgnoreMouseButtonEvents();

	void LogKeyboardEvents();
	void IgnoreKeyboardEvents();

	void LogMouseEvents();
	void IgnoreMouseEvents();

	void LogInputEvents();
	void IgnoreInputEvents();

	void LogAllEvents();
	void IgnoreAllEvents();

	bool IsEventLogged( WgEventType event ) { return m_eventFilter[event]; }

private:
	WgEventLogger( std::ostream& stream );
	~WgEventLogger();


	std::string	_formatTimestamp( int64_t ms ) const;
	std::string _formatSource( const WgEventPtr& _pEvent ) const;
	std::string _formatCopyTo( const WgEventPtr& _pEvent ) const;
	std::string _formatModkeys( const WgEventPtr& _pEvent ) const;
	std::string _formatPointerPos( const WgEventPtr& _pEvent ) const;
	std::string _formatPointerStyle( const WgPointerChangeEventPtr& _pEvent ) const;
	std::string _formatMouseButton( WgMouseButton button ) const;

	bool			m_eventFilter[WG_EVENT_MAX];

	std::ostream&	m_out;

};





#endif //WG_EVENTLOGGER_DOT_H
