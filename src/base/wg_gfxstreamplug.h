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

#ifndef	WG_GFXSTREAMPLUG_DOT_H
#define	WG_GFXSTREAMPLUG_DOT_H
#pragma once

#include <wg_object.h>
#include <wg_gfxinstream.h>
#include <wg_gfxoutstream.h>

namespace wg
{
	class GfxStreamPlug;
	typedef	StrongPtr<GfxStreamPlug>	GfxStreamPlug_p;
	typedef	WeakPtr<GfxStreamPlug>		GfxStreamPlug_wp;

	class GfxStreamPlug : public Object, protected GfxOutStreamHolder
	{
	public:

		const static int	c_maxOutputs = 4;

		//.____ Creation __________________________________________

		static GfxStreamPlug_p	create( int capacity = min(GfxStream::c_maxBlockSize*4, 16384) );

		//.____ Components _______________________________________

		GfxOutStream		input;
		GfxInStream			output[c_maxOutputs];


		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const override;
		const char *			className(void) const override;
		static const char		CLASSNAME[];
		static GfxStreamPlug_p	cast(Object * pObject);

		//.____ Control _______________________________________________________

		void			openOutput(int index);

		int				bufferSize();
		int				bufferCapacity();
		bool			setBufferCapacity(int capacity);
		void			clearBuffer();

	protected:
		GfxStreamPlug( int capacity );
		~GfxStreamPlug();

		class OutStreamProxy : public GfxInStreamHolder
		{
		public:
			Object *	_object() override;
			const Object *	_object() const override;

			void		_closeStream() override;
			bool		_reopenStream() override;
			bool		_isStreamOpen() override;

			bool		_hasChunk() override;
			GfxStream::Header	_peekChunk() override;
			char		_pullChar() override;
			short		_pullShort() override;
			int			_pullInt() override;
			float		_pullFloat() override;
			void		_pullBytes(int nBytes, char * pBytes) override;
			void		_skipBytes(int nBytes) override;

			int			size();

			int			readOfs;			// Set to -1 if not open.

			GfxStreamPlug *	pObj;
		};


		Object *		_object() override;
		const Object *	_object() const override;

		void			_reserveStream(int bytes) override;
		void			_flushStream() override;
		void			_closeStream() override;
		bool			_reopenStream() override;
		bool			_isStreamOpen() override;

		void			_pushChar(char c) override;
		void			_pushShort(short s) override;
		void			_pushInt(int i) override;
		void			_pushFloat(float f) override;
		void			_pushBytes(int nBytes, char * pBytes) override;

		void			_resize(int size);

		static const int c_bufferMargin = 4;		// Bytes of margin at the end for long writes and reads before looping


		char *			m_pBuffer;
		int				m_bufferSize;
		int				m_writeOfs;
		OutStreamProxy	m_outStream[c_maxOutputs];
	};

};


#endif //WG_GFXSTREAMPLUG_DOT_H
