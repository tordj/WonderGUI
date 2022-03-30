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
#ifndef WG_FILLER_DOT_H
#define WG_FILLER_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

namespace wg
{

	class Filler;
	typedef	StrongPtr<Filler>	Filler_p;
	typedef	WeakPtr<Filler>		Filler_wp;

	//____ Filler ____________________________________________________________
	/**
	 * @brief Simplest kind of widget.
	 *
	 * Filler is the simplest kind of widget that displays nothing but its skin
	 * and does nothing but posting the normal mouse messages that every widget does.
	 *
	 * It is typically used as backgrounds and for filling out empty spaces in
	 * organized layouts.
	 *
	 **/


	class Filler : public Widget
	{
	public:

		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			Size			defaultSize = { -1,-1 };
			int				scale = -1;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};


		//.____ Creation __________________________________________

		static Filler_p	create() { return Filler_p(new Filler()); }
		static Filler_p	create( const Blueprint& blueprint ) { return Filler_p(new Filler( blueprint )); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		void	setDefaultSize( const Size& size );

	protected:
		Filler();
		Filler(const Blueprint& blueprint);
		virtual ~Filler();

		SizeSPX	_defaultSize(int scale = -1) const override;

	private:
		Size			m_defaultSize;
	};



} // namespace wg
#endif //WG_FILLER_DOT_H
