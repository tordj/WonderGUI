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

#ifndef	WG_EMITTERSTANZA_DOT_H
#define WG_EMITTERSTANZA_DOT_H


//____ METHODS_SIGNAL Defines _________________________________________________

#define		METHODS_SIGNAL_0P( _sigtype_ )												\
	inline void AddCallback( _sigtype_ sig, void(*fp)(void*), void * pObj )				\
	{																					\
		AddCallbackUnsafe( sig, (void*) fp, pObj );										\
	}																					\
																						\
	void Emit( _sigtype_ signal )														\
	{																					\
		s_EmitRef++;																	\
		for( int i = 0 ; i < m_nCallbacks ; i++ )										\
			if( m_paCallbacks[i].signal == (WgSignal::Signal) signal )					\
				((void(*)(void*))m_paCallbacks[i].pCallback)( m_paCallbacks[i].pObj);	\
		s_EmitRef--;																	\
	}																					\
																						\
	inline int	RemoveCallback( WgSignal::Signal signal, void(*fp)(void*), void * pObj )\
	{																					\
		return RemoveCallback( signal, (void*) fp, pObj );								\
	}




#define		METHODS_SIGNAL_1P( _sigtype_, _ptype1_ )												\
	inline void AddCallback( _sigtype_ sig, void(*fp)(void*,_ptype1_), void * pObj )				\
	{																								\
		AddCallbackUnsafe( sig, (void*) fp, pObj );													\
	}																								\
																									\
	void Emit( _sigtype_ signal, _ptype1_ p1 )														\
	{																								\
		s_EmitRef++;																				\
		for( int i = 0 ; i < m_nCallbacks ; i++ )													\
			if( m_paCallbacks[i].signal == (WgSignal::Signal) signal )								\
				((void(*)(void*,_ptype1_))m_paCallbacks[i].pCallback)( m_paCallbacks[i].pObj, p1);	\
		s_EmitRef--;																				\
	}																								\
																									\
	inline int	RemoveCallback( WgSignal::Signal signal, void(*fp)(void*,_ptype1_), void * pObj )	\
	{																								\
		return RemoveCallback( signal, (void*) fp, pObj );											\
	}


#define		METHODS_SIGNAL_2P( _sigtype_, _ptype1_, _ptype2_ )													\
	inline void AddCallback( _sigtype_ sig, void(*fp)(void*,_ptype1_,_ptype2_), void * pObj )					\
	{																											\
		AddCallbackUnsafe( sig, (void*) fp, pObj );																\
	}																											\
																												\
	void Emit( _sigtype_ signal, _ptype1_ p1, _ptype2_ p2 )														\
	{																											\
		s_EmitRef++;																							\
		for( int i = 0 ; i < m_nCallbacks ; i++ )																\
			if( m_paCallbacks[i].signal == (WgSignal::Signal) signal )											\
				((void(*)(void*,_ptype1_,_ptype2_))m_paCallbacks[i].pCallback)( m_paCallbacks[i].pObj, p1, p2);	\
		s_EmitRef--;																							\
	}																											\
																												\
	inline int	RemoveCallback( WgSignal::Signal signal, void(*fp)(void*,_ptype1_,_ptype2_), void * pObj )		\
	{																											\
		return RemoveCallback( signal, (void*) fp, pObj );														\
	}


#define		METHODS_SIGNAL_3P( _sigtype_, _ptype1_, _ptype2_, _ptype3_ )													\
	inline void AddCallback( _sigtype_ sig, void(*fp)(void*,_ptype1_,_ptype2_,_ptype3_), void * pObj )						\
	{																														\
		AddCallbackUnsafe( sig, (void*) fp, pObj );																			\
	}																														\
																															\
void Emit( _sigtype_ signal, _ptype1_ p1, _ptype2_ p2, _ptype3_ p3 )														\
{																															\
		s_EmitRef++;																										\
		for( int i = 0 ; i < m_nCallbacks ; i++ )																			\
			if( m_paCallbacks[i].signal == (WgSignal::Signal) signal )														\
				((void(*)(void*,_ptype1_,_ptype2_,_ptype3_))m_paCallbacks[i].pCallback)( m_paCallbacks[i].pObj, p1, p2, p3);\
		s_EmitRef--;																										\
}																															\
																															\
	inline int	RemoveCallback( WgSignal::Signal signal, void(*fp)(void*,_ptype1_,_ptype2_), void * pObj )					\
	{																														\
		return RemoveCallback( signal, (void*) fp, pObj );																	\
	}


#endif //WG_SIGNALSTANZA_DOT_H
