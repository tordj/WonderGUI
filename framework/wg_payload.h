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

#ifndef WG_PAYLOAD_DOT_H
#define	WG_PAYLOAD_DOT_H
#pragma once

#include <wg_smartptr.h>

class WgWidget;

//____ WgPayloadType _______________________________________________________

enum WgPayloadType
{
  WG_PAYLOAD_WIDGET,
  WG_PAYLOAD_CUSTOM1,
    
};

//____ WgPayload ___________________________________________________________

class WgPayload;
typedef    WgSmartPtr<WgPayload>      WgPayloadPtr;

class WgPayload : public WgRefCounted
{
public:
    
    virtual WgPayloadType   Type() const = 0;
protected:
  WgPayload() {}
};

//____ WgPayloadWidget _____________________________________________________


class WgPayloadWidget;
typedef    WgSmartPtr<WgPayloadWidget>      WgPayloadWidgetPtr;

class WgPayloadWidget : public WgPayload
{
public:
    static WgPayloadWidgetPtr    Create(WgWidget * pWidget) { return WgPayloadWidgetPtr(new WgPayloadWidget( pWidget )); }

    WgPayloadType     Type() const override { return WG_PAYLOAD_WIDGET; }
    WgWidget *      Widget() const { return m_pWidget; }
    
protected:
    WgPayloadWidget( WgWidget * pWidget ) : m_pWidget(pWidget) {}
    
    WgWidget * m_pWidget;
};



#endif //WG_PAYLOAD_DOT_H
