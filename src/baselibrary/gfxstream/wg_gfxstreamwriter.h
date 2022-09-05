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

#ifndef	WG_GFXSTREAMWRITER_DOT_H
#define	WG_GFXSTREAMWRITER_DOT_H
#pragma once

#include <wg_gfxstreamsink.h>

#include <functional>

namespace wg
{

	class GfxStreamWriter;
	typedef	StrongPtr<GfxStreamWriter>	GfxStreamWriter_p;
	typedef	WeakPtr<GfxStreamWriter>	GfxStreamWriter_wp;

	class GfxStreamWriter : public Object, protected GfxStreamSink::Holder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamWriter_p	create( std::function<void(int nBytes, const void * pData)> dispatcher );

		//.____ Components _______________________________________

		GfxStreamSink		input;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

	protected:

		GfxStreamWriter(std::function<void(int nBytes, const void * pData)> packageDispatcher );
		~GfxStreamWriter();

		void	_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd) override;

		std::function<void(int nBytes, const void * pData)>	m_dispatcher;
	};


} // namespace wg
#endif //WG_GFXSTREAMWRITER_DOT_H
