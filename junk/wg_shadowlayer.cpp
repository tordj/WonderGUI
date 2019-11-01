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
#include <wg_context.h>

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

        SizeI padding = m_pSkin ? m_pSkin->_contentPadding() : SizeI(0,0);
        width -= padding.w;

		if (m_baseSlot.pWidget)
			matchBase = m_baseSlot.matchingHeight(width);
		if (m_frontSlot.pWidget)
			matchFront = m_frontSlot.matchingHeight(width);

		return std::max(matchFront, matchBase) + padding.h;
	}

	//____ _matchingWidth() _________________________________________________________________

	int ShadowLayer::_matchingWidth(int height) const
	{
		int matchFront = 0;
		int matchBase = 0;

        SizeI padding = m_pSkin ? m_pSkin->_contentPadding() : SizeI(0,0);
        height -= padding.h;
        
		if (m_baseSlot.pWidget)
			matchBase = m_baseSlot.matchingWidth(height);
		if (m_frontSlot.pWidget)
			matchFront = m_frontSlot.matchingWidth(height);

		return std::max(matchFront, matchBase) + padding.w;
	}

	//____ _preferredSize() _________________________________________________________________

	SizeI ShadowLayer::_preferredSize() const
	{
		SizeI prefFront;
		SizeI prefBase;
        SizeI padding = m_pSkin ? m_pSkin->_contentPadding() : SizeI(0,0);

		if (m_baseSlot.pWidget)
			prefBase = m_baseSlot.preferredSize();
		if (m_frontSlot.pWidget)
			prefFront = m_frontSlot.preferredSize();

		return SizeI::max(prefFront, prefBase) + padding;

	}

	//____ _minSize() _________________________________________________________________

	SizeI ShadowLayer::_minSize() const
	{
		SizeI minFront;
		SizeI minBase;
        SizeI padding = m_pSkin ? m_pSkin->_contentPadding() : SizeI(0,0);

		if (m_baseSlot.pWidget)
			minBase = m_baseSlot.minSize();
		if (m_frontSlot.pWidget)
			minFront = m_frontSlot.minSize();

		return SizeI::max(minFront, minBase) + padding;
	}

	//____ maxSize() _________________________________________________________________

	SizeI ShadowLayer::_maxSize() const
	{
		SizeI maxFront;
		SizeI maxBase;
        SizeI padding = m_pSkin ? m_pSkin->_contentPadding() : SizeI(0,0);

		if (m_baseSlot.pWidget)
			maxBase = m_baseSlot.maxSize();
		if (m_frontSlot.pWidget)
			maxFront = m_frontSlot.maxSize();

		return SizeI::min(maxFront, maxBase) + padding;
	}

	//____ _setSize() _______________________________________________________________

	void ShadowLayer::_setSize(const SizeI& size)
	{
		if (size != m_size)
        {
            m_pShadowSurface = nullptr;
            
            if( m_pSkin )
                m_frontSlot.geo = m_pSkin->_contentRect(size, m_state);
            else
                m_frontSlot.geo = { 0,0,size };
        }
		Layer::_setSize(size);
	}

    //____ _setSkin() _______________________________________________________________

    void ShadowLayer::_setSkin( Skin * pSkin )
    {
        //TODO: Update m_frontSlot.geo if content padding (for current state) has been affected.
        
        Layer::_setSkin(pSkin);
    }
    
    //____ _setState() _______________________________________________________________

    void ShadowLayer::_setState( State state )
    {
        //TODO: Update m_frontSlot.geo if content padding has been affected.
        
        Layer::_setState(state);
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
			_requestRender( m_frontSlot.geo );
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

			m_frontSlot.pWidget->_maskPatches(patches, m_frontSlot.geo, m_frontSlot.geo, _getBlendMode());

			// Make request render calls

			for (const RectI * pRect = patches.begin(); pRect < patches.end(); pRect++)
				_requestRender(*pRect);
		}
		else
			_requestRender( rect + m_frontSlot.geo.pos() );
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
            
