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
#		define WG_IS_BIG_ENDIAN 0
#		define WG_IS_LITTLE_ENDIAN 1
#	endif
#else
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
#endif


	#ifndef WG_IS_BIG_ENDIAN
	#error Could not detect endianness. You need to define WG_IS_BIG_ENDIAN and WG_IS_LITTLE_ENDIAN in wg_types.h
	#define WG_IS_BIG_ENDIAN 0
	#define WG_IS_LITTLE_ENDIAN 0
	#endif


	#if INTPTR_MAX == INT64_MAX
	#		define	WG_IS_64_BITS 1
	#elif INTPTR_MAX == INT32_MAX
	#		define WG_IS_64_BITS 0	
	#else
	#error Could not detect if target is 32 or 64 bits. You need to manually set WG_IS_64_BITS in wg_types.h
	#endif

//-----------------------------------------------------------------------------
// Defining the internal type "binalInt" which is a 32 or 64 bit integer with
// 15 and 24 binals respetively depending on if architecture is 32 or 64 bits.
// This is used internally for high-precision integer operations.

#if WG_IS_64_BITS

	typedef int64_t binalInt;
/*
	const int64_t BINAL_MUL = 32768;
	const int64_t BINAL_SHIFT = 15;
	const int64_t BINAL_MASK = 0x7FFF;
*/
	const int64_t BINAL_MUL = 16777216;
	const int64_t BINAL_SHIFT = 24;
	const int64_t BINAL_MASK = 0xFFFFFF;

#else

	typedef int32_t binalInt;
	const int32_t BINAL_MUL = 32768;
	const int32_t BINAL_SHIFT = 15;
	const int32_t BINAL_MASK = 0x7FFF;

#endif

//-----------------------------------------------------------------------------

	typedef	float	pts;
	typedef int		spx;		// Measurement in 1/64th of a pixel, e.g. pixels with 6 binals.


	static const int spx_max = 0x7FFFFFC0;	// On pixel boundary with margin for rounding operation.

	template<typename T, typename T2, typename T3> inline void limit(T& x, T2 min, T3 max) { if( x < min) x = min; if( x > max) x = max; }


//#define WGLIST(...) { __VA_ARGS__ }

#define WGBP(T, ...)\
	([&]{ wg::T::Blueprint _{}; __VA_ARGS__; return _; }())

