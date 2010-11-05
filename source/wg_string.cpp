#include <memory.h>

#include <wg_string.h>
#include <wg_charseq.h>
#include <wg_texttool.h>


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
		m_buffer.Trim();
	}
}

WgString::WgString( const WgCharSeq& seq )
{
	m_buffer.PushBack( seq );
}


WgString::~WgString()
{
}

const WgChar* WgString::Chars() const
{
	return m_buffer.Chars();
}

Uint32 WgString::Length() const
{
	return m_buffer.NbChars();
}

bool WgString::IsEmpty() const
{
	return m_buffer.NbChars()==0?true:false;
}

//____ Set() ___________________________________________________________________

void WgString::Set( const WgString& str )
{
	m_buffer = str.m_buffer;
}

void WgString::Set( const WgCharBuffer* pBuffer )
{
	if( pBuffer )
	{	
		m_buffer = * pBuffer;
		m_buffer.Trim();
	}
	else
		m_buffer.Reset();
}

void WgString::Set( const WgCharSeq& seq )
{
	m_buffer.Reset(seq.Length());
	m_buffer.PushBack(seq);
}

//____ Append() ________________________________________________________________

void WgString::Append( const WgString& str )
{
	m_buffer.PushBack(str.Chars(), str.Length());
}

void WgString::Append( const WgCharBuffer* pBuffer )
{
	if( pBuffer )
		m_buffer.PushBack( pBuffer->Chars(), pBuffer->NbChars() );
}

void WgString::Append( const WgCharSeq& seq )
{
	m_buffer.PushBack(seq);
}

//____ Prepend() _______________________________________________________________

void WgString::Prepend( const WgString& str )
{
	m_buffer.PushFront(str.Chars(), str.Length());
}

void WgString::Prepend( const WgCharBuffer* pBuffer )
{
	if( pBuffer )
		m_buffer.PushFront( pBuffer->Chars(), pBuffer->NbChars() );
}

void WgString::Prepend( const WgCharSeq& seq )
{
	m_buffer.PushFront(seq);
}

//____ Clear() _________________________________________________________________

void WgString::Clear()
{
	m_buffer.Reset();
}
