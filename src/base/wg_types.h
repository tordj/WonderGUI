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


#include <stdint.h>			// Use the C99 official header

namespace wg 
{
		
	#ifndef INT64_MIN
	#define INT64_MIN (-(9223372036854775807 ## L)-1)
	#endif
	
	#ifndef INT64_MAX
	#define INT64_MAX ((9223372036854775807 ## L))__
	#endif

	#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)
	#	 if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#		define WG_IS_BIG_ENDIAN 1
	#		define WG_IS_LITTLE_ENDIAN 0
	#	elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#		define WG_IS_BIG_ENDIAN 0
	#		define WG_IS_LITTLE_ENDIAN 1
	#	endif
	#else
	#	ifdef __BIG_ENDIAN__
	#		if __BIG_ENDIAN__
	#			define WG_IS_BIG_ENDIAN 1
	#			define WG_IS_LITTLE_ENDIAN 0
	#		endif
	#	endif

	#	ifdef __LITTLE_ENDIAN__
	#		if __LITTLE_ENDIAN__
	#			define WG_IS_BIG_ENDIAN 0
	#			define WG_IS_LITTLE_ENDIAN 1
	#		endif
	#	endif
	#endif


	#ifndef WG_IS_BIG_ENDIAN
	#error Could not detect endianness. You need to define WG_IS_BIG_ENDIAN and WG_IS_LITTLE_ENDIAN in wg_types.h
	#define WG_IS_BIG_ENDIAN 0
	#define WG_IS_LITTLE_ENDIAN 0
	#endif
	
	template<typename T> inline T WgMin(const T &a, const T &b) { return a < b ? a : b; }
	template<typename T> inline T WgMin(const T &a, const T &b, const T &c) { if( a < b ) return a < c ? a : c; else return b < c ? b : c; }
	template<typename T> inline T WgMax(const T &a, const T &b) { return a > b ? a : b; }
	template<typename T> inline T WgMax(const T &a, const T &b, const T &c) { if( a > b ) return a > c ? a : c; else return b > c ? b : c; }
	template<typename T> inline T WgAbs(T x)					{ return x >= 0 ? x : -x; }
	template<typename T> inline void WgSwap(T &a, T &b) { T c = a; a = b; b = c; }
	
	#define		LIMIT(x, min, max)	if( x < min) x = min; if( x > max) x = max;
	
	#define	WG_FONTSIZE_INHERIT 0x80000000
	
	enum StateEnum
	{
		WG_STATE_NORMAL						= 0,			///< Element is neither hovered, pressed, selected or focused.
		WG_STATE_SELECTED					= 1,			///< Element is in a selected state, like a selected checkbox or item in a list. 
		WG_STATE_FOCUSED					= 2,			///< Element has keyboard focus
		WG_STATE_FOCUSED_SELECTED			= 2+1,
		WG_STATE_HOVERED					= 4,			///< Mouse pointer is hovering over element.
		WG_STATE_HOVERED_SELECTED			= 4+1,
		WG_STATE_HOVERED_FOCUSED			= 4+2,
		WG_STATE_HOVERED_FOCUSED_SELECTED	= 4+2+1,
		WG_STATE_PRESSED					= 8+4,			///< Mouse button (usually left one) is pressed on element.
		WG_STATE_PRESSED_SELECTED			= 8+4+1,
		WG_STATE_PRESSED_FOCUSED			= 8+4+2,
		WG_STATE_PRESSED_FOCUSED_SELECTED	= 8+4+2+1,
		WG_STATE_DISABLED					= 16,			///< Element is disabled and can't be focused or pressed.
		WG_STATE_DISABLED_SELECTED			= 16+1,
	};
	
	#define	WG_NB_STATES		14			// Number of states
	#define	WG_MAX_STATE_VALUE	17			// Highest value for StateEnum
	
	class State 
	{
	public:
		State() { m_state = WG_STATE_NORMAL; }
		State( StateEnum state ) { m_state = state; }
	
	//	void		set( WgModeEnum state ) { m_state = state; }
	//	WgModeEnum	getEnum() const { return (WgModeEnum) m_state; }
	
