
#include <wg_util.h>
#include <wg_geo.h>
#include <wg_blockset.h>
#include <wg_surface.h>
#include <wg_gizmo.h>

bool WgUtil::AdjustScaledArea(const WgBlock& block, WgRect& area)
{
	if( block.IsFixedSize() )
	{
		int areaW = area.w;
		int areaH = area.h;

		int blockW = block.Width();
		int blockH = block.Height();

		if( areaW > blockW )
		{
			area.x += (areaW - blockW)/2;
			area.w = blockW;
		}

		if( areaH > blockH )
		{
			area.y += (areaH - blockH)/2;
			area.h = blockH;
		}

		return true;
	}


	if(!block.IsScaled())
		return false;

	const WgBorders& borders = block.GfxBorders();

	int areaW = area.w - borders.Width();
	int areaH = area.h - borders.Height();

	int blockW = block.Width() - borders.Width();
	int blockH = block.Height() - borders.Height();

	if(areaW <= 0 || areaH <= 0 || blockW <= 0 || blockH <= 0)
		return false;

	int adjustedW = borders.Width();
	int adjustedH = borders.Height();

	if(areaW * blockH > blockW * areaH)
	{
		adjustedH += areaH;
		adjustedW += int(float(adjustedH * blockW) / float(blockH));
	}
	else
	{
		adjustedW += areaW;
		adjustedH += int(float(adjustedW * blockH) / float(blockW));
	}

	area.x += (area.w - adjustedW) >> 1;
	area.y += (area.h - adjustedH) >> 1;
	area.w = adjustedW;
	area.h = adjustedH;

	return true;
}

//____ MarkTestBlock() ________________________________________________________

bool WgUtil::MarkTestBlock( WgCoord ofs, const WgBlock& block, WgRect area )
{
	AdjustScaledArea(block, area);

	// Sanity check & shortcuts.
	if( !area.Contains(ofs.x,ofs.y) )
		return false;

	if( block.IsOpaque() )
		return true;

	// Make cordinates relative area.

	ofs.x -= area.x;
	ofs.y -= area.y;


	const WgBorders& borders = block.GfxBorders();

	// Determine in which section the cordinate is (0-2 for x and y).

	int	xSection = 0;
	int ySection = 0;

	if( ofs.x >= area.w - borders.right )
		xSection = 2;
	else if( ofs.x > borders.left )
		xSection = 1;

	if( ofs.y >= area.h - borders.bottom )
		ySection = 2;
	else if( ofs.y > borders.top )
		ySection = 1;


	// Convert ofs.x to X-offset in bitmap, taking stretch/tile section into account.

	if( xSection == 2 )
	{
		ofs.x = block.Width() - (area.w - ofs.x);
	}
	else if( xSection == 1 )
	{
		int tileAreaWidth = block.Width() - borders.Width();

		bool bTile;

		if( ySection == 0 )
			bTile = block.HasTiledTopBorder();
		else if( ySection == 1 )
			bTile = block.HasTiledCenter();
		else
			bTile = block.HasTiledBottomBorder();

		if( bTile )
			ofs.x = ((ofs.x - borders.left) % tileAreaWidth) + borders.left;
		else
		{
			double screenWidth = area.w - borders.Width();	// Width of stretch-area on screen.
			ofs.x = (int) ((ofs.x-borders.left)/screenWidth * tileAreaWidth + borders.left);
		}
	}


	// Convert ofs.y to Y-offset in bitmap, taking stretch/tile section into account.

	if( ySection == 2 )
	{
		ofs.y = block.Height() - (area.h - ofs.y);
	}
	else if( ySection == 1 )
	{
		int tileAreaHeight = block.Height() - borders.Height();

		bool bTile;

		if( xSection == 0 )
			bTile = block.HasTiledLeftBorder();
		else if( xSection == 1 )
			bTile = block.HasTiledCenter();
		else
			bTile = block.HasTiledRightBorder();

		if( bTile )
			ofs.y = ((ofs.y - borders.top) % tileAreaHeight) + borders.top;
		else
		{
			double screenHeight = area.h - borders.Height();	// Height of stretch-area on screen.
			ofs.y = (int) ((ofs.y-borders.top)/screenHeight * tileAreaHeight + borders.top);
		}
	}

	Uint8 alpha = block.Surface()->GetOpacity(block.Rect().x+ofs.x, block.Rect().y+ofs.y);

	if( alpha )
		return true;
	else
		return false;
}

