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

		//____ Holder _________________________________________________________

		class Holder		/** @private */
		{
		public:
			virtual Object * _object() = 0;
			virtual const Object * _object() const = 0;

			virtual State		_componentState(const GeoComponent* pComponent) const = 0;
			virtual CoordSPX	_componentPos(const GeoComponent * pComponent) const = 0;
			virtual SizeSPX		_componentSize(const GeoComponent * pComponent) const = 0;
			virtual RectSPX		_componentGeo(const GeoComponent * pComponent) const = 0;
			virtual CoordSPX	_globalComponentPos(const GeoComponent * pComponent) const = 0;
			virtual RectSPX		_globalComponentGeo(const GeoComponent * pComponent) const = 0;

			virtual void	_componentRequestRender(const GeoComponent * pComponent) = 0;
			virtual void	_componentRequestRender(const GeoComponent * pComponent, const RectSPX& rect) = 0;
			virtual void	_componentRequestResize(const GeoComponent * pComponent) = 0;

			virtual void	_componentRequestFocus(const GeoComponent * pComponent) = 0;
			virtual void	_componentRequestInView(const GeoComponent * pComponent) = 0;
			virtual void	_componentRequestInView(const GeoComponent * pComponent, const RectSPX& mustHave, const RectSPX& niceToHave) = 0;

			virtual void	_receiveComponentNotif(GeoComponent * pComponent, ComponentNotif notification, int value, void * pData) = 0;
		};

		GeoComponent( Holder * pHolder ) : m_pHolder(pHolder) {}

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:
		inline CoordSPX	_pos() const { return m_pHolder->_componentPos(this); }
		inline SizeSPX	_size() const { return m_pHolder->_componentSize(this); }
		inline RectSPX	_geo() const { return m_pHolder->_componentGeo(this); }
		inline CoordSPX	_globalPos() const { return m_pHolder->_globalComponentPos(this); }
		inline RectSPX	_globalGeo() const { return m_pHolder->_globalComponentGeo(this); }


		inline void		_requestRender() const { m_pHolder->_componentRequestRender(this); }
		inline void		_requestRender(const RectSPX& rect) const { m_pHolder->_componentRequestRender(this, rect); }
		inline void		_requestResize() const { m_pHolder->_componentRequestResize(this); }
		inline void		_requestVisibility() const { m_pHolder->_componentRequestInView(this); }
		inline void		_requestVisibility(const RectSPX& mustHave, const RectSPX& niceToHave) const { m_pHolder->_componentRequestInView(this, mustHave, niceToHave); }
		inline void		_requestFocus() const { m_pHolder->_componentRequestFocus(this); }

		inline void		_notify(ComponentNotif notification, int value, void * pData) { m_pHolder->_receiveComponentNotif(this, notification, value, pData); }

		Object *		_object() override;
		const Object *	_object() const override;

	protected:
		Holder *		m_pHolder;
	};


} // namespace wg
#endif //WG_GEOCOMPONENT_DOT_H