		bool	setEnabled(bool bEnabled) { if(bEnabled) m_state &= ~WG_STATE_DISABLED; else m_state = (m_state & WG_STATE_SELECTED) | WG_STATE_DISABLED; return true; }
		bool	setSelected(bool bSelected) { if(bSelected) m_state |= WG_STATE_SELECTED; else m_state &= ~WG_STATE_SELECTED; return true; }
		bool	setFocused(bool bFocused) { if( m_state == WG_STATE_DISABLED ) return false; if(bFocused) m_state |= WG_STATE_FOCUSED; else m_state &= ~WG_STATE_FOCUSED; return true; }
		bool	setHovered(bool bHovered) { if( m_state == WG_STATE_DISABLED ) return false; if(bHovered) m_state |= WG_STATE_HOVERED; else m_state &= ~WG_STATE_PRESSED; return true; }
		bool	setPressed(bool bPressed) { if( m_state == WG_STATE_DISABLED ) return false; if(bPressed) m_state |= WG_STATE_PRESSED; else m_state &= ~(WG_STATE_PRESSED - WG_STATE_HOVERED); return true; }
	
	
		bool	isEnabled() const { return (m_state & WG_STATE_DISABLED) == WG_STATE_NORMAL; }
		bool	isSelected() const { return (m_state & WG_STATE_SELECTED) == WG_STATE_SELECTED; }
		bool	isFocused() const { return (m_state & WG_STATE_FOCUSED) == WG_STATE_FOCUSED; }
		bool	isHovered() const { return (m_state & WG_STATE_HOVERED) == WG_STATE_HOVERED; }
		bool	isPressed() const { return (m_state & WG_STATE_PRESSED) == WG_STATE_PRESSED; }
	
		inline bool operator==(StateEnum state) const { return m_state == state; }
		inline bool operator!=(StateEnum state) const { return m_state != state; }
	
		inline void operator=(StateEnum state) { m_state = state; }
	
		operator StateEnum() const { return (StateEnum) m_state; }
	
	private:
		int		m_state;
	};
	
	
	typedef unsigned int	RouteId;
	
	
	//____ TxtAttr ______________________________________________________________
	
	// Bitmask for various text attributes.
	
	enum TxtAttr
	{
		WG_TXT_UNDERLINED	= 0x1,
		WG_TXT_SELECTED		= 0x2
	};
	
	
	//____ ExtChar __________________________________________________________
	
	
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
	
		:[0-4]			set break level
		;				end break level
	
	
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
	
	
	enum ExtChar
	{
		// Code inside WonderGUI assumes these are all
		// in the range 0x80-0xA0, but that might change in the future.
	
		WG_BREAK_PERMITTED			= 0x82,
		WG_HYPHEN_BREAK_PERMITTED	= 0x83,
		WG_NO_BREAK_SPACE			= 0xA0,
	
		WG_ELLIPSIS					= 0x2026
	};
	
	//____ CodePage ______________________________________________________________
	
	enum CodePage
	{
		WG_CODEPAGE_LATIN1 = 0,
		WG_CODEPAGE_1250 = 1,		///< Windows Cental Europe
		WG_CODEPAGE_1251 = 2,		///< Windows Cyrillic
		WG_CODEPAGE_1252 = 3,		///< Windows Latin-1 extended
		WG_CODEPAGE_1253 = 4,		///< Windows Greek
		WG_CODEPAGE_1254 = 5,		///< Windows Turkish
		WG_CODEPAGE_1255 = 6,		///< Windows Hebrew
		WG_CODEPAGE_1256 = 7,		///< Windows Arabic
		WG_CODEPAGE_1257 = 8,		///< Windows Baltic
		WG_CODEPAGE_1258 = 9,		///< Windows Vietnam
		WG_CODEPAGE_874 = 10,		///< Windows Thai
	};
	
	#define WG_NB_CODEPAGES 11
	
	//____ BreakRules ____________________________________________________________
	
	enum BreakRules
	{
		WG_NO_BREAK		= 0,
		WG_BREAK_BEFORE	= 16,
		WG_BREAK_ON		= 32,
		WG_BREAK_AFTER	= 64
	};
	
