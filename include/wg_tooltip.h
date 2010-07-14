#ifndef	WG_TOOLTIP_DOT_H
#define WG_TOOLTIP_DOT_H

#ifndef	WG_STRING_DOT_H
#	include <wg_string.h>
#endif


#define DECLARE_TOOLTIP_SUPPORT()										\
	private:															\
		WgString	m_tooltipString;									\
	public:																\
		void	SetTooltipString( const WgString& str )			{ m_tooltipString = str; }	\
		void	SetTooltipString( const WgChar* pString )	{ m_tooltipString.Set( pString ); }	\
		void	SetTooltipString( const Uint16* pString )	{ m_tooltipString.Set( pString ); }	\
		void	SetTooltipString( const char* pString )		{ m_tooltipString.Set( pString ); }	\
		virtual WgString	GetTooltipString() const	{ return m_tooltipString; }


#endif //WG_TOOLTIP_DOT_H
