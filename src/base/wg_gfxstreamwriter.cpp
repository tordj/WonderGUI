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

#include <cstring>

#include <wg_gfxstreamwriter.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamWriter::TYPEINFO = { "GfxStreamWriter", &Object::TYPEINFO };


	//____ create() ___________________________________________________________

	GfxStreamWriter_p GfxStreamWriter::create(std::function<void(int nBytes, const void * pData)> dispatcher, int maxPackageSize )
	{
		return new GfxStreamWriter(dispatcher, maxPackageSize);
	}

	//____ constructor _____________________________________________________________

	GfxStreamWriter::GfxStreamWriter(std::function<void(int nBytes, const void * pData)> dispatcher, int maxPackageSize) : stream(this)
	{
		m_dispatcher = dispatcher;
		m_pBuffer	= new char[maxPackageSize];
		m_capacity	= maxPackageSize;
		m_size		= 0;
	}

	//____ Destructor _________________________________________________________

	GfxStreamWriter::~GfxStreamWriter()
	{
		delete[] m_pBuffer;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamWriter::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _object() __________________________________________________________

	Object * GfxStreamWriter::_object()
	{
		return this;
	}

	const Object * GfxStreamWriter::_object() const
	{
		return this;
	}

	//____ _flushStream() ____________________________________________________________

	void GfxStreamWriter::_flushStream()
	{
		m_dispatcher(m_size, m_pBuffer);
		m_size = 0;
	}

	//____ _closeStream() ____________________________________________________

	void GfxStreamWriter::_closeStream()
	{
		_flushStream();						// GfxStreamWriter is never closed, so we just flush.

		delete[] m_pBuffer;
		m_pBuffer = nullptr;
	}

	//____ _reopenStream() ______________________________________________________

	bool GfxStreamWriter::_reopenStream()
	{
		if (!m_pBuffer)
			m_pBuffer = new char[m_capacity];
		return true;
	}

	//____ _isStreamOpen() ____________________________________________________

	bool GfxStreamWriter::_isStreamOpen()
	{
		return m_pBuffer;
	}

	//____ _reserveStream() _________________________________________________________

	void GfxStreamWriter::_reserveStream(int bytes)
	{
		if (bytes <= m_capacity - m_size)
			return;

		_flushStream();

		if (bytes > m_capacity)
		{
			delete[] m_pBuffer;
			m_pBuffer = new char[bytes];
			m_capacity = bytes;
		}
	}

	//____ _pushChar() ________________________________________________________

	void GfxStreamWriter::_pushChar(char c)
	{
		m_pBuffer[m_size++] = c;
	}

	//____ _pushShort() ________________________________________________________

	void GfxStreamWriter::_pushShort(short s)
	{
		*(short*)(&m_pBuffer[m_size]) = s;
		m_size += 2;
	}

	//____ _pushInt() _________________________________________________________

	void GfxStreamWriter::_pushInt(int i)
	{
//		*(int*)(&m_pBuffer[m_size]) = i;

		*(short*)(&m_pBuffer[m_size]) = (short)i;
		*(short*)(&m_pBuffer[m_size+2]) = (short)(i >> 16);
		m_size += 4;
	}

	//____ _pushFloat() _______________________________________________________

	void GfxStreamWriter::_pushFloat(float f)
	{
//		*(float*)(&m_pBuffer[m_size]) = f;

		int i = *((int*)&f);

		*(short*)(&m_pBuffer[m_size]) = (short)i;
		*(short*)(&m_pBuffer[m_size + 2]) = (short)(i >> 16);
		m_size += 4;
	}

	//____ _pushBytes() _________________________________________________________

	void GfxStreamWriter::_pushBytes(int nBytes, char * pBytes)
	{
		std::memcpy(&m_pBuffer[m_size], pBytes, nBytes);
		m_size += nBytes;
	}


} // namespace wg
