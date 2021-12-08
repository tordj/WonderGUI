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
#include <wg_cgfxinstream.h>

namespace wg
{

	class GfxStreamLogger;
	typedef	StrongPtr<GfxStreamLogger>	GfxStreamLogger_p;
	typedef	WeakPtr<GfxStreamLogger>	GfxStreamLogger_wp;

	class GfxStreamLogger : public Object
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamLogger_p	create( CGfxInStream& in, std::ostream& out );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		bool		isEmpty() const;
		GfxStream::Header	peekChunk() const;

		void		logAll();
		bool		logChunk();
		bool		logFrame();

	protected:
		GfxStreamLogger( CGfxInStream& in, std::ostream& out );
		~GfxStreamLogger();

		int		_readPrintPatches();
		
		void	_readPrintRects( const char * header, int amount );
		void	_printRect( const char * header, const RectI& rect );
		void	_printRect( const char * header, const RectF& rect );
		void	_printColor( const char * header, HiColor color );
		void	_printBorder( const char * header, const BorderI& border );

		CGfxInStream_p	m_pGfxStream;
		std::ostream&	m_charStream;
	};

}

#endif // WG_GFXSTREAMLOGGER_DOT_H
