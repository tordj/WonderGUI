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

/* TODO: See if we can be better off using multiple size-objects instead of changing size all the time.
   TODO: Separate glyph bitmap from glyph struct, not render or prio cache slot until bitmap is accessed (probably has more to do with WgPen).
*/

#include <wg_userdefines.h>

#ifdef	WG_USE_FREETYPE

#include <wg_base.h>
#include <wg_vectorglyphs.h>
#include <wg_surface.h>
#include <assert.h>


#include <ft2build.h>
#include FT_FREETYPE_H


WgChain<WgVectorGlyphs::CacheSlot>	WgVectorGlyphs::s_cacheSlots[GLYPH_SLOT_SIZES];
WgChain<WgVectorGlyphs::CacheSurf>	WgVectorGlyphs::s_cacheSurfaces;
WgSurfaceFactory *					WgVectorGlyphs::s_pSurfaceFactory = 0;


//____ Constructor ____________________________________________________________

WgVectorGlyphs::WgVectorGlyphs( const void * pTTF_File, int bytes, int faceIndex )
{
	m_ftCharSize	= 0;
	m_accessCounter = 0;

	for( int i = 0 ; i < WG_MAX_FONTSIZE ; i++ )
		m_cachedGlyphsIndex[i] = 0;

	FT_Error err = FT_New_Memory_Face(	WgBase::GetFreeTypeLibrary(),
										(const FT_Byte *) pTTF_File,
										bytes,
										0,
										&m_ftFace );

	if( err )
	{
		int x = 0;
		//TODO: Error handling...
	}

	if( FT_HAS_KERNING( m_ftFace ) )
		int x = 0;


	SetRenderMode( CRISP_EDGES );
}

//____ Destructor _____________________________________________________________

WgVectorGlyphs::~WgVectorGlyphs()
{
	for( int i = 0 ; i < WG_MAX_FONTSIZE ; i++ )
	{
		if( m_cachedGlyphsIndex[i] != 0 )
			delete m_cachedGlyphsIndex[i];
	}

	FT_Error err = FT_Done_Face( m_ftFace );
}

//____ SetRenderMode() ________________________________________________________

bool WgVectorGlyphs::SetRenderMode( RenderMode mode )
{
	switch( mode )
	{
		case MONOCHROME:
			m_renderFlags = FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO;
			break;
		case CRISP_EDGES:
			m_renderFlags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
			break;
		case BEST_SHAPES:
			m_renderFlags = FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT;
			break;

		default:
			return false;
	}

	m_renderMode = mode;
	return true;
}

//____ GetKerning() ___________________________________________________________

int WgVectorGlyphs::GetKerning( const WgGlyph* pLeftGlyph, const WgGlyph* pRightGlyph, int size )
{
	if( pLeftGlyph == 0 || pRightGlyph == 0 )
		return 0;

	// Set size for FreeType

	if( m_ftCharSize != size )
	{
		FT_Error err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_ftCharSize = 0;
			return 0;
		}

		m_ftCharSize = size;
	}

	// Get kerning info

	FT_Vector	delta;
	FT_Get_Kerning( m_ftFace, pLeftGlyph->kerningIndex, pRightGlyph->kerningIndex, FT_KERNING_DEFAULT, &delta );

	return delta.x >> 6;
}

//____ GetWhitespaceAdvance() _________________________________________________

int WgVectorGlyphs::GetWhitespaceAdvance( int size )
{
	//TODO: We should probably cache this...

	FT_Error err;

	// Set size for FreeType

	if( m_ftCharSize != size )
	{
		err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_ftCharSize = 0;
			return 0;
		}

		m_ftCharSize = size;
	}

	// Load whitespace glyph

	err = FT_Load_Char( m_ftFace, ' ', FT_LOAD_RENDER );
	if( err )
		return 0;

	// Get and return advance

	return  (m_ftFace->glyph->linearHoriAdvance +32768) >> 16;

}

//____ GetHeight() ____________________________________________________________

int WgVectorGlyphs::GetHeight( int size ) 
{ 
	FT_Error err;

	if( m_ftCharSize != size )
	{
		err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_ftCharSize = 0;
			return 0;
		}

		m_ftCharSize = size;
	}

	return (m_ftFace->size->metrics.ascender - m_ftFace->size->metrics.descender+32) >> 6; 
}

//____ GetLineSpacing() ____________________________________________________________

