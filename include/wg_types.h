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

#ifndef	WG_TYPES_DOT_H
#define WG_TYPES_DOT_H

#ifdef WIN32
#ifndef for
//#	define		for		if(false){}else for
#endif
#endif

#ifdef _MSC_VER
typedef signed __int8     int8_t;
typedef signed __int16    int16_t;
typedef signed __int32    int32_t;
typedef signed __int64    int64_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;
#else
#	include <stdint.h>			// Use the C99 official header
#endif


#ifndef Uint8
	typedef unsigned char	Uint8;
	typedef signed char		Sint8;
	typedef unsigned short	Uint16;
	typedef signed short	Sint16;
	typedef unsigned int	Uint32;
	typedef signed int		Sint32;
	typedef uint64_t		Uint64;
	typedef int64_t			Sint64;
#endif

template<typename T> inline T WgMin(const T &a, const T &b) { return a < b ? a : b; }
template<typename T> inline T WgMax(const T &a, const T &b) { return a > b ? a : b; }
template<typename T> inline T WgAbs(T x)					{ return x >= 0 ? x : -x; }

#define		LIMIT(x, min, max)	if( x < min) x = min; if( x > max) x = max;

//____ WgMode _________________________________________________________________

enum WgMode //: Uint8
{
	WG_MODE_NORMAL =	0,
	WG_MODE_MARKED =	1,		// Widget is highlighted (mouse over)
	WG_MODE_SELECTED =	2,		// Widget is selected (pressed)
	WG_MODE_DISABLED =	3,		// Widget is disabled
	WG_MODE_SPECIAL =	4,		// Graphics block for a special, widget-specific mode.
	WG_MODE_ALL		=	5		// Only supported by certain methods!!!
};

#define	WG_NB_MODES		5		// Number of modes (excluding WG_MODE_ALL )


//____ WgTxtAttr ______________________________________________________________

// Bitmask for various text attributes.

enum WgTxtAttr
{
	WG_TXT_UNDERLINED	= 0x1,
	WG_TXT_SELECTED		= 0x2
};


//____ WgExtChar __________________________________________________________

#define WG_ESCAPE_CODE		0xA4

//0x1b

// Double escape codes should give the escape-code character.

/*
		NEW ONES

		{prop}		Set the named property. Named properties should start with a-z/A-Z.
					If property is unnamed you get {123} where the number is the current handle for the prop.

		-			break permitted
		=			hyphen break permitted
		n			linebreak (like \n).

		Predefined properties

		{n}			empty property (normal/default)
		{b}			bold
		{i}			italic
		{u}			underlined
		{b-i}		bold italic
		{b-u}		bold underlined
		{b-i-u}		bold italic underlined
		{i-u}		italic underlined
		{h1}		heading 1
		{h2}		heading 2
		{h3}		heading 3
		{h4}		heading 4
		{h5}		heading 5
		{u1}		user defined style 1
		{u2}		user defined style 2
		{u3}		user defined style 3
		{u4}		user defined style 4
		{u5}		user defined style 5

		{super}		superscript		// Includes top positioning 
		{sub}		subscript		// Includes bottom positioning
		{mono}		monospaced		// Includes monospacing

		{black}		black text
		{white}		white text

*/



/*
	{[rrggbbaa]		begin color
	}				end color

	[123			begin size, exactly 3 decimal digits sets the size.
	]				end size

	_				begin underlined
	| 				end underlined

	-				break permitted
	=				hyphen break permitted

	d				begin normal (default)
	b				begin bold
	i				begin italic
	I				begin bold italic
	s				begin subscript
	S				begin superscript
	m				begin monospace
	h[1-5]			begin heading
	u[1-5]			begin userdefined style

	#				end style

	(prop)			set a new text property, looked up from a ResDB.
					Other style/color settings are applied ontop of this text property.
					Setting prop as (null) sets an empty prop.



*/


enum WgExtChar
{
	// Code inside WonderGUI assumes these are all
	// in the range 0x80-0xA0, but that might change in the future.

	WG_BREAK_PERMITTED		= 0x82,
	WG_HYPHEN_BREAK_PERMITTED= 0x83,
	WG_NO_BREAK_SPACE		= 0xA0,
};


//____ WgBlendMode ____________________________________________________________

// BlendModes control how blits and fills are blended against the background.

enum WgBlendMode //: Uint8
{
	WG_BLENDMODE_OPAQUE,			///< Completely opaque blitting, ignoring alpha of source and tint-color.
	WG_BLENDMODE_BLEND,				///< Normal mode, alpha of source and tint-color is taken into account.
	WG_BLENDMODE_ADD,				///< RGBA Additive.
	WG_BLENDMODE_MULTIPLY			///< RGBA Multiply.
};

//____ WgFontStyle ____________________________________________________________

enum WgFontStyle
{
	// Needs to stay in sync with WgExtChar::WG_BEGIN_NORMAL and following enums!

	WG_STYLE_NORMAL		= 0,
	WG_STYLE_BOLD		= 1,
	WG_STYLE_BOLD_ITALIC= 2,
	WG_STYLE_ITALIC		= 3,
	WG_STYLE_SUPERSCRIPT= 4,
	WG_STYLE_SUBSCRIPT	= 5,
	WG_STYLE_MONOSPACE	= 6,
	WG_STYLE_HEADING_1	= 7,
	WG_STYLE_HEADING_2	= 8,
	WG_STYLE_HEADING_3	= 9,
	WG_STYLE_HEADING_4	= 10,
	WG_STYLE_HEADING_5	= 11,

	WG_STYLE_USER_1		= 12,
	WG_STYLE_USER_2		= 13,
	WG_STYLE_USER_3		= 14,
	WG_STYLE_USER_4		= 15,
	WG_STYLE_USER_5		= 16,

};

