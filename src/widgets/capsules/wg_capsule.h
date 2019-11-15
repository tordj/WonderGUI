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

#ifndef WG_CAPSULE_DOT_H
#define WG_CAPSULE_DOT_H
#pragma once

#include <wg_container.h>
#include <wg_islot.h>


namespace wg
{

	class Capsule;
	typedef	StrongPtr<Capsule>	Capsule_p;
	typedef	WeakPtr<Capsule>	Capsule_wp;


	//____ Capsule ______________________________________________________________

	/**
	* @brief	Base class for container widgets with only one child.
	*
	* A capsule is a light-weight container widget that can only have one child.
	*
	* The purpose of a capsule is to encapsulate a single widget or a branch in
	* the widget hierarchy to control or modify its appearance or behavior.
	*
	* Various subclasses of Capsule control specific aspects of their children
	* such as size, tint and transparency.
	*/

	class Capsule : public Container, protected SlotHolder
	{
	public:

		//.____ Interfaces _______________________________________

		ISlot		child;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Capsule_p	cast( Object * pObject );

	protected:
		Capsule();
		virtual ~Capsule();

		// Overloaded from ChildHolder

		CoordI		_childPos( Slot * pSlot ) const;
		SizeI		_childSize( Slot * pSlot ) const;

		void		_childRequestRender( Slot * pSlot );
		void		_childRequestRender( Slot * pSlot, const RectI& rect );
		void		_childRequestResize( Slot * pSlot );

		Widget *	_prevChild( const Slot * pSlot ) const;
		Widget *	_nextChild( const Slot * pSlot ) const;

		void		_releaseChild( Slot * pSlot );

		// Overloaded from ChildHolder

		Object *	_object();
		const Object * _object() const;

		void		_setWidget( Slot * pSlot, Widget * pWidget );

		// Overloaded from Container

		Widget *	_firstChild() const;
		Widget *	_lastChild() const;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const;

		// Overloaded from Widget

		int			_matchingHeight(int width) const;
		int			_matchingWidth(int height) const;

		SizeI		_preferredSize() const;


		void		_collectPatches( Patches& container, const RectI& geo, const RectI& clip );
		void		_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode );
		void		_cloneContent( const Widget * _pOrg );
		void		_resize( const SizeI& size );



		Slot		m_child;

	};


} // namespace wg
#endif //WG_CAPSULE_DOT_H
