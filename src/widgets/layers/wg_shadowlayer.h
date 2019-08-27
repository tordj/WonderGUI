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

#ifndef WG_SHADOWLAYER_DOT_H
#define WG_SHADOWLAYER_DOT_H
#pragma once

#include <vector>
#include <wg_layer.h>
#include <wg_ishadows.h>

namespace wg
{

	class   ShadowLayer;
	typedef	StrongPtr<ShadowLayer>		ShadowLayer_p;
	typedef	WeakPtr<ShadowLayer>		ShadowLayer_wp;



	//____ ShadowLayer ____________________________________________________________


	class ShadowLayer : public Layer, private ShadowHolder
	{
	public:

		//.____ Creation __________________________________________

		static ShadowLayer_p  create() { return ShadowLayer_p(new ShadowLayer()); }

		//.____ Interfaces _______________________________________

		IChild<Slot, ChildHolder>	front;
		IShadows					shadows;

		//.____ Identification __________________________________________

		bool                    isInstanceOf( const char * pClassName ) const override;
		const char *            className( void ) const override;
		static const char       CLASSNAME[];
		static ShadowLayer_p  cast( Object * pObject );

	protected:
		ShadowLayer();
		virtual ~ShadowLayer();
		virtual Widget* _newOfMyType() const override { return new ShadowLayer(); };

		// Overloaded from WidgetHolder

		void            _childRequestResize(Slot * pSlot) override;
		void			_releaseChild(Slot * pSlot) override;

		// Overloaded from Layer

		const LayerSlot *	_beginLayerSlots() const override;
		const LayerSlot *	_endLayerSlots() const override;
		int					_sizeOfLayerSlot() const override;

		void				_onRequestRender(const RectI& rect, const LayerSlot * pSlot) override;    // rect is in our coordinate system.

		// Overloaded from ShadowHolder

		Object *        _object() override;
		void	        _didAddShadows(int nb) override;
		void	        _willRemoveShadows(int ofs, int nb) override;

		// Overloaded from Widget

		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		SizeI			_preferredSize() const override;
		SizeI			_minSize() const override;
		SizeI			_maxSize() const override;
		void			_setSize(const SizeI& size) override;

		void            _cloneContent( const Widget * _pOrg ) override;
		void            _render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void            _receive(Msg * pMsg) override;


		LayerSlot       m_frontSlot;
		RouteId			m_tickRouteId;
		Surface_p		m_pShadowSurface;

		std::vector<Shadow>	m_shadows;
	};




} // namespace wg
#endif //WG_SHADOWLAYER_DOT_H

