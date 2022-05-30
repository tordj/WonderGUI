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
#ifndef WG_SIZECAPSULE_DOT_H
#define WG_SIZECAPSULE_DOT_H
#pragma once

#include <wg_capsule.h>

//TODO: Need to support skin padding.

namespace wg
{

	class SizeCapsule;
	typedef	StrongPtr<SizeCapsule>		SizeCapsule_p;
	typedef	WeakPtr<SizeCapsule>		SizeCapsule_wp;

	//____ SizeCapsule ________________________________________________________
	/**
	* @brief	A widget that controls the size of a single child.
	*
	* The SizeCapsule is used to change the size of a widget. This is done either by imposing
	* limits on its minimum and maximum size or by setting a specific default size.
	*
	* Width and height can be controlled separately
	* The SizeCapsule overrides the min, max and default size reported by its child, thus
	* affecting its geometry. Exactly how the geometry is affected depends on the parent widget.
	*
	*/

	class SizeCapsule : public Capsule
	{
	public:

		//____ Blueprint __________________________________________

		struct Blueprint
		{

			Object_p		baggage;
			Widget_p		child;
			Size			defaultSize = { -1,-1 };
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			Size			maxSize = { 10000000,10000000 };
			Size			minSize = { 0,0 };
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			int				scale = 64;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static SizeCapsule_p	create() { return SizeCapsule_p(new SizeCapsule()); }
		static SizeCapsule_p	create(const Blueprint& blueprint) { return SizeCapsule_p(new SizeCapsule(blueprint)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________


		bool	setSizes( Size minSize, Size defaultSize, Size maxSize );

		void	setDefaultSize( Size size );
		void	setMinSize( Size size );
		void	setMaxSize( Size size );

		//.____ Internal _________________________________________________

		spx		_matchingHeight(spx width, int scale = -1) const override;
		spx		_matchingWidth(spx height, int scale = -1) const override;

		SizeSPX	_defaultSize(int scale = -1) const override;
		SizeSPX	_minSize(int scale = -1) const override;
		SizeSPX	_maxSize(int scale = -1) const override;


	protected:
		SizeCapsule();
		SizeCapsule( const Blueprint& bp );
		virtual ~SizeCapsule();


	private:

		Size			m_minSize;
		Size			m_maxSize = { 10000000, 10000000 };
		Size			m_defaultSize = { -1, -1 };
	};


} // namespace wg
#endif //WG_SIZECAPSULE_DOT_H