#define WG_NB_FONTSTYLES	17


//____ WgCursorStyle __________________________________________________________

enum WgCursorStyle
{
	WG_CURSOR_ARROW,						// default arrow
	WG_CURSOR_DEFAULT = WG_CURSOR_ARROW,	// default arrow
	WG_CURSOR_HOURGLASS,					// hourglass
	WG_CURSOR_HAND,							// hand
	WG_CURSOR_CROSSHAIR,					// crosshair
	WG_CURSOR_HELP,							// help
	WG_CURSOR_IBEAM,						// I-beam
	WG_CURSOR_STOP,							// slashed circle
	WG_CURSOR_UP_ARROW,						// arrow pointing up
	WG_CURSOR_SIZE_ALL,						// four-pointed arrow in all directions
	WG_CURSOR_SIZE_NE_SW,					// double-pointed arrow pointing northeast and southwest
	WG_CURSOR_SIZE_NW_SE,					// double-pointed arrow pointing northwest and southeast
	WG_CURSOR_SIZE_N_S,						// double-pointed arrow pointing north and south
	WG_CURSOR_SIZE_W_E,						// double-pointed arrow pointing west and east
};



//____ WgTintMode _____________________________________________________________

// WgTintMode controls how TintColors are blended hierarchically.

enum WgTintMode
{
	TINTMODE_OPAQUE,
	TINTMODE_MULTIPLY
};


/*
enum	WgExtChar
{
		BREAK_PERMITTED_HERE	= 0x82,
//		BEGIN_SELECTED_AREA		= 0x86,
//		END_SELECTED_AREA		= 0x87,
		HYPHEN_BREAK_PERMITTED_HERE	= 0x91,
//		BEGIN_UNDERLINE			= 0x91,
//		END_UNDERLINE			= 0x92,

//		BEGIN_BOLD				= 0x93,
//		END_BOLD				= 0x94,
//		BEGIN_ITALIC			= 0x95,
//		END_ITALIC				= 0x96,
//		BEGIN_USEREFFECT1		= 0x97,
//		END_USEREFFECT1			= 0x98,
//		BEGIN_USEREFFECT2		= 0x99,
//		END_USEREFFECT2			= 0x9A,


//		END_COLOR				= 0x9C,
//		START_OF_OS_COMMAND		= 0x9D,
//		END_OF_OS_COMMAND		= 0x9E
};
*/

//____ Class: WgMinMax2D ______________________________________________________

class WgMinMax2D
{
public:
	WgMinMax2D() {}
	WgMinMax2D( Uint16 _minW, Uint16 _minH, Uint16 _maxW, Uint16 _maxH )
				{ minW = _minW; minH = _minH; maxW = _maxW; maxH = _maxH; }

	inline bool Union( const WgMinMax2D& r1, const WgMinMax2D& r2 );
	inline bool	Union( const WgMinMax2D& other );


	Uint16		minW, maxW;
	Uint16		minH, maxH;
};


inline bool WgMinMax2D::Union( const WgMinMax2D& r1 )
{
	if( r1.minW > minW )
		minW = r1.minW;

	if( r1.maxW < maxW )
		maxW = r1.maxW;

	if( r1.minH > minH )
		minH = r1.minH;

	if( r1.maxH < maxH )
		maxH = r1.maxH;

	if( maxW < minW || maxH < minH )
		return false;

	return true;
}

inline bool WgMinMax2D::Union( const WgMinMax2D& r1, const WgMinMax2D& r2 )
{
	if( r1.minW > r2.minW )
		minW = r1.minW;
	else
		minW = r2.minW;

	if( r1.maxW < r2.maxW )
		maxW = r1.maxW;
	else
		maxW = r2.maxW;

	if( r1.minH > r2.minH )
		minH = r1.minH;
	else
		minH = r2.minH;

	if( r1.maxH < r2.maxH )
		maxH = r1.maxH;
	else
		maxH = r2.maxH;

	if( maxW < minW || maxH < minH )
		return false;

	return true;
}


inline bool operator==( const WgMinMax2D& r1, const WgMinMax2D& r2 )
{
	if( r1.minW == r2.minW && r1.minH == r2.minH && r1.maxW == r2.maxW && r1.maxH == r2.maxH )
		return true;
	return false;
}

inline bool operator!=( const WgMinMax2D& r1, const WgMinMax2D& r2 )
{
	if( r1.minW == r2.minW && r1.minH == r2.minH && r1.maxW == r2.maxW && r1.maxH == r2.maxH )
		return false;
	return true;
}

//____ WgTextBlock ____________________________________________________________

class WgTextBlock
{
public:
	WgTextBlock() {}
	WgTextBlock( Uint32 _begLn, Uint32 _begCol, Uint32 _endLn, Uint32 _endCol ) 
					{ begLn = _begLn; begCol = _begCol; endLn = _endLn; endCol = _endCol; }
	WgTextBlock( Uint32 ln ) { begLn = ln; begCol = 0; endLn = ln; endCol = 0xFFFFFFFF; }

	Uint32 begLn;
	Uint32 begCol;
	Uint32 endLn;
	Uint32 endCol;
};


//____ WgUpdateInfo ___________________________________________________________

class		WgUpdateInfo
{
	public:
		WgUpdateInfo();

		bool	  bQuit;					// Have we received an order to quit?
		bool	  bKbd, bMouse, bActive;	// Are we having these focuses?

		Uint32  msTime;						// Counter in milliseconds since start of program (or something).
		Uint32	msDiff;						// Milliseconds between current and previous Update() call.
};



#endif // WG_TYPES_DOT_H
