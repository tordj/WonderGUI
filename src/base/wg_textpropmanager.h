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

namespace wg 
{
	
	
	class Textprop;
	class TextpropHolder;
	class Textprop_p;
	
	
	//____ TextpropHolder _______________________________________________________
	
	class TextpropHolder
	{
		friend class Textprop_p;
		friend class TextpropManager;
	
	private:
		TextpropHolder() {};
		~TextpropHolder() {};
	
		uint16_t			m_id;				///< Handle of this Textprop.
		uint8_t			m_indexOfs;			///< Offset in index table, need to know for quick removal.
		uint32_t			m_refCnt;			///< Reference count. Nb of WgStrongPtr and Char referencing this prop.
		int16_t			m_next;				///< Handle of next Textprop with same checksum or -1.
		int16_t			m_prev;				///< Handle of next Textprop with same checksum or -1.
	
		Textprop		m_prop;
	};
	
	//____ TextpropManager ______________________________________________________
	
	class TextpropManager
	{
		friend class Char;
		friend class Textprop;
		friend class Textprop_p;
		friend class TextTool;
		friend class CharBuffer;
		friend class TextMgr;
	
	public:
		TextpropManager();
	//	~TextpropManager();
	
	
		// These are here for test- and debugging purposes.
	
		inline static uint32_t getNbProperties() { return g_nPropUsed; }
		inline static uint32_t getPropBufferSize() { return g_nPropTotal; }
		inline static uint32_t getRefCnt( uint16_t hProp ) { return g_pPropBuffer[hProp].m_refCnt; }
	
		inline static void setMergeSimilar(bool bMerge) { g_bMergeSimilar = bMerge; }
	
	private:
	
		static const int NB_START_PROPS = 16;
	
		static inline void incRef( uint16_t hProp, uint32_t incCnt = 1 ) { g_pPropBuffer[hProp].m_refCnt += incCnt; }
	
		static inline void decRef( uint16_t hProp, uint32_t decCnt = 1 ) {	g_pPropBuffer[hProp].m_refCnt -= decCnt;
																	if( g_pPropBuffer[hProp].m_refCnt == 0 )
																		freeProp( hProp ); }
	
		static uint16_t	registerProp( const Textprop& prop );			// DOES NOT INCREASE REFCNT!
	
	//	static uint16_t 	getProp( const Font_p& pFont, const Color color, bool bColored,
	//							 bool bUnderlined, bool bSelected, TextLink_p& pLink );
	
		static const Textprop&	getProp( uint16_t hProp ) { return g_pPropBuffer[hProp].m_prop; }
		static const Textprop *	getPropPtr( uint16_t hProp ) { return &g_pPropBuffer[hProp].m_prop; }
	
	
	
		static void		freeProp( uint16_t hProp );
		static void		increaseBuffer( void );
	
	
		// Table for finding a certain Textprop quickly.
		// Uses a checksum on properties to choose one out of 256 linked lists which
		// is then searched linearly for any already existing object with same properties.
	
		static int16_t  				g_propIndex[256];
	
		static TextpropHolder		g_nullProp;
	
	
		static TextpropHolder *	g_pPropBuffer;
		static uint32_t				g_nPropUsed;
		static uint32_t				g_nPropTotal;
		static int16_t				g_firstFreeProp;
		static bool					g_bMergeSimilar;
	};
	
	

} // namespace wg
#endif // WG_TEXTPROPMANAGER_DOT_H


