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

#include <wg_gfxstreamreader.h>
#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamReader::TYPEINFO = { "GfxStreamReader", &Object::TYPEINFO };

	const int GfxStreamReader::c_bufferMargin;

	//____ create() ___________________________________________________________

	GfxStreamReader_p GfxStreamReader::create(std::function<int(int nBytes, void * pDest)> dataFetcher)
	{
		return new GfxStreamReader(dataFetcher);
	}

	//____ constructor _____________________________________________________________

	GfxStreamReader::GfxStreamReader(std::function<int(int nBytes, void * pDest)> dataFeeder) : stream(this)
	{
		m_fetcher = dataFeeder;

		m_pBuffer = new char[c_bufferSize+c_bufferMargin];
		m_readOfs = 0;
		m_writeOfs = 0;
		m_bOpen = true;
	}

	//____ Destructor _________________________________________________________

	GfxStreamReader::~GfxStreamReader()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamReader::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _hasChunk() ________________________________________________________

	bool GfxStreamReader::_hasChunk()
	{
		// Handle all ways of empty data

		if (m_readOfs == m_writeOfs)
			_fetchData();

		if (m_readOfs == m_writeOfs)
			return false;

		// We have some data, make sure we have a complete chunk

		int size = (m_writeOfs - m_readOfs + c_bufferSize) % c_bufferSize;

		if (size < 4 || size - 4 < *(uint16_t*)&m_pBuffer[m_readOfs+2])
		{
			_fetchData();
			size = (m_writeOfs - m_readOfs + c_bufferSize) % c_bufferSize;
		}

		if (size < 4 || size - 4 < *(uint16_t*)&m_pBuffer[m_readOfs+2])
			return false;

		return true;
	}

	//____ _peekChunk() _______________________________________________________

	GfxStream::Header GfxStreamReader::_peekChunk()
	{
		int sizeOfs = (m_readOfs + 2) % c_bufferSize;

		return { (GfxChunkId)(*(short*)&m_pBuffer[m_readOfs]), *(short*)&m_pBuffer[sizeOfs] };
	}


	//____ _pullChar() ________________________________________________________

	char GfxStreamReader::_pullChar()
	{
		char x = m_pBuffer[m_readOfs];
		m_readOfs = (m_readOfs + 1) % c_bufferSize;
		return x;
	}

	//____ _pullShort() _______________________________________________________

	short GfxStreamReader::_pullShort()
	{
		short x = *(short*)&m_pBuffer[m_readOfs];
		m_readOfs = (m_readOfs + 2) % c_bufferSize;
		return x;
	}

	//____ _pullInt() _______________________________________________________

	int GfxStreamReader::_pullInt()
	{
//		int x = *(int*)&m_pBuffer[m_readOfs];
		int x = *(uint16_t*)&m_pBuffer[m_readOfs] + (*(uint16_t*)&m_pBuffer[m_readOfs+2]<<16);
		m_readOfs = (m_readOfs + 4) % c_bufferSize;
		return x;
	}

	//____ _pullFloat() _______________________________________________________

	float GfxStreamReader::_pullFloat()
	{
//		float x = *(float*)&m_pBuffer[m_readOfs];
		float x;
		*((int*)&x) = *(uint16_t*)&m_pBuffer[m_readOfs] + (*(uint16_t*)&m_pBuffer[m_readOfs+2]<<16);
		m_readOfs = (m_readOfs + 4) % c_bufferSize;
		return x;
	}

	//____ _pullBytes() _______________________________________________________

	void GfxStreamReader::_pullBytes(int nBytes, char * pBytes)
	{
		if (m_readOfs + nBytes > c_bufferSize)
		{
			int toCopy = c_bufferSize - m_readOfs;
			std::memcpy(pBytes, &m_pBuffer[m_readOfs], toCopy);
			pBytes += toCopy;
			nBytes -= toCopy;
			m_readOfs = 0;
		}

		std::memcpy(pBytes, &m_pBuffer[m_readOfs], nBytes);
		m_readOfs = (m_readOfs + nBytes) % c_bufferSize;
	}

	//____ _skipBytes() _______________________________________________________

	void GfxStreamReader::_skipBytes(int nBytes)
	{
		m_readOfs = (m_readOfs + nBytes) % c_bufferSize;
	}


	//____ _isStreamOpen() ____________________________________________________

	bool GfxStreamReader::_isStreamOpen()
	{
		return m_bOpen;
	}

	//____ _openStream() ______________________________________________________

	bool GfxStreamReader::_reopenStream()
	{
		m_bOpen = true;
		return true;
	}

	//____ _closeStream() _____________________________________________________

	void GfxStreamReader::_closeStream()
	{
		// Will stop fetching data

		m_bOpen = false;
	}

	//____ _fetchData() _______________________________________________________

	void GfxStreamReader::_fetchData()
	{
		if (!m_bOpen)
			return;

		// Calcuate size of chunks to fill in the circular buffer

		int size = c_bufferSize - ((m_writeOfs - m_readOfs + c_bufferSize) % c_bufferSize) -2;		// -2 since we may not catch up to readOfs

		int chunk1 = min(size, c_bufferSize - m_writeOfs);		// Until readPos or end of buffer
		int chunk2 = size - chunk1;								// From start of buffer

		// Fetch data to first chunk

		int written = m_fetcher(chunk1, m_pBuffer + m_writeOfs);
		if (written == 0)
			return;

		// Update bufferMargin if we have written to any of the first bytes of buffer

		if (m_writeOfs < c_bufferMargin)
		{
			int copyEnd = min(c_bufferMargin, m_writeOfs + written);
			for (int i = m_writeOfs; i < copyEnd; i++)
				m_pBuffer[c_bufferSize + i] = m_pBuffer[i];
		}

		m_writeOfs = (m_writeOfs + written) % c_bufferSize;

		// Handle case where we got less data than requested

		if (written < chunk1)
			return;

		// Fetch data to second chunk

		if (chunk2 > 0)
		{
			written = m_fetcher(chunk2, m_pBuffer);
			m_writeOfs = written;

			// Update bufferMargin

			int copyEnd = min<int>(c_bufferMargin, written);
			for (int i = 0; i < copyEnd; i++)
				m_pBuffer[c_bufferSize + i] = m_pBuffer[i];
		}
	}


} // namespace wg
