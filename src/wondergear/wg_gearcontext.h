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

#ifndef	WG_GEARCONTEXT_DOT_H
#define	WG_GEARCONTEXT_DOT_H
#pragma once

#include <functional>

#include <wg_object.h>
#include <wg_strongptr.h>
#include <wg_geartypes.h>

namespace wg
{
	class GearContext;
	typedef	StrongPtr<GearContext>		GearContext_p;
	typedef	WeakPtr<GearContext>		GearContext_wp;

	class GearContext : public Object
	{
		friend class GearBase;

	public:
		//.____ Creation __________________________________________

		static GearContext_p	create() { return GearContext_p(new GearContext()); };

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content ___________________________________________________________

		void					setErrorHandler(std::function<void(Error&)> handler);
		std::function<void(Error&)>	errorHandler() const;

	protected:
		GearContext() {};
		virtual ~GearContext() {};

		std::function<void(Error&)>	m_pErrorHandler = nullptr;
	};

} // namespace wg
#endif //WG_GEARCONTEXT_DOT_H
