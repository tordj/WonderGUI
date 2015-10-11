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

#ifndef WG_INTERFACE_VALUEHOLDER_DOT_H
#define	WG_INTERFACE_VALUEHOLDER_DOT_H


#ifndef	WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

#ifndef WG_SCROLLBARTARGET_DOT_H
#	include <wg_scrollbartarget.h>
#endif

namespace wg 
{
	
	class	Font;
	
	
	class _Interface_ValueHolder : public ScrollbarTarget
	{
	public:
	
		_Interface_ValueHolder();
	
		bool	setFonts( const Font_p& _pFonts );
		bool	setRange( int64_t min, int64_t max );
		bool	setUnitSize( int unitSize );
	
		virtual void					setFormat( const ValueFormat_p& format ) = 0;
		virtual ValueFormat_p		format() const = 0;
	
		bool	setValue( int64_t value );
		bool	setFractionalValue( float fraction );		///< Set value as fraction of range (0 -> 1.f)
		bool	setFractionalRounding( int nValueDigits = 0, int lastDigitModulo = 1 );
	
		bool	setStepSize( int size );						///< Step size for incValue() and decValue().
		bool	incValue();														///< Increase value by step size.
		bool	decValue();														///< Decrease value by step size.
	
		bool	incValue( int increment );					///< Increase value by parameter.
		bool	decValue( int decrement );					///< Decrease value by parameter.
	
		inline int64_t	value() {return m_value;};
		float			fractionalValue();
		inline uint32_t	unitSize() {return m_unitSize;}
		inline uint32_t	stepSize() { return m_stepSize;}
		inline int64_t	minLimit() { return m_rangeMin; }
		inline int64_t	maxLimit() { return m_rangeMax; }
		inline uint32_t	valueDigits() { return m_nValueDigits; }
		inline uint32_t	modulator() { return m_modulator; }
	
	protected:
		virtual void	_valueModified() = 0;				///< Called when value has been modified.
		virtual void	_rangeModified() = 0;				///< Called when range (and thus fractional value) has been modified.
	
		void			_onCloneContent( const _Interface_ValueHolder * pOrg );
	
		bool			_setValue( int64_t value );
		bool			_setFractionalValue( float fraction );		///< Set value as fraction of range (0 -> 1.f)
	
		// Inherited from scrollbartarget
	
		float			_stepFwd();
		float			_stepBwd();
		float			_jumpFwd();
		float			_jumpBwd();
		float			_wheelRolled( int distance );
		float			_setPosition( float fraction );
		float			_getHandlePosition();
		float			_getHandleSize();
	
		//
	
		int64_t			m_rangeMin;
		int64_t			m_rangeMax;
		int64_t			m_value;
		int				m_stepSize;
		int				m_unitSize;
	
		int				m_nValueDigits;						///< Number of value digits to truncate a fractional value into. 0 = no runding.
		int				m_modulator;						///< Modulo for lowest value digit when truncating a fractional value. 1, 2 or 5.
	};
	
	

} // namespace wg
#endif //WG_INTERFACE_VALUEHOLDER_DOT_H
