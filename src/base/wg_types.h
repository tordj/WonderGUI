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

#undef min
#undef max

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

	template<typename T, typename T2, typename T3> inline void limit(T& x, T2 min, T3 max) { if( x < min) x = min; if( x > max) x = max; }

	//____ ErrorCode ____________________________________________________

	enum class ErrorCode
	{
		OutOfRange,
		InvalidInterator,
		OpenGL,
		Other
	};

	//____ StateEnum ____________________________________________________

	enum class StateEnum : uint8_t
	{
		Normal					= 0,			///< Element is neither hovered, pressed, selected or focused.
		Focused					= 1,			///< Element has keyboard focus
		Hovered					= 4,			///< Mouse pointer is hovering over element.
		HoveredFocused			= 4+1,
		Pressed					= 4+2,			///< Mouse button (usually left one) is pressed on element.
		PressedFocused			= 4+2+1,
		Selected				= 8,			///< Element is in a selected state, like a selected checkbox or item in a list.
		SelectedFocused			= 8+1,
		SelectedHovered			= 8+4,
		SelectedHoveredFocused	= 8+4+1,
		SelectedPressed			= 8+4+2,
		SelectedPressedFocused	= 8+4+2+1,
		Targeted				= 16+4,
		TargetedFocused			= 16+4+1,
		TargetedSelected		= 16+8+4,
		TargetedSelectedFocused = 16+8+4+1,
		Disabled				= 32,			///< Element is disabled and can't be focused or pressed.
		DisabledSelected		= 32+8,
	};

	static const int	StateEnum_Nb	= 18;			// Number of states
	static const int	StateEnum_MaxValue	= 40;		// Highest value for StateEnum

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
		bool	setHovered(bool bHovered) { if( m_state == ((uint8_t)StateEnum::Disabled) ) return false; if(bHovered) m_state |= ((uint8_t)StateEnum::Hovered); else m_state &= ~(uint8_t(StateEnum::Pressed)|uint8_t(StateEnum::Targeted)); return true; }
		bool	setPressed(bool bPressed) { if( m_state == ((uint8_t)StateEnum::Disabled) ) return false; if(bPressed) m_state |= ((uint8_t)StateEnum::Pressed); else m_state &= ~(((uint8_t)StateEnum::Pressed) - ((uint8_t)StateEnum::Hovered)); return true; }
		bool	setTargeted(bool bTargeted) { if (m_state == ((uint8_t)StateEnum::Disabled)) return false; if (bTargeted) m_state |= ((uint8_t)StateEnum::Targeted); else m_state &= ~((uint8_t)StateEnum::Targeted); return true; }


		bool	isEnabled() const { return (m_state & ((uint8_t)StateEnum::Disabled)) == ((uint8_t)StateEnum::Normal); }
		bool	isSelected() const { return (m_state & ((uint8_t)StateEnum::Selected)) == ((uint8_t)StateEnum::Selected); }
		bool	isFocused() const { return (m_state & ((uint8_t)StateEnum::Focused)) == ((uint8_t)StateEnum::Focused); }
		bool	isHovered() const { return (m_state & ((uint8_t)StateEnum::Hovered)) == ((uint8_t)StateEnum::Hovered); }
		bool	isPressed() const { return (m_state & ((uint8_t)StateEnum::Pressed)) == ((uint8_t)StateEnum::Pressed); }
		bool	isTargeted() const { return (m_state & ((uint8_t)StateEnum::Targeted)) == ((uint8_t)StateEnum::Targeted); }

		//._____ Operators _____________________________________________________

		inline bool operator==(StateEnum state) const { return m_state == ((uint8_t)state); }
		inline bool operator!=(StateEnum state) const { return m_state != ((uint8_t)state); }

		inline StateEnum operator=(StateEnum state) { m_state = ((uint8_t)state); return *this; }

		operator StateEnum() const { return (StateEnum) m_state; }

		inline State operator+(StateEnum state) const { int s = m_state | (uint8_t) state; if (s & (int) StateEnum::Disabled) s &= (int) StateEnum::DisabledSelected; return (StateEnum) s; }
		inline State operator-(StateEnum _state) const
		{
			int state = (int)_state;
			int hovered = int(StateEnum::Hovered);
			int hoverDependant = (int(StateEnum::Pressed) | int(StateEnum::Targeted)) & ~hovered;

			if ((state & int(StateEnum::Pressed)) == int(StateEnum::Pressed))
				state &= ~hovered;				// Special case: Don't remove hovered just because we remove pressed.
			int s = (m_state & ~state);
			if ((s & hovered) == 0)
				s &= ~hoverDependant;			// If we remove hovered we can't keep a state dependant on it.
			return (StateEnum)s;
		}

		inline State& operator+=(StateEnum state) { m_state |= (uint8_t) state; if (int(m_state) & int(StateEnum::Disabled)) m_state &= int(StateEnum::DisabledSelected); return *this; }
		inline State& operator-=(StateEnum _state)
		{
			int state = (int)_state;
			int hovered = int(StateEnum::Hovered);
			int hoverDependant = (int(StateEnum::Pressed) | int(StateEnum::Targeted)) & ~hovered;

			if ((state & int(StateEnum::Pressed)) == int(StateEnum::Pressed))
				state &= ~hovered;				// Special case: Don't remove hovered just because we remove pressed.
			m_state &= ~state;
			if ((m_state & hovered) == 0)
				m_state &= ~hoverDependant;			// If we remove hovered we can't keep a state dependant on it.
			return *this;
		}

	private:
		uint8_t		m_state;
	};

	//____ Bitmask ____________________________________________________

	template<typename T> class Bitmask
	{
	public:
		Bitmask<T>() { m_mask = 0; }
		Bitmask<T>(T v) { m_mask = v; }

		inline void setBit(int index) { m_mask |= (T(1) << index); }
		inline void setBit(int index, bool value) { m_mask &= ~(T(1) << index); m_mask |= (T(value) << index); }
		inline bool bit(int index) const { return ((m_mask & (T(1) << index)) != 0); }
		inline void clearBit(int index) { m_mask &= ~(T(1) << index); }

		inline Bitmask<T> operator=(const Bitmask<T>& r) { m_mask = r.m_mask; return *this; }
		inline Bitmask<T> operator=(T r) { m_mask = r; return *this; }

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

	//____ BlendMode ____________________________________________________________

	// BlendModes control how blits and fills are blended against their backgrounds and how colors are blended against each other.

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
		Min,				///< Blitting: Minimum value of each RGBA component.
							///< Color Blending: DstRGBA = min(SrcRGBA,DstRGBA
		Max,				///< Blitting: Maximum value of each RGBA component.
							///< Color Blending: DstRGBA = max(SrcRGBA,DstRGBA)
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
		Original,						///< Object maintains its original size.
		Stretch,						///< Object is stretched to fill the space, ignoring aspect ratio.
		Scale							///< Object is scaled to fill the space as much as possible, respecting original aspect ratio.
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

		DropPick,
		DropProbe,
		DropEnter,
		DropMove,
		DropLeave,
		DropDeliver,
		DropCancel,
		DropComplete,

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


	//____ SurfaceFlag ____________________________________________________________

	namespace SurfaceFlag
	{
		const int Static = 0;		// No content access/modification expected
		const int Dynamic = 1;		// Expect content to be accessed and/or modified
		const int WriteOnly = 2;	// Can only be locked in WriteOnly mode. Alpha can still be read pixel by pixel if present.
		const int Mipmapped = 4;	// Surface should be Mipmapped. Better downscaling, but takes more memory and time to initialize and unlock Write-access.
        const int Scale200 = 8;
	};


	//____ PixelFormat _____________________________________________________________

	enum class PixelFormat	//. autoExtras
	{
		Unknown,			///< Pixelformat is unkown or can't be expressed in a PixelDescription struct.
		Custom,				///< Pixelformat has no PixelFormat enum, but is fully specified through the PixelDescription struct.
		BGR_8,				///< One byte of blue, green and red in exactly that order in memory.
		BGRX_8,				///< One byte of blue, green, red and padding in exactly that order in memory.
		BGRA_8,				///< One byte of blue, green, red and alpha in exactly that order in memory.
		BGRA_4,				///< 4 bits each of blue, green, red and alpha in exactly that order in memory.
		BGR_565,			///< 5 bits of blue, 6 bits of green and 5 bits of red in exactly that order in memory.
		I8,					///< 8 bits of index into the CLUT (Color Lookup Table).
		A8					///< 8 bits of alpha only.
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
		bool		bIndexed;		///< True if pixels are index into CLUT, no RGB values in pixel.

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

	//____ GfxChunkId ____________________________________________________

	enum class GfxChunkId : uint16_t	//. autoExtras
	{
		OutOfData,

		BeginRender,
		EndRender,
		SetCanvas,
		SetClip,
		SetTintColor,
		SetBlendMode,
		SetBlitSource,

		Fill,
		FillSubpixel,
		PlotPixels,
		DrawLineFromTo,
		DrawLineStraight,


		Blit,
//		FlipBlit,
		StretchBlit,
//		StretchBlitSubpixel,

//		StretchFlipBlit,
//		StretchFlipBlitSubpixel,

//		RotScaleBlit,


//		DrawWave,
//		FlipDrawWave,

//		DrawElipse,

//		DrawSegments,
//		FlipDrawSegments,

		SimpleTransformBlit,
		ComplexTransformBlit,

		TransformDrawSegments,
		EdgeSamples,

//		BlitNinePatch,


		CreateSurface,
		SetSurfaceScaleMode,
		BeginSurfaceUpdate,
		SurfaceData,
		EndSurfaceUpdate,
		FillSurface,
		CopySurface,
		DeleteSurface
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


} // namespace wg

#include <wg_enumextras.h>

#endif // WG_TYPES_DOT_H
