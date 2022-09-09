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
#ifndef	WG_SCALEPREFERRED_SIZEBROKER_DOT_H
#define	WG_SCALEPREFERRED_SIZEBROKER_DOT_H

#include <wg_sizebroker.h>

namespace wg
{

	class ScalePreferredSizeBroker;
	typedef	StrongPtr<ScalePreferredSizeBroker>		ScalePreferredSizeBroker_p;
	typedef	WeakPtr<ScalePreferredSizeBroker>	ScalePreferredSizeBroker_wp;

	class ScalePreferredSizeBroker : public SizeBroker
	{
	public:
		//.____ Creation __________________________________________

		static ScalePreferredSizeBroker_p	create() { return ScalePreferredSizeBroker_p(new ScalePreferredSizeBroker()); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc __________________________________________________

		spx setItemLengths( SizeBrokerItem * pItems, int nItems, spx totalLength ) const override;
		spx setPreferredLengths( SizeBrokerItem * pItems, int nItems ) const override;
		bool mayAlterPreferredLengths() const override;

	protected:
		ScalePreferredSizeBroker() {};
		virtual ~ScalePreferredSizeBroker() {};

	};



} // namespace wg
#endif //WG_SCALEPREFERRED_SIZEBROKER_DOT_H