//____ PixelTypeToFormat() _____________________________________________________

bool WgUtil::PixelTypeToFormat( WgPixelType type, WgPixelFormat& wFormat )
{
	switch( type )
	{
		case WG_PIXEL_RGB_8:
			wFormat.type = type;
			wFormat.bits = 24;

			wFormat.B_mask = 0xFF;
			wFormat.G_mask = 0xFF00;
			wFormat.R_mask = 0xFF0000;
			wFormat.A_mask = 0x0;

			wFormat.B_shift = 0;
			wFormat.G_shift = 8;
			wFormat.R_shift = 16;
			wFormat.A_shift = 0;

			wFormat.B_bits = 8;
			wFormat.G_bits = 8;
			wFormat.R_bits = 8;
			wFormat.A_bits = 0;
			return true;

		case WG_PIXEL_ARGB_8:
			wFormat.type = type;
			wFormat.bits = 32;

			wFormat.B_mask = 0xFF;
			wFormat.G_mask = 0xFF00;
			wFormat.R_mask = 0xFF0000;
			wFormat.A_mask = 0xFF000000;

			wFormat.B_shift = 0;
			wFormat.G_shift = 8;
			wFormat.R_shift = 16;
			wFormat.A_shift = 24;

			wFormat.B_bits = 8;
			wFormat.G_bits = 8;
			wFormat.R_bits = 8;
			wFormat.A_bits = 8;
			return true;

		default:
			wFormat.type = WG_PIXEL_UNKNOWN;
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

//____ SizeFromPolicy() __________________________________________________________

int WgUtil::SizeFromPolicy( int defaultSize, int specifiedSize, WgSizePolicy policy )
{
	switch( policy )
	{
		case WG_DEFAULT:
			return defaultSize;
		case WG_BOUND:
			return specifiedSize;
		case WG_CONFINED:
			if( defaultSize > specifiedSize )
				return specifiedSize;
		case WG_EXPANDED:
			if( defaultSize < specifiedSize )
				return specifiedSize;
	}
}

//____ SizeFromPolicy() ________________________________________________________

WgSize WgUtil::SizeFromPolicy( const WgGizmo * pGizmo, WgSize specifiedSize, WgSizePolicy widthPolicy, WgSizePolicy heightPolicy )
{
	WgSize	defaultSize = pGizmo->DefaultSize();

	WgSize	sz;

	switch( widthPolicy )
	{
		case WG_DEFAULT:
		{
			sz.h = SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
			sz.w = pGizmo->WidthForHeight(sz.h);
			break;
		case WG_BOUND:
			sz.w = specifiedSize.w;
			sz.h = SizeFromPolicy( pGizmo->HeightForWidth(sz.w), specifiedSize.h, heightPolicy );
			break;
		case WG_CONFINED:
			if( defaultSize.w > specifiedSize.w )
			{
				sz.w = specifiedSize.w;
				sz.h = SizeFromPolicy( pGizmo->HeightForWidth(sz.w), specifiedSize.h, heightPolicy );
			}
			else
			{
				sz.h = SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
				sz.w = pGizmo->WidthForHeight(sz.h);
				if( sz.w > specifiedSize.w )
					sz.w = specifiedSize.w;
			}
			break;
		case WG_EXPANDED:
			if( defaultSize.w < specifiedSize.w )
			{
				sz.w = specifiedSize.w;
				sz.h = SizeFromPolicy( pGizmo->HeightForWidth(sz.w), specifiedSize.h, heightPolicy );
			}
			else
			{
				sz.h = SizeFromPolicy( defaultSize.h, specifiedSize.h, heightPolicy );
				sz.w = pGizmo->WidthForHeight(sz.h);
				if( sz.w < specifiedSize.w )
					sz.w = specifiedSize.w;
			}
			break;
		}
	}

	return sz;
}



//____ Checksum8::Add() ________________________________________________________

void WgUtil::Checksum8::Add( const void * pData, Uint32 nBytes )
{
	Uint32 x = remainder;

	for( Uint32 i = 0 ; i < nBytes ; i++ )
		x = ((x << 8) + ((Uint8*)pData)[i])%dividend;

	remainder = x;
}



//____ DecodeBase64() _________________________________________________________

Uint32 WgUtil::DecodeBase64( const char * pSrc, Uint32 nIn, char * pDest )
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
		Uint32	acc = 0;

		unsigned char x = conv[(Uint8) * pSrc++];
		if( x == 0xFF )	return 0;
		acc += x;
		acc <<= 6;

		x = conv[(Uint8) * pSrc++];
		if( x == 0xFF ) return 0;
		acc += x;
		acc <<= 6;

		x = conv[(Uint8) * pSrc++];
		if( x == 0xFF ) return 0;
		acc += x;
		acc <<= 6;

		x = conv[(Uint8) * pSrc++];
		if( x == 0xFF ) return 0;
		acc += x;

		* pDest++ = (char) (acc >> 16);
		* pDest++ = (char) (acc >> 8);
		* pDest++ = (char) acc;
	}

	Uint32 nChar = (nIn/4) * 3;

	if( pSrc[-2] == '=' )
		nChar-=2;
	else if( pSrc[-1] == '=' )
		nChar--;

	return nChar;
}

