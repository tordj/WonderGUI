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
#include <wg_cstandardslot.h>


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

	class Capsule : public Container
	{
	public:

		//.____ Components _______________________________________

		CStandardSlot		slot;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];

	protected:
		Capsule();
		virtual ~Capsule();

		// Overloaded from Widget

		int			_matchingHeight(int width) const override;
		int			_matchingWidth(int height) const override;

		SizeI		_preferredSize() const override;

		void		_collectPatches(Patches& container, const RectI& geo, const RectI& clip) override;
		void		_maskPatches(Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode) override;
		void		_cloneContent(const Widget * _pOrg) override;
		void		_resize(const SizeI& size) override;

		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;
		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		CoordI		_childPos( const StaticSlot * pSlot ) const override;

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		void		_releaseChild( StaticSlot * pSlot ) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pWidget) override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		//

		class SlotAccess : public CStandardSlot { friend class Capsule; };
		SlotAccess * _slot() { return static_cast<SlotAccess*>(&slot); }
		const SlotAccess * _slot() const { return static_cast<const SlotAccess*>(&slot); }
	};


} // namespace wg
#endif //WG_CAPSULE_DOT_H
