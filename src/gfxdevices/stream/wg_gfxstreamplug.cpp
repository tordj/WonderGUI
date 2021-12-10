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

#include <wg_gfxstreamplug.h>

#include <cstring>
#include <cassert>

namespace wg
{

	const TypeInfo GfxStreamPlug::TYPEINFO = { "GfxStreamPlug", &Object::TYPEINFO };

	//____ create() ___________________________________________________________

	GfxStreamPlug_p GfxStreamPlug::create(int capacity)
	{
		return new GfxStreamPlug(capacity);
	}

	//____ constructor ________________________________________________________

	GfxStreamPlug::GfxStreamPlug(int capacity) : input(this), output{ &m_outStream[0], &m_outStream[1], &m_outStream[2], &m_outStream[3] }
	{
		m_bufferSize = ((capacity+1)&0xFFFFFFFE)+2; // Enforce buffer size to even bytes, add 2-byte margin to prevent write pointer passing read pointer.

		m_pBuffer = new char[m_bufferSize];
		m_writeOfs = 0;

		for (int i = 0; i < c_maxOutputs; i++)
		{
			m_outStream[i].readOfs = -1;
			m_outStream[i].pObj = this;
		}
	}

	//____ Destructor _________________________________________________________

	GfxStreamPlug::~GfxStreamPlug()
	{
		delete [] m_pBuffer;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamPlug::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ openOutput() _______________________________________________________

	void GfxStreamPlug::openOutput(int index)
	{
		assert(index >= 0 && index < c_maxOutputs);
		output[index].reopen();
	}

	//____ bufferSize() _____________________________________________________________

	int GfxStreamPlug::bufferSize()
	{
		int maxSize = 0;

		for( int i = 0 ; i < c_maxOutputs ; i++ )
			maxSize = max(m_outStream[i].size(), maxSize);

		return maxSize;
	}

	//____ bufferCapacity() _________________________________________________________

	int GfxStreamPlug::bufferCapacity()
	{
		return m_bufferSize-2;
	}

	//____ setBufferCapacity() ______________________________________________________

	bool GfxStreamPlug::setBufferCapacity(int capacity)
	{
		if (capacity < bufferSize())
			return false;

		capacity = ((capacity + 1) & 0xFFFFFFFE);	// Enforce buffer size to even bytes.

		_resize(capacity+2);			// Buffer neds 2 extra bytes to keep byte alignment and prevent write pointer to wrap read pointers.
		return true;
	}

	//____ clearBuffer() ____________________________________________________________

	void GfxStreamPlug::clearBuffer()
	{
		m_writeOfs = 0;
		for (int i = 0; i < c_maxOutputs; i++)
			if( m_outStream[i].readOfs != -1 )
				m_outStream[i].readOfs = 0;
	}

	//____ resize() ___________________________________________________________

	void GfxStreamPlug::_resize(int newSize)
	{
		char * pNew = new char[newSize];

		int bytesToCopy = bufferSize();

		// Copy data that has not been read by all yet to beginning of new buffer

		if (bytesToCopy > 0)
		{
			int copyFromOfs = min(bytesToCopy, m_writeOfs);
			int copyFromEnd = bytesToCopy - copyFromOfs;

			if (copyFromEnd > 0)
				std::memcpy(pNew, &m_pBuffer[m_bufferSize - copyFromEnd], copyFromEnd);

			std::memcpy(pNew + copyFromEnd, &m_pBuffer[m_writeOfs - copyFromOfs], copyFromOfs );
		}

		// Update all read offsets.

		for (auto& stream : m_outStream)
		{
			if (stream.readOfs >= 0)
			{
				int size = stream.readOfs < m_writeOfs ? m_writeOfs - stream.readOfs : m_bufferSize - stream.readOfs + m_writeOfs;
				stream.readOfs = bytesToCopy - size;
			}
		}

		// Finalize

		m_bufferSize = newSize;
		m_writeOfs = bytesToCopy;
		delete[] m_pBuffer;
		m_pBuffer = pNew;
	}



	//____ _object() __________________________________________________________

	Object * GfxStreamPlug::_object()
	{
		return this;
	}

	const Object * GfxStreamPlug::_object() const
	{
		return this;
	}

	//____ _reserveStream() _________________________________________________________

	void GfxStreamPlug::_reserveStream(int bytes)
	{
		int contentSize = bufferSize();
		int _capacity = bufferCapacity();

		if (_capacity < contentSize + bytes)
		{
			while (_capacity < contentSize + bytes)
				_capacity *= 2;
			_resize(_capacity + 2);
		}
	}

	//____ _flushStream() ___________________________________________________________

	void GfxStreamPlug::_flushStream()
	{
		// Do nothing
	}

	//____ _closeStream() ___________________________________________________________

	void GfxStreamPlug::_closeStream()
	{
		// Do nothing
	}

	//____ _reopenStream() ___________________________________________________________

	bool GfxStreamPlug::_reopenStream()
	{
		return true;
	}

	//____ _isStreamOpen() ___________________________________________________________

	bool GfxStreamPlug::_isStreamOpen()
	{
		return true;
	}

	//____ _pushChar() ________________________________________________________

	void GfxStreamPlug::_pushChar(char c)
	{
		m_pBuffer[m_writeOfs] = c;
		m_writeOfs = (m_writeOfs + 1) % m_bufferSize;
	}

	//____ _pushShort() _______________________________________________________

	void GfxStreamPlug::_pushShort(short s)
	{        
		*(short*)(&m_pBuffer[m_writeOfs]) = s;
		m_writeOfs = (m_writeOfs + 2) % m_bufferSize;
	}

	//____ _pushInt() _______________________________________________________

	void GfxStreamPlug::_pushInt(int i)
	{
		// Push two bytes at a time since:
		// a) We need to take architectures that can only read/write ints at 4-byte alignment into acount.
		// b) We don't want to accidentally write outside our buffer (which always has a size divisible by 2).

//		*(int*)(&m_pBuffer[m_writeOfs]) = i;
//		m_writeOfs = (m_writeOfs + 4) % m_bufferSize;

		*(short*)(&m_pBuffer[m_writeOfs]) = (short) i;
		m_writeOfs = (m_writeOfs + 2) % m_bufferSize;
		*(short*)(&m_pBuffer[m_writeOfs]) = (short) (i>>16);
		m_writeOfs = (m_writeOfs + 2) % m_bufferSize;
	}

	//____ _pushFloat() _______________________________________________________

	void GfxStreamPlug::_pushFloat(float f)
	{
//		*(float*)(&m_pBuffer[m_writeOfs]) = f;
//		m_writeOfs = (m_writeOfs + 4) % m_bufferSize;

		int i = *((int*)&f);

		*(short*)(&m_pBuffer[m_writeOfs]) = (short)i;
		m_writeOfs = (m_writeOfs + 2) % m_bufferSize;
		*(short*)(&m_pBuffer[m_writeOfs]) = (short)(i >> 16);
		m_writeOfs = (m_writeOfs + 2) % m_bufferSize;
	}

	//____ _pushBytes() _______________________________________________________

	void GfxStreamPlug::_pushBytes(int nBytes, char * pBytes)
	{
		if (m_writeOfs + nBytes > m_bufferSize)
		{
			int toCopy = m_bufferSize - m_writeOfs;
			std::memcpy(&m_pBuffer[m_writeOfs], pBytes, toCopy);
			pBytes += toCopy;
			nBytes -= toCopy;
			m_writeOfs = 0;
		}

		std::memcpy( &m_pBuffer[m_writeOfs], pBytes, nBytes);
		m_writeOfs = (m_writeOfs + nBytes) % m_bufferSize;
	}

	//____ OutStreamProxy::_object() ______________________________________________________

	Object * GfxStreamPlug::OutStreamProxy::_object()
	{
		return pObj;
	}

	const Object * GfxStreamPlug::OutStreamProxy::_object() const
	{
		return pObj;
	}

	//____ OutStreamProxy::_closeStream() ______________________________________________________

	void GfxStreamPlug::OutStreamProxy::_closeStream()
	{
		readOfs = -1;
	}

	//____ OutStreamProxy::_reopentream() ______________________________________________________

	bool GfxStreamPlug::OutStreamProxy::_reopenStream()
	{
		if( readOfs < 0 )
			readOfs = pObj->m_writeOfs;
		return true;
	}

	//____ OutStreamProxy::_isStreamOpen() ______________________________________________________

	bool GfxStreamPlug::OutStreamProxy::_isStreamOpen()
	{
		return readOfs >= 0;
	}

	//____ OutStreamProxy::_hasChunk() ______________________________________________________

	bool GfxStreamPlug::OutStreamProxy::_hasChunk()
	{
		int sz = size();
		if (sz < 4)
			return false;

		int ofs = (readOfs + 2) % pObj->m_bufferSize;
		short chunkSize = *(short*)&pObj->m_pBuffer[ofs] + 4;

		if (sz < chunkSize)
			return false;

		return true;
	}

	//____ OutStreamProxy::_peekChunk() ______________________________________________________

	GfxStream::Header GfxStreamPlug::OutStreamProxy::_peekChunk()
	{
		int sizeOfs = (readOfs + 2) % pObj->m_bufferSize;

		return { (GfxChunkId)(*(uint16_t*)&pObj->m_pBuffer[readOfs]), *(uint16_t*)&pObj->m_pBuffer[sizeOfs] };
	}

	//____ OutStreamProxy::_pullChar() ______________________________________________________

	char GfxStreamPlug::OutStreamProxy::_pullChar()
	{
		char x = pObj->m_pBuffer[readOfs];
		readOfs = (readOfs + 1) % pObj->m_bufferSize;
		return x;
	}

	//____ OutStreamProxy::_pullShort() ______________________________________________________

	short GfxStreamPlug::OutStreamProxy::_pullShort()
	{
		short x = *(short*)&pObj->m_pBuffer[readOfs];
		readOfs = (readOfs + 2) % pObj->m_bufferSize;
		return x;
	}

	//____ OutStreamProxy::_pullInt() ______________________________________________________

	int GfxStreamPlug::OutStreamProxy::_pullInt()
	{
//		int x = *(int*)&pObj->m_pBuffer[readOfs];
//		readOfs = (readOfs + 4) % pObj->m_bufferSize;

		int x = *(uint16_t*)&pObj->m_pBuffer[readOfs];
		readOfs = (readOfs + 2) % pObj->m_bufferSize;
		x += (*(uint16_t*)&pObj->m_pBuffer[readOfs] << 16);
		readOfs = (readOfs + 2) % pObj->m_bufferSize;

		return x;
	}

	//____ OutStreamProxy::_pullFloat() ______________________________________________________

	float GfxStreamPlug::OutStreamProxy::_pullFloat()
	{
//		float x = *(float*)&pObj->m_pBuffer[readOfs];
//		readOfs = (readOfs + 4) % pObj->m_bufferSize;

		int x = *(uint16_t*)&pObj->m_pBuffer[readOfs];
		readOfs = (readOfs + 2) % pObj->m_bufferSize;
		x += (*(uint16_t*)&pObj->m_pBuffer[readOfs] << 16);
		readOfs = (readOfs + 2) % pObj->m_bufferSize;

		return * ((float*)&x);
	}

	//____ OutStreamProxy::_pullBytes() ______________________________________________________

	void GfxStreamPlug::OutStreamProxy::_pullBytes(int nBytes, char * pBytes)
	{
		int bufferSize = pObj->m_bufferSize;

		if (readOfs + nBytes > bufferSize)
		{
			int toCopy = bufferSize - readOfs;
			std::memcpy(pBytes, &pObj->m_pBuffer[readOfs], toCopy);
			pBytes += toCopy;
			nBytes -= toCopy;
			readOfs = 0;
		}

		std::memcpy(pBytes, &pObj->m_pBuffer[readOfs], nBytes);
		readOfs = (readOfs + nBytes) % bufferSize;
	}

	//____ OutStreamProxy::_skipBytes() ______________________________________________________

	void GfxStreamPlug::OutStreamProxy::_skipBytes(int nBytes)
	{
		readOfs = (readOfs + nBytes) % pObj->m_bufferSize;
	}


	//____ OutStreamProxy::size() ______________________________________________________

	int GfxStreamPlug::OutStreamProxy::size()
	{
		if (readOfs < 0)
			return 0;						// Stream does not exist.

		return readOfs <= pObj->m_writeOfs ? pObj->m_writeOfs - readOfs : pObj->m_bufferSize - readOfs + pObj->m_writeOfs;
	}

} // namespace wg
