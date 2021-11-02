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

#ifndef	WG_ENUMEXTRAS_DOT_H
#define WG_ENUMEXTRAS_DOT_H
#pragma once

#include <wg_types.h>

namespace wg
{

//. startAutoSection
/*=========================================================================

				>>> START OF AUTO GENERATED SECTION <<<

				Any modifications here will be overwritten!

=========================================================================*/

	const static CodePage        CodePage_min        = CodePage::Latin1;
	const static BlendMode       BlendMode_min       = BlendMode::Undefined;
	const static PointerStyle    PointerStyle_min    = PointerStyle::Arrow;
	const static MouseButton     MouseButton_min     = MouseButton::None;
	const static PlayMode        PlayMode_min        = PlayMode::Forward;
	const static SearchMode      SearchMode_min      = SearchMode::MarkPolicy;
	const static Placement       Placement_min       = Placement::NorthWest;
	const static Direction       Direction_min       = Direction::Up;
	const static Axis            Axis_min            = Axis::Undefined;
	const static SizePolicy      SizePolicy_min      = SizePolicy::Default;
	const static SizePolicy2D    SizePolicy2D_min    = SizePolicy2D::Original;
	const static MsgType         MsgType_min         = MsgType::Dummy;
	const static SortOrder       SortOrder_min       = SortOrder::None;
	const static SelectMode      SelectMode_min      = SelectMode::Unselectable;
	const static TextEditMode    TextEditMode_min    = TextEditMode::Static;
	const static SampleMethod    SampleMethod_min    = SampleMethod::Undefined;
	const static CanvasInit      CanvasInit_min      = CanvasInit::Keep;
	const static PixelFormat     PixelFormat_min     = PixelFormat::Undefined;
	const static MaskOp          MaskOp_min          = MaskOp::Recurse;
	const static GfxFlip         GfxFlip_min         = GfxFlip::Normal;
	const static CanvasRef       CanvasRef_min       = CanvasRef::None;

	const static CodePage        CodePage_max        = CodePage::_874;
	const static BlendMode       BlendMode_max       = BlendMode::Morph;
	const static PointerStyle    PointerStyle_max    = PointerStyle::ResizeBeamWE;
	const static MouseButton     MouseButton_max     = MouseButton::X2;
	const static PlayMode        PlayMode_max        = PlayMode::BackwardPingPong;
	const static SearchMode      SearchMode_max      = SearchMode::ActionTarget;
	const static Placement       Placement_max       = Placement::Center;
	const static Direction       Direction_max       = Direction::Left;
	const static Axis            Axis_max            = Axis::Y;
	const static SizePolicy      SizePolicy_max      = SizePolicy::Expanded;
	const static SizePolicy2D    SizePolicy2D_max    = SizePolicy2D::Scale;
	const static MsgType         MsgType_max         = MsgType::PianoKeyRelease;
	const static SortOrder       SortOrder_max       = SortOrder::Descending;
	const static SelectMode      SelectMode_max      = SelectMode::FlipOnSelect;
	const static TextEditMode    TextEditMode_max    = TextEditMode::Editable;
	const static SampleMethod       ScaleMode_max    = SampleMethod::Bilinear;
	const static CanvasInit      CanvasInit_max      = CanvasInit::Clear;
	const static PixelFormat     PixelFormat_max     = PixelFormat::A_8;
	const static MaskOp          MaskOp_max          = MaskOp::Mask;
	const static GfxFlip         GfxFlip_max         = GfxFlip::Rot270FlipY;
	const static CanvasRef       CanvasRef_max       = CanvasRef::Canvas_32;

	const static int             CodePage_size       = (int)CodePage::_874 + 1;
	const static int             BlendMode_size      = (int)BlendMode::Morph + 1;
	const static int             PointerStyle_size   = (int)PointerStyle::ResizeBeamWE + 1;
	const static int             MouseButton_size    = (int)MouseButton::X2 + 1;
	const static int             PlayMode_size       = (int)PlayMode::BackwardPingPong + 1;
	const static int             SearchMode_size     = (int)SearchMode::ActionTarget + 1;
	const static int             Placement_size      = (int)Placement::Center + 1;
	const static int             Direction_size      = (int)Direction::Left + 1;
	const static int             Axis_size           = (int)Axis::Y + 1;
	const static int             SizePolicy_size     = (int)SizePolicy::Expanded + 1;
	const static int             SizePolicy2D_size   = (int)SizePolicy2D::Scale + 1;
	const static int             MsgType_size        = (int)MsgType::PianoKeyRelease + 1;
	const static int             SortOrder_size      = (int)SortOrder::Descending + 1;
	const static int             SelectMode_size     = (int)SelectMode::FlipOnSelect + 1;
	const static int             TextEditMode_size   = (int)TextEditMode::Editable + 1;
	const static int             SampleMethod_size   = (int)SampleMethod::Bilinear + 1;
	const static int             CanvasInit_size     = (int)CanvasInit::Clear + 1;
	const static int             PixelFormat_size    = (int)PixelFormat::A_8 + 1;
	const static int             MaskOp_size         = (int)MaskOp::Mask + 1;
	const static int             GfxFlip_size        = (int)GfxFlip::Rot270FlipY + 1;
	const static int             CanvasRef_size      = (int)CanvasRef::Canvas_32 + 1;

	const char * toString(CodePage);
	const char * toString(BlendMode);
	const char * toString(PointerStyle);
	const char * toString(MouseButton);
	const char * toString(PlayMode);
	const char * toString(SearchMode);
	const char * toString(Placement);
	const char * toString(Direction);
	const char * toString(Axis);
	const char * toString(SizePolicy);
	const char * toString(SizePolicy2D);
	const char * toString(MsgType);
	const char * toString(SortOrder);
	const char * toString(SelectMode);
	const char * toString(TextEditMode);
	const char * toString(SampleMethod);
	const char * toString(CanvasInit);
	const char * toString(PixelFormat);
	const char * toString(MaskOp);
	const char * toString(GfxFlip);
	const char * toString(CanvasRef);

//=========================================================================
//. endAutoSection



} // namespace wg

#endif //WG_ENUMEXTRAS_DOT_H
