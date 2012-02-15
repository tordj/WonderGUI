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

WgVectorGlyphs::WgVectorGlyphs( char* pTTF_File, int bytes, int faceIndex )
{
	m_pData = pTTF_File;
	m_ftCharSize	= 0;
	m_accessCounter = 0;
	m_sizeOffset	= 0;

	for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
	{
		m_cachedGlyphsIndex[i] = 0;
		m_whitespaceAdvance[i] = 0;
	}

	FT_Error err = FT_New_Memory_Face(	WgBase::GetFreeTypeLibrary(),
										(const FT_Byte *)pTTF_File,
										bytes,
										0,
										&m_ftFace );

	if( err )
	{
//		int x = 0;
		//TODO: Error handling...
	}


	SetRenderMode( CRISP_EDGES );
}

//____ Destructor _____________________________________________________________

WgVectorGlyphs::~WgVectorGlyphs()
{
	for( int size = 0 ; size <= WG_MAX_FONTSIZE ; size++ )
	{
		if( m_cachedGlyphsIndex[size] != 0 )
		{
			for( int page = 0 ; page < 256 ; page++ )
			{
				if( m_cachedGlyphsIndex[size][page] != 0 )
					delete [] m_cachedGlyphsIndex[size][page];
			}

			delete [] m_cachedGlyphsIndex[size];
		}
	}

	FT_Done_Face( m_ftFace );
	delete[] m_pData;
}

//____ _setCharSize() __________________________________________________________

bool WgVectorGlyphs::_setCharSize( int size )
{
		FT_Error err = FT_Set_Char_Size( m_ftFace, size*64, 0, 0,0 );
//		FT_Error err = FT_Set_Pixel_Sizes( m_ftFace, 0, size );
		if( err )
		{
			m_ftCharSize = 0;
			return false;
		}

		switch( m_renderMode[size] )
		{
			case MONOCHROME:
				m_renderFlags = FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO;
				break;
			case CRISP_EDGES:
				m_renderFlags = FT_LOAD_TARGET_NORMAL;
				break;
			case BEST_SHAPES:
				m_renderFlags = FT_LOAD_TARGET_LIGHT;
				break;

			default:
				break;
		}

		m_ftCharSize = size;
		return true;
}



//____ SetRenderMode() ________________________________________________________

bool WgVectorGlyphs::SetRenderMode( RenderMode mode, int startSize, int endSize )
{
	if( startSize < 0 || startSize > endSize || startSize > WG_MAX_FONTSIZE )
		return false;

	if( endSize > WG_MAX_FONTSIZE )
		endSize = WG_MAX_FONTSIZE;

	for( int i = startSize ; i <= endSize ; i++ )
		m_renderMode[i] =mode;

	// Force update of m_renderFlags if current size is affected

	if( m_ftCharSize >= startSize && m_ftCharSize <= endSize )
		_setCharSize( m_ftCharSize );

	return true;
}

//____ GetKerning() ___________________________________________________________

int WgVectorGlyphs::GetKerning( WgGlyphPtr pLeftGlyph, WgGlyphPtr pRightGlyph, int size )
{
	size += m_sizeOffset;

	if( !pLeftGlyph || !pRightGlyph )
		return 0;

	// Set size for FreeType

	if( m_ftCharSize != size )
		if( !_setCharSize( size ) )
			return 0;

	// Get kerning info

	FT_Vector	delta;
	FT_Get_Kerning( m_ftFace, pLeftGlyph->KerningIndex(), pRightGlyph->KerningIndex(), FT_KERNING_DEFAULT, &delta );

	return delta.x >> 6;
}

//____ GetWhitespaceAdvance() _________________________________________________

int WgVectorGlyphs::GetWhitespaceAdvance( int size )
{
	size += m_sizeOffset;

	if( !m_whitespaceAdvance[size] )
	{
		FT_Error err;

		// Set size for FreeType

		if( m_ftCharSize != size )
			if( !_setCharSize( size ) )
				return 0;

		// Load whitespace glyph

		err = FT_Load_Char( m_ftFace, ' ', FT_LOAD_RENDER );
		if( err )
			return 0;

		// Get and return advance
		m_whitespaceAdvance[size] = m_ftFace->glyph->advance.x >> 6;
	}

	return m_whitespaceAdvance[size];
}

//____ GetHeight() ____________________________________________________________

int WgVectorGlyphs::GetHeight( int size )
{
	size += m_sizeOffset;

	if( m_ftCharSize != size )
		if( !_setCharSize( size ) )
			return 0;

	return (m_ftFace->size->metrics.ascender - m_ftFace->size->metrics.descender) >> 6;
}

//____ GetLineSpacing() ____________________________________________________________