//            RectI geo = it->shadow()->_contentRect( {pos, it->widget()->m_size}, StateEnum::Normal );

			Skin * pSkin = it->shadow();
			RectI geo = { pos - pSkin->_contentOfs(StateEnum::Normal), pSkin->_sizeForContent(it->widget()->m_size) };

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
		_requestRenderShadows(ofs, nb);
	}

	//____ _requestRenderShadows() _______________________________________________

	void ShadowLayer::_requestRenderShadows(int ofs, int nb)
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

	//____ _setShadowTint() ___________________________________________________

	void ShadowLayer::_setShadowTint(uint8_t alpha)
	{
		if (alpha != m_shadowTint)
		{
			m_shadowTint = alpha;
			_requestRenderShadows(0, (int)m_shadows.size());
		}
	}

	//____ _shadowTint() ______________________________________________________

	uint8_t ShadowLayer::_shadowTint() const
	{
		return m_shadowTint;
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
                //TODO: Should use A8 surface for shadow once GlGfxDevice can handle it (write to A8).
                
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
                        
//                        RectI geo = {pos, pWidget->m_size };

						Skin * pSkin = pShadow->shadow();
						RectI geo = { pos - pSkin->_contentOfs(StateEnum::Normal), pSkin->_sizeForContent(pWidget->m_size) };


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
		RectI contentGeo = m_frontSlot.geo + _canvas.pos();

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

			m_frontSlot.pWidget->_maskPatches(patches, contentGeo, contentGeo, pDevice->blendMode());		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

			clipPop = patchesToClipList(pDevice, patches);
		}


		// If we have a skin, render it

		if (m_pSkin)
			m_pSkin->_render(pDevice, _canvas, m_state);
			
		// Render base slot widgets

		if (m_baseSlot.pWidget)
			m_baseSlot.pWidget->_render(pDevice, contentGeo, contentGeo);

        // Update shadow layer

		bool bFullSurfaceUpdate = !m_pShadowSurface;

		if (!m_pShadowSurface)
		{
			auto pSurfaceFactory = Base::activeContext()->surfaceFactory();

			if (pSurfaceFactory)
				m_pShadowSurface = pSurfaceFactory->createSurface(rawToPixels(m_size), PixelFormat::BGRA_8);
		}
		
		if( m_pShadowSurface )
		{
            auto oldCanvas = pDevice->canvas();
			auto oldBlendMode = pDevice->blendMode();
			auto oldTint = pDevice->tintColor();

            pDevice->setCanvas(m_pShadowSurface, false);

            RectI        fullClipList;
            ClipPopData orgClipList;
            if (bFullSurfaceUpdate)
            {
                orgClipList = pushClipList(pDevice);
                fullClipList = { 0,0, rawToPixels(m_size) };
                pDevice->setClipList(1, &fullClipList);
            }
            
			pDevice->setTintColor(Color::White);
            pDevice->setBlendMode(BlendMode::Replace);
            pDevice->fill( {255,255,255,0} );

			pDevice->setBlendMode(BlendMode::Max);
			for( auto& shadow : m_shadows )
				shadow.shadow()->_render(pDevice, _shadowGeo(&shadow) + contentGeo.pos(), StateEnum::Normal);

			if (bFullSurfaceUpdate)
				popClipList(pDevice, orgClipList);

			pDevice->setCanvas(oldCanvas, false);

			// Render shadows from shadow layer

			pDevice->setTintColor({ 255, 255, 255, m_shadowTint } );
            pDevice->setBlendMode(BlendMode::Subtract);
			pDevice->setBlitSource(m_pShadowSurface);
			pDevice->blit( {0,0} );

			pDevice->setTintColor(oldTint);
            pDevice->setBlendMode(oldBlendMode);

        }

		//Render front slot widgets

        popClipList(pDevice, clipPop);

        if (m_frontSlot.pWidget)
			m_frontSlot.pWidget->_render(pDevice, contentGeo, contentGeo);

	}


} // namespace wg
