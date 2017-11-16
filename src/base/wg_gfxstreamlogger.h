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

#ifndef	WG_GFXSTREAMLOGGER_DOT_H
#define	WG_GFXSTREAMLOGGER_DOT_H
#pragma once

#include <iostream>

#include <wg_object.h>
#include <wg_gfxinstream.h>

namespace wg
{

	class GfxStreamLogger;
	typedef	StrongPtr<GfxStreamLogger>	GfxStreamLogger_p;
	typedef	WeakPtr<GfxStreamLogger>	GfxStreamLogger_wp;

	class GfxStreamLogger : public Object
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamLogger_p	create( GfxInStream& in, std::ostream& out );

		//.____ Identification __________________________________________

		bool				isInstanceOf(const char * pClassName) const;
		const char *		className(void) const;
		static const char	CLASSNAME[];
		static GfxStreamLogger_p	cast(Object * pObject);

		//.____ Control _______________________________________________________

		bool		isEmpty() const;
		GfxStream::Header	peekChunk() const;

		void		logAll();
		bool		logChunk();
		bool		logFrame();

	protected:
		GfxStreamLogger( GfxInStream& in, std::ostream& out );
		~GfxStreamLogger();


		GfxInStream_p	m_pGfxStream;
		std::ostream&	m_charStream;
	};

}

#endif // WG_GFXSTREAMLOGGER_DOT_H