	//____ BlendMode ____________________________________________________________
	
	// BlendModes control how blits and fills are blended against the background.
	
	enum BlendMode //: uint8_t
	{
		WG_BLENDMODE_OPAQUE,			///< Completely opaque blitting, ignoring alpha of source and tint-color.
		WG_BLENDMODE_BLEND,				///< Normal mode, alpha of source and tint-color is taken into account.
		WG_BLENDMODE_ADD,				///< RGBA Additive, alpha of source and tint-color is taken into account.
		WG_BLENDMODE_MULTIPLY,			///< RGBA Multiply, alpha of source and tint-color is taken into account.
		WG_BLENDMODE_INVERT				///< Inverts destination RGB values where alpha of source is non-zero. Ignores RBG components. Uses alpha of tint-color.
	};
	
	//____ FontAlt ____________________________________________________________
	
	enum FontAlt
	{
		// Needs to stay in sync with ExtChar::WG_BEGIN_NORMAL and following enums!
	
		WG_FONT_NORMAL		= 0,
		WG_FONT_BOLD		= 1,
		WG_FONT_BOLD_ITALIC= 2,
		WG_FONT_ITALIC		= 3,
		WG_FONT_SUPERSCRIPT= 4,
		WG_FONT_SUBSCRIPT	= 5,
		WG_FONT_MONOSPACE	= 6,
		WG_FONT_HEADING_1	= 7,
		WG_FONT_HEADING_2	= 8,
		WG_FONT_HEADING_3	= 9,
		WG_FONT_HEADING_4	= 10,
		WG_FONT_HEADING_5	= 11,
	
		WG_FONT_USER_1		= 12,
		WG_FONT_USER_2		= 13,
		WG_FONT_USER_3		= 14,
		WG_FONT_USER_4		= 15,
		WG_FONT_USER_5		= 16,
	
		WG_FONT_INHERIT	= 99,
	};
	
	#define WG_NB_FONTSTYLES	17
	
	
	//____ PointerStyle __________________________________________________________
	
	enum PointerStyle
	{
		WG_POINTER_ARROW,						// default arrow
		WG_POINTER_DEFAULT = WG_POINTER_ARROW,	// default arrow
		WG_POINTER_HOURGLASS,					// hourglass
		WG_POINTER_HAND,							// hand
		WG_POINTER_CROSSHAIR,					// crosshair
		WG_POINTER_HELP,							// help
		WG_POINTER_IBEAM,						// I-beam
		WG_POINTER_STOP,							// slashed circle
		WG_POINTER_UP_ARROW,						// arrow pointing up
		WG_POINTER_SIZE_ALL,						// four-pointed arrow in all directions
		WG_POINTER_SIZE_NE_SW,					// double-pointed arrow pointing northeast and southwest
		WG_POINTER_SIZE_NW_SE,					// double-pointed arrow pointing northwest and southeast
		WG_POINTER_SIZE_N_S,						// double-pointed arrow pointing north and south
		WG_POINTER_SIZE_W_E,						// double-pointed arrow pointing west and east
	};
	
	//____ MouseButton _________________________________________________________
	
	enum MouseButton
	{
		WG_BUTTON_NONE = 0,
		WG_BUTTON_LEFT,
		WG_BUTTON_MIDDLE,
		WG_BUTTON_RIGHT,
		WG_BUTTON_X1,
		WG_BUTTON_X2,
	};
	
	#define	WG_MAX_BUTTONS		WG_BUTTON_X2+1	
	
	
	//____ AnimMode _____________________________________________________________
	
	enum AnimMode
	{
		WG_FORWARD_ONCE,
		WG_BACKWARD_ONCE,
		WG_FORWARD_LOOPING,
		WG_BACKWARD_LOOPING,
		WG_FORWARD_PINGPONG,
		WG_BACKWARD_PINGPONG
	};
	
	
	//____ TintMode _____________________________________________________________
	
	// TintMode controls how TintColors are blended hierarchically.
	
