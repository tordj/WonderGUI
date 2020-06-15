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
	const static Origo           Origo_min           = Origo::NorthWest;
	const static Direction       Direction_min       = Direction::Up;
	const static Axis            Axis_min            = Axis::X;
	const static SizePolicy      SizePolicy_min      = SizePolicy::Default;
	const static SizePolicy2D    SizePolicy2D_min    = SizePolicy2D::Original;
	const static MsgType         MsgType_min         = MsgType::Dummy;
	const static SortOrder       SortOrder_min       = SortOrder::None;
	const static SelectMode      SelectMode_min      = SelectMode::Unselectable;
	const static TextEditMode    TextEditMode_min    = TextEditMode::Static;
	const static AccessMode      AccessMode_min      = AccessMode::None;
	const static ScaleMode       ScaleMode_min       = ScaleMode::Nearest;
	const static PixelFormat     PixelFormat_min     = PixelFormat::Unknown;
	const static MaskOp          MaskOp_min          = MaskOp::Recurse;
	const static GfxChunkId      GfxChunkId_min      = GfxChunkId::OutOfData;
	const static GfxFlip         GfxFlip_min         = GfxFlip::Normal;

	const static CodePage        CodePage_max        = CodePage::_874;
	const static BlendMode       BlendMode_max       = BlendMode::Morph;
	const static PointerStyle    PointerStyle_max    = PointerStyle::ResizeBeamWE;
	const static MouseButton     MouseButton_max     = MouseButton::X2;
	const static PlayMode        PlayMode_max        = PlayMode::BackwardPingPong;
	const static SearchMode      SearchMode_max      = SearchMode::ActionTarget;
	const static Origo           Origo_max           = Origo::Center;
	const static Direction       Direction_max       = Direction::Left;
	const static Axis            Axis_max            = Axis::Y;
	const static SizePolicy      SizePolicy_max      = SizePolicy::Expanded;
	const static SizePolicy2D    SizePolicy2D_max    = SizePolicy2D::Scale;
	const static MsgType         MsgType_max         = MsgType::PianoKeyRelease;
	const static SortOrder       SortOrder_max       = SortOrder::Descending;
	const static SelectMode      SelectMode_max      = SelectMode::FlipOnSelect;
	const static TextEditMode    TextEditMode_max    = TextEditMode::Editable;
	const static AccessMode      AccessMode_max      = AccessMode::ReadWrite;
	const static ScaleMode       ScaleMode_max       = ScaleMode::Interpolate;
	const static PixelFormat     PixelFormat_max     = PixelFormat::A_8;
	const static MaskOp          MaskOp_max          = MaskOp::Mask;
	const static GfxChunkId      GfxChunkId_max      = GfxChunkId::DeleteSurface;
	const static GfxFlip         GfxFlip_max         = GfxFlip::Rot270FlipY;

	const static int             CodePage_size       = (int)CodePage::_874 + 1;
	const static int             BlendMode_size      = (int)BlendMode::Morph + 1;
	const static int             PointerStyle_size   = (int)PointerStyle::ResizeBeamWE + 1;
	const static int             MouseButton_size    = (int)MouseButton::X2 + 1;
	const static int             PlayMode_size       = (int)PlayMode::BackwardPingPong + 1;
	const static int             SearchMode_size     = (int)SearchMode::ActionTarget + 1;
	const static int             Origo_size          = (int)Origo::Center + 1;
	const static int             Direction_size      = (int)Direction::Left + 1;
	const static int             Axis_size           = (int)Axis::Y + 1;
	const static int             SizePolicy_size     = (int)SizePolicy::Expanded + 1;
	const static int             SizePolicy2D_size   = (int)SizePolicy2D::Scale + 1;
	const static int             MsgType_size        = (int)MsgType::PianoKeyRelease + 1;
	const static int             SortOrder_size      = (int)SortOrder::Descending + 1;
	const static int             SelectMode_size     = (int)SelectMode::FlipOnSelect + 1;
	const static int             TextEditMode_size   = (int)TextEditMode::Editable + 1;
	const static int             AccessMode_size     = (int)AccessMode::ReadWrite + 1;
	const static int             ScaleMode_size      = (int)ScaleMode::Interpolate + 1;
	const static int             PixelFormat_size    = (int)PixelFormat::A_8 + 1;
	const static int             MaskOp_size         = (int)MaskOp::Mask + 1;
	const static int             GfxChunkId_size     = (int)GfxChunkId::DeleteSurface + 1;
	const static int             GfxFlip_size        = (int)GfxFlip::Rot270FlipY + 1;

	const char * toString(CodePage);
	const char * toString(BlendMode);
	const char * toString(PointerStyle);
	const char * toString(MouseButton);
	const char * toString(PlayMode);
	const char * toString(SearchMode);
	const char * toString(Origo);
	const char * toString(Direction);
	const char * toString(Axis);
	const char * toString(SizePolicy);
	const char * toString(SizePolicy2D);
	const char * toString(MsgType);
	const char * toString(SortOrder);
	const char * toString(SelectMode);
	const char * toString(TextEditMode);
	const char * toString(AccessMode);
	const char * toString(ScaleMode);
	const char * toString(PixelFormat);
	const char * toString(MaskOp);
	const char * toString(GfxChunkId);
	const char * toString(GfxFlip);

//=========================================================================
//. endAutoSection



} // namespace wg

#endif //WG_ENUMEXTRAS_DOT_H
