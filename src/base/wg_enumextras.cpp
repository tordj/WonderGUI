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

#include <wg_enumextras.h>
#include <assert.h>

namespace wg
{

//. startAutoSection
/*=========================================================================

				>>> START OF AUTO GENERATED SECTION <<<

				Any modifications here will be overwritten!

=========================================================================*/

	const char * toString(CodePage i)
	{
		static const char * names[] = { 
			"Latin1",
			"_1250",
			"_1251",
			"_1252",
			"_1253",
			"_1254",
			"_1255",
			"_1256",
			"_1257",
			"_1258",
			"_874" };

		return names[(int)i];
	}

	const char * toString(BlendMode i)
	{
		static const char * names[] = { 
			"Undefined",
			"Ignore",
			"Replace",
			"Blend",
			"Add",
			"Subtract",
			"Multiply",
			"Invert",
			"Min",
			"Max",
			"Morph" };

		return names[(int)i];
	}

	const char * toString(PointerStyle i)
	{
		static const char * names[] = { 
			"Arrow",
			"Default = Arrow",
			"Hourglass",
			"Hand",
			"OpenHand",
			"ClosedHand",
			"Crosshair",
			"Help",
			"Ibeam",
			"Stop",
			"UpArrow",
			"ResizeAll",
			"ResizeNeSw",
			"ResizeNwSe",
			"ResizeNS",
			"ResizeWE",
			"ResizeBeamNS",
			"ResizeBeamWE" };

		return names[(int)i];
	}

	const char * toString(MouseButton i)
	{
		static const char * names[] = { 
			"None = 0",
			"Left",
			"Middle",
			"Right",
			"X1",
			"X2" };

		return names[(int)i];
	}

	const char * toString(PlayMode i)
	{
		static const char * names[] = { 
			"Forward",
			"Backward",
			"Looping",
			"BackwardLooping",
			"PingPong",
			"BackwardPingPong" };

		return names[(int)i];
	}

	const char * toString(SearchMode i)
	{
		static const char * names[] = { 
			"MarkPolicy",
			"Geometry",
			"ActionTarget" };

		return names[(int)i];
	}

	const char * toString(Origo i)
	{
		static const char * names[] = { 
			"NorthWest",
			"North",
			"NorthEast",
			"East",
			"SouthEast",
			"South",
			"SouthWest",
			"West",
			"Center" };

		return names[(int)i];
	}

	const char * toString(Direction i)
	{
		static const char * names[] = { 
			"Up",
			"Right",
			"Down",
			"Left" };

		return names[(int)i];
	}

	const char * toString(Axis i)
	{
		static const char * names[] = { 
			"X",
			"Y" };

		return names[(int)i];
	}

	const char * toString(SizePolicy i)
	{
		static const char * names[] = { 
			"Default = 0",
			"Bound",
			"Confined",
			"Expanded" };

		return names[(int)i];
	}

	const char * toString(SizePolicy2D i)
	{
		static const char * names[] = { 
			"Original",
			"Stretch",
			"Scale" };

		return names[(int)i];
	}

	const char * toString(MsgType i)
	{
		static const char * names[] = { 
			"Dummy = 0",
			"Tick",
			"PointerChange",
			"FocusGained",
			"FocusLost",
			"MouseEnter",
			"MouseMove",
			"MouseLeave",
			"MousePress",
			"MouseRepeat",
			"MouseDrag",
			"MouseRelease",
			"MouseClick",
			"MouseDoubleClick",
			"KeyPress",
			"KeyRepeat",
			"KeyRelease",
			"TextInput",
			"EditCommand",
			"WheelRoll",
			"DropPick",
			"DropProbe",
			"DropEnter",
			"DropMove",
			"DropLeave",
			"DropDeliver",
			"DropCancel",
			"DropComplete",
			"Select",
			"Toggle",
			"ValueUpdate",
			"RangeUpdate",
			"TextEdit",
			"ItemToggle",
			"ItemMousePress",
			"ItemsSelect",
			"ItemsUnselect",
			"PopupClosed",
			"ModalMoveOutside",
			"ModalBlockedPress",
			"ModalBlockedRelease",
			"PianoKeyPress",
			"PianoKeyRelease" };

		return names[(int)i];
	}

	const char * toString(SortOrder i)
	{
		static const char * names[] = { 
			"None",
			"Ascending",
			"Descending" };

		return names[(int)i];
	}

	const char * toString(SelectMode i)
	{
		static const char * names[] = { 
			"Unselectable",
			"SingleEntry",
			"MultiEntries",
			"FlipOnSelect" };

		return names[(int)i];
	}

	const char * toString(TextEditMode i)
	{
		static const char * names[] = { 
			"Static",
			"Selectable",
			"Editable" };

		return names[(int)i];
	}

	const char * toString(ScaleMode i)
	{
		static const char * names[] = { 
			"Nearest",
			"Interpolate" };

		return names[(int)i];
	}

	const char * toString(CanvasInit i)
	{
		static const char * names[] = { 
			"Keep",
			"Discard",
			"Clear" };

		return names[(int)i];
	}

	const char * toString(PixelFormat i)
	{
		static const char * names[] = { 
			"Unknown",
			"Custom",
			"BGR_8",
			"BGR_8_sRGB",
			"BGR_8_linear",
			"BGRX_8",
			"BGRX_8_sRGB",
			"BGRX_8_linear",
			"BGRA_8",
			"BGRA_8_sRGB",
			"BGRA_8_linear",
			"BGRA_4_linear",
			"BGR_565_linear",
			"CLUT_8",
			"CLUT_8_sRGB",
			"CLUT_8_linear",
			"A_8" };

		return names[(int)i];
	}

	const char * toString(MaskOp i)
	{
		static const char * names[] = { 
			"Recurse",
			"Skip",
			"Mask" };

		return names[(int)i];
	}

	const char * toString(GfxChunkId i)
	{
		static const char * names[] = { 
			"OutOfData",
			"BeginRender",
			"EndRender",
			"SetCanvas",
			"SetClip",
			"SetTintColor",
			"SetBlendMode",
			"SetBlitSource",
			"Fill",
			"FillSubpixel",
			"PlotPixels",
			"DrawLineFromTo",
			"DrawLineStraight",
			"Blit",
			"StretchBlit",
			"SimpleTransformBlit",
			"ComplexTransformBlit",
			"TransformDrawSegments",
			"EdgeSamples",
			"CreateSurface",
			"SetSurfaceScaleMode",
			"BeginSurfaceUpdate",
			"SurfaceData",
			"EndSurfaceUpdate",
			"FillSurface",
			"CopySurface",
			"DeleteSurface" };

		return names[(int)i];
	}

	const char * toString(GfxFlip i)
	{
		static const char * names[] = { 
			"Normal = 0",
			"FlipX",
			"FlipY",
			"Rot90",
			"Rot90FlipX",
			"Rot90FlipY",
			"Rot180",
			"Rot180FlipX",
			"Rot180FlipY",
			"Rot270",
			"Rot270FlipX",
			"Rot270FlipY" };

		return names[(int)i];
	}


//=========================================================================
//. endAutoSection



}	//namespace wg
