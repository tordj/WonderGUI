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

#include <wg_gfxstreamwriter.h>
#include <assert.h>

namespace wg 
{
	
	const char GfxStreamWriter::CLASSNAME[] = {"GfxStreamWriter"};
	

	//____ create() ___________________________________________________________

	GfxStreamWriter_p GfxStreamWriter::create(std::function<void(int nBytes, const void * pData)> dispatcher, int maxPackageSize )
	{
		return new GfxStreamWriter(dispatcher, maxPackageSize);
	}

	//____ Constructor _____________________________________________________________
	
	GfxStreamWriter::GfxStreamWriter(std::function<void(int nBytes, const void * pData)> dispatcher, int packageSize) : stream(this)
	{
		m_dispatcher = dispatcher;
		m_pBuffer	= new char[packageSize];
		m_capacity	= packageSize;
		m_size		= 0;
		m_pChunkHead = nullptr;
	}
	
	//____ Destructor _________________________________________________________

	GfxStreamWriter::~GfxStreamWriter()
	{
		delete[] m_pBuffer;
	}

	//____ isInstanceOf() _________________________________________________________
	
	bool GfxStreamWriter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * GfxStreamWriter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	GfxStreamWriter_p GfxStreamWriter::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return GfxStreamWriter_p( static_cast<GfxStreamWriter*>(pObject) );
	
		return 0;
	}

	//____ _object() __________________________________________________________

	Object * GfxStreamWriter::_object()
	{
		return this;
	}

	//____ _flushStream() ____________________________________________________________

	void GfxStreamWriter::_flushStream()
	{
		assert(m_pChunkHead == nullptr);

		m_dispatcher(m_size, m_pBuffer);
		m_size = 0;
	}

	//____ _closeStream() ____________________________________________________________

	void GfxStreamWriter::_closeStream()
	{
		_flushStream();
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
		*(int*)(&m_pBuffer[m_size]) = i; 
		m_size += 4; 
	}

	//____ _pushFloat() _______________________________________________________

	void GfxStreamWriter::_pushFloat(float f) 
	{ 
		*(float*)(&m_pBuffer[m_size]) = f; 
		m_size += 4; 
	}

	//____ _pushBytes() _________________________________________________________

	void GfxStreamWriter::_pushBytes(int nBytes, char * pBytes)
	{
		std::memcpy(&m_pBuffer[m_size], pBytes, nBytes);
		m_size += nBytes;
	}


} // namespace wg
