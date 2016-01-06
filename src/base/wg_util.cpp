
#include <wg_util.h>
#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_widget.h>
#include <wg_panel.h>

namespace wg 
{
	
	//____ markTestStretchRect() __________________________________________________
	
	bool WgUtil::markTestStretchRect( Coord ofs, const Surface_p& pSurface, const Rect& source, const Rect& area, int opacityTreshold )
	{
		// Sanity check & shortcuts.
		if( !pSurface || !area.contains(ofs.x,ofs.y) || source.isEmpty() || area.isEmpty() || opacityTreshold > 255 )
			return false;
	
		if( pSurface->isOpaque() || opacityTreshold <= 0 )
			return true;
	
		// Make cordinates relative area.
	
		ofs.x -= area.x;
		ofs.y -= area.y;
	
		// Convert offset in area to offset in bitmap.
	
		ofs.x = (int) (ofs.x/((double)area.w) * source.w);
		ofs.y = (int) (ofs.y/((double)area.h) * source.h);
	
		// Do alpha test
	
		int alpha = pSurface->alpha(source.x+ofs.x, source.y+ofs.y);
	
		return (alpha >= opacityTreshold);
	}
	
	
	
	//____ pixelTypeToFormat() _____________________________________________________
	
	bool WgUtil::pixelTypeToFormat( PixelType type, PixelFormat& wFormat )
	{
		switch( type )
		{
			case PixelType::BGR_8:
				wFormat.type = type;
				wFormat.bits = 24;

				wFormat.R_bits = 8;
				wFormat.G_bits = 8;
				wFormat.B_bits = 8;
				wFormat.A_bits = 0;

	#if IS_LITTLE_ENDIAN
				wFormat.R_mask = 0xFF0000;
				wFormat.G_mask = 0xFF00;
				wFormat.B_mask = 0xFF;
				wFormat.A_mask = 0x0;

				wFormat.R_shift = 16;
				wFormat.G_shift = 8;
				wFormat.B_shift = 0;
				wFormat.A_shift = 0;
	#else
				wFormat.R_mask = 0xFF;
				wFormat.G_mask = 0xFF00;
				wFormat.B_mask = 0xFF0000;
				wFormat.A_mask = 0x0;

				wFormat.R_shift = 0;
				wFormat.G_shift = 8;
				wFormat.B_shift = 16;
				wFormat.A_shift = 0;
	#endif

				return true;

			case PixelType::BGRA_8:
				wFormat.type = type;
				wFormat.bits = 32;

				wFormat.R_bits = 8;
				wFormat.G_bits = 8;
				wFormat.B_bits = 8;
				wFormat.A_bits = 8;

	#if IS_LITTLE_ENDIAN
				wFormat.A_mask = 0xFF000000;
				wFormat.R_mask = 0xFF0000;
				wFormat.G_mask = 0xFF00;
				wFormat.B_mask = 0xFF;

				wFormat.A_shift = 24;
				wFormat.R_shift = 16;
				wFormat.G_shift = 8;
				wFormat.B_shift = 0;
	#else
				wFormat.A_mask = 0xFF;
				wFormat.R_mask = 0xFF00;
				wFormat.G_mask = 0xFF0000;
				wFormat.B_mask = 0xFF000000;

				wFormat.A_shift = 0;
				wFormat.R_shift = 8;
				wFormat.G_shift = 16;
				wFormat.B_shift = 24;
	#endif

				return true;

			default:
				wFormat.type = PixelType::Unknown;
				wFormat.bits = 0;

				wFormat.R_mask = 0;
				wFormat.G_mask = 0;
				wFormat.B_mask = 0;
				wFormat.A_mask = 0;

				wFormat.R_shift = 0;
				wFormat.G_shift = 0;
				wFormat.B_shift = 0;
				wFormat.A_shift = 0;

				wFormat.R_bits = 0;
				wFormat.G_bits = 0;
				wFormat.B_bits = 0;
				wFormat.A_bits = 0;
				return false;
		}
			
		
	}
	
	//____ sizeFromPolicy() __________________________________________________________
	
