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

#ifndef WG_IROVALUEDISPLAY_DOT_H
#define WG_IROVALUEDISPLAY_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_textstyle.h>
#include <wg_textmapper.h>


//#ifndef WG_VALUEFORMAT_DOT_H
//#	include <wg_valueformat.h>
//#endif

#include <wg_cvalue.h>

namespace wg
{


	class IROValueDisplay;
	typedef	StrongComponentPtr<IROValueDisplay>	IROValueDisplay_p;
	typedef	WeakComponentPtr<IROValueDisplay>	IROValueDisplay_wp;

	/**
	 * @brief Interface for a read-only value with modifiable appearance
	 *
	 * Interface that provides read-only access to the value of a value component and allows
	 * modification of its appearance such as formatting and text properties.
	 *
	 */

	class IROValueDisplay : public Component
	{
	public:
		/** @private */

		IROValueDisplay(CValue* pComponent) : m_pComponent(pComponent) {}

		//.____ Appearance _____________________________________________

		inline void				setFormatter( ValueFormatter * pFormatter ) { m_pComponent->setFormatter(pFormatter); }
		inline void				clearFormatter() { m_pComponent->clearFormatter(); }
		inline ValueFormatter_p	formatter() const { return m_pComponent->formatter(); }

		inline void				setStyle( TextStyle * pStyle ) { m_pComponent->setStyle(pStyle); }
		inline void				clearStyle() { m_pComponent->clearStyle(); }
		inline TextStyle_p		style() const { return m_pComponent->style(); }

		inline void				setTextMapper( TextMapper * pTextMapper ) { m_pComponent->setTextMapper(pTextMapper); }
		inline void				clearTextMapper() { m_pComponent->clearTextMapper(); }
		inline TextMapper_p		textMapper() const { return m_pComponent->textMapper(); }

		//.____ Content _______________________________________________

		inline int64_t		value() const { return m_pComponent->value(); }
		inline int			scale() const { return m_pComponent->scale(); }


		//.____ State _______________________________________________

		inline State			state() const { return m_pComponent->state(); }

		//.____ Misc __________________________________________________

		inline IROValueDisplay_p			ptr() { return IROValueDisplay_p(this); }


	protected:
		Object * 		_object() override;
		const Object * 		_object() const override;

		CValue *		m_pComponent;
	};



} // namespace wg
#endif //WG_IROVALUEDISPLAY_DOT_H
