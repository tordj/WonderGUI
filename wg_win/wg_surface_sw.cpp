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

#include <wg_surface_sw.h>

//____ Constructor _____________________________________________________________

WgSurfaceSW::WgSurfaceSW( Uint32 iWidth, Uint32 iHeight, Uint32 iBitdepth )
{
	m_width		= iWidth;
	m_height	= iHeight;
	m_bitdepth	= iBitdepth;
	m_pitch		= iWidth;
	m_pData		= new Uint8[ m_width*m_height*m_bitdepth/8 ];
}

//____ Destructor ______________________________________________________________

WgSurfaceSW::~WgSurfaceSW()
{
	delete [] m_pData;
}

//____ GetWidth() ______________________________________________________________

Uint32 WgSurfaceSW::GetWidth() const
{
	return m_width;
}

//____ GetHeight() _____________________________________________________________

Uint32 WgSurfaceSW::GetHeight() const
{
	return m_height;
}

//____ IsOpaque() ______________________________________________________________

bool WgSurfaceSW::IsOpaque() const
{
	if( m_bitdepth == 24 )
		return true;
		
	return false;
}

//____ GetPixel() ______________________________________________________________

Uint32 WgSurfaceSW::GetPixel( Uint32 x, Uint32 y ) const
{
	if( m_bitdepth == 32 )
	{
		return * ((Uint32*) &m_pData[ 4*(m_width*y+x) ]);
	}
	else 
	{
		Uint32 k = * ((Uint32*) &m_pData[ 3*(m_width*y+x) ]);
		k |= 0xff000000; // ???
		return k;
	}}

//____ GetOpacity() ____________________________________________________________

Uint8 WgSurfaceSW::GetOpacity( Uint32 x, Uint32 y ) const
{
	if( x >= m_width )
		return 0;
	if( y >= m_height )
		return 0;
	
	if( m_bitdepth == 32 )
		return m_pData[ 4*(m_width*y+x) + 3 ];
	else
		return 0xff;
}

//____ Col2Pixel() _____________________________________________________________

Uint32 WgSurfaceSW::Col2Pixel( const WgColor& col ) const
{
}

//____ Pixel2Col() _____________________________________________________________

WgColor WgSurfaceSW::Pixel2Col( Uint32 pixel ) const
{
}