int WgVectorGlyphs::GetLineSpacing( int size ) 
{ 
	FT_Error err;

	if( m_ftCharSize != size )
	{
		err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_ftCharSize = 0;
			return 0;
		}

		m_ftCharSize = size;
	}

	return (m_ftFace->size->metrics.height+32) >> 6; 
}


//____ GetBaseline() ____________________________________________________________

int WgVectorGlyphs::GetBaseline( int size ) 
{ 
	FT_Error err;

	if( m_ftCharSize != size )
	{
		err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_ftCharSize = 0;
			return 0;
		}

		m_ftCharSize = size;
	}

	return (m_ftFace->size->metrics.ascender+32) >> 6; 
}


//____ GetNbGlyphs() __________________________________________________________

int WgVectorGlyphs::GetNbGlyphs() 
{ 
	return m_ftFace->num_glyphs; 
}

//____ HasGlyphs() ____________________________________________________________

bool WgVectorGlyphs::HasGlyphs() 
{ 
	return m_ftFace->num_glyphs?true:false; 
}

//____ IsMonospace() __________________________________________________________

bool WgVectorGlyphs::IsMonospace() 
{ 
	return FT_IS_FIXED_WIDTH( m_ftFace )>0?true:false; 
}

//____ GetMaxGlyphAdvance() ___________________________________________________

int WgVectorGlyphs::GetMaxGlyphAdvance( int size ) 
{ 
	return (m_ftFace->size->metrics.max_advance+32) >> 6; 
}


//____ HasGlyph() _____________________________________________________________

bool WgVectorGlyphs::HasGlyph( Uint16 ch )
{
	int index = FT_Get_Char_Index( m_ftFace, ch );
	if( index == 0 )
		return false;

	return true;
}

//____ GetGlyph() _____________________________________________________________

WgGlyph * WgVectorGlyphs::GetGlyph( Uint16 ch, int size )
{
	// Sanity check

	if( size > WG_MAX_FONTSIZE || size < 0 )
		return 0;

	// Get cached glyph if we have one

	CacheSlot * pSlot = FindSlotInIndex( ch, size );
	if( pSlot == 0 )
	{
		FT_Error err;

		//-----------------------------------------
		// Get empty cache slot and fill with glyph
		//-----------------------------------------

		// Set size for FreeType

		if( m_ftCharSize != size )
		{
			err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
			if( err )
			{
				m_ftCharSize = 0;
				return 0;
			}

			m_ftCharSize = size;
		}

		// Load Glyph

		err = FT_Load_Char( m_ftFace, ch, m_renderFlags );
		if( err )
			return 0;

		// Get some details about the glyph

		int width = m_ftFace->glyph->bitmap.width;
		int height = m_ftFace->glyph->bitmap.rows;

		int advance = (m_ftFace->glyph->linearHoriAdvance + 32768) >> 16;
		int xBearing = m_ftFace->glyph->bitmap_left;
		int yBearing = -m_ftFace->glyph->bitmap_top;

		// Get a cache slot

		pSlot = GetCacheSlot( width, height );
		if( pSlot == 0 )
			return 0;

		// Fill in glyph details

		pSlot->pOwner = this;
		pSlot->size = size;
		pSlot->character = ch;

		pSlot->glyph.advance = advance;
		pSlot->glyph.bearingX = xBearing;
		pSlot->glyph.bearingY = yBearing;
		pSlot->glyph.kerningIndex = FT_Get_Char_Index( m_ftFace, ch );
		pSlot->glyph.pSurf = pSlot->pSurf->pSurf;
		pSlot->glyph.rect = WgRect(pSlot->rect.x, pSlot->rect.y, width, height);

		//

		CopyBitmap( &m_ftFace->glyph->bitmap, pSlot );	// Copy our glyph bitmap to the slot
		AddSlotToIndex( ch, size, pSlot );				// Put a pointer in our index
	}

	TouchSlot( pSlot );								// Notify cache that we have accessed this one
	return &pSlot->glyph;
}



//____ CopyBitmap() ____________________________________________________________

// Currently only supports 32-bit RGBA surfaces!

