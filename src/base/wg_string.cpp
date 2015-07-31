#include <memory.h>

#include <wg_string.h>
#include <wg_charseq.h>
#include <wg_texttool.h>

namespace wg 
{
	
	
	//____ Constructor _____________________________________________________________
	
	WgString::WgString()
	{
	}
	
	WgString::WgString( const WgString& str )
	{
		m_buffer = str.m_buffer;
	}
	
	WgString::WgString( const WgCharBuffer* pBuffer )
	{
		if( pBuffer )
		{
			m_buffer = * pBuffer;
			m_buffer.trim();
		}
	}
	
	WgString::WgString( const WgCharSeq& seq )
	{
		m_buffer.pushBack( seq );
	}
	
	
	WgString::~WgString()
	{
	}
	
	const WgChar* WgString::chars() const
	{
		return m_buffer.chars();
	}
	
	int WgString::length() const
	{
		return (int) m_buffer.nbChars();
	}
	
	bool WgString::isEmpty() const
	{
		return m_buffer.nbChars()==0?true:false;
	}
	
	//____ set() ___________________________________________________________________
	
	void WgString::set( const WgString& str )
	{
		m_buffer = str.m_buffer;
	}
	
	void WgString::set( const WgCharBuffer* pBuffer )
	{
		if( pBuffer )
		{	
			m_buffer = * pBuffer;
			m_buffer.trim();
		}
		else
			m_buffer.reset();
	}
	
	void WgString::set( const WgCharSeq& seq )
	{
		m_buffer.reset(seq.length());
		m_buffer.pushBack(seq);
	}
	
	//____ append() ________________________________________________________________
	
	void WgString::append( const WgString& str )
	{
		m_buffer.pushBack(str.chars(), str.length());
	}
	
	void WgString::append( const WgCharBuffer* pBuffer )
	{
		if( pBuffer )
			m_buffer.pushBack( pBuffer->chars(), pBuffer->nbChars() );
	}
	
	void WgString::append( const WgCharSeq& seq )
	{
		m_buffer.pushBack(seq);
	}
	
	//____ prepend() _______________________________________________________________
	
	void WgString::prepend( const WgString& str )
	{
		m_buffer.pushFront(str.chars(), str.length());
	}
	
	void WgString::prepend( const WgCharBuffer* pBuffer )
	{
		if( pBuffer )
			m_buffer.pushFront( pBuffer->chars(), pBuffer->nbChars() );
	}
	
	void WgString::prepend( const WgCharSeq& seq )
	{
		m_buffer.pushFront(seq);
	}
	
	//____ clear() _________________________________________________________________
	
	void WgString::clear()
	{
		m_buffer.reset();
	}

} // namespace wg