int WgVectorGlyphs::GetLineSpacing( int size )
{
	size += m_sizeOffset;

	if( m_ftCharSize != size )
		if( !_setCharSize( size ) )
			return 0;

	return (m_ftFace->size->metrics.height) >> 6;
}


//____ GetBaseline() ____________________________________________________________

int WgVectorGlyphs::GetBaseline( int size )
{
	size += m_sizeOffset;

	if( m_ftCharSize != size )
		if( !_setCharSize( size ) )
			return 0;

	return (m_ftFace->size->metrics.ascender) >> 6;
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
	size += m_sizeOffset;

	return m_ftFace->size->metrics.max_advance >> 6;
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

WgGlyphPtr WgVectorGlyphs::GetGlyph( Uint16 ch, int size )
{
	size += m_sizeOffset;

	// Sanity check

	if( size > WG_MAX_FONTSIZE || size < 0 )
		return 0;

	// Get cached glyph if we have one

	Glyph * pGlyph = _findGlyph( ch, size );
	if( pGlyph == 0 )
	{
		FT_Error err;

		// Set size for FreeType

		if( m_ftCharSize != size )
			if( !_setCharSize( size ) )
				return 0;

		// Load Glyph

		FT_UInt char_index = FT_Get_Char_Index( m_ftFace, ch );
		if( char_index == 0 )
			return 0;			// We got index for missing glyph.

		err = FT_Load_Glyph( m_ftFace, char_index, m_renderFlags );
		if( err )
			return 0;

		// Get some details about the glyph

		int advance = m_ftFace->glyph->advance.x >> 6;

		// Get a Glyph object and fill in details

		pGlyph = _addGlyph( ch, size, advance, char_index );
	}

	return pGlyph;
}



/*
WgGlyphPtr WgVectorGlyphs::GetGlyph( Uint16 ch, int size )
{
	size += m_sizeOffset;

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
			if( !SetCharSize( size ) )
				return 0;

		// Load Glyph

		FT_UInt char_index = FT_Get_Char_Index( m_ftFace, ch );
		if( char_index == 0 )
			return 0;			// We got index for missing glyph.

		err = FT_Load_Glyph( m_ftFace, char_index, m_renderFlags );
		if( err )
			return 0;

		// Get some details about the glyph

		int width = m_ftFace->glyph->bitmap.width;
		int height = m_ftFace->glyph->bitmap.rows;

		int advance = m_ftFace->glyph->advance.x >> 6;
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
		pSlot->glyph.kerningIndex = char_index;
		pSlot->glyph.pSurf = pSlot->pSurf->pSurf;
		pSlot->glyph.rect = WgRect(pSlot->rect.x, pSlot->rect.y, width, height);

		//

		CopyBitmap( &m_ftFace->glyph->bitmap, pSlot );	// Copy our glyph bitmap to the slot
		AddSlotToIndex( ch, size, pSlot );				// Put a pointer in our index
	}

	TouchSlot( pSlot );								// Notify cache that we have accessed this one
	return &pSlot->glyph;
}
*/

//____ _______________________________________________________

WgVectorGlyphs::CacheSlot * WgVectorGlyphs::_generateBitmap( Glyph * pGlyph )
{
	FT_Error err;

	// Set size for FreeType

	if( m_ftCharSize != pGlyph->m_size )
		if( !_setCharSize( pGlyph->m_size ) )
			return 0;

	// Load Glyph

	err = FT_Load_Glyph( m_ftFace, pGlyph->KerningIndex(), FT_LOAD_RENDER | m_renderFlags );
	if( err )
		return 0;

	// Get some details about the glyph

	int width = m_ftFace->glyph->bitmap.width;
	int height = m_ftFace->glyph->bitmap.rows;

	// Get a cache slot

	CacheSlot * pSlot = GetCacheSlot( width, height );

	if( pSlot )
	{
		// Fill in missing slot details

		pSlot->pGlyph = pGlyph;
		pSlot->bitmap.rect = WgRect(pSlot->rect.x, pSlot->rect.y, width, height);
		pSlot->bitmap.bearingX = m_ftFace->glyph->bitmap_left;
		pSlot->bitmap.bearingY = -m_ftFace->glyph->bitmap_top;

		//

		_copyBitmap( &m_ftFace->glyph->bitmap, pSlot );	// Copy our glyph bitmap to the slot
	}

	return pSlot;
}



//____ _copyBitmap() ____________________________________________________________

// Currently only supports 32-bit RGBA surfaces!

void WgVectorGlyphs::_copyBitmap( FT_Bitmap * pBitmap, CacheSlot * pSlot )
{
	WgSurface * pSurf = pSlot->bitmap.pSurface;

	unsigned char * pDest = (unsigned char*) pSurf->LockRegion( WG_WRITE_ONLY, pSlot->bitmap.rect );
	assert( pDest != 0 );
	assert( pSurf->PixelFormat()->type == WG_PIXEL_ARGB_8 );

	int dest_pitch = pSurf->Pitch();

	// Copy glyph bitmap into alpha channel of slot, making sure to clear any
	// left over area of slots alpha channel.


	switch( m_renderFlags )
	{
		case (FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO):
			_copyA1ToRGBA8( pBitmap->buffer, pBitmap->width, pBitmap->rows, pBitmap->pitch, pDest, pSlot->rect.w, pSlot->rect.h, dest_pitch );
			break;
		case (FT_LOAD_TARGET_NORMAL):
		case (FT_LOAD_TARGET_LIGHT):
			_copyA8ToRGBA8( pBitmap->buffer, pBitmap->width, pBitmap->rows, pBitmap->pitch, pDest, pSlot->rect.w, pSlot->rect.h, dest_pitch );
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


//____ _copyA8ToRGBA8() _____________________________________________________

void WgVectorGlyphs::_copyA8ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch,
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

//____ _copyA1ToRGBA8() _____________________________________________________

void WgVectorGlyphs::_copyA1ToRGBA8( const Uint8 * pSrc, int src_width, int src_height, int src_pitch,
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

//___ _addGlyph() ________________________________________________________

WgVectorGlyphs::Glyph * WgVectorGlyphs::_addGlyph( Uint16 ch, int size, int advance, Uint32 kerningIndex )
{
	if( m_cachedGlyphsIndex[size] == 0 )
	{
		Glyph ** p = new Glyph*[256];
		memset( p, 0, 256*sizeof(Glyph*) );

		m_cachedGlyphsIndex[size] = p;
	}

	if( m_cachedGlyphsIndex[size][ch>>8] == 0 )
	{
		Glyph * p = new Glyph[256];

		m_cachedGlyphsIndex[size][ch>>8] = p;
	}

	assert( !m_cachedGlyphsIndex[size][ch>>8][ch&0xFF].IsInitialized() );

	m_cachedGlyphsIndex[size][ch>>8][ch&0xFF] = Glyph( ch, size, advance, kerningIndex, this );

	return &m_cachedGlyphsIndex[size][ch>>8][ch&0xFF];
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
		CacheSlot * p = s_cacheSlots[i].First();
		while( p )
		{
			if( p->pGlyph )
				p->pGlyph->SlotLost();
			p = p->Next();
		}

		s_cacheSlots[i].Clear();
	}

	s_cacheSurfaces.Clear();
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

	CacheSlot * pSlot = s_cacheSlots[index].Last();
	if( pSlot == 0 || pSlot->pGlyph != 0 )
	{
		AddCacheSlots( &s_cacheSlots[index], WgSize(size,size), 16 );
		pSlot = s_cacheSlots[index].Last();
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
	s_cacheSurfaces.PushBack( pCache );

	// Create the slots

	WgRect	slot( 0, 0, slotSize );
	int		nSlots = 0;

	for( slot.y = 0 ; slot.y + slotSize.h < texSize.h ; slot.y += slotSize.h + 1 )
	{
		for( slot.x = 0 ; slot.x + slotSize.w < texSize.w ; slot.x += slotSize.w + 1 )
		{
			CacheSlot * pSlot = new CacheSlot( pCache, slot );
			pChain->PushBack(pSlot);
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

WgVectorGlyphs::Glyph::Glyph()
: WgGlyph( 0, 0, 0 )
{
	m_pSlot = 0;
	m_size = 0;
	m_character = 0;
}


WgVectorGlyphs::Glyph::Glyph( Uint16 character, Uint16 size, int advance, Uint32 kerningIndex, WgGlyphSet * pGlyphSet )
: WgGlyph( advance, kerningIndex, pGlyphSet )
{
	m_pSlot = 0;
	m_size = size;
	m_character = character;
}

WgVectorGlyphs::Glyph::~Glyph()
{
	if(  m_pSlot != 0 )
	{
		m_pSlot->pGlyph = 0;
		m_pSlot->access = 0;

		m_pSlot->MoveLast();
	}
}

const WgGlyphBitmap * WgVectorGlyphs::Glyph::GetBitmap()
{
	if( !m_pSlot )
	{
		m_pSlot = ((WgVectorGlyphs*)m_pGlyphSet)->_generateBitmap( this );
	}

	((WgVectorGlyphs*)m_pGlyphSet)->_touchSlot(m_pSlot);
	return &m_pSlot->bitmap;
}



#endif //WG_USE_FREETYPE
