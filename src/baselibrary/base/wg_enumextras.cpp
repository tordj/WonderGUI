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

	const char * toString(TintMode i)
	{
		static const char * names[] = { 
			"None",
			"Flat",
			"GradientX",
			"GradientY",
			"GradientXY" };

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
			"Default",
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
			"None",
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

	const char * toString(MarkPolicy i)
	{
		static const char * names[] = { 
			"Ignore",
			"AlphaTest",
			"Geometry" };

		return names[(int)i];
	}

	const char * toString(Placement i)
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
			"Undefined",
			"X",
			"Y" };

		return names[(int)i];
	}

	const char * toString(SizeConstraint i)
	{
		static const char * names[] = { 
			"None",
			"Equal",
			"LessOrEqual",
			"GreaterOrEqual" };

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
			"Dummy",
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
			"ScrollbarMove",
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

	const char * toString(SampleMethod i)
	{
		static const char * names[] = { 
			"Nearest",
			"Bilinear",
			"Undefined" };

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
			"Undefined",
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
			"RGB_565_bigendian",
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

	const char * toString(GfxFlip i)
	{
		static const char * names[] = { 
			"Normal",
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

	const char * toString(CanvasRef i)
	{
		static const char * names[] = { 
			"None",
			"Default",
			"Canvas_1",
			"Canvas_2",
			"Canvas_3",
			"Canvas_4",
			"Canvas_5",
			"Canvas_6",
			"Canvas_7",
			"Canvas_8",
			"Canvas_9",
			"Canvas_10",
			"Canvas_11",
			"Canvas_12",
			"Canvas_13",
			"Canvas_14",
			"Canvas_15",
			"Canvas_16",
			"Canvas_17",
			"Canvas_18",
			"Canvas_19",
			"Canvas_20",
			"Canvas_21",
			"Canvas_22",
			"Canvas_23",
			"Canvas_24",
			"Canvas_25",
			"Canvas_26",
			"Canvas_27",
			"Canvas_28",
			"Canvas_29",
			"Canvas_30",
			"Canvas_31",
			"Canvas_32" };

		return names[(int)i];
	}

	const char * toString(GfxChunkId i)
	{
		static const char * names[] = { 
			"OutOfData",
			"BeginRender",
			"EndRender",
			"Flush",
			"BeginCanvasUpdate",
			"EndCanvasUpdate",
			"SetClipList",
			"ResetClipList",
			"PushClipList",
			"PopClipList",
			"SetTintColor",
			"SetTintGradient",
			"ClearTintGradient",
			"SetBlendMode",
			"SetBlitSource",
			"SetMorphFactor",
			"SetRenderLayer",
			"Fill",
			"FillRect",
			"PlotPixels",
			"DrawLineFromTo",
			"DrawLineStraight",
			"Blit",
			"BlitRect",
			"FlipBlit",
			"FlipBlitRect",
			"StretchBlit",
			"StretchBlitRect",
			"StretchFlipBlit",
			"StretchFlipBlitRect",
			"PrecisionBlit",
			"TransformBlit",
			"RotScaleBlit",
			"Tile",
			"FlipTile",
			"ScaleTile",
			"ScaleFlipTile",
			"DrawWave",
			"FlipDrawWave",
			"DrawElipse",
			"DrawPieChart",
			"DrawSegments",
			"FlipDrawSegments",
			"BlitNinePatch",
			"EdgeSamples",
			"CreateSurface",
			"BeginSurfaceUpdate",
			"SurfacePixels",
			"EndSurfaceUpdate",
			"FillSurface",
			"CopySurface",
			"DeleteSurface",
			"ProtocolVersion",
			"TimeStampMS",
			"CanvasList" };

		return names[(int)i];
	}


//=========================================================================
//. endAutoSection



}	//namespace wg