	enum TintMode
	{
		WG_TINTMODE_OPAQUE,
		WG_TINTMODE_MULTIPLY
	};
	
	
	/*
	enum	ExtChar
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
	
	
	//____ SearchMode _____________________________________________________________
	
	enum SearchMode
	{
		WG_SEARCH_MARKPOLICY,			///< Perform a mark test on Widget.
		WG_SEARCH_GEOMETRY,				///< Goes strictly on geometry, ignores alpha.
		WG_SEARCH_ACTION_TARGET,		///< Like MARKPOLICY, but takes modality into account.
	};
	
	//____ Origo _____________________________________________________________
	
	enum Origo
	{
		// Clockwise from upper left corner, center last. Must be in range 0-8
	
		WG_NORTHWEST	= 0,
		WG_NORTH		= 1,
		WG_NORTHEAST	= 2,
		WG_EAST			= 3,
		WG_SOUTHEAST	= 4,
		WG_SOUTH		= 5,
		WG_SOUTHWEST	= 6,
		WG_WEST			= 7,
		WG_CENTER		= 8
	};
	
	//____ Direction ____________________________________________________________
	
	enum Direction
	{
		WG_UP,
		WG_DOWN,
		WG_LEFT,
		WG_RIGHT
	};
	
	//____ Orientation __________________________________________________________
	
	enum Orientation
	{
		WG_HORIZONTAL,
		WG_VERTICAL
	};
	
	//____ SizePolicy ___________________________________________________________
	/**
		SizePolicy is used by certain containers, including FlexPanel and ScrollPanel, 
		to limit/control the geometry of children. Different SizePolicies can be set
		for horizontal and vertical size.
	
		It is used in combination with a size specified by parent. In the case of
		ScrollPanel it is the size of the window to the scrollarea. In the case of
		FlexPanel, it is a size specified in the childs hook.
	*/
	enum SizePolicy
	{
		WG_DEFAULT = 0,			///< Childs size is unaffected by size specified by parent, so it gets its preferred size.
		WG_BOUND,				///< Child is bound to the exact size specified by parent.
		WG_CONFINED,			///< Childs size is limited to the size specified by parent.
		WG_EXPANDED,			///< Childs size is set to at least the size specified by parent.
	};
	
	//____ MsgType ______________________________________________________________
	
	enum	MsgType
	{
		WG_MSG_DUMMY = 0,
		WG_MSG_TICK,
		WG_MSG_POINTER_CHANGE,
	
		WG_MSG_FOCUS_GAINED,
		WG_MSG_FOCUS_LOST,
	
		WG_MSG_MOUSE_ENTER,
		WG_MSG_MOUSE_MOVE,
		WG_MSG_MOUSE_LEAVE,
	
		WG_MSG_MOUSE_PRESS,
		WG_MSG_MOUSE_REPEAT,
		WG_MSG_MOUSE_DRAG,
		WG_MSG_MOUSE_RELEASE,
		WG_MSG_MOUSE_CLICK,
		WG_MSG_MOUSE_DOUBLE_CLICK,
	
		WG_MSG_KEY_PRESS,
		WG_MSG_KEY_REPEAT,
		WG_MSG_KEY_RELEASE,
		WG_MSG_TEXT_INPUT,
		WG_MSG_WHEEL_ROLL,
	
		WG_MSG_SELECT,						// Non-value widget triggered, like a button being pressed.
		WG_MSG_TOGGLE,						// Boolean value widget toggled, like for checkboxes, radiobuttons etc.
		WG_MSG_VALUE_UPDATE,					// Value of widget changed, like for editvalue, animations, sliders etc
		WG_MSG_RANGE_UPDATE,					// Range widget updated, such as scrollbar, scrollpanel, rangesliders etc.
	
