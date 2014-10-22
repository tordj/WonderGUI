#ifndef	WG_STRING_DOT_H
#define WG_STRING_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CHARBUFFER_DOT_H
#	include <wg_charbuffer.h>
#endif

#ifndef WG_CHARSEQ_DOT_H
#	include <wg_charseq.h>
#endif


class WgChar;

//____ WgString _____________________________________________________________
/**
 * @brief Decorated string.
 *
 * WgString is a simple string-class for decorated strings. Each character
 * in the string can have its own style.
 *
 **/


class WgString
{
public:
	friend class WgCharBuffer;

	WgString();
	WgString( const WgString& str );
	WgString( const WgCharBuffer* pBuffer );
	WgString( const WgCharSeq& seq );
	~WgString();

	const WgChar*	Chars() const;
	int				Length() const;
	bool			IsEmpty() const;

	void			Set( const WgString& str );
	void			Set( const WgCharBuffer* pBuffer );
	void			Set( const WgCharSeq& seq );

	void			Append( const WgString& str );
	void			Append( const WgCharBuffer* pBuffer );
	void			Append( const WgCharSeq& seq );

	void			Prepend( const WgString& str );
	void			Prepend( const WgCharBuffer* pBuffer );
	void			Prepend( const WgCharSeq& seq );

	void			Clear();


	inline WgString& operator = ( const WgString& kSource )
	{
		Set( kSource );
		return *this;
	}
	inline WgString& operator = ( const WgCharBuffer* pBuffer )
	{
		Set( pBuffer );
		return *this;
	}
	inline WgString& operator = ( const WgCharSeq& seq )
	{
		Set( seq );
		return *this;
	}

	inline WgString& operator += ( const WgString& kSource )
	{
		Append( kSource );
		return *this;
	}
	inline WgString& operator += ( const WgCharBuffer* pBuffer )
	{
		Append( pBuffer );
		return *this;
	}
	inline WgString& operator += ( const WgCharSeq& seq )
	{
		Append( seq );
		return *this;
	}

	inline WgString operator + ( const WgString& kSource ) const
	{
		WgString str(*this);
		str.Append( kSource );
		return str;
	}
	inline WgString operator + ( const WgCharBuffer* pBuffer ) const
	{
		WgString str(*this);
		str.Append( pBuffer );
		return str;
	}
	inline WgString operator + ( const WgCharSeq& seq ) const
	{
		WgString str(*this);
		str.Append( seq );
		return str;
	}


	bool operator == ( const WgString& kOther ) const	{ return (m_buffer == kOther.m_buffer); }


private:

	WgCharBuffer	m_buffer;
};

#endif //WG_STRING_DOT_H
