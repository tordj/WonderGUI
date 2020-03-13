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

#ifndef	WG_PANEL_DOT_H
#define	WG_PANEL_DOT_H
#pragma once

#include <wg_container.h>
#include <wg_slot.h>

namespace wg
{

	class Patches;

	class Panel;
	typedef	StrongPtr<Panel>	Panel_p;
	typedef	WeakPtr<Panel>		Panel_wp;

	/**
	 * @brief	Base class for layout widgets.
	 *
	 * Base class for layout widgets.
	 */

	//____ Panel ________________________________________________________________

	class Panel : public Container
	{
	public:

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];

		//.____ Behavior _______________________________________________________

		void		setTooltipGroup( bool bTooltipGroup ) { m_bTooltipGroup = bTooltipGroup; }
		bool		isTooltipGroup() const { return m_bTooltipGroup; }

		void		setMaskOp( MaskOp operation );
		MaskOp		maskOp() const { return m_maskOp; }


	protected:
		Panel();
		virtual ~Panel() {};

		bool			_isPanel() const override;
		virtual void	_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode ) override;
		virtual void	_cloneContent( const Widget * _pOrg ) override;

		class SlotAccess : public StaticSlot { friend class Panel; };
		Panel::SlotAccess * _access(StaticSlot * pSlot) { return static_cast<SlotAccess*>(pSlot); }
		const Panel::SlotAccess * _access(const StaticSlot * pSlot) const { return static_cast<const SlotAccess*>(pSlot); }


		bool		m_bTooltipGroup;	// All descendants belongs to the same tooltip group.
		MaskOp		m_maskOp;			// Specifies how container masks background.
	};




} // namespace wg
#endif //WG_PANEL_DOT_H