		WG_MSG_TEXT_EDIT,						// Text widget edited, like texteditor, editline, etc.
	
	
		WG_MSG_ITEM_TOGGLE,
	//	WG_MSG_ITEM_MOUSE_ENTER,
	//	WG_MSG_ITEM_MOUSE_LEAVE,
		WG_MSG_ITEM_MOUSE_PRESS,
	//	WG_MSG_ITEM_MOUSE_DRAG,
	//	WG_MSG_ITEM_MOUSE_REPEAT,
	//	WG_MSG_ITEM_MOUSE_RELEASE,
	//	WG_MSG_ITEM_MOUSE_CLICK,
	//	WG_MSG_ITEM_MOUSE_DOUBLE_CLICK,
		WG_MSG_ITEMS_SELECT,
		WG_MSG_ITEMS_UNSELECT,
	
		WG_MSG_POPUP_CLOSED,
	
		WG_MSG_MODAL_MOVE_OUTSIDE,
		WG_MSG_MODAL_BLOCKED_PRESS,
		WG_MSG_MODAL_BLOCKED_RELEASE,
	
		WG_MSG_LINK_SELECT,
		WG_MSG_LINK_MOUSE_ENTER,
		WG_MSG_LINK_MOUSE_LEAVE,
		WG_MSG_LINK_MOUSE_PRESS,
		WG_MSG_LINK_MOUSE_REPEAT,
		WG_MSG_LINK_MOUSE_RELEASE,
		WG_MSG_LINK_MOUSE_CLICK,
		WG_MSG_LINK_MOUSE_DOUBLE_CLICK,
		
		WG_MSG_MAX
	};
	
	
	//____ SortOrder ____________________________________________________________
	
	enum SortOrder
	{
		WG_SORT_ASCENDING,
		WG_SORT_DESCENDING
	};
	
	//____ Unit ____________________________________________________________
	
	enum Unit
	{
		WG_PX,
		WG_EM
	};
	
	
	//____ SelectMode ___________________________________________________________
	
	enum SelectMode
	{
		WG_SELECT_NONE,			///< Entries can not be selected.
		WG_SELECT_SINGLE,		///< Only a single entry can be selected at a time.
		WG_SELECT_MULTI,		///< Multiple entries can be selected at the same time.
		WG_SELECT_FLIP			///< Multiple entries can be selected at the same time and are switched individually.
	};
	
	//____ TextEditMode _________________________________________________________
	
	enum TextEditMode
	{
		WG_TEXT_STATIC,
		WG_TEXT_SELECTABLE,
		WG_TEXT_EDITABLE,
	};
	
	
	//____ TextPos ______________________________________________________________
	
	class TextPos
	{
	public:
		TextPos() : line(0), col(0) {}
		TextPos( int line, int col ) : line(line), col(col) {}
	
		int	line;
		int col;
	};
	
	//___  TextDecoration ________________________________________________________
	
	enum TextDecoration
	{
		WG_DECORATION_INHERIT,
		WG_DECORATION_NONE,
		WG_DECORATION_UNDERLINE
	};
	
	//____ Range ____________________________________________________________
	
	class Range
	{
	public:
		Range() : ofs(0), len(0) {}
		Range( int ofs, int len ) : ofs(ofs), len(len) {}
	
		inline int	begin() const { return ofs; }
		inline int	end() const { return ofs+len; }
	
		int ofs;
		int len;
	};
	
	
	//____ SortContext __________________________________________________________
	
	struct SortContext
	{
	public:
		bool	bAscend;
	};
	
	//____ AccessMode ____________________________________________________________
	
	enum AccessMode
	{
		WG_NO_ACCESS,
		WG_READ_ONLY,
		WG_WRITE_ONLY,
		WG_READ_WRITE
	};
	
	//____ PixelType _____________________________________________________________
	
