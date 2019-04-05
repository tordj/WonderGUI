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

namespace wg 
{
	class   DragNDropLayer;
	typedef	StrongPtr<DragNDropLayer>	DragNDropLayer_p;
	typedef	WeakPtr<DragNDropLayer>		DragNDropLayer_wp;

    //____ DragNDropLayer ____________________________________________________________
    
    class DragNDropLayer : public Layer
    {
    public:
        
        //.____ Creation __________________________________________
        
        static DragNDropLayer_p  create() { return DragNDropLayer_p(new DragNDropLayer()); }
        
        //.____ Identification __________________________________________
        
        bool                    isInstanceOf( const char * pClassName ) const;
        const char *            className( void ) const;
        static const char       CLASSNAME[];
        static DragNDropLayer_p  cast( Object * pObject );
        
    protected:
        DragNDropLayer();
        virtual ~DragNDropLayer();
        virtual Widget* _newOfMyType() const { return new DragNDropLayer(); };
        
        
//        DragNDropLayer *    _getDragNDropLayer() const { return const_cast<DragNDropLayer*>(this); }
        
        // Overloaded from Panel
        
        Widget *        _findWidget( const Coord& ofs, SearchMode mode );
        
        // Overloaded from WidgetHolder
        
        void            _childRequestResize(Slot * pSlot);
        
        // Overloaded from Layer
        
        const LayerSlot *     _beginLayerSlots() const;
        const LayerSlot *     _endLayerSlots() const;
        int             _sizeOfLayerSlot() const;
        
        void            _onRequestRender(const Rect& rect, const LayerSlot * pSlot);    // rect is in our coordinate system.
        
        // Overloaded from Widget
        
        void            _cloneContent( const Widget * _pOrg );
        void            _receive( Msg * pMsg );
        
        LayerSlot       m_dragSlot;            // Slot for widget being dragged, when it is dragged.
        
    };

    
    
} // namespace wg
#endif //WG_DRAGNDROPLAYER_DOT_H

