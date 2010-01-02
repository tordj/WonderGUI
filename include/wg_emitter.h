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

#ifndef WG_EMITTER_DOT_H
#define WG_EMITTER_DOT_H

#include <wg_types.h>
#include <wg_emitterstanza.h>
#include <wg_signalstanza.h>

class WgItem;
class WgMenuItem;
class WgTreeEntry;

//____ WgCallback ______________________________________________________________

struct WgCallback
{
	WgSignal::Signal	signal;			// Signal that activates callback.
	void *				pCallback;		// Callback receiving the call.
	void *				pObj;			// Object to send to callback.
};

//____ WgEmitter _______________________________________________________________

class WgEmitter
{
	friend class WgWidget;									// Need this to be able to clone.

protected:

	Uint16			m_nCallbacks;
	WgCallback * 	m_paCallbacks;

public:
	WgEmitter();
	~WgEmitter();

	void AddCallbackUnsafe( WgSignal::Signal signal, void * pCallback, void * pObj );

	METHODS_SIGNAL_0P(WgSignal::Signal_void);
	METHODS_SIGNAL_1P(WgSignal::Signal_bool,bool);
	METHODS_SIGNAL_1P(WgSignal::Signal_int,int);
	METHODS_SIGNAL_1P(WgSignal::Signal_float,float);
	METHODS_SIGNAL_1P(WgSignal::Signal_voidptr,const void *);
	METHODS_SIGNAL_1P(WgSignal::Signal_charptr,const char *);
	METHODS_SIGNAL_1P(WgSignal::Signal_item,WgItem *);
	METHODS_SIGNAL_1P(WgSignal::Signal_menuitem, WgMenuItem * );
	METHODS_SIGNAL_1P(WgSignal::Signal_treeentry, WgTreeEntry * );
	METHODS_SIGNAL_2P(WgSignal::Signal_bool_bool,bool,bool);
	METHODS_SIGNAL_2P(WgSignal::Signal_int_int,int,int);
	METHODS_SIGNAL_2P(WgSignal::Signal_float_float,float,float);


	int	RemoveCallback( WgSignal::Signal signal, void * pCallback, void * pObj );
	int	RemoveCallbacks( WgSignal::Signal signal );
	int	RemoveCallbacks( void * pObj );
	int	RemoveCallbacks();

private:
	static int s_EmitRef;
};




#endif // WG_EMITTER_DOT_H
