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

class	WgFont;


class Wg_Interface_ValueHolder
{
public:

	Wg_Interface_ValueHolder();

	bool	SetFonts( WgFont * _pFonts );
	bool	SetRange( Sint64 min, Sint64 max );
	bool	SetUnitSize( Uint32 unitSize );

	virtual void					SetFormat( const WgValueFormat& format ) = 0;
	virtual const WgValueFormat&	GetFormat() const = 0;

	bool	SetValue( Sint64 value );
	bool	SetFractionalValue( float fraction );		///< Set value as fraction of range (0 -> 1.f)
	bool	SetFractionalRounding( Uint32 nValueDigits = 0, Uint32 lastDigitModulo = 1 );

	bool	SetStepSize( Uint32 size );						///< Step size for IncValue() and decValue().
	bool	IncValue();														///< Increase value by step size.
	bool	DecValue();														///< Decrease value by step size.

	bool	IncValue( int increment );					///< Increase value by parameter.
	bool	DecValue( int decrement );					///< Decrease value by parameter.

	inline Sint64	Value() {return m_value;};
	float			FractionalValue();
	inline Uint32	UnitSize() {return m_unitSize;}
	inline Uint32	StepSize() { return m_stepSize;}
	inline Sint64	MinLimit() { return m_rangeMin; }
	inline Sint64	MaxLimit() { return m_rangeMax; }
	inline Uint32	ValueDigits() { return m_nValueDigits; }
	inline Uint32	Modulator() { return m_modulator; }

protected:
	virtual void	_valueModified() = 0;				///< Called when value has been modified.
	virtual void	_rangeModified() = 0;				///< Called when range (and thus fractional value) has been modified.

	void			_cloneInterface( Wg_Interface_ValueHolder * _pClone );
	void			_onCloneContent( const Wg_Interface_ValueHolder * pOrg );

	Sint64			m_rangeMin;
	Sint64			m_rangeMax;
	Sint64			m_value;
	Uint32			m_stepSize;
	Uint32			m_unitSize;

	Uint32			m_nValueDigits;						///< Number of value digits to truncate a fractional value into. 0 = no runding.
	Uint32			m_modulator;						///< Modulo for lowest value digit when truncating a fractional value. 1, 2 or 5.
};


#endif //WG_INTERFACE_VALUEHOLDER_DOT_H

