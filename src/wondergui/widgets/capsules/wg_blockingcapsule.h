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
#ifndef WG_BLOCKINGCAPSULE_DOT_H
#define WG_BLOCKINGCAPSULE_DOT_H
#pragma once

#include <vector>

#include <wg_capsule.h>


//TODO: Need to support skin padding.

namespace wg
{

	class BlockingCapsule;
	typedef	StrongPtr<BlockingCapsule>		BlockingCapsule_p;
	typedef	WeakPtr<BlockingCapsule>		BlockingCapsule_wp;

	//____ BlockingCapsule ________________________________________________________

	class BlockingCapsule : public Capsule
	{
	public:

		//____ Blueprint __________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			Widget_p		child;
			bool			disabled = false;
			bool			dropTarget = false;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			bool			invertedBlock = false;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Undefined;
			int				scale = 0;
			bool			selectable = true;
			Skin_p			skin;
			bool			stickyFocus = false;
			bool			tabLock = false;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static BlockingCapsule_p	create();
		static BlockingCapsule_p	create(const Blueprint& blueprint);

		//.____ Components _______________________________________

		std::vector<Widget_wp>		blocked;
		
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal _________________________________________________

		Widget* _findWidget(const CoordSPX& ofs, SearchMode mode) override;


	protected:
		BlockingCapsule();
		
		template<class BP> BlockingCapsule( const BP& bp ) : Capsule(bp)
		{
			m_bInvertedBlock = bp.invertedBlock;
			
			if (bp.child)
				slot.setWidget(bp.child);
		}
		
		virtual ~BlockingCapsule();
		
	private:
	
		bool	m_bInvertedBlock = false;
	};


} // namespace wg
#endif //WG_BLOCKINGCAPSULE_DOT_H
