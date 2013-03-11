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
#ifndef WG_FRAMESKIN_DOT_H
#define WG_FRAMESKIN_DOT_H

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif


class WgFrameSkin : public WgSkin
{
public:
	static WgSkinPtr Create( int thickness, WgColor col );
	
	~WgFrameSkin() {};
	
	void Render( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip );
	bool IsOpaque() const;
	
private:
	WgFrameSkin( int thickness, WgColor col );

	WgColor		m_color;
    int         m_thickness;
	
};

#endif //WG_FRAMESKIN_DOT_H