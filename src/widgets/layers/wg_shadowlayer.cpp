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

#include <wg_shadowlayer.h>
#include <wg_msgrouter.h>
#include <wg_msg.h>
#include <wg_gfxdevice.h>
#include <wg_surfacefactory.h>
#include <wg_patches.h>
#include <wg_base.h>

#include <algorithm>

namespace wg
{
	using namespace Util;

	const char ShadowLayer::CLASSNAME[] = {"ShadowLayer"};

	//____ Constructor ____________________________________________________________

	ShadowLayer::ShadowLayer() : front(&m_frontSlot, this), shadows(this,&m_shadows)
	{
		m_tickRouteId = Base::msgRouter()->addRoute(MsgType::Tick, this);
	}

	//____ Destructor _____________________________________________________________

	ShadowLayer::~ShadowLayer()
	{
		if( m_tickRouteId )
			Base::msgRouter()->deleteRoute( m_tickRouteId );
	}

	//____ isInstanceOf() _________________________________________________________

	bool ShadowLayer::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Layer::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * ShadowLayer::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	ShadowLayer_p ShadowLayer::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ShadowLayer_p( static_cast<ShadowLayer*>(pObject) );

		return 0;
	}

	//____ _matchingHeight() _________________________________________________________________

	int ShadowLayer::_matchingHeight(int width) const
	{
		int matchFront = 0;
		int matchBase = 0;

		if (m_baseSlot.pWidget)
			matchBase = m_baseSlot.matchingHeight(width);
		if (m_frontSlot.pWidget)
			matchFront = m_frontSlot.matchingHeight(width);

		return std::max(matchFront, matchBase);
	}

	//____ _matchingWidth() _________________________________________________________________

	int ShadowLayer::_matchingWidth(int height) const
	{
		int matchFront = 0;
		int matchBase = 0;

		if (m_baseSlot.pWidget)
			matchBase = m_baseSlot.matchingWidth(height);
		if (m_frontSlot.pWidget)
			matchFront = m_frontSlot.matchingWidth(height);

		return std::max(matchFront, matchBase);
	}

	//____ _preferredSize() _________________________________________________________________

	SizeI ShadowLayer::_preferredSize() const
	{
		SizeI prefFront;
		SizeI prefBase;

		if (m_baseSlot.pWidget)
			prefBase = m_baseSlot.preferredSize();
		if (m_frontSlot.pWidget)
			prefFront = m_frontSlot.preferredSize();

		return SizeI::max(prefFront, prefBase);

	}

	//____ _minSize() _________________________________________________________________

	SizeI ShadowLayer::_minSize() const
	{
		SizeI minFront;
		SizeI minBase;

		if (m_baseSlot.pWidget)
			minBase = m_baseSlot.minSize();
		if (m_frontSlot.pWidget)
			minFront = m_frontSlot.minSize();

		return SizeI::min(minFront, minBase);
	}

	//____ maxSize() _________________________________________________________________

	SizeI ShadowLayer::_maxSize() const
	{
		SizeI maxFront;
		SizeI maxBase;

		if (m_baseSlot.pWidget)
			maxBase = m_baseSlot.maxSize();
		if (m_frontSlot.pWidget)
			maxFront = m_frontSlot.maxSize();

		return SizeI::max(maxFront, maxBase);
	}

	//____ _childRequestResize() ______________________________________________

	void ShadowLayer::_childRequestResize(Slot * pSlot)
	{
		_requestResize();
	}

	//____ _releaseChild() ____________________________________________________

	void ShadowLayer::_releaseChild(Slot * pSlot)
	{
		if (pSlot == &m_baseSlot)
			Layer::_releaseChild(pSlot);
		else
		{
			m_frontSlot.replaceWidget(this, nullptr);
			_requestRender({ 0, 0, m_size } );
			_requestResize();
		}
	}

	//____ _beginLayerSlots() _______________________________________________

	const LayerSlot * ShadowLayer::_beginLayerSlots() const
	{
		return &m_frontSlot;
	}

	//____ _endLayerSlots() ________________________________________________

	const LayerSlot * ShadowLayer::_endLayerSlots() const
	{
		if( m_frontSlot.pWidget )
			return (&m_frontSlot) + 1;
		else
			return &m_frontSlot;
	}

	//____ _sizeOfLayerSlot() _______________________________________________

	int ShadowLayer::_sizeOfLayerSlot() const
	{
		return sizeof(LayerSlot);
	}

	//____ _onRequestRender() _______________________________________________

	void ShadowLayer::_onRequestRender(const RectI& rect, const LayerSlot * pSlot)
	{
		// Widgets from base slot can be masked by widgets from front slot.

		if (pSlot == &m_baseSlot)
		{
			Patches patches;
			patches.add(rect);

			// Remove portions of patches that are covered by opaque front widgets

			m_frontSlot.pWidget->_maskPatches(patches, m_size, RectI(0, 0, 65536, 65536), _getBlendMode());

			// Make request render calls

			for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
				_requestRender(*pRect);
		}
		else
			_requestRender( rect );
	}

	//____ _object() __________________________________________________________

	Object * ShadowLayer::_object()
	{
		return this;
	}

	//____ _didAddShadows() ____________________________________________________

	void ShadowLayer::_didAddShadows(int nb)
	{
		for (auto it = m_shadows.end() - nb; it != m_shadows.end(); it++)
		{
            CoordI pos;
            _descendantPos(it->widget(), pos);
            
            RectI geo = it->shadow()->_contentRect( {pos, it->widget()->m_size}, StateEnum::Normal );
            _setShadowGeo(&(*it), geo);

            Patches patches;
            patches.add(geo);
            
            // Remove portions of patches that are covered by opaque front widgets
            
            m_frontSlot.pWidget->_maskPatches(patches, m_size, m_size, _getBlendMode());
            
            // Make request render calls
            
            for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
                _requestRender(*pRect);
		}


	}

	//____ _willRemoveShadows() _______________________________________________

	void ShadowLayer::_willRemoveShadows(int ofs, int nb)
	{
        for (auto it = m_shadows.end() - nb; it != m_shadows.end(); it++)
        {
            RectI geo = _shadowGeo(&(*it));
            
            Patches patches;
            patches.add(geo);
            
            // Remove portions of patches that are covered by opaque front widgets
            
            m_frontSlot.pWidget->_maskPatches(patches, m_size, m_size, _getBlendMode());
            
            // Make request render calls
            
            for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
                _requestRender(*pRect);
        }
	}

	//____ _cloneContent() ____________________________________________________

	void ShadowLayer::_cloneContent( const Widget * _pOrg )
	{

	}

	//____ _receive() __________________________________________________________

	void ShadowLayer::_receive( Msg * _pMsg )
	{
		switch (_pMsg->type())
		{
			case MsgType::Tick:
			{
                //TODO: Should not be on tick, but on pre-render stage.
                //TODO: Needs to work with absent frontLayer;
                //TODO: Should check that shadow casting children are descendants of our frontLayer, not us.

                // Check for removed children and changes to geo that will affect shadows.
                
                Patches patches;

                for( auto it = m_shadows.begin() ; it < m_shadows.end() ; it++ )
                {
                    Widget * pWidget = it->widget();
                    Shadow * pShadow = &(*it);
                    
                    // Remove shadow for deleted widget.
                    
                    if( pWidget == nullptr )
                    {
                        patches.add( _shadowGeo(pShadow) );
                        it = m_shadows.erase(it);
                    }
                    
                    //
                    
                    CoordI pos;
                    if( _descendantPos(pWidget, pos))
                    {
                        // Widget is still our descendant, check
                        // so its geo has not changed.
                        
                        RectI geo = {pos, pWidget->m_size };
                        RectI oldGeo = _shadowGeo(pShadow);
                        if( geo != oldGeo )
                        {
                            patches.add(oldGeo);
                            patches.add(geo);
                            _setShadowGeo(pShadow, geo);
                        }
                    }
                    else
                    {
                        // Widget is currently not a descendant of us,
                        // hide the shadow.
                        
                        RectI geo = _shadowGeo(pShadow);
                        if( !geo.isEmpty() )
                        {
                            patches.add(geo);
                            _setShadowGeo(pShadow, {0,0,0,0} );
                        }
                    }
                }
                
                // Early out if there is nothing to update in shadow layer.
                
                if( patches.isEmpty() )
                    break;
                
                // Mask foreground from shadow updates and request render on the remains.
                
                if( m_frontSlot.pWidget )
                    m_frontSlot.pWidget->_maskPatches(patches, m_size, m_size, BlendMode::Blend);
                
                for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
                    _requestRender(*pRect);

				break;
			}

			default:
				break;
		}

		Layer::_receive(_pMsg);
	}

	

	//____ _render() __________________________________________________________

	void ShadowLayer::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		RectI contentGeo = _canvas;

		// Generate masked patches for our skin, baseSlot widget, and shadow.

		ClipPopData clipPop;

		if (m_frontSlot.pWidget)
		{
			// Collect dirty patches from gfxDevice

			int nClipRects = pDevice->clipListSize();
			auto pClipRects = pDevice->clipList();

			Patches patches(nClipRects);

			for (int i = 0; i < nClipRects; i++)
				patches.push(pixelsToRaw(pClipRects[i]));

			m_frontSlot.pWidget->_maskPatches(patches, _canvas, _canvas, pDevice->blendMode());		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

			clipPop = patchesToClipList(pDevice, patches);
		}


		// If we have a skin, render it and modify contentGeo

		if (m_pSkin)
		{
			m_pSkin->_render(pDevice, _canvas, m_state);
			
			contentGeo = m_pSkin->_contentRect(_canvas, m_state);
		}

		// Render base slot widgets

		if (m_baseSlot.pWidget)
			m_baseSlot.pWidget->_render(pDevice, contentGeo, contentGeo);

        // Update shadow layer
/*
        auto oldCanvas = pDevice->canvas();
        auto oldBlendMode = pDevice->blendMode();
        
        pDevice->setCanvas(m_pShadowSurface);
        pDevice->setBlendMode(BlendMode::Max);

        for( auto& shadow : m_shadows )
        {
            shadow.shadow()->_render(pDevice, _shadowGeo(&shadow) + contentGeo.pos(), StateEnum::Normal);
        }
        
        pDevice->setBlendMode(oldBlendMode);
        pDevice->setCanvas(oldCanvas);

        // Render shadows

        pDevice->setBlitSource(m_pShadowSurface);
        pDevice->blit( {0,0} );
*/
		//Render front slot widgets

        popClipList(pDevice, clipPop);

        if (m_frontSlot.pWidget)
			m_frontSlot.pWidget->_render(pDevice, contentGeo, contentGeo);

	}


} // namespace wg
