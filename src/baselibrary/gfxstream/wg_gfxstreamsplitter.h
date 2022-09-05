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

#ifndef	WG_GFXSTREAMSPLITTER_DOT_H
#define	WG_GFXSTREAMSPLITTER_DOT_H
#pragma once

#include <wg_gfxstreamsink.h>

#include <vector>

namespace wg
{

	class GfxStreamSplitter;
	typedef	StrongPtr<GfxStreamSplitter>	GfxStreamSplitter_p;
	typedef	WeakPtr<GfxStreamSplitter>	GfxStreamSplitter_wp;

	class GfxStreamSplitter : public Object, protected GfxStreamSink::Holder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamSplitter_p	create();
		static GfxStreamSplitter_p	create( const std::initializer_list<GfxStreamSink_p>& outputs );

		//.____ Components _______________________________________

		GfxStreamSink		input;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control ________________________________________________
		
		void	clearOutputs();
		void	addOutput( const GfxStreamSink_p& pOutput );
		void	removeOutput(const GfxStreamSink_p& pOutput);
		
		
	protected:

		GfxStreamSplitter();
		GfxStreamSplitter(const std::initializer_list<GfxStreamSink_p>& outputs);
		~GfxStreamSplitter();

		void	_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd) override;

		std::vector<GfxStreamSink_p> m_outputs;
	};


} // namespace wg
#endif //WG_GFXSTREAMSPLITTER_DOT_H
