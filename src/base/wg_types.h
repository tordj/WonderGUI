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
#pragma once

#include <cstdint>

namespace wg 
{
		
#if defined(_WIN32) 	
#	if defined(_M_X64) || defined(_M_IX86)
#		define IS_BIG_ENDIAN 0
#		define IS_LITTLE_ENDIAN 1
#	endif
#else
	#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)
	#	 if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#		define IS_BIG_ENDIAN 1
	#		define IS_LITTLE_ENDIAN 0
	#	elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#		define IS_BIG_ENDIAN 0
	#		define IS_LITTLE_ENDIAN 1
	#	endif
	#else
	#	ifdef __BIG_ENDIAN__
	#		if __BIG_ENDIAN__
	#			define IS_BIG_ENDIAN 1
	#			define IS_LITTLE_ENDIAN 0
	#		endif
	#	endif

	#	ifdef __LITTLE_ENDIAN__
	#		if __LITTLE_ENDIAN__
	#			define IS_BIG_ENDIAN 0
	#			define IS_LITTLE_ENDIAN 1
	#		endif
	#	endif
	#endif
#endif


	#ifndef IS_BIG_ENDIAN
	#error Could not detect endianness. You need to define IS_BIG_ENDIAN and IS_LITTLE_ENDIAN in wg_types.h
	#define IS_BIG_ENDIAN 0
	#define IS_LITTLE_ENDIAN 0
	#endif
	
	template<typename T> inline T min(const T &a, const T &b) { return a < b ? a : b; }
	template<typename T> inline T min(const T &a, const T &b, const T &c) { if( a < b ) return a < c ? a : c; else return b < c ? b : c; }
	template<typename T> inline T max(const T &a, const T &b) { return a > b ? a : b; }
	template<typename T> inline T max(const T &a, const T &b, const T &c) { if( a > b ) return a > c ? a : c; else return b > c ? b : c; }
	
	template<typename T> inline void limit(T& x, T min, T max) { if( x < min) x = min; if( x > max) x = max; }
			
	enum class StateEnum : uint8_t
	{
		Normal					= 0,			///< Element is neither hovered, pressed, selected or focused.
		Selected				= 1,			///< Element is in a selected state, like a selected checkbox or item in a list. 
		Focused					= 2,			///< Element has keyboard focus
		FocusedSelected			= 2+1,
		Hovered					= 4,			///< Mouse pointer is hovering over element.
		HoveredSelected			= 4+1,
		HoveredFocused			= 4+2,
		HoveredFocusedSelected	= 4+2+1,
		Pressed					= 8+4,			///< Mouse button (usually left one) is pressed on element.
		PressedSelected			= 8+4+1,
		PressedFocused			= 8+4+2,
		PressedFocusedSelected	= 8+4+2+1,
		Disabled				= 16,			///< Element is disabled and can't be focused or pressed.
		DisabledSelected		= 16+1,
	};
	
	static const int	StateEnum_Nb	= 14;			// Number of states
	static const int	StateEnum_MaxValue	= 17;			// Highest value for StateEnum
	
	class State 
	{
	public:

		//.____ Creation _______________________________________________________

		State() { m_state = (uint8_t) StateEnum::Normal; }
		State( StateEnum state ) { m_state = (uint8_t) state; }

		//.____ State __________________________________________________________
	
		bool	setEnabled(bool bEnabled) { if(bEnabled) m_state &= ~ ((uint8_t)StateEnum::Disabled); else m_state = (m_state & ((uint8_t)StateEnum::Selected)) | ((uint8_t)StateEnum::Disabled); return true; }
		bool	setSelected(bool bSelected) { if(bSelected) m_state |= ((uint8_t)StateEnum::Selected); else m_state &= ~((uint8_t)StateEnum::Selected); return true; }
		bool	setFocused(bool bFocused) { if( m_state == ((uint8_t)StateEnum::Disabled) ) return false; if(bFocused) m_state |= ((uint8_t)StateEnum::Focused); else m_state &= ~((uint8_t)StateEnum::Focused); return true; }
		bool	setHovered(bool bHovered) { if( m_state == ((uint8_t)StateEnum::Disabled) ) return false; if(bHovered) m_state |= ((uint8_t)StateEnum::Hovered); else m_state &= ~((uint8_t)StateEnum::Pressed); return true; }
		bool	setPressed(bool bPressed) { if( m_state == ((uint8_t)StateEnum::Disabled) ) return false; if(bPressed) m_state |= ((uint8_t)StateEnum::Pressed); else m_state &= ~(((uint8_t)StateEnum::Pressed) - ((uint8_t)StateEnum::Hovered)); return true; }
	
	
		bool	isEnabled() const { return (m_state & ((uint8_t)StateEnum::Disabled)) == ((uint8_t)StateEnum::Normal); }
		bool	isSelected() const { return (m_state & ((uint8_t)StateEnum::Selected)) == ((uint8_t)StateEnum::Selected); }
		bool	isFocused() const { return (m_state & ((uint8_t)StateEnum::Focused)) == ((uint8_t)StateEnum::Focused); }
		bool	isHovered() const { return (m_state & ((uint8_t)StateEnum::Hovered)) == ((uint8_t)StateEnum::Hovered); }
		bool	isPressed() const { return (m_state & ((uint8_t)StateEnum::Pressed)) == ((uint8_t)StateEnum::Pressed); }
	
