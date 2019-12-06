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
#include <wg_cslot.h>


namespace wg
{

	class CapsuleSlot : public CSlotImpl<BasicSlot>
	{
		friend class Capsule;
	public:
		using CSlotImpl<BasicSlot>::operator=;
	protected:
		CapsuleSlot(BasicSlot::Holder* pHolder) : CSlotImpl(pHolder) {}
	};

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

		//.____ Components _______________________________________

		CapsuleSlot		slot;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];
		static Capsule_p	cast( Object * pObject );

	protected:
		Capsule();
		virtual ~Capsule();

		// Overloaded from SlotHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }
		const Object * _object() const override;

		CoordI		_childGlobalPos(const BasicSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(const BasicSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection(const BasicSlot * pSlot) const override { return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(BasicSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(BasicSlot * pSlot) override { return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		CoordI		_childPos( const BasicSlot * pSlot ) const override;

		void		_childRequestRender( BasicSlot * pSlot ) override;
		void		_childRequestRender( BasicSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( BasicSlot * pSlot ) override;

		Widget *	_prevChild( const BasicSlot * pSlot ) const override;
		Widget *	_nextChild( const BasicSlot * pSlot ) const override;

		void		_releaseChild( BasicSlot * pSlot ) override;
		void		_replaceChild(BasicSlot * pSlot, Widget * pWidget) override;




		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		// Overloaded from Widget

		int			_matchingHeight(int width) const override;
		int			_matchingWidth(int height) const override;

		SizeI		_preferredSize() const override;


		void		_collectPatches( Patches& container, const RectI& geo, const RectI& clip ) override;
		void		_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode ) override;
		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const SizeI& size ) override;

	};


} // namespace wg
#endif //WG_CAPSULE_DOT_H
