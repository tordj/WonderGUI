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
#ifndef WG_SKIN_DOT_H
#define WG_SKIN_DOT_H

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

class WgSkin;
class WgRect;
class WgGfxDevice;

typedef	WgSmartPtr<WgSkin>	WgSkinPtr;


class WgSkin : public WgRefCounted
{
public:
	virtual ~WgSkin() {};
	
	virtual void Render( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _clip ) = 0;
	virtual bool IsOpaque() const = 0;
	
protected:
	WgSkin() {};
	
};

#endif //WG_SKIN_DOT_H