		//._____ Operators _____________________________________________________

		inline bool operator==(StateEnum state) const { return m_state == ((uint8_t)state); }
		inline bool operator!=(StateEnum state) const { return m_state != ((uint8_t)state); }
	
		inline void operator=(StateEnum state) { m_state = ((uint8_t)state); }
	
		operator StateEnum() const { return (StateEnum) m_state; }
	
	private:
		uint8_t		m_state;
	};
	
	
	typedef unsigned int	RouteId;
	
	typedef uint16_t		TextStyle_h;
		
	
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
	
	
	enum class ExtChar : uint16_t
	{
		BreakPermitted			= 0x82,
		HyphenBreakPermitted	= 0x83,
		NoBreakSpace			= 0xA0,
	
		Ellipsis				= 0x2026
	};
	
	//____ CodePage ______________________________________________________________
	
	enum class CodePage		//. autoExtras
	{
		Latin1,
		_1250,		///< Windows Cental Europe
		_1251,		///< Windows Cyrillic
		_1252,		///< Windows Latin-1 extended
		_1253,		///< Windows Greek
		_1254,		///< Windows Turkish
		_1255,		///< Windows Hebrew
		_1256,		///< Windows Arabic
		_1257,		///< Windows Baltic
		_1258,		///< Windows Vietnam
		_874,		///< Windows Thai
	};
		
	//____ BreakRules ____________________________________________________________
	
	enum class BreakRules : uint8_t
	{
		NoBreak		= 0,
		BreakBefore	= 16,
		BreakOn		= 32,
		BreakAfter	= 64
	};
	
	//____ BlendMode ____________________________________________________________
	
	// BlendModes control how blits and fills are blended against their backgrounds and how colors are blended against each other.
			
	enum class BlendMode : uint8_t	//. autoExtras
	{
		Undefined,			///< Blitting: Defaults to Blend
							///< Color Blending: Defaults to ignore
							///< This value is used internally to distinguish undefined values from an explicitly set ignore,
		Ignore,				///< Blitting: No blitting performed.
							///< Color Blending: DstRGBA = DstRGBA
		Blend,				///< Blitting: Normal mode, alpha of source and tint-color is taken into account.
							///< Color Blending: DstA = SrcA, DstRGB = SrcRGB + ((TintRGB-SrcRGB)*TintA/255)
		Invert,				///< Blitting: Inverts destination RGB values where alpha of source is non-zero. Ignores RBG components. Uses alpha of tint-color.
							///< Color Blending: DstA = SrcA, DstRGB = ((255 - SrcRGB)*TintA + SrcRGB*(255-TintA))/255
		Replace,			///< Blitting: Completely opaque blitting, ignoring alpha of source and tint-color.
							///< Color Blending: DstRGBA = SrcRGBA
		Add,				///< Blitting: RGB Additive, alpha of source and tint-color is taken into account.
							///< Color Blending: DstRGBA = SrcRGBA + TintRGBA
		Subtract,			///< Blitting: RGB Subtractive, alpha of source and tint-color is taken into account.
							///< Color Blending: DstRGBA = SrcRGBA - TintRGBA
		Multiply			///< Blitting: RGB Multiply, alpha of source and tint-color is taken into account.
							///< Color Blending: DstRGBA = SrcRGBA * TintRGBA/255
	};

	
	//____ PointerStyle __________________________________________________________
	
