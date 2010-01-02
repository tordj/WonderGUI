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

#ifndef WG_TEXTPROPMANAGER_DOT_H
#define WG_TEXTPROPMANAGER_DOT_H

#include <wg_types.h>
#include <wg_textprop.h>


class WgTextProp;
class WgTextPropHolder;
class WgTextPropPtr;


//____ WgTextPropHolder _______________________________________________________

class WgTextPropHolder
{
	friend class WgTextPropPtr;
	friend class WgTextPropManager;

private:
	WgTextPropHolder() {};
	~WgTextPropHolder() {};

	Uint16			m_id;				///< Handle of this WgTextProp.
	Uint8			m_indexOfs;			///< Offset in index table, need to know for quick removal.
	Uint32			m_refCnt;			///< Reference count. Nb of WgSmartPtr and WgChar referencing this prop.
	Sint16			m_next;				///< Handle of next WgTextProp with same checksum or -1.
	Sint16			m_prev;				///< Handle of next WgTextProp with same checksum or -1.

	WgTextProp		m_prop;
};

//____ WgTextPropManager ______________________________________________________

class WgTextPropManager
{
	friend class WgChar;
	friend class WgTextProp;
	friend class WgTextPropPtr;
	friend class WgTextTool;
	friend class WgCharBuffer;
	friend class WgTextMgr;

public:
	WgTextPropManager();
//	~WgTextPropManager();


	// These are here for test- and debugging purposes.

	inline static Uint32 GetNbProperties() { return g_nPropUsed; }
	inline static Uint32 GetPropBufferSize() { return g_nPropTotal; }
	inline static Uint32 GetRefCnt( Uint16 hProp ) { return g_pPropBuffer[hProp].m_refCnt; }

private:

	static const int NB_START_PROPS = 16;

	static inline void IncRef( Uint16 hProp, Uint32 incCnt = 1 ) { g_pPropBuffer[hProp].m_refCnt += incCnt; }

	static inline void DecRef( Uint16 hProp, Uint32 decCnt = 1 ) {	g_pPropBuffer[hProp].m_refCnt -= decCnt;
																if( g_pPropBuffer[hProp].m_refCnt == 0 )
																	FreeProp( hProp ); }

	static Uint16	RegisterProp( const WgTextProp& prop );			// DOES NOT INCREASE REFCNT!

//	static Uint16 	GetProp( WgFont * pFont, const WgColor color, bool bColored,
//							 bool bUnderlined, bool bSelected, WgTextLinkPtr& pLink );

	static const WgTextProp&	GetProp( Uint16 hProp ) { return g_pPropBuffer[hProp].m_prop; }
	static const WgTextProp *	GetPropPtr( Uint16 hProp ) { return &g_pPropBuffer[hProp].m_prop; }



	static void		FreeProp( Uint16 hProp );
	static void		IncreaseBuffer( void );


	// Table for finding a certain WgTextProp quickly.
	// Uses a checksum on properties to choose one out of 256 linked lists which
	// is then searched linearly for any already existing object with same properties.

	static Sint16  				g_propIndex[256];

	static WgTextPropHolder		g_nullProp;


	static WgTextPropHolder *	g_pPropBuffer;
	static Uint32				g_nPropUsed;
	static Uint32				g_nPropTotal;
	static Sint16				g_firstFreeProp;
};


#endif // WG_TEXTPROPMANAGER_DOT_H


