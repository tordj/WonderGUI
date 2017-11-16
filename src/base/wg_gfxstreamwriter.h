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

#include <wg_gfxoutstream.h>

#include <functional>

namespace wg
{

	class GfxStreamWriter;
	typedef	StrongPtr<GfxStreamWriter>	GfxStreamWriter_p;
	typedef	WeakPtr<GfxStreamWriter>	GfxStreamWriter_wp;

	class GfxStreamWriter : public Object, protected GfxOutStreamHolder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamWriter_p	create( std::function<void(int nBytes, const void * pData)> dispatcher, int maxPackageSize = GfxStream::c_maxBlockSize*2 );

		//.____ Interfaces _______________________________________

		GfxOutStream		stream;

		//.____ Identification __________________________________________

		bool				isInstanceOf(const char * pClassName) const;
		const char *		className(void) const;
		static const char	CLASSNAME[];
		static GfxStreamWriter_p	cast(Object * pObject);

		//.____ Content _______________________________________________________

		inline int	packageSpace() const { return m_capacity - m_size; }
		inline int	packageSize() const { return m_size; }

	protected:

		GfxStreamWriter(std::function<void(int nBytes, const void * pData)> packageDispatcher, int packageSize );
		~GfxStreamWriter();


		Object *	_object() override;

		void		_flushStream() override;
		void		_reserveStream(int bytes) override;
		void		_closeStream() override;
		bool		_reopenStream() override;
		bool		_isStreamOpen() override;


		void		_pushChar(char c) override;
		void		_pushShort(short s) override;
		void		_pushInt(int i) override;
		void		_pushFloat(float f) override;
		void		_pushBytes(int nBytes, char * pBytes) override;

		std::function<void(int nBytes, const void * pData)>	m_dispatcher;

		char *		m_pBuffer;
		int			m_capacity;
		int			m_size;
	};


} // namespace wg
#endif //WG_GFXSTREAMWRITER_DOT_H
