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

#include <wg_dragndroplayer.h>

namespace wg
{
    const char DragNDropLayer::CLASSNAME[] = {"DragNDropLayer"};

    //____ Constructor ____________________________________________________________
    
    DragNDropLayer::DragNDropLayer()
    {
    }
    
    //____ Destructor _____________________________________________________________
    
    DragNDropLayer::~DragNDropLayer()
    {
    }
    
    //____ isInstanceOf() _________________________________________________________
    
    bool DragNDropLayer::isInstanceOf( const char * pClassName ) const
    {
        if( pClassName==CLASSNAME )
            return true;
        
        return Layer::isInstanceOf(pClassName);
    }
    
    //____ className() ____________________________________________________________
    
    const char * DragNDropLayer::className( void ) const
    {
        return CLASSNAME;
    }
    
    //____ cast() _________________________________________________________________
    
    DragNDropLayer_p DragNDropLayer::cast( Object * pObject )
    {
        if( pObject && pObject->isInstanceOf(CLASSNAME) )
            return DragNDropLayer_p( static_cast<DragNDropLayer*>(pObject) );
        
        return 0;
    }
    
    //____ _findWidget() __________________________________________________________
    
    Widget * DragNDropLayer::_findWidget( const Coord& ofs, SearchMode mode )
    {
        // Widget being dragged is totally transparent to the mouse, so we just
        // forward to our child.

        if( m_baseSlot.pWidget )
            return _findWidget( ofs, mode );
        else if( mode == SearchMode::Geometry )
            return this;
        
        return nullptr;
    }
    
    //____ _childRequestResize() ______________________________________________
    
    void DragNDropLayer::_childRequestResize(Slot * pSlot)
    {
        if( pSlot == &m_baseSlot )
            _requestResize();
        else
        {
            auto p = static_cast<LayerSlot*>(pSlot);
            p->geo.setSize(p->pWidget->preferredSize());
            p->pWidget->_setSize(p->geo);
            
            //TODO: Should we request render (on both sizes) too?
        }
    }
    
    //____ _beginLayerSlots() _______________________________________________
    
    const LayerSlot * DragNDropLayer::_beginLayerSlots() const
    {
        return &m_dragSlot;
    }

    //____ _endLayerSlots() ________________________________________________
    
    const LayerSlot * DragNDropLayer::_endLayerSlots() const
    {
        if( m_dragSlot.pWidget )
            return (&m_dragSlot) + 1;
        else
            return &m_dragSlot;
    }
    
    //____ _sizeOfLayerSlot() _______________________________________________
    
    int DragNDropLayer::_sizeOfLayerSlot() const
    {
        return sizeof(LayerSlot);
    }
    
    //____ _onRequestRender() _______________________________________________
    
    void DragNDropLayer::_onRequestRender(const Rect& rect, const LayerSlot * pSlot)
    {
        // We don't mask against drag widget, it is assumed to be to small/transparent/irregular
        // for that to make sense.
        
        // Clip our geometry and pass it on
        
        _requestRender( Rect( rect, Rect(0,0,m_size) ));
    }
    
    //____ _cloneContent() ____________________________________________________
    
    void DragNDropLayer::_cloneContent( const Widget * _pOrg )
    {
        
    }

    //____ _receive() __________________________________________________________
    
    void DragNDropLayer::_receive( Msg * pMsg )
    {
        
    }

    
    
} // namespace wg
