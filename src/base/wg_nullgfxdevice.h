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

#ifndef WG_NULLGFXDEVICE_DOT_H
#define WG_NULLGFXDEVICE_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

namespace wg 
{
	
	class Surface;
	class Rect;
	class Color;
	
	class NullGfxDevice;
	typedef	StrongPtr<NullGfxDevice,GfxDevice_p>	NullGfxDevice_p;
	typedef	WeakPtr<NullGfxDevice,GfxDevice_wp>	NullGfxDevice_wp;
	
	class NullGfxDevice : public GfxDevice
	{
	public:
		static NullGfxDevice_p	create( Size size );
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static NullGfxDevice_p	cast( const Object_p& pObject );
	
		//
	
		void	fill( const Rect& rect, const Color& col );
		void	blit( const Surface_p& src, const Rect& srcrect, int dx, int dy  );
		void	tileBlit( const Surface_p& src, const Rect& srcrect, const Rect& dest );
	
		void	fillSubPixel( const RectF& rect, const Color& col );
		void	stretchBlitSubPixel( const Surface_p& pSrc, float sx, float sy, float sw, float sh,
							   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );
	
		void	clipDrawHorrLine( const Rect& clip, const Coord& start, int length, const Color& col );
		void	clipDrawVertLine( const Rect& clip, const Coord& start, int length, const Color& col );
		void	clipPlotSoftPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color& col, float thickness );
        void    clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors);
	
		void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f );
		void	drawArcNE( const Rect& rect, Color color );
		void	drawElipse( const Rect& rect, Color color );
		void	drawFilledElipse( const Rect& rect, Color color );
	
		void	clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f );
		void	clipDrawArcNE( const Rect& clip, const Rect& rect, Color color );
		void	clipDrawElipse( const Rect& clip, const Rect& rect, Color color );
		void	clipDrawFilledElipse( const Rect& clip, const Rect& rect, Color color );
	
	protected:
		NullGfxDevice( Size size );
		~NullGfxDevice();
	
	};
	

} // namespace wg
#endif //WG_NULLGFXDEVICE_DOT_H