//____ OrientationToOfs() ________________________________________________________

WgCoord WgUtil::OrientationToOfs( WgOrientation orientation, WgSize base )
{
	switch( orientation )
	{
		default:
		case WG_NORTHWEST:
			return WgCoord(0,0);

		case WG_NORTH:
			return WgCoord( base.w/2,0 );

		case WG_NORTHEAST:
			return WgCoord( base.w,0 );

		case WG_EAST:
			return WgCoord( base.w, base.h/2 );

		case WG_SOUTHEAST:
			return WgCoord( base.w, base.h );

		case WG_SOUTH:
			return WgCoord( base.w/2, base.h );

		case WG_SOUTHWEST:
			return WgCoord( 0, base.h );

		case WG_WEST:
			return WgCoord( 0, base.h/2 );

		case WG_CENTER:
			return WgCoord( base.w/2, base.h/2 );
	}
}

//____ OrientationToRect() ________________________________________________________

WgRect WgUtil::OrientationToRect( WgOrientation orientation, WgSize base, WgSize rect )
{
	switch( orientation )
	{
		default:
		case WG_NORTHWEST:
			return WgRect(0,0, rect);

		case WG_NORTH:
			return WgRect( base.w/2 - rect.w/2, 0, rect );

		case WG_NORTHEAST:
			return WgRect( base.w - rect.w, 0, rect );

		case WG_EAST:
			return WgRect( base.w - rect.w, base.h/2 - rect.h/2, rect );

		case WG_SOUTHEAST:
			return WgRect( base.w - rect.w, base.h - rect.h, rect );

		case WG_SOUTH:
			return WgRect( base.w/2 - rect.w/2, base.h - rect.h, rect );

		case WG_SOUTHWEST:
			return WgRect( 0, base.h - rect.h, rect );

		case WG_WEST:
			return WgRect( 0, base.h/2 - rect.h/2, rect );

		case WG_CENTER:
			return WgRect( base.w/2 - rect.w/2, base.h/2 - rect.h/2, rect );
	}
}
