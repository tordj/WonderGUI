
#include <wg_util.h>
#include <wg_geo.h>
#include <wg_blockset.h>
#include <wg_surface.h>


bool WgUtil::AdjustScaledArea(const WgBlock& block, WgRect& area)
{
	if( block.IsFixedSize() )
	{
		int areaW = area.w;
		int areaH = area.h;

		int blockW = block.GetWidth();
		int blockH = block.GetHeight();

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

	const WgBorders& borders = block.GetBorders();

	int areaW = area.w - borders.width();
	int areaH = area.h - borders.height();

	int blockW = block.GetWidth() - borders.width();
	int blockH = block.GetHeight() - borders.height();

	if(areaW <= 0 || areaH <= 0 || blockW <= 0 || blockH <= 0)
		return false;

	int adjustedW = borders.width();
	int adjustedH = borders.height();

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
	if( !area.contains(ofs.x,ofs.y) )
		return false;

	if( block.IsOpaque() )
		return true;

	// Make cordinates relative area.

	ofs.x -= area.x;
	ofs.y -= area.y;


	const WgBorders& borders = block.GetBorders();

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
		ofs.x = block.GetWidth() - (area.w - ofs.x);
	}
	else if( xSection == 1 )
	{
		int tileAreaWidth = block.GetWidth() - borders.width();

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
			double screenWidth = area.w - borders.width();	// Width of stretch-area on screen.
			ofs.x = (int) ((ofs.x-borders.left)/screenWidth * tileAreaWidth + borders.left);
		}
	}


	// Convert ofs.y to Y-offset in bitmap, taking stretch/tile section into account.

	if( ySection == 2 )
	{
		ofs.y = block.GetHeight() - (area.h - ofs.y);
	}
	else if( ySection == 1 )
	{
		int tileAreaHeight = block.GetHeight() - borders.height();

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
			double screenHeight = area.h - borders.height();	// Height of stretch-area on screen.
			ofs.y = (int) ((ofs.y-borders.top)/screenHeight * tileAreaHeight + borders.top);
		}
	}

	Uint8 alpha = block.GetSurface()->GetOpacity(block.GetRect().x+ofs.x, block.GetRect().y+ofs.y);

	if( alpha )
		return true;
	else
		return false;
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

//____ LocationToOfs() ________________________________________________________

WgCoord WgUtil::LocationToOfs( WgLocation location, WgSize base )
{
	switch( location )
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