	enum PixelType
	{
		WG_PIXEL_UNKNOWN,			///< Pixelformat is unkown or can't be expressed in a PixelFormat struct.
		WG_PIXEL_CUSTOM,			///< Pixelformat has no PixelType enum, but is fully specified through the PixelFormat struct.
		WG_PIXEL_RGB_8,				///< One byte of red, green and blue respectively in memory in exactly that order.
		WG_PIXEL_RGBA_8				///< One byte of red, green, blue and alpha respectively in memory in exactly that order.
	};
	
	
	//____ PixelFormat __________________________________________________________
	/**
	 * @brief Describes the format of a pixel.
	 *
	 * Describes the format of a pixel.
	 *
	 * The format of the pixel is described in three ways by a PixelFormat object:
	 *
	 * First a PixelType enum that contains predefined values for common pixel formats.
	 * This allows for human readable information and quick lockups.
	 *
	 * Secondly a set of variables containing the number of bits for each pixel and the
	 * individual red, green, blue and alpha channels. This allows for quickly and easily
	 * checking the size of a pixel and the color depths and presence and depth of the alpha channel.
	 *
	 * Thirdly a pair of mask and shift variables for each individual channel, specifies the position
	 * of each channel and allows for quick conversion to and from the default 32-bit RGBA format used by Color.
	 *
	 * Not all pixel formats (like those of index/palette-based surfaces) can
	 * be fully described by a PixelFormat object. In that case the member type is set to WG_PIXEL_UNKNOWN.
	 *
	 * As long as the type member is not set to WG_PIXEL_UNKNOWN, you can extract the value of any channel of a
	 * pixel by applying the mask and shift variables. I.e. to extract the value of red from a pixel
	 * as an 8-bit value in the range 0-255, you use the formula:
	 *
	 * redValue = (pixel & R_mask) >> R_shift
	 *
	 * Thus you can convert any specified pixel type to a Color structure using the following routine:
	 *
	 * uint32_t	pixel;
	 * PixelFormat * pFormat;
	 *
	 * 	Color col( (pixel & pFormat->R_mask) >> pFormat->R_shift,
	 *				 (pixel & pFormat->G_mask) >> pFormat->G_shift,
	 *				 (pixel & pFormat->B_mask) >> pFormat->B_shift,
	 *				 (pixel & pFormat->A_mask) >> pFormat->A_shift );
	 *
	 * To convert a Color object to a pixel value you can use:
	 *
	 * Color color;
	 * PixelFormat * pFormat;
	 *
	 * 	uint32_t pix = ((color.r << pFormat->R_shift) & pFormat->R_mask) |
	 *				 ((color.g << pFormat->G_shift) & pFormat->G_mask) |
	 *				 ((color.b << pFormat->B_shift) & pFormat->B_mask) |
	 *				 ((color.a << pFormat->A_shift) & pFormat->A_mask);
	 *
	 * This is essentially what the default implementation for Surface::colorToPixel() and Surface::pixelToColor() does.
	 *
	 **/
	
	
	struct PixelFormat
	{
		PixelType	type;			///< Enum specifying the format if it exacty matches a predefined format, otherwise set to CUSTOM or UNKNOWN.
		int			bits;			///< Number of bits for the pixel, includes any non-used padding bits.
	
		uint32_t	R_mask;				///< bitmask for getting the red bits out of the pixel
		uint32_t	G_mask;				///< bitmask for getting the green bits out of the pixel
		uint32_t	B_mask;				///< bitmask for getting the blue bits out of the pixel
		uint32_t	A_mask;				///< bitmask for getting the alpha bits out of the pixel
	
		int		R_shift;			///< amount to shift the red bits to get an 8-bit representation of red. This can be negative.
		int		G_shift;			///< amount to shift the green bits to get an 8-bit representation of red. This can be negative.
		int		B_shift;			///< amount to shift the blue bits to get an 8-bit representation of red. This can be negative.
		int		A_shift;			///< amount to shift the alpha bits to get an 8-bit representation of red. This can be negative.
	
		uint8_t	R_bits;				///< number of bits for red in the pixel
		uint8_t	G_bits;				///< number of bits for green in the pixel
		uint8_t	B_bits;				///< number of bits for blue in the pixel
		uint8_t	A_bits;				///< number of bits for alpha in the pixel
	};
	
	
	//____ MaskOp ____________________________________________________________
	
	enum MaskOp
	{
		WG_MASKOP_RECURSE = 0,	///< Recurse through children, let them mask background individually.
		WG_MASKOP_SKIP = 1,		///< Do not mask background against container or children.
		WG_MASKOP_MASK = 2		///< Mask background against whole container.
	};
	
	

} // namespace wg
#endif // WG_TYPES_DOT_H
