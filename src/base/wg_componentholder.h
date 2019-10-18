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

#ifndef WG_COMPONENTHOLDER_DOT_H
#define WG_COMPONENTHOLDER_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_geo.h>

namespace wg
{
	class Component;
	class Object;

	class ComponentHolder		/** @private */
	{
		friend class Component;
	protected:
		virtual Object * _object() = 0;
		virtual const Object * _object() const = 0;

		virtual CoordI	_componentPos( const Component * pComponent ) const = 0;
		virtual SizeI	_componentSize( const Component * pComponent ) const = 0;
		virtual RectI	_componentGeo( const Component * pComponent ) const = 0;
		virtual CoordI	_globalComponentPos( const Component * pComponent ) const = 0;
		virtual RectI	_globalComponentGeo( const Component * pComponent ) const = 0;

		virtual void	_componentRequestRender( const Component * pComponent ) = 0;
		virtual void	_componentRequestRender( const Component * pComponent, const RectI& rect ) = 0;
		virtual void	_componentRequestResize( const Component * pComponent ) = 0;

 		virtual void	_componentRequestFocus( const Component * pComponent ) = 0;
		virtual void	_componentRequestInView( const Component * pComponent ) = 0;
		virtual void	_componentRequestInView( const Component * pComponent, const RectI& mustHave, const RectI& niceToHave ) = 0;



		virtual void	_receiveComponentNotif( Component * pComponent, ComponentNotif notification, int value, void * pData ) = 0;

	};

} // namespace wg


#endif //WG_COMPONENTHOLDER_DOT_H
