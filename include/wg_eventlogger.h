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


class WgEventLogger : public WgEventListener
{
public:

	WgEventLogger( std::ostream& stream );
	~WgEventLogger();

	void OnEvent( const WgEvent::Event * _pEvent );

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
	std::string	_formatTimestamp( int64_t ms );
	std::string _formatWidget( const WgEvent::Event * _pEvent );
	std::string _formatModkeys( const WgEvent::Event * _pEvent );
	std::string _formatPointerPos( const WgEvent::Event * _pEvent );
	std::string _formatPointerStyle( const WgEvent::PointerChange * _pEvent );


	bool			m_eventFilter[WG_EVENT_MAX];

	std::ostream&	m_out;

};





#endif //WG_EVENTLOGGER_DOT_H
