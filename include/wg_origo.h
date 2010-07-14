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

#ifndef	WG_ORIGO_DOT_H
#define	WG_ORIGO_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

//____ Define: WgOrigo ________________________________________________________

class	WgOrigo
{
public:
	WgOrigo() {anchor = 0; hotspot = 0; };
	~WgOrigo() {};
		
	// Static methods for creating WgOrigo objects.

	inline static WgOrigo& topLeft() {return g_topLeft;};
	inline static WgOrigo& topCenter() {return g_topCenter;};
	inline static WgOrigo& topRight() {return g_topRight;};
	inline static WgOrigo& midLeft() {return g_midLeft;};
	inline static WgOrigo& midCenter() {return g_midCenter;};
	inline static WgOrigo& midRight() {return g_midRight;};
	inline static WgOrigo& bottomLeft() {return g_bottomLeft;};
	inline static WgOrigo& bottomCenter() {return g_bottomCenter;};
	inline static WgOrigo& bottomRight() {return g_bottomRight;};
	
	static WgOrigo relative( float x, float y );
	static WgOrigo specific( float anchX, float anchY, float hsX, float hsY );
	static WgOrigo specificUnlimited( float anchX, float anchY, float hsX, float hsY );
	static WgOrigo cornerTopLeft( float x, float y );
	static WgOrigo cornerBottomRight( float x, float y );
	
	static WgOrigo grid( int x, int y, int columns, int lines );
	static WgOrigo columns( int x, int columns, float y );
	static WgOrigo lines( int y, int lines, float x );

	// Methods for reading elementparts of Origo.

	inline float	anchorX() const { return xa; };
	inline float	anchorY() const { return ya; };

	inline float	hotspotX() const { return xhs; };
	inline float	hotspotY() const { return yhs; };

	//

	inline Uint32	calcOfsX( Uint32 parentWidth, Uint32 childWidth ) const { return (Uint32) (parentWidth*xa - childWidth*xhs); }
	inline Uint32	calcOfsY( Uint32 parentHeight, Uint32 childHeight ) const { return (Uint32) (parentHeight*ya - childHeight*yhs); }

	// Operators
	
	bool operator==( const WgOrigo& a ) const { if( a.anchor == anchor && a.hotspot == a.hotspot ) return true; return false; };
	bool operator!=( const WgOrigo& a ) const { if( a.anchor == anchor && a.hotspot == a.hotspot ) return false; return true; };	
	
private:

	union
	{
		Uint64		anchor;
		struct
		{
			float	xa;
			float	ya;
		};
	};

	union
	{
		Uint64		hotspot;
		struct
		{
			float	xhs;
			float	yhs;
		};
	};


	static WgOrigo	g_topLeft;
	static WgOrigo	g_topCenter;
	static WgOrigo	g_topRight;

	static WgOrigo	g_midLeft;
	static WgOrigo	g_midCenter;
	static WgOrigo	g_midRight;

	static WgOrigo	g_bottomLeft;
	static WgOrigo	g_bottomCenter;
	static WgOrigo	g_bottomRight;
	
	
	
};

#endif // WG_ORIGO_DOT_H