void WgVectorGlyphs::CopyBitmap( FT_Bitmap * pBitmap, CacheSlot * pSlot )
{
	WgSurface * pSurf = pSlot->glyph.pSurf;

	unsigned char * pBuffer = (unsigned char*) pSurf->Lock( WgSurface::WRITE_ONLY );
	assert( pBuffer != 0 );
	assert( pSurf->GetPixelFormat()->type == WgSurface::RGBA_8 );

	int dest_pitch = pSurf->GetPitch();

	// Copy glyph bitmap into alpha channel of slot, making sure to clear any
	// left over area of slots alpha channel.

	unsigned char * pDest = pBuffer + dest_pitch*pSlot->rect.y + 4*pSlot->rect.x;
	unsigned char * pSrc = pBitmap->buffer;


	switch( m_renderFlags )
	{
		case (FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO):
			CopyA1ToRGBA8( pBitmap->buffer, pBitmap->width, pBitmap->rows, pBitmap->pitch, pDest, pSlot->rect.w, pSlot->rect.h, dest_pitch );
			break;
		case (FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL):
		case (FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT):
			CopyA8ToRGBA8( pBitmap->buffer, pBitmap->width, pBitmap->rows, pBitmap->pitch, pDest, pSlot->rect.w, pSlot->rect.h, dest_pitch );
			break;

		default:
			assert( false );
	}


	// Testcode
/*
	int i = 128;
	for( unsigned int y = 0 ; y < pSurf->GetHeight() ; y++ )
	{
		for( unsigned int x = 0 ; x < pSurf->GetWidth() ; x++ )
			pBuffer[y*dest_pitch + x] = i++;
	}
*/

	pSurf->Unlock();
}


//____ CopyA8ToRGBA8() _____________________________________________________

void WgVectorGlyphs::CopyA8ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, 
								    Uint8 * pDest, int dest_width, int dest_height, int dest_pitch )
{

	int y = 0;
	for( ; y < src_height ; y++ )
	{
		int x = 0;
		for( ; x < src_width ; x++ )
			pDest[x*4+3] = pSrc[x];

		for( ; x < dest_width ; x++ )
			pDest[x*4+3] = 0;

		pSrc  += src_pitch;
		pDest += dest_pitch;
	}

	for( ; y < dest_height ; y++ )
	{
		for( int x = 0 ; x < dest_width ; x++ )
			pDest[x*4+3] = 0;

		pDest += dest_pitch;
	}
}

//____ CopyA1ToRGBA8() _____________________________________________________

void WgVectorGlyphs::CopyA1ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch, 
								    Uint8 * pDest, int dest_width, int dest_height, int dest_pitch )
{
	Uint8 lookup[2] = { 0, 255 };

	int y = 0;
	for( ; y < src_height ; y++ )
	{
		
		int x = 0;
		for( ; x < src_width ; x++ )
		{
			pDest[x*4+3] = lookup[(((pSrc[x>>3])<<(x&7))&0xFF)>>7];
		}

		for( ; x < dest_width ; x++ )
			pDest[x*4+3] = 0;

		pSrc  += src_pitch;
		pDest += dest_pitch;
	}

	for( ; y < dest_height ; y++ )
	{
		for( int x = 0 ; x < dest_width ; x++ )
			pDest[x*4+3] = 0;

		pDest += dest_pitch;
	}
}



//___ AddSlotToIndex() ________________________________________________________

void WgVectorGlyphs::AddSlotToIndex( Uint16 ch, int size, CacheSlot * pSlot )
{
	if( m_cachedGlyphsIndex[size] == 0 )
	{
		CacheSlot *** p = new CacheSlot**[256];
		memset( p, 0, 256*sizeof(CacheSlot**) );
		
		m_cachedGlyphsIndex[size] = p;
	}

	if( m_cachedGlyphsIndex[size][ch>>8] == 0 )
	{
		CacheSlot ** p = new CacheSlot*[256];
		memset( p, 0, 256*sizeof(CacheSlot*) );
		
		m_cachedGlyphsIndex[size][ch>>8] = p;
	}
	
	assert( m_cachedGlyphsIndex[size][ch>>8][ch&0xFF] == 0 );

	m_cachedGlyphsIndex[size][ch>>8][ch&0xFF] = pSlot;
}


//____ RemoveSlotFromIndex() __________________________________________________

void WgVectorGlyphs::RemoveSlotFromIndex( const CacheSlot * pSlot )
{
	
	int	size	= pSlot->size;
	Uint16 ch	= pSlot->character;

	assert( FindSlotInIndex(ch, size) != 0 );

	m_cachedGlyphsIndex[size][ch>>8][ch&0xFF] = 0;
}