	int WgUtil::sizeFromPolicy( int defaultSize, int specifiedSize, SizePolicy policy )
	{
		switch( policy )
		{
			case SizePolicy::Default:
				return defaultSize;
			case SizePolicy::Bound:
				return specifiedSize;
			case SizePolicy::Confined:
				if( defaultSize > specifiedSize )
					return specifiedSize;
			case SizePolicy::Expanded:
				if( defaultSize < specifiedSize )
					return specifiedSize;
		}
		return defaultSize;
	}
	
	//____ Checksum8::add() ________________________________________________________
	
	void WgUtil::Checksum8::add( const void * pData, uint32_t nBytes )
	{
		uint32_t x = remainder;
	
		for( uint32_t i = 0 ; i < nBytes ; i++ )
			x = ((x << 8) + ((uint8_t*)pData)[i])%dividend;
	
		remainder = x;
	}
	
	
	
	//____ decodeBase64() _________________________________________________________
	
	uint32_t WgUtil::decodeBase64( const char * pSrc, uint32_t nIn, char * pDest )
	{
		const static unsigned char conv[256] =
						{	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
							0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
							0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
							0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
							0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
							0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
							0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
							0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
							0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
							0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	
		if( nIn == 0 || (nIn%4)!= 0 )
			return 0;
	
	
	
		for( unsigned int i = 0 ; i < nIn/4 ; i++ )
		{
			uint32_t	acc = 0;
	
			unsigned char x = conv[(uint8_t) * pSrc++];
			if( x == 0xFF )	return 0;
			acc += x;
			acc <<= 6;
	
			x = conv[(uint8_t) * pSrc++];
			if( x == 0xFF ) return 0;
			acc += x;
			acc <<= 6;
	
			x = conv[(uint8_t) * pSrc++];
			if( x == 0xFF ) return 0;
			acc += x;
			acc <<= 6;
	
			x = conv[(uint8_t) * pSrc++];
			if( x == 0xFF ) return 0;
			acc += x;
	
			* pDest++ = (char) (acc >> 16);
			* pDest++ = (char) (acc >> 8);
			* pDest++ = (char) acc;
		}
	
		uint32_t nChar = (nIn/4) * 3;
	
		if( pSrc[-2] == '=' )
			nChar-=2;
		else if( pSrc[-1] == '=' )
			nChar--;
	
		return nChar;
	}
	
	//____ origoToOfs() ________________________________________________________
	
	Coord WgUtil::origoToOfs( Origo origo, Size base )
	{
		switch( origo )
		{
			default:
			case Origo::NorthWest:
				return Coord(0,0);
	
			case Origo::North:
				return Coord( base.w/2,0 );
	
			case Origo::NorthEast:
				return Coord( base.w,0 );
	
			case Origo::East:
				return Coord( base.w, base.h/2 );
	
			case Origo::SouthEast:
				return Coord( base.w, base.h );
	
			case Origo::South:
				return Coord( base.w/2, base.h );
	
			case Origo::SouthWest:
				return Coord( 0, base.h );
	
			case Origo::West:
				return Coord( 0, base.h/2 );
	
			case Origo::Center:
				return Coord( base.w/2, base.h/2 );
		}
	}
	
	//____ origoToRect() ________________________________________________________
	
	Rect WgUtil::origoToRect( Origo origo, Size base, Size rect )
	{
		switch( origo )
		{
			default:
			case Origo::NorthWest:
				return Rect(0,0, rect);
	
			case Origo::North:
				return Rect( base.w/2 - rect.w/2, 0, rect );
	
			case Origo::NorthEast:
				return Rect( base.w - rect.w, 0, rect );
	
			case Origo::East:
				return Rect( base.w - rect.w, base.h/2 - rect.h/2, rect );
	
			case Origo::SouthEast:
				return Rect( base.w - rect.w, base.h - rect.h, rect );
	
			case Origo::South:
				return Rect( base.w/2 - rect.w/2, base.h - rect.h, rect );
	
			case Origo::SouthWest:
				return Rect( 0, base.h - rect.h, rect );
	
			case Origo::West:
				return Rect( 0, base.h/2 - rect.h/2, rect );
	
			case Origo::Center:
				return Rect( base.w/2 - rect.w/2, base.h/2 - rect.h/2, rect );
		}
	}

} // namespace wg
