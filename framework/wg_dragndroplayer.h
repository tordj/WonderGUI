/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef WG_DRAGNDROPLAYER_DOT_H
#define WG_DRAGNDROPLAYER_DOT_H
#pragma once

#include <vector>
#include <wg_layer.h>
#include <wg_payload.h>
#include <wg_key.h>

class WgSurfaceFactory;

//____ WgDragNDropHook ___________________________________________________________

class WgDragNDropHook : public WgLayerHook     /** @private */
{
    friend class WgDragNDropLayer;

public:
    const char *Type(void) const;
    static const char * ClassType();

    WgDragNDropLayer *    Parent() const;

protected:

    WgDragNDropHook(WgDragNDropLayer * pParent) : m_pParent(pParent) {}

    void        _requestResize();

    WgHook *    _prevHook() const;
    WgHook *    _nextHook() const;
    WgContainer* _parent() const;

    WgDragNDropLayer * m_pParent;
};


//____ WgDragNDropLayer ____________________________________________________________

class WgDragNDropLayer : public WgLayer
{
    friend class WgDragNDropHook;
public:
    WgDragNDropLayer();
    virtual ~WgDragNDropLayer();

    virtual const char *Type(void) const override;
    static const char * GetClass();
    virtual WgWidget * NewOfMyType() const override { return new WgDragNDropLayer(); };

    bool            Pick( WgWidget * pWidget, WgCoord pickOfs );
    void            SetSurfaceFactory( WgSurfaceFactory * pFactory ) { m_pSurfaceFactory = pFactory; }

    WgWidget *      FindWidget( const WgCoord& ofs, WgSearchMode mode ) override;

    bool            DeleteChild(WgWidget * pWidget) override { return 0; }
    WgWidget *      ReleaseChild(WgWidget * pWidget) override { return 0; }

    bool            DeleteAllChildren() override { return 0; }
    bool            ReleaseAllChildren() override { return 0; }

    enum DragState
    {
        Idle,
        Picking,    // Mouse button pressed, awaiting drag to pass treshold
        Picked,     // Drag passed treshold, DropPickMsg sent.
        Dragging,   // We have a payload, a drag-widget and are dragging.
        Targeting,  // We are hovering a widget that has accepted our target probing.
        Delivering, // We have released mouse button on a targeted widget. Deliver + Complete/Cancel cycle is taking place.
    };

    DragState GetDragState() const { return m_dragState; }

protected:

    // Overloaded from Panel

    void            _onRequestRender(const WgRect& rect, const WgLayerHook * pHook) override;    // rect is in our coordinate system.

    // Overloaded from Layer

    WgLayerHook *    _firstLayerHook() const override;

    WgHook*          _firstHook() const override;        // Fist Hook returned is the normal child, then follows the dragged one.
    WgHook*          _lastHook() const override;        //

    WgHook *        _firstHookWithGeo(WgRect& geo) const override;
    WgHook *        _nextHookWithGeo(WgRect& geo, WgHook * pHook) const override;

    WgHook *        _lastHookWithGeo(WgRect& geo) const override;
    WgHook *        _prevHookWithGeo(WgRect& geo, WgHook * pHook) const override;


    // Overloaded from Widget

    void            _onCloneContent(const WgWidget * _pOrg) override;
    void            _onEvent(const WgEvent::Event * pEvent, WgEventHandler * pHandler) override;
    void            _onNewSize(const WgSize& size) override;

    void            _renderPatches(wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches) override;

    void            _replaceWidgetInHook(WgWidget * pNewWidget);

    //

    void            _complete( WgWidget * pDeliveredTo);
    void            _cancel();
    void            _replaceDragWidget( WgWidget * pNewWidget );

    void            _updateDropHovered( WgCoord hoverPos );
    void            _clearDropHovered();

    int             _widgetPosInList( const WgWidget * pWidget, const std::vector<WgWidgetWeakPtr>& list );


    DragState		m_dragState = DragState::Idle;

    WgDragNDropHook     m_dragHook;            // Slot for widget being dragged, when it is dragged.

    WgWidgetWeakPtr	m_pPicked;
    WgPayloadPtr	m_pPayload;

    int				m_dragStartTreshold = 3;
    WgCoord         m_dragWidgetOfs;               // Drag widgets offset from pointer.

    WgWidgetWeakPtr   m_pProbed;                     // Last widget we sent a DropProbeMsg to. To avoid sending multiple messages in a row to same while hovering.
    WgWidgetWeakPtr   m_pTargeted;                   // Widget targeted while in state Targeting.
    WgCoord           m_targetOfs;                  // Pointer ofs within targeted widget.
    
    WgWidgetWeakPtr   m_pDropHovered;               // Widget curently hovered while in Dragging or Targeting stage.    
    std::vector<WgWidgetWeakPtr>    m_vHoveredInside;    // All widgets that pointer is considered to be inside (= m_pDropHovered + its ancestors).

    
    WgSurfaceFactory * m_pSurfaceFactory = nullptr;

    bool            m_bDeleteDraggedWhenDone = false;
};

#endif //WG_DRAGNDROPLAYER_DOT_H
