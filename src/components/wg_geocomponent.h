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

#ifndef	WG_GEOCOMPONENT_DOT_H
#define	WG_GEOCOMPONENT_DOT_H
#pragma once

#include <wg_component.h>

#include <wg_geo.h>
#include <wg_types.h>

namespace wg
{

	//____ GeoComponent _________________________________________________________________

	class GeoComponent : public Component		/** @private */
	{
	public:
		class Holder		/** @private */
		{
			friend class GeoComponent;
		protected:
			virtual Object * _object() = 0;
			virtual const Object * _object() const = 0;

			virtual CoordI	_componentPos(const GeoComponent * pComponent) const = 0;
			virtual SizeI	_componentSize(const GeoComponent * pComponent) const = 0;
			virtual RectI	_componentGeo(const GeoComponent * pComponent) const = 0;
			virtual CoordI	_globalComponentPos(const GeoComponent * pComponent) const = 0;
			virtual RectI	_globalComponentGeo(const GeoComponent * pComponent) const = 0;

			virtual void	_componentRequestRender(const GeoComponent * pComponent) = 0;
			virtual void	_componentRequestRender(const GeoComponent * pComponent, const RectI& rect) = 0;
			virtual void	_componentRequestResize(const GeoComponent * pComponent) = 0;

			virtual void	_componentRequestFocus(const GeoComponent * pComponent) = 0;
			virtual void	_componentRequestInView(const GeoComponent * pComponent) = 0;
			virtual void	_componentRequestInView(const GeoComponent * pComponent, const RectI& mustHave, const RectI& niceToHave) = 0;

			virtual void	_receiveComponentNotif(GeoComponent * pComponent, ComponentNotif notification, int value, void * pData) = 0;
		};


		GeoComponent( Holder * pHolder ) : m_pHolder(pHolder) {}

		inline CoordI	_pos() const { return m_pHolder->_componentPos(this); }
		inline SizeI	_size() const { return m_pHolder->_componentSize(this); }
		inline RectI	_geo() const { return m_pHolder->_componentGeo(this); }
		inline CoordI	_globalPos() const { return m_pHolder->_globalComponentPos(this); }
		inline RectI	_globalGeo() const { return m_pHolder->_globalComponentGeo(this); }


		inline void		_requestRender() const { m_pHolder->_componentRequestRender(this); }
		inline void		_requestRender(const RectI& rect) const { m_pHolder->_componentRequestRender(this, rect); }
		inline void		_requestResize() const { m_pHolder->_componentRequestResize(this); }
		inline void		_requestVisibility() const { m_pHolder->_componentRequestInView(this); }
		inline void		_requestVisibility(const RectI& mustHave, const RectI& niceToHave) const { m_pHolder->_componentRequestInView(this, mustHave, niceToHave); }
		inline void		_requestFocus() const { m_pHolder->_componentRequestFocus(this); }

		inline void		_notify(ComponentNotif notification, int value, void * pData) { m_pHolder->_receiveComponentNotif(this, notification, value, pData); }

		inline Object *			_object() override { return m_pHolder->_object(); }
		inline const Object *	_object() const override { return m_pHolder->_object(); }

	protected:
		Holder *		m_pHolder;
	};




} // namespace wg
#endif //WG_GEOCOMPONENT_DOT_H
