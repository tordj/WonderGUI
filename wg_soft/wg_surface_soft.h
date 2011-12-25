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
#ifndef WG_SURFACE_SOFT_DOT_H
#define WG_SURFACE_SOFT_DOT_H

#ifndef WG_TYPES_DOT_H
#include "wg_types.h"
#endif

#ifndef WG_COLOR_DOT_H
#include "wg_color.h"
#endif

#ifndef WG_GEO_DOT_H
#include "wg_geo.h"
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#include <vector>
using namespace std;

//____ Class WgSurfaceSoft _____________________________________________________________________

class WgSurfaceSoft : public WgSurface
{
 public:
  enum BlitMode
  {
    BM_OPAQUE,
    BM_COLORKEY,
    BM_ALPHA,
    BM_COLORKEY_WITH_ALPHA
  };

  WgSurfaceSoft( Uint32 iWidth, Uint32 iHeight, Uint32 iBitdepth = 32 );
  WgSurfaceSoft( const WgSurfaceSoft& other )
  {
	copy(other);
  }

  ~WgSurfaceSoft();

  // Assignment operator
  const WgSurfaceSoft& operator= (const WgSurfaceSoft& other)
  {
	if(this != &other)
	{
		// Free old data before copy
		if(m_pData != 0)
		{
			delete [] m_pData;
			m_pData = 0;
		}
		if(m_pStoredAlpha != 0)
		{
			delete [] m_pStoredAlpha;
			m_pStoredAlpha = 0;
		}
                                
		copy(other);
	}
                
	return (*this);
  }

	WgSize		Size() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( WgCoord coord ) const;
	Uint8		GetOpacity( WgCoord coord ) const;

	void *		Lock( LockStatus mode );
	void *		LockRegion( LockStatus mode, const WgRect& region );
	void		Unlock();

	inline BlitMode blitMode() const;

  inline float scaleAlpha() { return m_fScaleAlpha; }
  void setScaleAlpha(float fScaleAlpha);
   
  void   setBlitMode( BlitMode mode );
  void putPixels(const vector<int> &x, const vector<int> &y, const vector<Uint32> &col, int length, bool replace);
									   			               
protected:
	
	BlitMode	m_blitMode;
	WgSize		m_size;
	int			m_bitdepth;
	Uint8*		m_pData;

	Uint8*   	m_pStoredAlpha;

	float    	m_fScaleAlpha;
  
private:

	void copy(const WgSurfaceSoft& other)
	{
		m_blitMode = other.m_blitMode;
		m_width = other.m_width;
		m_height = other.m_height;
		m_bitdepth = other.m_bitdepth;
		m_fScaleAlpha = other.m_fScaleAlpha;

		m_pData = new Uint8[ m_width*m_height*m_bitdepth/8 ];
		for(unsigned int i = 0; i<m_width*m_height*m_bitdepth/8;i++)
			m_pData[i] = other.m_pData[i];
			
		if(other.m_pStoredAlpha != 0)
		{
			m_pStoredAlpha = new Uint8[ m_width*m_height ];
			for(unsigned int i = 0; i<m_width*m_height;i++)
				m_pStoredAlpha[i] = other.m_pStoredAlpha[i];
		} else
		{
			m_pStoredAlpha = 0;
		}
	}
};


//____ WgSurface::blitMode() ______________________________________________________________

inline WgSurfaceSoft::BlitMode WgSurfaceSoft::blitMode() const
{
  return m_blitMode;
}


//========================================================================================
#endif // WG_SURFACE_SOFT_DOT_H
