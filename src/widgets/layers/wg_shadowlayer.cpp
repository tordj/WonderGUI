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
	const char ShadowLayer::CLASSNAME[] = {"ShadowLayer"};

	//____ Constructor ____________________________________________________________

	ShadowLayer::ShadowLayer() : front(&m_baseSlot, this), shadows(this,&m_shadows)
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

	//____ matchingHeight() _________________________________________________________________

	int ShadowLayer::matchingHeight(int width) const
	{
		int matchFront = 0;
		int matchBase = 0;

		if (m_baseSlot.pWidget)
			matchBase = m_baseSlot.pWidget->matchingHeight(width);
		if (m_frontSlot.pWidget)
			matchFront = m_frontSlot.pWidget->matchingHeight(width);

		return std::max(matchFront, matchBase);
	}

	//____ matchingWidth() _________________________________________________________________

	int ShadowLayer::matchingWidth(int height) const
	{
		int matchFront = 0;
		int matchBase = 0;

		if (m_baseSlot.pWidget)
			matchBase = m_baseSlot.pWidget->matchingWidth(height);
		if (m_frontSlot.pWidget)
			matchFront = m_frontSlot.pWidget->matchingWidth(height);

		return std::max(matchFront, matchBase);
	}

	//____ preferredSize() _________________________________________________________________

	SizeI ShadowLayer::preferredSize() const
	{
		SizeI prefFront;
		SizeI prefBase;

		if (m_baseSlot.pWidget)
			prefBase = m_baseSlot.pWidget->preferredSize();
		if (m_frontSlot.pWidget)
			prefFront = m_frontSlot.pWidget->preferredSize();

		return SizeI::max(prefFront, prefBase);

	}

	//____ minSize() _________________________________________________________________

	SizeI ShadowLayer::minSize() const
	{
		SizeI minFront;
		SizeI minBase;

		if (m_baseSlot.pWidget)
			minBase = m_baseSlot.pWidget->minSize();
		if (m_frontSlot.pWidget)
			minFront = m_frontSlot.pWidget->minSize();

		return SizeI::min(minFront, minBase);
	}

	//____ maxSize() _________________________________________________________________

	SizeI ShadowLayer::maxSize() const
	{
		SizeI maxFront;
		SizeI maxBase;

		if (m_baseSlot.pWidget)
			maxBase = m_baseSlot.pWidget->maxSize();
		if (m_frontSlot.pWidget)
			maxFront = m_frontSlot.pWidget->maxSize();

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
			pSlot->replaceWidget(this, nullptr);
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

			m_frontSlot.pWidget->_maskPatches(patches, size(), RectI(0, 0, 65536, 65536), _getBlendMode());

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
			SizeI sz = it->widget()->size();
			sz += it->shadow()->contentPadding();


		}


	}

	//____ _willRemoveShadows() _______________________________________________

	void ShadowLayer::_willRemoveShadows(int ofs, int nb)
	{

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
				break;
			}

			default:
				break;
		}

		Layer::_receive(_pMsg);
	}

	

	//____ _renderPatches() __________________________________________________________

	void ShadowLayer::_renderPatches( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window, const Patches& _patches )
	{
		RectI contentGeo = _canvas;

		// Generate masked patches for our skin, baseSlot widget, and shadow.

		Patches patches(_patches);

		if(m_frontSlot.pWidget)
			m_frontSlot.pWidget->_maskPatches(patches, _canvas, _canvas, pDevice->blendMode());		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!


		// If we have a skin, render it and modify contentGeo

		if (m_pSkin)
		{
			pDevice->setClipList(patches.size(), patches.begin());
			m_pSkin->render(pDevice, _canvas, m_state);
			
			contentGeo = m_pSkin->contentRect(_canvas, m_state);
		}

		// Render base slot widgets

		if (m_baseSlot.pWidget)
			m_baseSlot.pWidget->_renderPatches(pDevice, contentGeo, contentGeo, patches);

		// Render shadows


		//Render front slot widgets

		if (m_frontSlot.pWidget)
			m_frontSlot.pWidget->_renderPatches(pDevice, contentGeo, contentGeo, _patches);
	}


} // namespace wg
