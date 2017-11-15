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

#ifndef	WG_GFXSTREAMREADER_DOT_H
#define	WG_GFXSTREAMREADER_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_object.h>
#include <wg_pointers.h>
#include <wg_gfxinstream.h>

#include <functional>

namespace wg
{

	class GfxStreamReader;
	typedef	StrongPtr<GfxStreamReader>	GfxStreamReader_p;
	typedef	WeakPtr<GfxStreamReader>	GfxStreamReader_wp;

	class GfxStreamReader : public Object, protected GfxInStreamHolder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamReader_p	create( std::function<int(int nBytes, void * pDest)> dataFetcher );

		//.____ Interfaces _______________________________________

		GfxInStream		stream;

		//.____ Identification __________________________________________

		bool				isInstanceOf(const char * pClassName) const;
		const char *		className(void) const;
		static const char	CLASSNAME[];
		static GfxStreamReader_p	cast(Object * pObject);



	protected:

		GfxStreamReader(std::function<int(int nBytes, void * pDest)> dataFeeder );
		~GfxStreamReader();

		Object * _object() override { return this; }

		bool		_hasChunk() override;
		GfxStream::Header	_peekChunk() override;
		char		_pullChar() override;
		short		_pullShort() override;
		int			_pullInt() override;
		float		_pullFloat() override;
		void		_pullBytes(int nBytes, char * pBytes) override;

		void		_closeStream() override;


		void		_fetchData();

		std::function<int(int nBytes, void * pDest)>	m_fetcher;

		static const int c_bufferSize = 8192;		// Needs to be 2^x
		static const int c_bufferMargin = 4;		// Bytes of margin at the end for long reads before looping

		char *		m_pBuffer;
		int			m_readOfs;
		int			m_writeOfs;
	};



} // namespace wg
#endif //WG_GFXSTREAMREADER_DOT_H
#pragma once
