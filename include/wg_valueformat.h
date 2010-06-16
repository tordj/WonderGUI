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


#ifndef	WG_VALUEFORMAT_DOT_H
#define WG_VALUEFORMAT_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

//____ WgValueFormat __________________________________________________________

class WgValueFormat
{
	public:
		WgValueFormat();
		WgValueFormat( const WgCharSeq& format );
		WgValueFormat( const WgValueFormat& in );
		WgValueFormat(	int nInt, int nDec, int grouping = 0, bool bPlus = false,
						Uint16 _separator = 0xA0 /*0xA0=NO_BREAK_SPACE*/, Uint16 period = 0x2e, bool bForcePeriod = false, const char * pPrefix = 0, const char * pSuffix = 0 );

		void setFormat( int nInt, int nDec, int grouping, bool bPlus = false,
						Uint16 _separator = 0, Uint16 period = 0, bool bForcePeriod = false );

		void setPrefix( const Uint16 * pText );
		void setPrefix( const char * pText );
		inline const Uint16* getPrefix() const { return prefix; }

		void setSuffix( const Uint16 * pText );
		void setSuffix( const char * pText );
		inline const Uint16* getSuffix() const { return suffix; }
	
		inline void setTextProperties( WgTextPropPtr& _pProperties ) { pTextProperties = _pProperties; bSetTextProp = true; }
		inline void clearTextProperties() { bSetTextProp = false; }
		inline WgTextPropPtr getTextProperties() const { return pTextProperties; }

		inline void setNoDecimalThreshold( int maxValue ) { noDecimalThreshold = maxValue; }
		inline void clearNoDecimalThreshold() { noDecimalThreshold = 0; }
		inline int  getNoDecimalThreshold() const { return noDecimalThreshold; }

		inline void setForceDecimals( bool bForce ) { bForceDecimals = bForce; }
		inline bool	getForceDecimals() const { return bForceDecimals; }

		inline void setZeroIsNegative( bool bNegative ) { bZeroIsNegative = bNegative; }
		inline bool	getZeroIsNegative() const { return bZeroIsNegative; }

		inline void		setPeriod(Uint16 _period) { period = _period; }
		inline Uint16	getPeriod() const { return period; }

		inline void		setForcePeriod(bool _bForcePeriod) { bForcePeriod = _bForcePeriod; }
		inline bool		getForcePeriod() const { return bForcePeriod; }

		inline void		setIntegers(int nInt) { integers = nInt; }
		inline Uint8	getIntegers() const { return integers; }

		inline void		setDecimals(int nDec) { decimals= nDec; }
		inline Uint8	getDecimals() const { return decimals; }

		inline void		setGrouping(int _grouping) { grouping = _grouping; }
		inline Uint8	getGrouping() const { return grouping; }

		inline void		setPlus(bool _bPlus) { bPlus = _bPlus; }
		inline bool		getPlus() const { return bPlus; }

		inline void		setSeparator(Uint16 _separator) { separator = _separator; }
		inline Uint16	getSeparator() const { return separator; }


		Uint8		integers;			/// Lowest number of integers to display, padded with zeroes.
		Uint8		decimals;			/// Number of decimals to display.
		Uint8		grouping;			/// Number of integers to group together. 0 = no grouping
		Uint16		separator;			/// Character to separate integer groups with.
		Uint16		period;				/// Character used for separating integer and decimal part
		Uint16		prefix[4];			/// Maximum of 4 characters preceding the value, like $, £ or similar.
		Uint16		suffix[4];			/// Maximum of 4 characters following the value.
		bool		bPlus;				/// Set if a plus sign should be preceding positive value.
		bool		bZeroIsNegative;	/// Set if zero value should be deemed negative and be preceeded by minus.
		bool		bForcePeriod;		/// Display period even if there are no decimals?
		WgTextPropPtr pTextProperties;	/// Text properties for generated text.
		bool		bSetTextProp;		/// Set if properties should be set for text.
		bool		bForceDecimals;		/// Display decimals even if they are 0
		int			noDecimalThreshold;	/// If the value is >= this, no decimals will be displayed.
};


#endif