	enum class PointerStyle : uint8_t	//. autoExtras
	{
		Arrow,						// default arrow
		Default = Arrow,	// default arrow
		Hourglass,					// hourglass
		Hand,							// hand
		Crosshair,					// crosshair
		Help,							// help
		Ibeam,						// I-beam
		Stop,							// slashed circle
		UpArrow,						// arrow pointing up
		ResizeAll,						// four-pointed arrow in all directions
		ResizeNeSw,					// double-pointed arrow pointing northeast and southwest
		ResizeNwSe,					// double-pointed arrow pointing northwest and southeast
		ResizeNS,						// double-pointed arrow pointing north and south
		ResizeWE,						// double-pointed arrow pointing west and east
	};
	
	//____ MouseButton _________________________________________________________
	
	enum class MouseButton : uint8_t	//. autoExtras
	{
		None = 0,
		Left,
		Middle,
		Right,
		X1,
		X2,
	};
	
	
	//____ AnimMode _____________________________________________________________
	
	enum class AnimMode : uint8_t	//. autoExtras
	{
		Forward,
		Backward,
		Looping,
		BackwardLooping,
		PingPong,
		BackwardPingPong
	};
		
	
	//____ SearchMode _____________________________________________________________
	
	enum class SearchMode	//. autoExtras
	{
		MarkPolicy,			///< Perform a mark test on Widget.
		Geometry,				///< Goes strictly on geometry, ignores alpha.
		ActionTarget,		///< Like MARKPOLICY, but takes modality into account.
	};
	
	//____ Origo _____________________________________________________________
	
	enum class Origo : uint8_t	//. autoExtras
	{
		// Clockwise from upper left corner, center last. Must be in range 0-8
	
		NorthWest,
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		Center
	};
	
	//____ Direction ____________________________________________________________
	
	enum class Direction : uint8_t	//. autoExtras
	{
		Up,
		Right,
		Down,
		Left
	};
	
	//____ Orientation __________________________________________________________
	
	enum class Orientation : uint8_t	//. autoExtras
	{
		Horizontal,
		Vertical
	};
	
	//____ SizePolicy ___________________________________________________________
	/**
		SizePolicy is used by certain containers, including FlexPanel and ScrollPanel, 
		to limit/control the geometry of children. Different SizePolicies can be set
		for horizontal and vertical size.
	
		It is used in combination with a size specified by parent. In the case of
		ScrollPanel it is the size of the window to the scrollarea. In the case of
		FlexPanel, it is a size specified for the child.
	*/
	enum class SizePolicy : uint8_t		//. autoExtras
	{
		Default = 0,			///< Childs size is unaffected by size specified by parent, so it gets its preferred size.
		Bound,				///< Child is bound to the exact size specified by parent.
		Confined,			///< Childs size is limited to the size specified by parent.
		Expanded,			///< Childs size is set to at least the size specified by parent.
	};
	

	//____ SizePolicy2D ___________________________________________________________

	enum class SizePolicy2D : uint8_t	//. autoExtras
	{
		Default,
		Stretch,
		Scale
	};


	//____ MsgType ______________________________________________________________
	
	enum class MsgType	//. autoExtras
	{
		Dummy = 0,
		Tick,
		PointerChange,
	
		FocusGained,
		FocusLost,
	
		MouseEnter,
		MouseMove,
		MouseLeave,
	
		MousePress,
		MouseRepeat,
		MouseDrag,
		MouseRelease,
		MouseClick,
		MouseDoubleClick,
	
		KeyPress,
		KeyRepeat,
		KeyRelease,
		TextInput,
		EditCommand,
		WheelRoll,
	
		Select,						// Non-value widget triggered, like a button being pressed.
		Toggle,						// Boolean value widget toggled, like for checkboxes, radiobuttons etc.
		ValueUpdate,					// Value of widget changed, like for editvalue, animations, sliders etc
		RangeUpdate,					// Range widget updated, such as scrollbar, scrollpanel, rangesliders etc.
	
		TextEdit,						// Text widget edited, like texteditor, editline, etc.
	
	
		ItemToggle,
	//	ItemMouseEnter,
	//	ItemMouseLeave,
		ItemMousePress,
	//	ItemMouseDrag,
	//	ItemMouseRepeat,
	//	ItemMouseRelease,
	//	ItemMouseClick,
	//	ItemMouseDoubleClick,
		ItemsSelect,
		ItemsUnselect,
	
		PopupClosed,
	
