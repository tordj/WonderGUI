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
	
	
	class Char;
	
	//____ String _____________________________________________________________
	/**
	 * @brief Decorated string.
	 *
	 * String is a simple string-class for decorated strings. Each character
	 * in the string can have its own style.
	 *
	 **/
	
	
	class String
	{
	public:
		friend class CharBuffer;
	
		String();
		String( const String& str );
		String( const CharBuffer* pBuffer );
		String( const CharSeq& seq );
		~String();
	
		const Char*	chars() const;
		int				length() const;
		bool			isEmpty() const;
	
		void			set( const String& str );
		void			set( const CharBuffer* pBuffer );
		void			set( const CharSeq& seq );
	
		void			append( const String& str );
		void			append( const CharBuffer* pBuffer );
		void			append( const CharSeq& seq );
	
		void			prepend( const String& str );
		void			prepend( const CharBuffer* pBuffer );
		void			prepend( const CharSeq& seq );
	
		void			clear();
	
	
		inline String& operator = ( const String& kSource )
		{
			set( kSource );
			return *this;
		}
		inline String& operator = ( const CharBuffer* pBuffer )
		{
			set( pBuffer );
			return *this;
		}
		inline String& operator = ( const CharSeq& seq )
		{
			set( seq );
			return *this;
		}
	
		inline String& operator += ( const String& kSource )
		{
			append( kSource );
			return *this;
		}
		inline String& operator += ( const CharBuffer* pBuffer )
		{
			append( pBuffer );
			return *this;
		}
		inline String& operator += ( const CharSeq& seq )
		{
			append( seq );
			return *this;
		}
	
		inline String operator + ( const String& kSource ) const
		{
			String str(*this);
			str.append( kSource );
			return str;
		}
		inline String operator + ( const CharBuffer* pBuffer ) const
		{
			String str(*this);
			str.append( pBuffer );
			return str;
		}
		inline String operator + ( const CharSeq& seq ) const
		{
			String str(*this);
			str.append( seq );
			return str;
		}
	
	
		bool operator == ( const String& kOther ) const	{ return (m_buffer == kOther.m_buffer); }
	
	
	private:
	
		CharBuffer	m_buffer;
	};
	

} // namespace wg
#endif //WG_STRING_DOT_H