//____ SetSurfaceFactory() ____________________________________________________

void WgVectorGlyphs::SetSurfaceFactory( WgSurfaceFactory * pFactory )
{
	s_pSurfaceFactory = pFactory;
}


//____ ClearCache() ___________________________________________________________

void WgVectorGlyphs::ClearCache()
{
	for( int i = 0 ; i < GLYPH_SLOT_SIZES ; i++ )
	{
		CacheSlot * p = s_cacheSlots[i].getFirst();
		while( p )
		{
			p->pOwner->SlotLost( p );
			p = p->getNext();
		}

		s_cacheSlots[i].clear();
	}

	s_cacheSurfaces.clear();
}


//____ GetCacheSlot() _________________________________________________________

WgVectorGlyphs::CacheSlot * WgVectorGlyphs::GetCacheSlot( int width, int height )
{
	// Calculate size and index

	int size = ((width>height ? width:height)+GLYPH_PIXEL_SIZE_QUANTIZATION-1);

	if( size < MIN_GLYPH_PIXEL_SIZE )
		size = MIN_GLYPH_PIXEL_SIZE;

	assert( size <= MAX_GLYPH_PIXEL_SIZE );

	int index = (size-MIN_GLYPH_PIXEL_SIZE) / GLYPH_PIXEL_SIZE_QUANTIZATION;

	// Make sure we have 

	CacheSlot * pSlot = s_cacheSlots[index].getLast();
	if( pSlot == 0 || pSlot->character != 0 )
	{
		AddCacheSlots( &s_cacheSlots[index], WgSize(size,size), 16 );
		pSlot = s_cacheSlots[index].getLast();
	}

	return pSlot;
}


//____ AddCacheSlots() ___________________________________________________
/*
	Creates a new cache surface of 2^x size big enough to at least hold the 
	specified minimum amount of slots. Fills the surface with white and alpha 0.
	Creates all slots that can fit into this surface and adds them to the specified chain.
*/

int WgVectorGlyphs::AddCacheSlots( WgChain<CacheSlot> * pChain, const WgSize& slotSize, int minSlots )
{
	// Create and add the cache surface

	WgSize texSize = CalcTextureSize( slotSize, 16 );

	WgSurface * pSurf = s_pSurfaceFactory->CreateSurface( texSize );
	pSurf->Fill( WgColor( 255,255,255,0 ) );

	CacheSurf * pCache = new CacheSurf( pSurf );
	s_cacheSurfaces.push_back( pCache );

	// Create the slots

	WgRect	slot( 0, 0, slotSize );
	int		nSlots = 0;

	for( slot.y = 0 ; slot.y + slotSize.h < texSize.h ; slot.y += slotSize.h + 1 )
	{
		for( slot.x = 0 ; slot.x + slotSize.w < texSize.w ; slot.x += slotSize.w + 1 )
		{
			CacheSlot * pSlot = new CacheSlot( pCache, slot );
			pChain->push_back(pSlot);
			nSlots++;
		}
	}

	return nSlots;
}



//____ MaxSlotsInSurface() ____________________________________________________

int WgVectorGlyphs::MaxSlotsInSurface( const WgSize& surf, const WgSize& slot )
{
	int rows = (surf.w+1)/(slot.w+1);			// +1 since we need one pixel spacing between each slot.
	int columns = (surf.h+1)/(surf.h+1);

	return rows*columns;
}


//____ CalcTextureSize() ______________________________________________________

WgSize WgVectorGlyphs::CalcTextureSize( const WgSize& slotSize, int nSlots )
{
	WgSize	surfSize( 128, 128 );

	while( MaxSlotsInSurface(surfSize, slotSize) < nSlots )
	{
		if( surfSize.w > surfSize.h )
			surfSize.h *= 2;
		else if( surfSize.w < surfSize.h )
			surfSize.w *= 2;
		else
		{
			if( MaxSlotsInSurface( WgSize( surfSize.w, surfSize.h*2 ), slotSize ) > 
				MaxSlotsInSurface( WgSize( surfSize.w*2, surfSize.h ), slotSize ) )
				surfSize.h *= 2;
			else
				surfSize.w *= 2;
		}
	}

	return surfSize;
}




WgVectorGlyphs::CacheSurf::~CacheSurf()
{ 
	delete pSurf; 
}


#endif //WG_USE_FREETYPE