#define WGTRACK(pObj) wg::Base::_trackObj(pObj, __FILE__, __LINE__ ) 


	//____ ErrorLevel ________________________________________________

	enum class ErrorLevel		//. autoExtras
	{
		Warning,
		SilentFail,
		Serious,
		Critical
	};

	//____ ErrorCode ____________________________________________________

	enum class ErrorCode			//. autoExtras
	{
		OutOfRange,
		InvalidIterator,
		InvalidParam,
		FailedPrerequisite,
		OpenGL,						// OpenGL has issued a GLerror
		Internal,
		SystemIntegrity,
		IllegalCall,
		ResourceExhausted,			// A limited internal resource has been exhausted
		RenderFailure,
		Other
	};

	//___ StateBits ____________________________________________________

	enum class StateBits
	{
		Focused = 0,
		Hovered = 1,
		Pressed = 2,
		Selected = 3,
		Targeted = 4,
		Disabled = 5
	};

	static const int	StateBits_Nb = 6;
	static const int	StateBits_MaxValue = 5;


	//____ Bitmask ____________________________________________________

	template<typename T> class Bitmask
	{
	public:
	  Bitmask() { m_mask = 0; }
		Bitmask(T v) { m_mask = v; }

		inline void setBit(int index) { m_mask |= (T(1) << index); }
		inline void setBit(int index, bool value) { m_mask &= ~(T(1) << index); m_mask |= (T(value) << index); }
		inline bool bit(int index) const { return ((m_mask & (T(1) << index)) != 0); }
		inline void clearBit(int index) { m_mask &= ~(T(1) << index); }
		inline bool isEmpty() const { return m_mask == 0; }
		inline void flipBit(int index) { m_mask ^= (T(1) << index);  }

		inline Bitmask<T> operator=(const Bitmask<T>& r) { m_mask = r.m_mask; return *this; }
		inline Bitmask<T> operator=(T r) { m_mask = r; return *this; }

		inline Bitmask<T> operator&(const Bitmask<T>& r) { return Bitmask<T>(m_mask & r.m_mask); }
		inline Bitmask<T> operator|(const Bitmask<T>& r) { return Bitmask<T>(m_mask | r.m_mask); }
		inline Bitmask<T> operator^(const Bitmask<T>& r) { return Bitmask<T>(m_mask ^ r.m_mask); }
		inline Bitmask<T> operator~() { return Bitmask<T>(~m_mask); }

		inline Bitmask<T> operator&=(const Bitmask<T>& r) { m_mask &= r.m_mask; return *this; }
		inline Bitmask<T> operator|=(const Bitmask<T>& r) { m_mask |= r.m_mask; return *this; }
		inline Bitmask<T> operator^=(const Bitmask<T>& r) { m_mask ^= r.m_mask; return *this; }


		inline operator T() const { return m_mask; }

		T	 mask() const { return m_mask; }

	private:
		T	m_mask;
	};


	typedef unsigned int	RouteId;

	typedef uint16_t		TextStyle_h;

	//____ TypeInfo __________________________________________________________

	struct TypeInfo
	{
		const char * className;
		const TypeInfo * pSuperClass;

		inline bool operator==(const TypeInfo& other) const { return (this == &other); }		// Same address is same structure.
		inline bool operator!=(const TypeInfo& other) const { return (this != &other); }		// Same address is same structure.
	};

	//____ ExtChar __________________________________________________________

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

	static const CodePage DefaultCodePage = CodePage::_1252;

	//____ BreakRules ____________________________________________________________

	enum class BreakRules : uint8_t
	{
		NoBreak		= 0,
		BreakBefore	= 16,
		BreakOn		= 32,
		BreakAfter	= 64
	};

	//____ TintMode ___________________________________________________________

	enum class TintMode : uint8_t //.autoExtras
	{
		None = 0,
		Flat,
		GradientX,
		GradientY,
		GradientXY
	};

	//____ BlendMode ____________________________________________________________

	// BlendModes control how blits and fills are blended against their backgrounds and how colors are blended against each other.
	// There are two groups of BlendModes.
	// The first one is for combining two surfaces into one and consists of Replace, Morph and Blend.
	// The second one is for manipulating RGB values of destination and therefore does not update the
	// destinations alpha channel (if any), unless the destination lacks RGB values, in which case the
	// alpha is manipulated instead.


	enum class BlendMode : uint8_t	//. autoExtras
	{
		Undefined,			///< Blitting: Defaults to Blend
							///< Color Blending: Defaults to ignore
							///< This value is used internally to distinguish undefined values from an explicitly set ignore,
		Ignore,				///< Blitting: No blitting performed.
							///< Color Blending: DstRGBA = DstRGBA
		Replace,			///< Blitting: Completely opaque blitting, ignoring alpha of source and tint-color.
							///< Color Blending: DstRGBA = SrcRGBA
		Blend,				///< Blitting: Normal mode, alpha of source and tint-color is taken into account.
							///< Color Blending: DstA = SrcA, DstRGB = SrcRGB + ((TintRGB-SrcRGB)*TintA/255)
		Add,				///< Blitting: RGB Additive, alpha of source and tint-color is taken into account.
							///< Color Blending: DstRGBA = SrcRGBA + TintRGBA
		Subtract,			///< Blitting: RGB Subtractive, alpha is ignored.
							///< Color Blending: DstRGBA = SrcRGBA - TintRGBA
		Multiply,			///< Blitting: RGB Multiply, alpha is ignored.
							///< Color Blending: DstRGB = SrcRGBA * TintRGBA/255
		Invert,				///< Blitting: Inverts destination RGB values where alpha of source is non-zero. Ignores RBG components. Uses alpha of tint-color.
							///< Color Blending: DstA = SrcA, DstRGB = ((255 - SrcRGB)*TintA + SrcRGB*(255-TintA))/255
		Min,				///< Blitting: Minimum value of each RGB component, alpha is ignored.
							///< Color Blending: DstRGBA = min(SrcRGBA,DstRGBA)
		Max,				///< Blitting: Maximum value of each RGB component, alpha is ignored.
							///< Color Blending: DstRGBA = max(SrcRGBA,DstRGBA)
		Morph				///< Blitting: Transition RGBA into source by morph factor.
							///< Color Blending: A 50% mix of the two colors.
	};

	//____ PointerStyle __________________________________________________________

	enum class PointerStyle : uint8_t	//. autoExtras
	{
		Arrow,				// default arrow
		Default = Arrow,	// default arrow
		Hourglass,			// hourglass
		Hand,				// hand
		OpenHand,			// open hand
		ClosedHand,			// closed hand
		Crosshair,			// crosshair
		Help,				// help
		Ibeam,				// I-beam
		Stop,				// slashed circle
		UpArrow,			// arrow pointing up
		ResizeAll,			// four-pointed arrow in all directions
		ResizeNeSw,			// double-pointed arrow pointing northeast and southwest
		ResizeNwSe,			// double-pointed arrow pointing northwest and southeast
		ResizeNS,			// double-pointed arrow pointing north and south
		ResizeWE,			// double-pointed arrow pointing west and east
		ResizeBeamNS,		// double-pointed arrow pointing north and south
		ResizeBeamWE,		// double-pointed arrow pointing north and south
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


	//____ PlayMode _____________________________________________________________

	enum class PlayMode : uint8_t	//. autoExtras
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
		MarkPolicy,			///< Perform a mark test on widget according to widgets MarkPolicy.
		Geometry,			///< Goes strictly on geometry, ignores markPolicy.
		ActionTarget,		///< Like MarkPolicy, but takes modality into account.
	};

	//____ MarkPolicy ____________________________________________________________

	enum class MarkPolicy : uint8_t	//. autoExtras
	{
		Ignore,				///< Widget can not be marked, hovered or pressed by pointer.
		AlphaTest,			///< An alpha-test is performed to determine if widget is marked.
		Geometry			///< Widget is marked if pointer is within its geometry.
	};

	//____ Placement _____________________________________________________________

	enum class Placement : uint8_t	//. autoExtras
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


	//____ Axis __________________________________________________________

	enum class Axis : uint8_t	//. autoExtras
	{
        Undefined,
		X,
		Y
	};

	//____ XSections _______________________________________________________

	enum class XSections : uint8_t
	{
		None	= 0,
		Left	= 1 << 0,
		Center	= 1 << 1,
		Right	= 1 << 2,
		All = Left | Center | Right
	};

	inline XSections operator|(XSections lhs, XSections rhs) {
		return static_cast<XSections>( static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs) );
	}

	inline XSections operator&(XSections lhs, XSections rhs) {
		return static_cast<XSections>( static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs) );
	}

	//____ YSections _______________________________________________________

	enum class YSections : uint8_t
	{
		None = 0,
		Top = 1 << 0,
		Center = 1 << 1,
		Bottom = 1 << 2,
		All = Top | Center | Bottom
	};

	inline YSections operator|(YSections lhs, YSections rhs) {
		return static_cast<YSections>( static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs) );
	}

	inline YSections operator&(YSections lhs, YSections rhs) {
		return static_cast<YSections>( static_cast<uint8_t>(lhs) &	static_cast<uint8_t>(rhs) );
	}



	//____ SizeConstraint ___________________________________________________________
	/**
		SizeConstraint is used by certain containers, including FlexPanel and ScrollPanel,
		to limit/control the geometry of children. Different SizePolicies can be set
		for horizontal and vertical size.

		It is used in combination with a size specified by parent. In the case of
		ScrollPanel it is the size of the window to the scrollarea. In the case of
		FlexPanel, it is a size specified for the child.
	*/
	enum class SizeConstraint : uint8_t		//. autoExtras
	{
		None = 0,				///< Childs size is unaffected by size specified by parent, so it gets its default size.
		Equal,					///< Child is bound to the exact size specified by parent.
		LessOrEqual,			///< Childs size is limited to the size specified by parent.
		GreaterOrEqual			///< Childs size is set to at least the size specified by parent.
	};


	//____ SizePolicy2D ___________________________________________________________

	enum class SizePolicy2D : uint8_t	//. autoExtras
	{
		Original,						///< Object maintains its original size.
		Stretch,						///< Object is stretched to fill the space, ignoring aspect ratio.
		Scale							///< Object is scaled to fill the space as much as possible, respecting original aspect ratio.
	};


	//____ MsgType ______________________________________________________________

	enum class MsgType	//. autoExtras
	{
		Dummy = 0,
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

		DropPick,
		DropProbe,
		DropEnter,
		DropMove,
		DropLeave,
		DropDeliver,
		DropCancel,
		DropComplete,

		Select,						// Non-value widget triggered, like a button being pressed.
		Unselect,					// Used by ToggleGroup when ToggleButton is unselected (but no other selected).
		Toggle,						// Boolean value widget toggled, like for checkboxes, radiobuttons etc.
		ScrollbarMove,
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

		PianoKeyPress,
		PianoKeyRelease

	};


	//____ SortOrder ____________________________________________________________

	enum class SortOrder	//. autoExtras
	{
		None,
		Ascending,
		Descending
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


	//____ ComponentNotif ___________________________________________________________

	enum class ComponentNotif
	{
		SortOrderChanged,
		ValueModified,
		ValueEdited,
		SpanModified,
		StepForward,
		StepBackward,
		JumpForward,
		JumpBackward,
		Other
	};

	//____ SampleMethod ____________________________________________________________

	enum class SampleMethod : uint8_t	//. autoExtras
	{
		Nearest,
		Bilinear,
		Undefined			// Needs to be last! Default to Bilinear if it is accelerated, otherwise Nearest.
	};

	//____ CanvasInit ______________________________________________________________

enum class CanvasInit 		//. autoExtras
	{
		Keep,
		Discard,
		Clear
	};


	//____ PixelFormat _____________________________________________________________

	enum class PixelFormat : uint8_t	//. autoExtras
	{
		Undefined,			///< Pixelformat is undefined.
		BGR_8,				///< One byte of blue, green and red in exactly that order in memory.
		BGR_8_sRGB,			///< One byte of blue, green and red in exactly that order in memory.
		BGR_8_linear,		///< One byte of blue, green and red in exactly that order in memory.

		BGRX_8,				///< One byte of blue, green, red and padding in exactly that order in memory.
		BGRX_8_sRGB,		///< One byte of blue, green, red and padding in exactly that order in memory.
		BGRX_8_linear,		///< One byte of blue, green, red and padding in exactly that order in memory.

		BGRA_8,				///< One byte of blue, green, red and alpha in exactly that order in memory.
		BGRA_8_sRGB,		///< One byte of blue, green, red and alpha in exactly that order in memory.
		BGRA_8_linear,		///< One byte of blue, green, red and alpha in exactly that order in memory.

		BGRA_4_linear,		///< 4 bits each of blue, green, red and alpha in exactly that order in memory.
		BGR_565_linear,		///< 5 bits of blue, 6 bits of green and 5 bits of red in exactly that order in memory.

		Index_8,				///< 8 bits of index into the palette.
		Index_8_sRGB,		///< 8 bits of index into the palette.
		Index_8_linear,		///< 8 bits of index into the palette.

		RGB_565_bigendian,	///< 3 high bits of green, 5 bits of blue, 5 bits of red and 3 low bits of green in exactly that order in memory.

		A_8,				///< 8 bits of alpha only.
	};


	//____ PixelDescription __________________________________________________________
	/**
	 * @brief Describes the format of a pixel.
	 *
	 * Describes the format of a pixel.
	 *
	 * The format of the pixel is described in three ways by a PixelDescription object:
	 *
	 * First a PixelFormat enum that contains predefined values for common pixel formats.
	 * This allows for human readable information and quick lockups.
	 *
	 * Secondly a set of variables containing the number of bits for each pixel and the
	 * individual red, green, blue and alpha channels. This allows for quickly and easily
	 * checking the size of a pixel and the color depths and presence and depth of the alpha channel.
	 *
	 * Thirdly a set of mask, shift and loss variables for each individual channel, specifies the position
	 * of each channel and allows for quick conversion to and from the default 32-bit RGBA format used by Color.
	 *
	 * If a pixel format can not be fully described by a PixelDescription object, the member type is set to Unknown.
	 *
	 * As long as the type member is not set to Unknown, you can extract the value of any channel of a
	 * pixel by applying the mask and shift variables. I.e. to extract the value of red from a pixel
	 * as an 8-bit value in the range 0-255, you use the formula:
	 *
	 * redValue = ((pixel & R_mask) >> R_shift) << R_loss
	 *
	 * Thus you can convert any specified pixel type to a Color structure using the following routine:
	 *
	 * uint32_t	pixel;
	 * PixelDescription * pFormat;
	 *
	 * 	Color col( ((pixel & pFormat->R_mask) >> pFormat->R_shift) << pFormat->R_loss,
	 *			   ((pixel & pFormat->G_mask) >> pFormat->G_shift) << pFormat->G_loss,
	 *			   ((pixel & pFormat->B_mask) >> pFormat->B_shift) << pFormat->B_loss,
	 *			   ((pixel & pFormat->A_mask) >> pFormat->A_shift) << pFormat->A_loss );
	 *
	 * To convert a Color object to a pixel value you can use:
	 *
	 * Color color;
	 * PixelDescription * pFormat;
	 *
	 * 	uint32_t pix = ((color.r >> pFormat->R_loss) << pFormat->R_shift) |
	 *				   ((color.g >> pFormat->G_loss) << pFormat->G_shift) |
	 *				   ((color.b >> pFormat->B_loss) << pFormat->B_shift) |
	 *				   ((color.a >> pFormat->A_loss) << pFormat->A_shift);
	 *
	 * This is essentially what the default implementation for Surface::colorToPixel() and Surface::pixelToColor() does.
	 *
	 **/


	struct PixelDescription
	{
	public:
		//.____ Properties _____________________________________________________

		PixelFormat	format;			///< Enum specifying the format if it exacty matches a predefined format, otherwise set to CUSTOM or UNKNOWN.
		int			bits;			///< Number of bits for the pixel, includes any non-used padding bits.
		bool		bIndexed;		///< True if pixels are index into palette, no RGB values in pixel.
		bool		bLinear;		///< True if RGB values are linear (as opposed to in sRGB format, e.g. logarithmic with gamma 2.2).
		bool 		bBigEndian;		///< Set if 16- or 32-bit pixel is in big-endian format.

		uint32_t	R_mask;			///< bitmask for getting the red bits out of the pixel
		uint32_t	G_mask;			///< bitmask for getting the green bits out of the pixel
		uint32_t	B_mask;			///< bitmask for getting the blue bits out of the pixel
		uint32_t	A_mask;			///< bitmask for getting the alpha bits out of the pixel

		uint8_t		R_shift;		///< amount to shift the red bits to the right to get the value.
		uint8_t		G_shift;		///< amount to shift the green bits to the right to get the value.
		uint8_t		B_shift;		///< amount to shift the blue bits to the right to get the value.
		uint8_t		A_shift;		///< amount to shift the alpha bits to the right to get the value.

		uint8_t		R_loss;			///< amount to shift the red bits to the right to get the value.
		uint8_t		G_loss;			///< amount to shift the green bits to the right to get the value.
		uint8_t		B_loss;			///< amount to shift the blue bits to the right to get the value.
		uint8_t		A_loss;			///< amount to shift the alpha bits to the right to get the value.


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

	//____ GfxFlip ____________________________________________________________

	enum class GfxFlip : uint8_t	//. autoExtras
	{
		Normal = 0,
		FlipX,
		FlipY,
		Rot90,
		Rot90FlipX,
		Rot90FlipY,
		Rot180,
		Rot180FlipX,
		Rot180FlipY,
		Rot270,
		Rot270FlipX,
		Rot270FlipY,
	};

	//____ CanvasRef ____________________________________________________________

	enum class CanvasRef			//.autoExtras
	{
		None,
		Default,
		Canvas_1,
		Canvas_2,
		Canvas_3,
		Canvas_4,
		Canvas_5,
		Canvas_6,
		Canvas_7,
		Canvas_8,
		Canvas_9,
		Canvas_10,
		Canvas_11,
		Canvas_12,
		Canvas_13,
		Canvas_14,
		Canvas_15,
		Canvas_16,
		Canvas_17,
		Canvas_18,
		Canvas_19,
		Canvas_20,
		Canvas_21,
		Canvas_22,
		Canvas_23,
		Canvas_24,
		Canvas_25,
		Canvas_26,
		Canvas_27,
		Canvas_28,
		Canvas_29,
		Canvas_30,
		Canvas_31,
		Canvas_32
	};

	//____ DataSegment ________________________________________________________

	struct DataSegment
	{
		uint8_t*	pBegin;
		uint8_t*	pEnd;
	};

	//____ GfxChunkId ____________________________________________________

	enum class GfxChunkId : uint8_t    //. autoExtras
	{
		OutOfData = 0,
		BeginRender = 1,
		EndRender = 2,
		Flush = 3,

		BeginCanvasUpdate = 4,
		EndCanvasUpdate = 5,

		SetClipList = 6,
		ResetClipList = 7,
		PushClipList = 8,
		PopClipList = 9,

		SetTintColor = 10,
		SetTintGradient = 11,
		ClearTintGradient = 12,

		SetBlendMode = 13,
		SetBlitSource = 14,
		SetMorphFactor = 15,
		SetRenderLayer = 16,

		Fill = 17,
		FillRect = 18,
		Undefined1 = 19,
		PlotPixels = 20,
		DrawLineFromTo = 21,
		DrawLineStraight = 22,

		Blit = 23,
		BlitRect = 24,
		FlipBlit = 25,
		FlipBlitRect = 26,

		StretchBlit = 27,
		StretchBlitRect = 28,

		StretchFlipBlit = 30,
		StretchFlipBlitRect = 31,

		PrecisionBlit = 29,
		TransformBlit = 32,
		RotScaleBlit = 33,
		Tile = 34,
		FlipTile = 35,

		ScaleTile = 36,
		ScaleFlipTile = 37,

		DrawWave = 38,
		FlipDrawWave = 39,

		DrawElipse = 40,
		DrawPieChart = 41,

		DrawSegments = 42,
		FlipDrawSegments = 43,

		BlitNinePatch = 44,

		EdgeSamples = 45,

		CreateSurface = 46,
		BeginSurfaceUpdate = 47,
		SurfacePixels = 48,
		EndSurfaceUpdate = 49,
		FillSurface = 50,
		CopySurface = 51,
		DeleteSurface = 52,

		ProtocolVersion = 53,
		TimeStampMS = 54,
		CanvasList = 55
	};

	typedef uint8_t GfxChunkFlags;


} // namespace wg

#include <wg_enumextras.h>

#endif // WG_TYPES_DOT_H
