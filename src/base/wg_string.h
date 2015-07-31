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

namespace wg 
{
	
	
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
	
		const WgChar*	chars() const;
		int				length() const;
		bool			isEmpty() const;
	
		void			set( const WgString& str );
		void			set( const WgCharBuffer* pBuffer );
		void			set( const WgCharSeq& seq );
	
		void			append( const WgString& str );
		void			append( const WgCharBuffer* pBuffer );
		void			append( const WgCharSeq& seq );
	
		void			prepend( const WgString& str );
		void			prepend( const WgCharBuffer* pBuffer );
		void			prepend( const WgCharSeq& seq );
	
		void			clear();
	
	
		inline WgString& operator = ( const WgString& kSource )
		{
			set( kSource );
			return *this;
		}
		inline WgString& operator = ( const WgCharBuffer* pBuffer )
		{
			set( pBuffer );
			return *this;
		}
		inline WgString& operator = ( const WgCharSeq& seq )
		{
			set( seq );
			return *this;
		}
	
		inline WgString& operator += ( const WgString& kSource )
		{
			append( kSource );
			return *this;
		}
		inline WgString& operator += ( const WgCharBuffer* pBuffer )
		{
			append( pBuffer );
			return *this;
		}
		inline WgString& operator += ( const WgCharSeq& seq )
		{
			append( seq );
			return *this;
		}
	
		inline WgString operator + ( const WgString& kSource ) const
		{
			WgString str(*this);
			str.append( kSource );
			return str;
		}
		inline WgString operator + ( const WgCharBuffer* pBuffer ) const
		{
			WgString str(*this);
			str.append( pBuffer );
			return str;
		}
		inline WgString operator + ( const WgCharSeq& seq ) const
		{
			WgString str(*this);
			str.append( seq );
			return str;
		}
	
	
		bool operator == ( const WgString& kOther ) const	{ return (m_buffer == kOther.m_buffer); }
	
	
	private:
	
		WgCharBuffer	m_buffer;
	};
	

} // namespace wg
#endif //WG_STRING_DOT_H
