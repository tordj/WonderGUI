
#include <wg_util.h>
#include <wg_geo.h>
#include <wg_blockset.h>
#include <wg3_surface.h>
#include <wg_widget.h>
#include <wg_panel.h>
#include <wg_gfxanim.h>
#include <wg_pen.h>


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

	const WgBorders& borders = block.CanvasFrame();

	int areaW = area.w - borders.width();
	int areaH = area.h - borders.height();

	int blockW = block.Width() - borders.width();
	int blockH = block.Height() - borders.height();

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

bool WgUtil::MarkTestBlock( WgCoord ofs, const WgBlock& block, WgRect canvas, int opacityTreshold )
{
	if (!block.IsValid())
		return false;

	AdjustScaledArea(block, canvas);

	// Sanity check & shortcuts.
	if( !canvas.contains(ofs) )
		return false;

	int alpha;
	if( block.IsOpaque() )
		return ( 255 >= opacityTreshold);

	// Make cordinates relative canvas.

	ofs.x -= canvas.x;
	ofs.y -= canvas.y;


	const WgBorders& canvasFrame = block.CanvasFrame();
	const WgBorders& sourceFrame = block.SourceFrame();

	int outputScale = block.OutputScale();
	int surfaceScale = block.SurfaceScale();

	WgSize dimensions = block.Size();	

	// Determine in which section the cordinate is (0-2 for x and y).

	int	xSection = 0;
	int ySection = 0;

	if( ofs.x >= canvas.w - canvasFrame.right )
		xSection = 2;
	else if( ofs.x > canvasFrame.left )
		xSection = 1;

	if( ofs.y >= canvas.h - canvasFrame.bottom )
		ySection = 2;
	else if( ofs.y > canvasFrame.top )
		ySection = 1;


	// Convert ofs.x to X-offset in bitmap, taking stretch/tile section into account.

    if( xSection == 0 )
    {
        ofs.x = (ofs.x * surfaceScale) / outputScale;
    }
	else if( xSection == 2 )
	{
        ofs.x = ofs.x - (canvas.w - canvasFrame.right);           // Offset in right border of canvas
        ofs.x = (ofs.x * surfaceScale) / outputScale;            // Scale from canvas to source coordinates
        ofs.x += dimensions.w - sourceFrame.right;          // Add offset for right border
        
//			ofs.x = dimensions.w - (canvas.w - ofs.x);
	}
	else if( xSection == 1 )
	{
		int tileAreaWidth = dimensions.w - sourceFrame.width();

        int canvasStretchWidth = canvas.w - canvasFrame.width();	// Width of stretch-area on screen.
        
        ofs.x = ofs.x - canvasFrame.left;               // Offset in middle section of canvas
//        ofs.x = (ofs.x * surfaceScale) / outputScale;        // Scale from canvas to source offset
        ofs.x = (int)((ofs.x / (float)canvasStretchWidth)*tileAreaWidth) + sourceFrame.left;
	}	

	// Convert ofs.y to Y-offset in bitmap, taking stretch/tile section into account.

    if( ySection == 0 )
    {
        ofs.y = (ofs.y * surfaceScale) / outputScale;
    }
	if( ySection == 2 )
	{
        ofs.y = ofs.y - (canvas.h - canvasFrame.bottom);           // Offset in bottom border of canvas
        ofs.y = (ofs.y * surfaceScale) / outputScale;            // Scale from canvas to source coordinates
        ofs.y += dimensions.h - sourceFrame.bottom;          // Add offset for bottom border
	}
	else if( ySection == 1 )
	{
		int tileAreaHeight = dimensions.h - sourceFrame.height();	
        int canvasStretchHeight = canvas.h - canvasFrame.height();	// Height of stretch-area on screen.
        
        ofs.y = ofs.y - canvasFrame.top;               // Offset in middle section of canvas
  //      ofs.y = (ofs.y * surfaceScale) / outputScale;        // Scale from canvas to source offset
        ofs.y = (int)((ofs.y / (float)canvasStretchHeight)*tileAreaHeight) + sourceFrame.top;
	}

	WgCoord srcOfs = block.Rect().pos();

    alpha = block.Surface()->alpha( {srcOfs.x+ofs.x, srcOfs.y+ofs.y} );
	
	return ( alpha >= opacityTreshold);
}


//____ PixelTypeToFormat() _____________________________________________________