		ModalMoveOutside,
		ModalBlockedPress,
		ModalBlockedRelease,		
	};
	
	
	//____ SortOrder ____________________________________________________________
	
	enum class SortOrder	//. autoExtras
	{
		None,
		Ascending,
		Descending
	};
	
	//____ Unit ____________________________________________________________
	
	enum class Unit
	{
		Px,
		Em
	};
	
	
	//____ SelectMode ___________________________________________________________
	
	enum class SelectMode : uint8_t	//. autoExtras
	{
		Unselectable,		///< Entries can not be selected.
		SingleEntry,		///< Only a single entry can be selected at a time.
		MultiEntries,		///< Multiple entries can be selected at the same time.
		FlipOnSelect		///< Multiple entries can be selected at the same time and are switched individually.
	};
	
	//____ TextEditMode _________________________________________________________
	
	enum class TextEditMode : uint8_t	//. autoExtras
	{
		Static,
		Selectable,
		Editable
	};
	
	//____ EditCmd _____________________________________________________________

	enum class EditCmd
	{
		None,
		Cut,
		Copy,
		Paste,
		Undo,
		Redo,
		SelectAll,
		Escape,
		Break
	};
		
	//___  TextDecoration ________________________________________________________
	
	enum class TextDecoration
	{
		Undefined,
		None,
		Underline
	};
	

	//____ ItemNotif ___________________________________________________________

	enum class ItemNotif 
	{
		SortOrderChanged,
		ValueModified,
		ValueEdited,
		SpanModified,
		Other
	};
	
	//____ AccessMode ____________________________________________________________
	
	enum class AccessMode	//. autoExtras
	{
		None,
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	//____ ScaleMode ____________________________________________________________
	
	enum class ScaleMode	//. autoExtras
	{
		Nearest,
		Interpolate,
	};


	//____ int ____________________________________________________________
	
	namespace SurfaceHint
	{
		const int Static = 0;		// No content access/modification expected
		const int Dynamic = 1;		// Expect content to be accessed and/or modified
		const int  WriteOnly = 2;	// Can only be locked in WriteOnly mode. Alpha can still be read pixel by pixel if present.
	};

	
	//____ PixelType _____________________________________________________________
	
	enum class PixelType	//. autoExtras
	{
		Unknown,			///< Pixelformat is unkown or can't be expressed in a PixelFormat struct.
		Custom,				///< Pixelformat has no PixelType enum, but is fully specified through the PixelFormat struct.
		BGR_8,				///< One byte of blue, green and red respectively in memory in exactly that order.
		BGRA_8				///< One byte of blue, green, red and alpha respectively in memory in exactly that order.
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
	 * be fully described by a PixelFormat object. In that case the member type is set to Unknown.
	 *
	 * As long as the type member is not set to Unknown, you can extract the value of any channel of a
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
	public:	
		//.____ Properties _____________________________________________________
		
		PixelType	type;			///< Enum specifying the format if it exacty matches a predefined format, otherwise set to CUSTOM or UNKNOWN.
		int			bits;			///< Number of bits for the pixel, includes any non-used padding bits.
	
		uint32_t	R_mask;			///< bitmask for getting the red bits out of the pixel
		uint32_t	G_mask;			///< bitmask for getting the green bits out of the pixel
		uint32_t	B_mask;			///< bitmask for getting the blue bits out of the pixel
		uint32_t	A_mask;			///< bitmask for getting the alpha bits out of the pixel
	
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
	
	enum class MaskOp	//. autoExtras
	{
		Recurse,		///< Recurse through children, let them mask background individually.
		Skip,			///< Do not mask background against container or children.
		Mask			///< Mask background against whole container.
	};
	
	//____ GfxChunkId ____________________________________________________

	enum class GfxChunkId : uint16_t	//. autoExtras
	{
		OutOfData,

		BeginRender,
		EndRender,
		SetCanvas,
		SetTintColor,
		SetBlendMode,
		Fill,
		PlotPixels,
		DrawLine,
		ClipDrawLine,						// Draw line between begin- and end-points
		ClipDrawLine2,						// Draw line using begin-point, direction and length.
		ClipDrawHorrWave,
		Blit,
		StretchBlit,
		FillSubPixel,

		CreateSurface,
		SetSurfaceScaleMode,
		BeginSurfaceUpdate,
		SurfaceData,
		EndSurfaceUpdate,
		FillSurface,
		CopySurface,
		DeleteSurface
	};



} // namespace wg
#endif // WG_TYPES_DOT_H