bool WgUtil::PixelTypeToFormat( WgPixelType type, WgPixelFormat& wFormat )
{
	switch( type )
	{
		case WgPixelType::BGR_8:
			wFormat.format = type;
			wFormat.bits = 24;

			wFormat.R_bits = 8;
			wFormat.G_bits = 8;
			wFormat.B_bits = 8;
			wFormat.A_bits = 0;

#if WG_IS_LITTLE_ENDIAN
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

		case WgPixelType::BGRA_8:
			wFormat.format = type;
			wFormat.bits = 32;

			wFormat.R_bits = 8;
			wFormat.G_bits = 8;
			wFormat.B_bits = 8;
			wFormat.A_bits = 8;

#if WG_IS_LITTLE_ENDIAN
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
			wFormat.format = WgPixelType::Unknown;
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
		case WgSizePolicy::Default:
			return defaultSize;
		case WgSizePolicy::Bound:
			return specifiedSize;
		case WgSizePolicy::Confined:
			if( defaultSize > specifiedSize )
				return specifiedSize;
		case WgSizePolicy::Expanded:
			if( defaultSize < specifiedSize )
				return specifiedSize;
	}
	return defaultSize;
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

//____ OrigoToOfs() ________________________________________________________

WgCoord WgUtil::OrigoToOfs( WgOrigo origo, WgSize base )
{
	switch( origo )
	{
		default:
		case WgOrigo::NorthWest:
			return WgCoord(0,0);

		case WgOrigo::North:
			return WgCoord( base.w/2,0 );

		case WgOrigo::NorthEast:
			return WgCoord( base.w,0 );

		case WgOrigo::East:
			return WgCoord( base.w, base.h/2 );

		case WgOrigo::SouthEast:
			return WgCoord( base.w, base.h );

		case WgOrigo::South:
			return WgCoord( base.w/2, base.h );

		case WgOrigo::SouthWest:
			return WgCoord( 0, base.h );

		case WgOrigo::West:
			return WgCoord( 0, base.h/2 );

		case WgOrigo::Center:
			return WgCoord( base.w/2, base.h/2 );
	}
}

//____ OrigoToRect() ________________________________________________________

WgRect WgUtil::OrigoToRect( WgOrigo origo, WgSize base, WgSize rect )
{
	switch( origo )
	{
		default:
		case WgOrigo::NorthWest:
			return WgRect(0,0, rect);

		case WgOrigo::North:
			return WgRect( base.w/2 - rect.w/2, 0, rect );

		case WgOrigo::NorthEast:
			return WgRect( base.w - rect.w, 0, rect );

		case WgOrigo::East:
			return WgRect( base.w - rect.w, base.h/2 - rect.h/2, rect );

		case WgOrigo::SouthEast:
			return WgRect( base.w - rect.w, base.h - rect.h, rect );

		case WgOrigo::South:
			return WgRect( base.w/2 - rect.w/2, base.h - rect.h, rect );

		case WgOrigo::SouthWest:
			return WgRect( 0, base.h - rect.h, rect );

		case WgOrigo::West:
			return WgRect( 0, base.h/2 - rect.h/2, rect );

		case WgOrigo::Center:
			return WgRect( base.w/2 - rect.w/2, base.h/2 - rect.h/2, rect );
	}
}

//____ StateToMode() __________________________________________________________

WgMode WgUtil::StateToMode(WgState state)
{
	WgMode mode = WG_MODE_NORMAL;

	if (!state.isEnabled())
		mode = WG_MODE_DISABLED;
	else if (state.isHovered() && state.isSelected())
		mode = WG_MODE_SPECIAL;	
	else if (state.isPressed() || state.isSelected())
		mode = WG_MODE_SELECTED;
	else if (state.isHovered())
		mode = WG_MODE_MARKED;

	return mode;
}

//____ ModeToState() __________________________________________________________

WgState WgUtil::ModeToState(WgMode mode)
{
    WgState state = WgStateEnum::Normal;
    
    switch( mode )
    {
        case WG_MODE_NORMAL:
            state = WgStateEnum::Normal;
            break;
        case WG_MODE_MARKED:
            state = WgStateEnum::Hovered;
            break;
        case WG_MODE_SELECTED:
            state = WgStateEnum::SelectedPressed;
            break;
        case WG_MODE_SPECIAL:
            state = WgStateEnum::SelectedHovered;
            break;
        case WG_MODE_DISABLED:
            state = WgStateEnum::Disabled;
            break;
    }
    
    return state;
}


//____ ParametricBlendInOut() __________________________________________________________

float WgUtil::ParametricBlendInOut(float t)
{
    float sqt = t*t;
    return sqt / (2.0f * (sqt - t) + 1.0f);
}

//____ lineWidth() ____________________________________________________________________

uint32_t WgUtil::lineWidth( void * pNode, const wg::TextAttr& attr, wg::State state, const wg::Char * pString )
{
    wg::TextAttr    attr2;
    
    WgPen pen;
    Uint16 hStyle = 0xFFFF;
    
    while( !pString->isEndOfLine() )
    {
        if( pString->styleHandle() != hStyle )
        {
            attr2 = attr;
            
            auto pStyle = pString->stylePtr();
            
            if( pStyle )
                pStyle->addToAttr( state, &attr2 );
            pen.SetAttributes( attr2 );
        }
        pen.SetChar( pString->code() );
        pen.ApplyKerning();
        pen.AdvancePos();
        pString++;
    }
    
    // We include the terminator in case it is set to be visible.
    
    pen.SetChar( pString->code() );
    pen.ApplyKerning();
    pen.AdvancePos();
    
    return pen.GetPosX();
}
