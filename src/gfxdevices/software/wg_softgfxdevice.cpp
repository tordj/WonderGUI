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

#include <wg_softgfxdevice.h>
#include <wg_softsurfacefactory.h>
#include <wg_util.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <wg_base.h>
#include <wg_context.h>

#include <wg_softkernelstanza.impl.h>


#include <cassert>

using namespace std;

namespace wg
{
	using namespace Util;

	const TypeInfo SoftGfxDevice::TYPEINFO = { "SoftGfxDevice", &GfxDevice::TYPEINFO };

	int SoftGfxDevice::s_mulTab[256];

	int16_t		SoftGfxDevice::s_limit4096Tab[4097 * 3];

	SoftGfxDevice::PlotOp_p		SoftGfxDevice::s_plotOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::FillOp_p		SoftGfxDevice::s_fillOpTab[TintMode_size][BlendMode_size][PixelFormat_size];
	SoftGfxDevice::LineOp_p		SoftGfxDevice::s_lineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::ClipLineOp_p SoftGfxDevice::s_clipLineOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::PlotListOp_p SoftGfxDevice::s_plotListOpTab[BlendMode_size][PixelFormat_size];
	SoftGfxDevice::SegmentOp_p	SoftGfxDevice::s_segmentOpTab[2][BlendMode_size][PixelFormat_size];		// bVerticalTint, BlendMode, PixelFormat

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_pass2OpTab[TintMode_size][BlendMode_size][PixelFormat_size];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_pass2OpTab_fast8[TintMode_size][BlendMode_size][PixelFormat_size];

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_internal_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGRA_8_linear_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGR_8_linear_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_moveTo_BGR_8_sRGB_OpTab[PixelFormat_size][2];

	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGRA_8_linear_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGR_8_linear_OpTab[PixelFormat_size][2];
	SoftGfxDevice::SimpleBlitOp_p		SoftGfxDevice::s_blendTo_BGR_8_sRGB_OpTab[PixelFormat_size][2];

	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformTo_internal_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformTo_BGRA_8_linear_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformTo_BGR_8_linear_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformTo_BGR_8_sRGB_OpTab[PixelFormat_size][2][3];

	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformBlendTo_BGRA_8_linear_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformBlendTo_BGRA_8_sRGB_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformBlendTo_BGR_8_linear_OpTab[PixelFormat_size][2][3];
	SoftGfxDevice::ComplexBlitOp_p		SoftGfxDevice::s_transformBlendTo_BGR_8_sRGB_OpTab[PixelFormat_size][2][3];


	const int16_t SoftGfxDevice::s_channel_4_1[256] =    { 0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15,
											0, 4096*1/15, 4096*2/15, 4096*3/15, 4096*4/15, 4096*5/15, 4096*6/15, 4096*7/15, 4096*8/15, 4096*9/15, 4096*10/15, 4096*11/15, 4096*12/15, 4096*13/15, 4096*14/15, 4096*15/15 };

	const int16_t SoftGfxDevice::s_channel_4_2[256] =    { 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15, 4096*0/15,
											4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15, 4096*1/15,		
											4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15, 4096*2/15,
											4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15, 4096*3/15,
											4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15, 4096*4/15,
											4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15, 4096*5/15,
											4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15, 4096*6/15,
											4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15, 4096*7/15,
											4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15, 4096*8/15,
											4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15, 4096*9/15,
											4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15, 4096*10/15,
											4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15, 4096*11/15,
											4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15, 4096*12/15,
											4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15, 4096*13/15,
											4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15, 4096*14/15,
											4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15, 4096*15/15 };


	const int16_t SoftGfxDevice::s_channel_5[256] =      { 4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
											4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31,
											4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
											4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31,
											4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
											4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31,
											4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
											4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31,
											4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
											4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31,
											4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
											4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31,
											4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
											4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31,
                                            4096*0/31, 4096*1/31, 4096*2/31, 4096*3/31, 4096*4/31, 4096*5/31, 4096*6/31, 4096*7/31, 4096*8/31, 4096*9/31, 4096*10/31, 4096*11/31, 4096*12/31, 4096*13/31, 4096*14/31, 4096*15/31,
                                            4096*16/31, 4096*17/31, 4096*18/31, 4096*19/31, 4096*20/31, 4096*21/31, 4096*22/31, 4096*23/31, 4096*24/31, 4096*25/31, 4096*26/31, 4096*27/31, 4096*28/31, 4096*29/31, 4096*30/31, 4096*31/31 };

	const int16_t SoftGfxDevice::s_channel_6[256] =      { 4096*0/63, 4096*1/63, 4096*2/63, 4096*3/63, 4096*4/63, 4096*5/63, 4096*6/63, 4096*7/63, 4096*8/63, 4096*9/63, 4096*10/63, 4096*11/63, 4096*12/63, 4096*13/63, 4096*14/63, 4096*15/63,
											4096*32/63, 4096*33/63, 4096*34/63, 4096*35/63, 4096*36/63, 4096*37/63, 4096*38/63, 4096*39/63, 4096*40/63, 4096*41/63, 4096*42/63, 4096*43/63, 4096*44/63, 4096*45/63, 4096*46/63, 4096*47/63,
											4096*16/63, 4096*17/63, 4096*18/63, 4096*19/63, 4096*20/63, 4096*21/63, 4096*22/63, 4096*23/63, 4096*24/63, 4096*25/63, 4096*26/63, 4096*27/63, 4096*28/63, 4096*29/63, 4096*30/63, 4096*31/63,
											4096*48/63, 4096*49/63, 4096*50/63, 4096*51/63, 4096*52/63, 4096*53/63, 4096*54/63, 4096*55/63, 4096*56/63, 4096*57/63, 4096*58/63, 4096*59/63, 4096*60/63, 4096*61/63, 4096*62/63, 4096*63/63,
											4096*0/63, 4096*1/63, 4096*2/63, 4096*3/63, 4096*4/63, 4096*5/63, 4096*6/63, 4096*7/63, 4096*8/63, 4096*9/63, 4096*10/63, 4096*11/63, 4096*12/63, 4096*13/63, 4096*14/63, 4096*15/63,
											4096*16/63, 4096*17/63, 4096*18/63, 4096*19/63, 4096*20/63, 4096*21/63, 4096*22/63, 4096*23/63, 4096*24/63, 4096*25/63, 4096*26/63, 4096*27/63, 4096*28/63, 4096*29/63, 4096*30/63, 4096*31/63,
											4096*32/63, 4096*33/63, 4096*34/63, 4096*35/63, 4096*36/63, 4096*37/63, 4096*38/63, 4096*39/63, 4096*40/63, 4096*41/63, 4096*42/63, 4096*43/63, 4096*44/63, 4096*45/63, 4096*46/63, 4096*47/63,
											4096*48/63, 4096*49/63, 4096*50/63, 4096*51/63, 4096*52/63, 4096*53/63, 4096*54/63, 4096*55/63, 4096*56/63, 4096*57/63, 4096*58/63, 4096*59/63, 4096*60/63, 4096*61/63, 4096*62/63, 4096*63/63,
											4096*0/63, 4096*1/63, 4096*2/63, 4096*3/63, 4096*4/63, 4096*5/63, 4096*6/63, 4096*7/63, 4096*8/63, 4096*9/63, 4096*10/63, 4096*11/63, 4096*12/63, 4096*13/63, 4096*14/63, 4096*15/63,
											4096*16/63, 4096*17/63, 4096*18/63, 4096*19/63, 4096*20/63, 4096*21/63, 4096*22/63, 4096*23/63, 4096*24/63, 4096*25/63, 4096*26/63, 4096*27/63, 4096*28/63, 4096*29/63, 4096*30/63, 4096*31/63,
											4096*32/63, 4096*33/63, 4096*34/63, 4096*35/63, 4096*36/63, 4096*37/63, 4096*38/63, 4096*39/63, 4096*40/63, 4096*41/63, 4096*42/63, 4096*43/63, 4096*44/63, 4096*45/63, 4096*46/63, 4096*47/63,
											4096*48/63, 4096*49/63, 4096*50/63, 4096*51/63, 4096*52/63, 4096*53/63, 4096*54/63, 4096*55/63, 4096*56/63, 4096*57/63, 4096*58/63, 4096*59/63, 4096*60/63, 4096*61/63, 4096*62/63, 4096*63/63,
											4096*0/63, 4096*1/63, 4096*2/63, 4096*3/63, 4096*4/63, 4096*5/63, 4096*6/63, 4096*7/63, 4096*8/63, 4096*9/63, 4096*10/63, 4096*11/63, 4096*12/63, 4096*13/63, 4096*14/63, 4096*15/63,
											4096*16/63, 4096*17/63, 4096*18/63, 4096*19/63, 4096*20/63, 4096*21/63, 4096*22/63, 4096*23/63, 4096*24/63, 4096*25/63, 4096*26/63, 4096*27/63, 4096*28/63, 4096*29/63, 4096*30/63, 4096*31/63,
											4096*32/63, 4096*33/63, 4096*34/63, 4096*35/63, 4096*36/63, 4096*37/63, 4096*38/63, 4096*39/63, 4096*40/63, 4096*41/63, 4096*42/63, 4096*43/63, 4096*44/63, 4096*45/63, 4096*46/63, 4096*47/63,
											4096*48/63, 4096*49/63, 4096*50/63, 4096*51/63, 4096*52/63, 4096*53/63, 4096*54/63, 4096*55/63, 4096*56/63, 4096*57/63, 4096*58/63, 4096*59/63, 4096*60/63, 4096*61/63, 4096*62/63, 4096*63/63 };



	const uint8_t SoftGfxDevice::s_fast8_channel_4_1[256] = {	0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
											0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

	const uint8_t SoftGfxDevice::s_fast8_channel_4_2[256] = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
											0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
											0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
											0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
											0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
											0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
											0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
											0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,
											0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
											0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
											0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
											0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
											0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
											0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd,
											0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
											0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	const uint8_t SoftGfxDevice::s_fast8_channel_5[256]	= {		0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff,
											0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
											0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff };

	const uint8_t SoftGfxDevice::s_fast8_channel_6[256] = {		0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff,
											0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff,
											0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff,
											0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
											0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
											0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
											0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff	};


	//____ create() _______________________________________________________________

	SoftGfxDevice_p SoftGfxDevice::create()
	{
		return SoftGfxDevice_p(new SoftGfxDevice());
	}

	//____ constructor _____________________________________________________________

	SoftGfxDevice::SoftGfxDevice()
	{
		m_pCanvasPixels = nullptr;
		m_pRenderLayerSurface = nullptr;
		m_canvasPixelBits = 0;
		m_canvasPitch = 0;
		m_colTrans.morphFactor = 2048;
		_initTables();

	}

	//____ Destructor ______________________________________________________________

	SoftGfxDevice::~SoftGfxDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SoftGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& SoftGfxDevice::surfaceType( void ) const
	{
		return SoftSurface::TYPEINFO;
	}

	//____ defineCanvas() ____________________________________________________

	bool SoftGfxDevice::defineCanvas( CanvasRef ref, SoftSurface * pSurface )
	{
	
		auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );

		if( it == m_definedCanvases.end() )
		{
			if( pSurface )
				m_definedCanvases.push_back( CanvasInfo( ref, pSurface, pSurface->pixelSize()*64, pSurface->scale() ) );
		}
		else
		{
			if( pSurface )
			{
				it->pSurface = pSurface;
				it->size = pSurface->pixelSize()*64;
				it->scale = pSurface->scale();
			}
			else
			{
				m_definedCanvases.erase(it);
			}
		}

		return true;
	}

	//____ canvas() _____________________________________________________

	const CanvasInfo& SoftGfxDevice::canvas(CanvasRef ref) const
	{
		auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](const CanvasInfo& entry) { return (ref == entry.ref); });

		if (it != m_definedCanvases.end())
			return *it;

		return m_dummyCanvas;
	}

	//____ surfaceFactory() _______________________________________________________

	SurfaceFactory_p SoftGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = SoftSurfaceFactory::create();

		return m_pSurfaceFactory;
	}

	//____ setTintColor() _____________________________________________________

	void SoftGfxDevice::setTintColor(HiColor color)
	{
		if (color == m_tintColor)
			return;

		GfxDevice::setTintColor(color);
		_updateTintSettings();

/*
		m_colTrans.flatTintColor = color;

		TintMode tintMode;

		if (m_nTintGradients == 0)
		{
			if (color == Color::White)
				tintMode = TintMode::None;
			else
				tintMode = TintMode::Flat;

			bool bOpacityChanged = (color.a != m_tintColor.a) && (color.a == 255 || m_tintColor.a == 255);

			if (bOpacityChanged || tintMode != m_tintModeSingle)
			{
				m_tintModeSingle = tintMode;
				m_tintModeMulti = tintMode;

				_updateBlitFunctions();
			}
		}
*/
	}

	//____ setTintGradient() __________________________________________________

	void SoftGfxDevice::setTintGradient(const RectI& rect, const Gradient& gradient)
	{
		GfxDevice::setTintGradient(rect, gradient);
		m_colTrans.tintRect = Util::align(rect) / 64;
		_updateTintSettings();
	}

	//____ clearTintGradient() _______________________________________________

	void SoftGfxDevice::clearTintGradient()
	{
		GfxDevice::clearTintGradient();
		_updateTintSettings();
	}

	//____ _updateTintSettings() ______________________________________________

	void SoftGfxDevice::_updateTintSettings()
	{
		TintMode tintMode;

		const int16_t* pUnpackTab = Base::activeContext()->gammaCorrection() ? HiColor::unpackSRGBTab : HiColor::unpackLinearTab;

		if (!m_bTintGradient)
		{
			tintMode = (m_tintColor == Color::White) ? TintMode::None : TintMode::Flat;
			m_colTrans.flatTintColor = m_tintColor;
			m_bTintOpaque = (m_tintColor.a == 4096);
		}
		else
		{
			// Update m_gradientData and generate diffMasks for X- and Y-gradients.

			int diffMaskX = 0, diffMaskY = 0;


			uint32_t	flatTintColorR = m_tintColor.r;
			uint32_t	flatTintColorG = m_tintColor.g;
			uint32_t	flatTintColorB = m_tintColor.b;
			uint32_t	flatTintColorA = m_tintColor.a;

			uint32_t	r[4], g[4], b[4], a[4];							// Scale: 0 -> (1 << 18)

			HiColor* pCol = &m_tintGradient.topLeft;
			for (int i = 0; i < 4; i++)
			{
				r[i] = (pCol->r * flatTintColorR) >> 6;
				g[i] = (pCol->g * flatTintColorG) >> 6;
				b[i] = (pCol->b * flatTintColorB) >> 6;
				a[i] = (pCol->a * flatTintColorA) >> 6;
				pCol++;
			}

			m_colTrans.topLeftR = r[0];
			m_colTrans.topLeftG = g[0];
			m_colTrans.topLeftB = b[0];
			m_colTrans.topLeftA = a[0];

			m_colTrans.topRightR = r[1];
			m_colTrans.topRightG = g[1];
			m_colTrans.topRightB = b[1];
			m_colTrans.topRightA = a[1];

			int width = Util::align(m_tintGradientRect.h)/64;

			m_colTrans.leftIncR = int(r[3] - r[0]) / width;
			m_colTrans.leftIncG = int(g[3] - g[0]) / width;
			m_colTrans.leftIncB = int(b[3] - b[0]) / width;
			m_colTrans.leftIncA = int(a[3] - a[0]) / width;

			m_colTrans.rightIncR = int(r[2] - r[1]) / width;
			m_colTrans.rightIncG = int(g[2] - g[1]) / width;
			m_colTrans.rightIncB = int(b[2] - b[1]) / width;
			m_colTrans.rightIncA = int(a[2] - a[1]) / width;

			diffMaskX |= (m_tintGradient.topLeft.argb - m_tintGradient.topRight.argb) | (m_tintGradient.bottomRight.argb - m_tintGradient.bottomLeft.argb);
			diffMaskY |= (m_tintGradient.topLeft.argb - m_tintGradient.bottomLeft.argb) | (m_tintGradient.topRight.argb - m_tintGradient.bottomRight.argb);

			// Calculate tintMode

			if( (diffMaskX | diffMaskY) == 0 )
			{
				if (m_tintGradient.topLeft.isOpaqueWhite() )
					tintMode = TintMode::None;
				else
				{
					m_colTrans.flatTintColor.r = r[0] >> 6;
					m_colTrans.flatTintColor.g = g[0] >> 6;
					m_colTrans.flatTintColor.b = b[0] >> 6;
					m_colTrans.flatTintColor.a = a[0] >> 6;
					tintMode = TintMode::Flat;
				}
			}
			else
			{
				if (diffMaskY == 0)
					tintMode = TintMode::GradientX;
				else if (diffMaskX == 0)
					tintMode = TintMode::GradientY;
				else
					tintMode = TintMode::GradientXY;
			}

			// Update m_bTintOpaque

			int value = int(m_tintColor.a) + int(m_tintGradient.topLeft.a) + int(m_tintGradient.topRight.a) 
						+ int(m_tintGradient.bottomRight.a) + int(m_tintGradient.bottomLeft.a);

			m_bTintOpaque = (value == 4096*5);
		}

		// Update tintMode and blitFunctions.

		if (tintMode != m_colTrans.mode)
		{
			m_colTrans.mode = tintMode;
			_updateBlitFunctions();
		}

	}

	//____ setBlendMode() _____________________________________________________

	bool SoftGfxDevice::setBlendMode(BlendMode blendMode)
	{
		if (blendMode == m_blendMode)
			return true;

		bool retVal = GfxDevice::setBlendMode(blendMode);
		if (retVal)
			_updateBlitFunctions();
		return retVal;
	}

	//____ setMorphFactor() ___________________________________________________

	void SoftGfxDevice::setMorphFactor(float factor)
	{
		limit(factor, 0.f, 1.f);
		m_morphFactor = factor;
		m_colTrans.morphFactor = int(factor * 4096);
	}

	//____ beginRender() _______________________________________________________

	bool SoftGfxDevice::beginRender()
	{
		if( m_bRendering )
			return false;

		setTintColor(Color::White);
		setBlendMode(BlendMode::Blend);

		m_bRendering = true;
		return true;
	}

	//____ endRender() _________________________________________________________

	bool SoftGfxDevice::endRender()
	{
		if( !m_bRendering )
			return false;

		m_bRendering = false;
		return true;
	}

	//____ fill() ______________________________________________________

	void SoftGfxDevice::fill(const RectSPX& rect, HiColor col)
	{
		if (!m_pRenderLayerSurface || !m_pCanvasPixels )
			return;

		// Clipping

		if( !m_clipBounds.intersectsWith(rect) )
			return;

		// Prepare colors


		// Skip calls that won't affect destination

		if ((col.a == 0 || m_tintColor.a == 0) && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract ) )
			return;

		// Optimize calls

		BlendMode blendMode = m_blendMode;
		if (blendMode == BlendMode::Blend && col.a == 4096 && m_bTintOpaque)
		{
			blendMode = BlendMode::Replace;
		}

		//

		if( ((rect.x | rect.y | rect.w | rect.h) & 0x3F) == 0 )
		{
			// No subpixel precision, make it quick and easy

			RectI pixelRect = roundToPixels(rect);

			int pixelBytes = m_canvasPixelBits / 8;
			FillOp_p pFunc = s_fillOpTab[(int)m_colTrans.mode][(int)blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
			if (pFunc == nullptr)
				return;

			for (int i = 0; i < m_nClipRects; i++)
			{
				RectI patch =  RectI::getIntersection(m_pClipRects[i]/64, pixelRect);
				if (patch.w == 0 || patch.h == 0)
					continue;

				uint8_t * pDst = m_pCanvasPixels + patch.y *m_canvasPitch + patch.x * pixelBytes;
				pFunc(pDst, pixelBytes, m_canvasPitch - patch.w*pixelBytes, patch.h, patch.w, col, m_colTrans, patch.pos());
			}
		}
		else
		{
					BlendMode	edgeBlendMode = (m_blendMode == BlendMode::Replace) ? BlendMode::Blend : m_blendMode; // Need to blend edges and corners even if fill is replace

			int pixelBytes = m_canvasPixelBits / 8;
			FillOp_p pFillOp = s_fillOpTab[(int)m_colTrans.mode][(int)blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
			FillOp_p pEdgeOp = s_fillOpTab[(int)m_colTrans.mode][(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];
	//		PlotOp_p pPlotOp = s_plotOpTab[(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];

			for (int i = 0; i < m_nClipRects; i++)
			{
				HiColor color = col;

				RectI  patch = RectI::getIntersection(rect, m_pClipRects[i]);
				if (patch.w == 0 || patch.h == 0)
					continue;

				// Fill all but anti-aliased edges

				int x1 = ((patch.x + 63) >> 6);
				int y1 = ((patch.y + 63) >> 6);
				int x2 = ((patch.x + patch.w) >> 6);
				int y2 = ((patch.y + patch.h) >> 6);


				uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x1 * pixelBytes;
				pFillOp(pDst, pixelBytes, m_canvasPitch - (x2 - x1)*pixelBytes, y2 - y1, x2 - x1, color, m_colTrans, { x1,y1 });

				// Draw the sides

				int aaLeft = 4096 - (patch.x & 0x3F) * 64;
				int aaTop = 4096 - (patch.y & 0x3F) * 64;
				int aaRight = ((patch.x + patch.w) & 0x3F) * 64;
				int aaBottom = ((patch.y + patch.h) & 0x3F) * 64;

				int aaTopLeft = aaTop * aaLeft / 4096;
				int aaTopRight = aaTop * aaRight / 4096;
				int aaBottomLeft = aaBottom * aaLeft / 4096;
				int aaBottomRight = aaBottom * aaRight / 4096;


				if (m_blendMode != BlendMode::Replace)
				{
					int alpha = color.a;

					aaLeft = aaLeft * alpha >> 12;
					aaTop = aaTop * alpha >> 12;
					aaRight = aaRight * alpha >> 12;
					aaBottom = aaBottom * alpha >> 12;

					aaTopLeft = aaTopLeft * alpha >> 12;
					aaTopRight = aaTopRight * alpha >> 12;
					aaBottomLeft = aaBottomLeft * alpha >> 12;
					aaBottomRight = aaBottomRight * alpha >> 12;
				}

				RectI pixelPatch = patch / 64;

				if (aaTop != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + pixelPatch.y * m_canvasPitch + x1 * pixelBytes;
					int length = x2 - x1;
					color.a = aaTop;
					pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,pixelPatch.y });
				}

				if (aaBottom != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x1 * pixelBytes;
					int length = x2 - x1;
					color.a = aaBottom;
					pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,y2 });
				}

				if (aaLeft != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + pixelPatch.x * pixelBytes;
					int length = y2 - y1;
					color.a = aaLeft;
					pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { pixelPatch.x, y1 });
				}

				if (aaRight != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x2 * pixelBytes;
					int length = y2 - y1;
					color.a = aaRight;
					pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { x2, y1 });
				}

				// Draw corner pieces


				if (aaTopLeft != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + pixelPatch.y * m_canvasPitch + pixelPatch.x * pixelBytes;
					color.a = aaTopLeft;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { pixelPatch.x, pixelPatch.y } );
	//				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, (int)patch.y } );
				}

				if (aaTopRight != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + pixelPatch.y * m_canvasPitch + x2 * pixelBytes;
					color.a = aaTopRight;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { x2, pixelPatch.y });
	//				pPlotOp(pDst, color, m_colTrans, { x2, (int)patch.y });
				}

				if (aaBottomLeft != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + pixelPatch.x * pixelBytes;
					color.a = aaBottomLeft;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { pixelPatch.x, y2 });
	//				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, y2 } );
				}

				if (aaBottomRight != 0)
				{
					uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x2 * pixelBytes;
					color.a = aaBottomRight;
					pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { x2, y2 });
	//				pPlotOp(pDst, color, m_colTrans, { x2,y2 });
				}
			}

		}


	}
	
	void SoftGfxDevice::fill(const RectF& rect, HiColor col)
	{
		if (!m_pRenderLayerSurface || !m_pCanvasPixels)
			return;

		// Clipping

		RectF clip = RectF::getIntersection(rect, RectF(m_clipBounds/64));
		if (clip.w == 0 || clip.h == 0)
			return;

		// Skip calls that won't affect destination

		if ((col.a == 0 || m_tintColor.a == 0) && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract))
			return;

		// Optimize calls

		BlendMode blendMode = m_blendMode;
		if (blendMode == BlendMode::Blend && col.a == 4096 && m_bTintOpaque)
		{
			blendMode = BlendMode::Replace;
		}


		BlendMode	edgeBlendMode = (m_blendMode == BlendMode::Replace) ? BlendMode::Blend : m_blendMode; // Need to blend edges and corners even if fill is replace

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p pFillOp = s_fillOpTab[(int)m_colTrans.mode][(int)blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		FillOp_p pEdgeOp = s_fillOpTab[(int)m_colTrans.mode][(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];
//		PlotOp_p pPlotOp = s_plotOpTab[(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];

		for (int i = 0; i < m_nClipRects; i++)
		{
			HiColor color = col;

			RectF  patch = RectF::getIntersection(rect, RectF(m_pClipRects[i]/64));
			if (patch.w == 0.f || patch.h == 0.f)
				continue;

			// Fill all but anti-aliased edges

			int x1 = (int)(patch.x + 0.999f);
			int y1 = (int)(patch.y + 0.999f);
			int x2 = (int)(patch.x + patch.w);
			int y2 = (int)(patch.y + patch.h);


			uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x1 * pixelBytes;
			pFillOp(pDst, pixelBytes, m_canvasPitch - (x2 - x1)*pixelBytes, y2 - y1, x2 - x1, color, m_colTrans, { x1,y1 });

			// Draw the sides

			int aaLeft = (4096 - (int)(patch.x * 4096)) & 0xFFF;
			int aaTop = (4096 - (int)(patch.y * 4096)) & 0xFFF;
			int aaRight = ((int)((patch.x + patch.w) * 4096)) & 0xFFF;
			int aaBottom = ((int)((patch.y + patch.h) * 4096)) & 0xFFF;

			int aaTopLeft = aaTop * aaLeft / 4096;
			int aaTopRight = aaTop * aaRight / 4096;
			int aaBottomLeft = aaBottom * aaLeft / 4096;
			int aaBottomRight = aaBottom * aaRight / 4096;


			if (m_blendMode != BlendMode::Replace)
			{
				int alpha = color.a;

				aaLeft = aaLeft * alpha >> 12;
				aaTop = aaTop * alpha >> 12;
				aaRight = aaRight * alpha >> 12;
				aaBottom = aaBottom * alpha >> 12;

				aaTopLeft = aaTopLeft * alpha >> 12;
				aaTopRight = aaTopRight * alpha >> 12;
				aaBottomLeft = aaBottomLeft * alpha >> 12;
				aaBottomRight = aaBottomRight * alpha >> 12;
			}


			if (aaTop != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + ((int)patch.y) * m_canvasPitch + x1 * pixelBytes;
				int length = x2 - x1;
				color.a = aaTop;
				pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,(int)patch.y });
			}

			if (aaBottom != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x1 * pixelBytes;
				int length = x2 - x1;
				color.a = aaBottom;
				pEdgeOp(pDst, pixelBytes, 0, 1, length, color, m_colTrans, { x1,y2 });
			}

			if (aaLeft != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + ((int)patch.x) * pixelBytes;
				int length = y2 - y1;
				color.a = aaLeft;
				pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { (int)patch.x, y1 });
			}

			if (aaRight != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y1 * m_canvasPitch + x2 * pixelBytes;
				int length = y2 - y1;
				color.a = aaRight;
				pEdgeOp(pDst, m_canvasPitch, 0, 1, length, color, m_colTrans, { x2, y1 });
			}

			// Draw corner pieces


			if (aaTopLeft != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + ((int)patch.y) * m_canvasPitch + ((int)patch.x) * pixelBytes;
				color.a = aaTopLeft;
				pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { (int)patch.x, (int)patch.y } );
//				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, (int)patch.y } );
			}

			if (aaTopRight != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + ((int)patch.y) * m_canvasPitch + x2 * pixelBytes;
				color.a = aaTopRight;
				pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { x2, (int)patch.y });
//				pPlotOp(pDst, color, m_colTrans, { x2, (int)patch.y });
			}

			if (aaBottomLeft != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + ((int)patch.x) * pixelBytes;
				color.a = aaBottomLeft;
				pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { (int)patch.x, y2 });
//				pPlotOp(pDst, color, m_colTrans, { (int)patch.x, y2 } );
			}

			if (aaBottomRight != 0)
			{
				uint8_t * pDst = m_pCanvasPixels + y2 * m_canvasPitch + x2 * pixelBytes;
				color.a = aaBottomRight;
				pEdgeOp(pDst, 0, 0, 1, 1, color, m_colTrans, { x2, y2 });
//				pPlotOp(pDst, color, m_colTrans, { x2,y2 });
			}
		}
	}

	//____ drawLine() ____ [from/to] __________________________________________

	void SoftGfxDevice::drawLine(CoordSPX beg, CoordSPX end, HiColor color, float thickness)
	{
		//TODO: Proper 26:6 support
		beg = roundToPixels(beg);
		end = roundToPixels(end);

		if (!m_pRenderLayerSurface || !m_pCanvasPixels)
			return;

		HiColor fillColor = color * m_tintColor;

		// Skip calls that won't affect destination

		if (fillColor.a == 0 && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract))
			return;

		//

		ClipLineOp_p pOp = s_clipLineOpTab[(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		if (pOp == nullptr)
			return;

		uint8_t *	pRow;
		int		rowInc, pixelInc;
		int 	length, width;
		int		pos, slope;
		int		clipStart, clipEnd;

		if (std::abs(beg.x - end.x) > std::abs(beg.y - end.y))
		{
			// Prepare mainly horizontal line segment

			if (beg.x > end.x)
				swap(beg, end);

			length = end.x - beg.x;
			slope = ((end.y - beg.y) << 16) / length;

			width = _scaleLineThickness(thickness, slope);
			pos = (beg.y << 16) - width / 2 + 32768;

			rowInc = m_canvasPixelBits / 8;
			pixelInc = m_canvasPitch;

			pRow = m_pCanvasPixels + beg.x * rowInc;

			// Loop through patches

			for( int i = 0 ; i < m_nClipRects ; i++ )
			{
				// Do clipping

				const RectI clip = m_pClipRects[i]/64;

				int _length = length;
				int _pos = pos;
				uint8_t * _pRow = pRow;

				if (beg.x < clip.x)
				{
					int cut = clip.x - beg.x;
					_length -= cut;
					_pRow += rowInc * cut;
					_pos += slope * cut;
				}

				if (end.x > clip.x + clip.w)
					_length -= end.x - (clip.x + clip.w);

				clipStart = clip.y << 16;
				clipEnd = (clip.y + clip.h) << 16;

				//  Draw

				pOp(clipStart, clipEnd, _pRow, rowInc, pixelInc, _length, width, _pos, slope, fillColor, m_colTrans, { 0,0 });
			}
		}
		else
		{
			// Prepare mainly vertical line segment

			if (beg.y > end.y)
				swap(beg, end);

			length = end.y - beg.y;
			if (length == 0)
				return;											// TODO: Should stil draw the caps!

			slope = ((end.x - beg.x) << 16) / length;
			width = _scaleLineThickness(thickness, slope);
			pos = (beg.x << 16) - width / 2 + 32768;

			rowInc = m_canvasPitch;
			pixelInc = m_canvasPixelBits / 8;

			pRow = m_pCanvasPixels + beg.y * rowInc;

			// Loop through patches

			for (int i = 0; i < m_nClipRects; i++)
			{
				// Do clipping

				const RectI clip = m_pClipRects[i]/64;

				int _length = length;
				int _pos = pos;
				uint8_t * _pRow = pRow;

				if (beg.y < clip.y)
				{
					int cut = clip.y - beg.y;
					_length -= cut;
					_pRow += rowInc * cut;
					_pos += slope * cut;
				}

				if (end.y > clip.y + clip.h)
					_length -= end.y - (clip.y + clip.h);

				clipStart = clip.x << 16;
				clipEnd = (clip.x + clip.w) << 16;

				//  Draw

				pOp(clipStart, clipEnd, _pRow, rowInc, pixelInc, _length, width, _pos, slope, fillColor, m_colTrans, { 0,0 });
			}
		}
	}

	//____ drawLine() ____ [start/direction] ________________________________

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void SoftGfxDevice::drawLine(CoordSPX _begin, Direction dir, spx _length, HiColor _col, float thickness)
	{
		//TODO: Proper 26:6 support
		_begin = roundToPixels(_begin);
		_length = roundToPixels(_length);

		//TODO: Optimize!

		if (thickness <= 0.f)
			return;

		BlendMode edgeBlendMode = m_blendMode;
		if (m_blendMode == BlendMode::Replace)
		{
			_col.a = 255;						// Needed since we still blend the edges.
			edgeBlendMode = BlendMode::Blend;
		}

		_col = _col * m_tintColor;

		int pixelBytes = m_canvasPixelBits / 8;
		FillOp_p	pCenterOp = s_fillOpTab[(int)TintMode::None][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		FillOp_p	pEdgeOp = s_fillOpTab[(int)TintMode::None][(int)edgeBlendMode][(int)m_pRenderLayerSurface->pixelFormat()];

		for (int i = 0; i < m_nClipRects; i++)
		{
			const RectI& clip = m_pClipRects[i]/64;

			CoordI begin = _begin;
			int length = _length;

			switch (dir)
			{
			case Direction::Left:
				begin.x -= length;
			case Direction::Right:
			{
				if (begin.x > clip.x + clip.w)
					continue;

				if (begin.x < clip.x)
				{
					length -= clip.x - begin.x;
					if (length <= 0)
						continue;
					begin.x = clip.x;
				}

				if (begin.x + length > clip.x + clip.w)
				{
					length = clip.x + clip.w - begin.x;
					if (length <= 0)
						continue;
				}

				if (thickness <= 1.f)
				{
					if (begin.y < clip.y || begin.y >= clip.y + clip.h)
						continue;

					HiColor col = _col;
					col.a = (int16_t)(thickness * col.a);

					uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
					pEdgeOp(pBegin, pixelBytes, 0, 1, length, col, m_colTrans, { 0,0 });
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					HiColor edgeColor(_col.r, _col.g, _col.b, (int16_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					if (begin.y + expanse <= clip.y || begin.y - expanse >= clip.y + clip.h)
						continue;

					int beginY = begin.y - expanse;
					int endY = begin.y + expanse + 1;

					if (beginY < clip.y)
						beginY = clip.y - 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + beginY * m_canvasPitch + begin.x * pixelBytes;
						pEdgeOp(pBegin, pixelBytes, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
						//					_drawStraightLine({ begin.x, beginY }, Axis::X, length, edgeColor);
					}

					if (endY > clip.y + clip.h)
						endY = clip.y + clip.h + 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + (endY - 1) * m_canvasPitch + begin.x * pixelBytes;
						pEdgeOp(pBegin, pixelBytes, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}

					int bodyThickness = endY - beginY - 2;
					uint8_t * pBegin = m_pCanvasPixels + (beginY + 1) * m_canvasPitch + begin.x * pixelBytes;
					pCenterOp(pBegin, pixelBytes, m_canvasPitch - length * pixelBytes, bodyThickness, length, _col, m_colTrans, { 0,0 });
				}

				break;
			}
			case Direction::Up:
				begin.y -= length;
			case Direction::Down:
				if (begin.y > clip.y + clip.h)
					continue;

				if (begin.y < clip.y)
				{
					length -= clip.y - begin.y;
					if (length <= 0)
						continue;
					begin.y = clip.y;
				}

				if (begin.y + length > clip.y + clip.h)
				{
					length = clip.y + clip.h - begin.y;
					if (length <= 0)
						continue;
				}

				if (thickness <= 1.f)
				{
					if (begin.x < clip.x || begin.x >= clip.x + clip.w)
						continue;

					HiColor col = _col;
					col.a = (int16_t)(thickness * col.a);

					uint8_t * pBegin = m_pCanvasPixels + begin.y *m_canvasPitch + begin.x * pixelBytes;
					pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, col, m_colTrans, { 0,0 });
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					HiColor edgeColor(_col.r, _col.g, _col.b, (int16_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					if (begin.x + expanse <= clip.x || begin.x - expanse >= clip.x + clip.w)
						continue;

					int beginX = begin.x - expanse;
					int endX = begin.x + expanse + 1;

					if (beginX < clip.x)
						beginX = clip.x - 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + beginX * pixelBytes;
						pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}

					if (endX > clip.x + clip.w)
						endX = clip.x + clip.w + 1;
					else
					{
						uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (endX - 1) * pixelBytes;
						pEdgeOp(pBegin, m_canvasPitch, 0, 1, length, edgeColor, m_colTrans, { 0,0 });
					}


					int bodyThickness = endX - beginX - 2;
					uint8_t * pBegin = m_pCanvasPixels + begin.y * m_canvasPitch + (beginX + 1) * pixelBytes;
					pCenterOp(pBegin, m_canvasPitch, pixelBytes - length * m_canvasPitch, bodyThickness, length, _col, m_colTrans, { 0,0 });
				}

				break;
			}
		}
	}

	//____ _transformDrawSegments() _________________________________________

	void SoftGfxDevice::_transformDrawSegments(const RectSPX& _destIn, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * _pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int _simpleTransform[2][2])
	{
		//TODO: Proper 26:6 support
		RectI _dest = roundToPixels(_destIn);

		RectI dest = _dest;

		SegmentEdge edges[c_maxSegments - 1];

		int xPitch = m_canvasPixelBits / 8;
		int yPitch = m_canvasPitch;

		// We need to modify our transform since we are moving the destination pointer, not the source pointer, according to the transform.

		int simpleTransform[2][2];

		simpleTransform[0][0] = _simpleTransform[0][0];
		simpleTransform[1][1] = _simpleTransform[1][1];

		if ((_simpleTransform[0][0] | _simpleTransform[1][1]) == 0)
		{
			simpleTransform[0][1] = _simpleTransform[1][0];
			simpleTransform[1][0] = _simpleTransform[0][1];
		}
		else
		{
			simpleTransform[0][1] = _simpleTransform[0][1];
			simpleTransform[1][0] = _simpleTransform[1][0];
		}

		// Calculate pitches

		int colPitch = simpleTransform[0][0] * xPitch + simpleTransform[0][1] * yPitch;
		int rowPitch = simpleTransform[1][0] * xPitch + simpleTransform[1][1] * yPitch;

		// Calculate start coordinate

		CoordI start = dest.pos();

		if (simpleTransform[0][0] + simpleTransform[1][0] < 0)
			start.x += dest.w - 1;

		if (simpleTransform[0][1] + simpleTransform[1][1] < 0)
			start.y += dest.h - 1;

		// Detect if strip columns are lined horizontally or verically

		bool bHorizontalColumns = (abs(colPitch) == xPitch);

		// Limit size of destination rect by number of edgestrips.

		if (bHorizontalColumns)
		{
			if (dest.w > nEdgeStrips - 1)
			{
				if (colPitch < 0)
					dest.x += dest.w - nEdgeStrips - 1;

				dest.w = nEdgeStrips - 1;
			}
		}
		else
		{
			if (dest.h > nEdgeStrips - 1)
			{
				if (colPitch < 0)
					dest.y += dest.h - nEdgeStrips - 1;

				dest.h = nEdgeStrips - 1;
			}
		}

		// Apply tinting

		int16_t	colors[c_maxSegments*4][4];				// RGBA order of elements
		bool	transparentSegments[c_maxSegments];
		bool	opaqueSegments[c_maxSegments];

		SegmentGradient* pGradientsY = nullptr;
		SegmentGradient* pGradientsX = nullptr;
		int		gradientsYBufferSize = 0;
		int		gradientsXBufferSize = 0;

		// Determine combined tint-mode

		bool	bTintFlat = false;			// Set if we tint in any way, flat, x, y, or xy.
		bool	bTintX = false;
		bool	bTintY = false;

		if (tintMode != TintMode::None)
		{
			bTintFlat = !m_tintColor.isOpaqueWhite();

			if (tintMode == TintMode::GradientXY || m_colTrans.mode == TintMode::GradientXY)
			{
				bTintX = bTintY = true;
			}
			else
			{
				if (tintMode == TintMode::GradientY || m_colTrans.mode == TintMode::GradientY)
					bTintY = true;
				if (tintMode == TintMode::GradientX || m_colTrans.mode == TintMode::GradientX)
					bTintX = true;
			}
		}

		// Unpack input colors and fill in transparentSegments

		if (!bTintX && !bTintY)
		{
			// If we just use flat tinting (or no tint at all), we tint our segment colors right away

			for (int i = 0; i < nSegments; i++)
			{
				colors[i][0] = (pSegmentColors[i].r * m_colTrans.flatTintColor.r) >> 12;
				colors[i][1] = (pSegmentColors[i].g * m_colTrans.flatTintColor.g) >> 12;
				colors[i][2] = (pSegmentColors[i].b * m_colTrans.flatTintColor.b) >> 12;
				colors[i][3] = (pSegmentColors[i].a * m_colTrans.flatTintColor.a) >> 12;

				transparentSegments[i] = (colors[i][3] == 0);
				opaqueSegments[i] = (colors[i][3] == 4096);
			}
		}
		else
		{
			int colorsPerSegment = bTintX && bTintY ? 4 : 2;

			for (int i = 0; i < nSegments*colorsPerSegment; i++)
			{
				colors[i][0] = pSegmentColors[i].r;
				colors[i][1] = pSegmentColors[i].g;
				colors[i][2] = pSegmentColors[i].b;
				colors[i][3] = pSegmentColors[i].a;
			}
		}


		// If we instead have gradients we have things to take care of...

		if (bTintX || bTintY)
		{
			// Generate the buffers that we will need

			if (bTintY)
			{
				gradientsYBufferSize = sizeof(SegmentGradient) * nSegments;
				pGradientsY = (SegmentGradient*)Base::memStackAlloc(gradientsYBufferSize);
			}

			if (bTintX)
			{
				// We use two gradients per segment in X, one for top and bottom of rectangle each.

				gradientsXBufferSize = sizeof(SegmentGradient) * nSegments * 2;
				pGradientsX = (SegmentGradient*)Base::memStackAlloc(gradientsXBufferSize);
			}


			// Calculate RGBA values from m_colTrans for our four corners.

			int		baseB[4], baseG[4], baseR[4], baseA[4];
			int		tempB[4], tempG[4], tempR[4], tempA[4];

			_colTransRect(tempB, tempG, tempR, tempA, _dest);

			// Rotate the colors of our four corners if we are flipped

			static const int cornerSwitchMap[2][2][2][4] = 
							{ {{{0,3,2,1},				// [ 0,-1,-1, 0] = Rot90FlipY
								{1,2,3,0}},				// [ 0,-1, 1, 0] = Rot90
								{{3,0,1,2},				// [ 0, 1,-1, 0] = Rot270
								{2,1,0,3}}},			// [ 0, 1, 1, 0] = Rot90FlipX
								{{{2,3,0,1},			// [-1, 0, 0,-1] = Rot180
								{1,0,3,2}},				// [-1, 0, 0, 1] = FlipX
								{{3,2,1,0},				// [ 1, 0, 0,-1] = FlipY
								{0,1,2,3}}} };			// [ 1, 0, 0, 1] = Normal

			int i1, i2, i3;
			if (_simpleTransform[0][0] == 0)
			{
				i1 = 0;
				i2 = _simpleTransform[0][1] == 1 ? 1 : 0;
				i3 = _simpleTransform[1][0] == 1 ? 1 : 0;
			}
			else
			{
				i1 = 1;
				i2 = _simpleTransform[0][0] == 1 ? 1 : 0;
				i3 = _simpleTransform[1][1] == 1 ? 1 : 0;
			}

			const int* pSwitch = cornerSwitchMap[i1][i2][i3];

			for (int i = 0; i < 4; i++)
			{
				int n = pSwitch[i];
				baseB[i] = tempB[n];
				baseG[i] = tempG[n];
				baseR[i] = tempR[n];
				baseA[i] = tempA[n];
			}

			// Lets process each segment

			for (int seg = 0; seg < nSegments; seg++)
			{

				int		segB[4], segG[4], segR[4], segA[4];

				// Calculate RGBA values for our four corners

				if (tintMode == TintMode::Flat)
				{
					for (int i = 0; i < 4; i++)
					{
						segR[i] = baseR[i] * colors[seg][0];
						segG[i] = baseG[i] * colors[seg][1];
						segB[i] = baseB[i] * colors[seg][2];
						segA[i] = baseA[i] * colors[seg][3];
					}
				}
				else
				{
					if (tintMode == TintMode::GradientX)
					{
						segR[0] = baseR[0] * colors[seg*2][0];
						segG[0] = baseG[0] * colors[seg * 2][1];
						segB[0] = baseB[0] * colors[seg * 2][2];
						segA[0] = baseA[0] * colors[seg * 2][3];

						segR[1] = baseR[1] * colors[seg * 2+1][0];
						segG[1] = baseG[1] * colors[seg * 2+1][1];
						segB[1] = baseB[1] * colors[seg * 2+1][2];
						segA[1] = baseA[1] * colors[seg * 2+1][3];

						segR[2] = baseR[2] * colors[seg * 2 + 1][0];
						segG[2] = baseG[2] * colors[seg * 2 + 1][1];
						segB[2] = baseB[2] * colors[seg * 2 + 1][2];
						segA[2] = baseA[2] * colors[seg * 2 + 1][3];

						segR[3] = baseR[3] * colors[seg * 2][0];
						segG[3] = baseG[3] * colors[seg * 2][1];
						segB[3] = baseB[3] * colors[seg * 2][2];
						segA[3] = baseA[3] * colors[seg * 2][3];
					}
					else if (tintMode == TintMode::GradientY)
					{
						segR[0] = baseR[0] * colors[seg * 2][0];
						segG[0] = baseG[0] * colors[seg * 2][1];
						segB[0] = baseB[0] * colors[seg * 2][2];
						segA[0] = baseA[0] * colors[seg * 2][3];

						segR[1] = baseR[1] * colors[seg * 2][0];
						segG[1] = baseG[1] * colors[seg * 2][1];
						segB[1] = baseB[1] * colors[seg * 2][2];
						segA[1] = baseA[1] * colors[seg * 2][3];

						segR[2] = baseR[2] * colors[seg * 2 + 1][0];
						segG[2] = baseG[2] * colors[seg * 2 + 1][1];
						segB[2] = baseB[2] * colors[seg * 2 + 1][2];
						segA[2] = baseA[2] * colors[seg * 2 + 1][3];

						segR[3] = baseR[3] * colors[seg * 2 + 1][0];
						segG[3] = baseG[3] * colors[seg * 2 + 1][1];
						segB[3] = baseB[3] * colors[seg * 2 + 1][2];
						segA[3] = baseA[3] * colors[seg * 2 + 1][3];
					}
					else
					{
						segR[0] = baseR[0] * colors[seg * 4][0];
						segG[0] = baseG[0] * colors[seg * 4][1];
						segB[0] = baseB[0] * colors[seg * 4][2];
						segA[0] = baseA[0] * colors[seg * 4][3];

						segR[1] = baseR[1] * colors[seg * 4 + 1][0];
						segG[1] = baseG[1] * colors[seg * 4 + 1][1];
						segB[1] = baseB[1] * colors[seg * 4 + 1][2];
						segA[1] = baseA[1] * colors[seg * 4 + 1][3];

						segR[2] = baseR[2] * colors[seg * 4 + 2][0];
						segG[2] = baseG[2] * colors[seg * 4 + 2][1];
						segB[2] = baseB[2] * colors[seg * 4 + 2][2];
						segA[2] = baseA[2] * colors[seg * 4 + 2][3];

						segR[3] = baseR[3] * colors[seg * 4 + 3][0];
						segG[3] = baseG[3] * colors[seg * 4 + 3][1];
						segB[3] = baseB[3] * colors[seg * 4 + 3][2];
						segA[3] = baseA[3] * colors[seg * 4 + 3][3];
					}
				}

				// We now have the segments corner colors. Let's save
				// that information in the correct format for future
				// processing depending on tint mode.

				// Filling in the x-gradient if we have one. Two SegmentGradient structs for
				// each segment, one for gradient along top and of dest rectangle and one along the bottom.

				if (bTintX)
				{
					// Fill in top gradient

					auto p = &pGradientsX[seg * 2];

					p->begB = segB[0];
					p->begG = segG[0];
					p->begR = segR[0];
					p->begA = segA[0];

					p->incB = (segB[1] - segB[0]) / dest.w;
					p->incG = (segG[1] - segG[0]) / dest.w;
					p->incR = (segR[1] - segR[0]) / dest.w;
					p->incA = (segA[1] - segA[0]) / dest.w;

					// Fill in bottom gradient

					p++;

					p->begB = segB[3];
					p->begG = segG[3];
					p->begR = segR[3];
					p->begA = segA[3];

					p->incB = (segB[2] - segB[3]) / dest.w;
					p->incG = (segG[2] - segG[3]) / dest.w;
					p->incR = (segR[2] - segR[3]) / dest.w;
					p->incA = (segA[2] - segA[3]) / dest.w;
				}

				// If we don't have any x-gradient we can fill in y-gradient once and for all

				if (bTintY && !bTintX)
				{
					auto p = &pGradientsY[seg];

					p->begB = segB[0];
					p->begG = segG[0];
					p->begR = segR[0];
					p->begA = segA[0];

					p->incB = (segB[3] - segB[0]) / dest.h;
					p->incG = (segG[3] - segG[0]) / dest.h;
					p->incR = (segR[3] - segR[0]) / dest.h;
					p->incA = (segA[3] - segA[0]) / dest.h;
				}

				// Possibly mark this segment as transparent

				transparentSegments[seg] = (segA[0] + segA[1] + segA[2] + segA[3] == 0);
				opaqueSegments[seg] = (segA[0] + segA[1] + segA[2] + segA[3] == 4096*4096*4);
			}
		}

		// Modify opaqueSegments if our state isn't blend
		
		if( m_blendMode != BlendMode::Blend )
		{
			bool val = (m_blendMode == BlendMode::Replace);
			
			for (int seg = 0; seg < nSegments; seg++)
				opaqueSegments[seg] = val;
		}
		
		
		// Set start position and clip dest

		uint8_t* pOrigo = m_pCanvasPixels + start.y * yPitch + start.x * xPitch;

		if (!dest.intersectsWith(m_clipBounds/64))
			return;

		SegmentOp_p	pOp = s_segmentOpTab[(int)bTintY][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		if (pOp == nullptr)
			return;

		// Loop through patches

		for (int patchIdx = 0; patchIdx < m_nClipRects; patchIdx++)
		{
			// Clip patch

			RectI patch = RectI::getIntersection(dest, m_pClipRects[patchIdx]/64);
			if (patch.w == 0 || patch.h == 0)
				continue;

			// Calculate stripstart, clipBeg/clipEnd and first edge for patch

			int columnOfs;
			int rowOfs;

			int columns;
			int rows;

			if (bHorizontalColumns)
			{
				columnOfs = colPitch > 0 ? patch.x - dest.x : dest.right() - patch.right();
				rowOfs = rowPitch > 0 ? patch.y - dest.y : dest.bottom() - patch.bottom();

				columns = patch.w;
				rows = patch.h;
			}
			else
			{
				columnOfs = colPitch > 0 ? patch.y - dest.y : dest.bottom() - patch.bottom();
				rowOfs = rowPitch > 0 ? patch.x - dest.x : dest.right() - patch.right();

				columns = patch.h;
				rows = patch.w;
			}

			const int* pEdgeStrips = _pEdgeStrips + columnOfs * edgeStripPitch;
			uint8_t* pStripStart = pOrigo + columnOfs * colPitch;

			int clipBeg = rowOfs * 256;
			int clipEnd = clipBeg + (rows * 256);

			for (int x = 0; x < columns; x++)
			{
				int nEdges = 0;
				int skippedSegments = 0;

				for (int y = 0; y < nSegments - 1; y++)
				{
					int beg = pEdgeStrips[y];
					int end = pEdgeStrips[y + edgeStripPitch];

					if (beg > end)
						swap(beg, end);

					if (beg >= clipEnd)
						break;

					if (end > clipBeg)
					{
						int coverageInc = (end == beg) ? 0 : (65536 * 256) / (end - beg);
						int coverage = 0;

						if (beg < clipBeg)
						{
							int cut = clipBeg - beg;
							beg = clipBeg;
							coverage += (coverageInc * cut) >> 8;
						}

						if (end > clipEnd)
							end = clipEnd;


						edges[nEdges].begin = beg;
						edges[nEdges].end = end;
						edges[nEdges].coverage = coverage;
						edges[nEdges].coverageInc = coverageInc;
						nEdges++;
					}
					else
						skippedSegments++;
				}

				// Update tinting if we have X-gradient

				const int16_t * pColors = &colors[skippedSegments][0];

				if (bTintX)
				{
					if (bTintY)
					{
						auto pOut = pGradientsY + skippedSegments;
						auto pIn = pGradientsX + skippedSegments * 2;

						for (int i = 0; i < nEdges + 1; i++)
						{
							int beg = pIn[0].begB + pIn[0].incB * columnOfs;
							int inc = (pIn[1].begB + pIn[1].incB * columnOfs - beg) / dest.h;
							pOut[i].begB = beg;
							pOut[i].incB = inc;

							beg = pIn[0].begG + pIn[0].incG * columnOfs;
							inc = (pIn[1].begG + pIn[1].incG * columnOfs - beg) / dest.h;
							pOut[i].begG = beg;
							pOut[i].incG = inc;

							beg = pIn[0].begR + pIn[0].incR * columnOfs;
							inc = (pIn[1].begR + pIn[1].incR * columnOfs - beg) / dest.h;
							pOut[i].begR = beg;
							pOut[i].incR = inc;

							beg = pIn[0].begA + pIn[0].incA * columnOfs;
							inc = (pIn[1].begA + pIn[1].incA * columnOfs - beg) / dest.h;
							pOut[i].begA = beg;
							pOut[i].incA = inc;

							pIn += 2;
						}
					}
					else
					{
						// We just use the color array and fill in correct color values for this column.

						auto pGrad = pGradientsX + skippedSegments * 2;

						for (int i = 0; i < nEdges + 1; i++)
						{
							colors[i][0] = (pGrad->begR + pGrad->incR * columnOfs) >> 12;
							colors[i][1] = (pGrad->begG + pGrad->incG * columnOfs) >> 12;
							colors[i][2] = (pGrad->begB + pGrad->incB * columnOfs) >> 12;
							colors[i][3] = (pGrad->begA + pGrad->incA * columnOfs) >> 12;
							pGrad += 2;												// Skipping bottom gradient data since we don't need it.
						}

						pColors = &colors[0][0];
					}
				}

				//

				pOp(clipBeg, clipEnd, pStripStart, rowPitch, nEdges, edges, pColors, pGradientsY + skippedSegments, transparentSegments + skippedSegments, opaqueSegments + skippedSegments, int(m_morphFactor*4096));
				pEdgeStrips += edgeStripPitch;
				pStripStart += colPitch;
				columnOfs++;
			}
		}

		// Free what we have reserved on the memStack.

		if (gradientsXBufferSize > 0)
			Base::memStackRelease(gradientsXBufferSize);

		if (gradientsYBufferSize > 0)
			Base::memStackRelease(gradientsYBufferSize);

	}




	//____ _colTransRect() _____________________________________________________

	void SoftGfxDevice::_colTransRect( int outB[4], int outG[4], int outR[4], int outA[], const RectI& dest)
	{
		// Then we tint with our out tint

		if (m_colTrans.mode == TintMode::None)
		{
			for (int i = 0; i < 4; i++)
			{
				outB[i] = 0x1000;
				outG[i] = 0x1000;
				outR[i] = 0x1000;
				outA[i] = 0x1000;
			}
		}
		else if (m_colTrans.mode == TintMode::Flat)
		{
			int r = m_colTrans.flatTintColor.r;
			int g = m_colTrans.flatTintColor.g;
			int b = m_colTrans.flatTintColor.b;
			int a = m_colTrans.flatTintColor.a;

			for (int i = 0; i < 4; i++)
			{
				outB[i] = b;
				outG[i] = g;
				outR[i] = r;
				outA[i] = a;
			}
		}
		else if (m_colTrans.mode == TintMode::GradientX)
		{
			int ofsX = dest.x - m_colTrans.tintRect.x;
			int xIncB = int(m_colTrans.topRightB - m_colTrans.topLeftB) / m_colTrans.tintRect.w;

			int leftB = (m_colTrans.topLeftB + xIncB * ofsX) >> 6;
			int rightB = (m_colTrans.topLeftB + xIncB * (ofsX + dest.w)) >> 6;

			outB[0] = leftB;
			outB[1] = rightB;
			outB[2] = rightB;
			outB[3] = leftB;

			//

			int xIncG = int(m_colTrans.topRightG - m_colTrans.topLeftG) / m_colTrans.tintRect.w;

			int leftG = (m_colTrans.topLeftG + xIncG * ofsX) >> 6;
			int rightG = (m_colTrans.topLeftG + xIncG * (ofsX + dest.w)) >> 6;

			outG[0] = leftG;
			outG[1] = rightG;
			outG[2] = rightG;
			outG[3] = leftG;

			//

			int xIncR = int(m_colTrans.topRightR - m_colTrans.topLeftR) / m_colTrans.tintRect.w;

			int leftR = (m_colTrans.topLeftR + xIncR * ofsX) >> 6;
			int rightR = (m_colTrans.topLeftR + xIncR * (ofsX + dest.w)) >> 6;

			outR[0] = leftR;
			outR[1] = rightR;
			outR[2] = rightR;
			outR[3] = leftR;

			//

			int xIncA = int(m_colTrans.topRightA - m_colTrans.topLeftA) / m_colTrans.tintRect.w;

			int leftA = (m_colTrans.topLeftA + xIncA * ofsX) >> 6;
			int rightA = (m_colTrans.topLeftA + xIncA * (ofsX + dest.w)) >> 6;

			outA[0] = leftA;
			outA[1] = rightA;
			outA[2] = rightA;
			outA[3] = leftA;

		}
		else if (m_colTrans.mode == TintMode::GradientY)
		{
			int ofsY = dest.y - m_colTrans.tintRect.y;

			int topB = (m_colTrans.topLeftB + m_colTrans.leftIncB * ofsY) >> 6;
			int bottomB = (m_colTrans.topLeftB + m_colTrans.leftIncB * (ofsY + dest.h)) >> 6;

			outB[0] = topB;
			outB[1] = topB;
			outB[2] = bottomB;
			outB[3] = bottomB;

			//

			int topG = (m_colTrans.topLeftG + m_colTrans.leftIncG * ofsY) >> 6;
			int bottomG = (m_colTrans.topLeftG + m_colTrans.leftIncG * (ofsY + dest.h)) >> 6;

			outG[0] = topG;
			outG[1] = topG;
			outG[2] = bottomG;
			outG[3] = bottomG;

			//

			int topR = (m_colTrans.topLeftR + m_colTrans.leftIncR * ofsY) >> 6;
			int bottomR = (m_colTrans.topLeftR + m_colTrans.leftIncR * (ofsY + dest.h)) >> 6;

			outR[0] = topR;
			outR[1] = topR;
			outR[2] = bottomR;
			outR[3] = bottomR;

			//

			int topA = (m_colTrans.topLeftA + m_colTrans.leftIncA * ofsY) >> 6;
			int bottomA = (m_colTrans.topLeftA + m_colTrans.leftIncA * (ofsY + dest.h)) >> 6;

			outA[0] = topA;
			outA[1] = topA;
			outA[2] = bottomA;
			outA[3] = bottomA;
		}
		else if (m_colTrans.mode == TintMode::GradientXY)
		{
			CoordI ofs = dest.pos() - m_colTrans.tintRect.pos();

			int begB = m_colTrans.topLeftB + m_colTrans.leftIncB * ofs.y;
			int xIncB = (m_colTrans.topRightB + m_colTrans.rightIncB * ofs.y - begB) / m_colTrans.tintRect.w;

			outB[0] = (begB + xIncB * ofs.x) >> 6;
			outB[1] = (begB + xIncB * (ofs.x + dest.w)) >> 6;

			begB = m_colTrans.topLeftB + m_colTrans.leftIncB * (ofs.y + dest.h);
			xIncB = (m_colTrans.topRightB + m_colTrans.rightIncB * (ofs.y + dest.h) - begB) / m_colTrans.tintRect.w;

			outB[2] = (begB + xIncB * (ofs.x + dest.w)) >> 6;
			outB[3] = (begB + xIncB * ofs.x) >> 6;

			//

			int begG = m_colTrans.topLeftG + m_colTrans.leftIncG * ofs.y;
			int xIncG = (m_colTrans.topRightG + m_colTrans.rightIncG * ofs.y - begG) / m_colTrans.tintRect.w;

			outG[0] = (begG + xIncG * ofs.x) >> 6;
			outG[1] = (begG + xIncG * (ofs.x + dest.w)) >> 6;

			begG = m_colTrans.topLeftG + m_colTrans.leftIncG * (ofs.y + dest.h);
			xIncG = (m_colTrans.topRightG + m_colTrans.rightIncG * (ofs.y + dest.h) - begG) / m_colTrans.tintRect.w;

			outG[2] = (begG + xIncG * (ofs.x + dest.w)) >> 6;
			outG[3] = (begG + xIncG * ofs.x) >> 6;

			//

			int begR = m_colTrans.topLeftR + m_colTrans.leftIncR * ofs.y;
			int xIncR = (m_colTrans.topRightR + m_colTrans.rightIncR * ofs.y - begR) / m_colTrans.tintRect.w;

			outR[0] = (begR + xIncR * ofs.x) >> 6;
			outR[1] = (begR + xIncR * (ofs.x + dest.w)) >> 6;

			begR = m_colTrans.topLeftR + m_colTrans.leftIncR * (ofs.y + dest.h);
			xIncR = (m_colTrans.topRightR + m_colTrans.rightIncR * (ofs.y + dest.h) - begR) / m_colTrans.tintRect.w;

			outR[2] = (begR + xIncR * (ofs.x + dest.w)) >> 6;
			outR[3] = (begR + xIncR * ofs.x) >> 6;

			//

			int begA = m_colTrans.topLeftA + m_colTrans.leftIncA * ofs.y;
			int xIncA = (m_colTrans.topRightA + m_colTrans.rightIncA * ofs.y - begA) / m_colTrans.tintRect.w;

			outA[0] = (begA + xIncA * ofs.x) >> 6;
			outA[1] = (begA + xIncA * (ofs.x + dest.w)) >> 6;

			begA = m_colTrans.topLeftA + m_colTrans.leftIncA * (ofs.y + dest.h);
			xIncA = (m_colTrans.topRightA + m_colTrans.rightIncA * (ofs.y + dest.h) - begA) / m_colTrans.tintRect.w;

			outA[2] = (begA + xIncA * (ofs.x + dest.w)) >> 6;
			outA[3] = (begA + xIncA * ofs.x) >> 6;
		}
	}




	//____ _lineToEdges() __________________________________________________________

	void SoftGfxDevice::_lineToEdges(const WaveLine * pWave, int offset, int nPoints, SegmentEdge * pDest, int pitch )
	{
		static int brush[128];
		static float prevThickness = -1.f;

		float thickness = pWave->thickness;
		int brushSteps = (int)(thickness / 2 + 0.99f);

		// Generate brush

		if (thickness != prevThickness)
		{
			int scaledThickness = (int)(thickness / 2 * 256);

			brush[0] = scaledThickness;
			for (int i = 1; i < brushSteps; i++)
			{
				brush[i] = (scaledThickness * s_pCurveTab[c_nCurveTabEntries - (i*c_nCurveTabEntries) / brushSteps - 1]) >> 16;
				//				printf( "%d - %d - %d\n", i, brush[i], m_pCurveTab[0][(c_nCurveTabEntries - 1) - (i * c_nCurveTabEntries) / brushSteps]);
			}
			prevThickness = thickness;
		}

		int nTracePoints = max(0, min(nPoints, pWave->length - offset));
		int nFillPoints = nPoints - nTracePoints;

		// Trace...

		int * pSrc = pWave->pWave + offset;
		SegmentEdge * pWrite = pDest;
		for (int i = 0; i < nTracePoints; i++)
		{
			// Start with top and bottom for current point

			int top = pSrc[i] - brush[0];
			int bottom = pSrc[i] + brush[0];

			// Check brush's coverage from previous points

			int end = min(i + 1, brushSteps);

			for (int j = 1; j < end; j++)
			{
				int topCover = pSrc[i - j] - brush[j];
				int bottomCover = pSrc[i - j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Check brush's coverage from following points

			end = min(nPoints - i, brushSteps);

			for (int j = 1; j < end; j++)
			{
				int topCover = pSrc[i + j] - brush[j];
				int bottomCover = pSrc[i + j] + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Save traced values

			pWrite[0].begin = top;
			pWrite[1].begin = bottom;


			pWrite += pitch;

		}

		// Fill out the edge specifications if we didn't have enough wave data

		if (nFillPoints)
		{
			int top = pWave->hold - brush[0];
			int bottom = pWave->hold + brush[0];

			for (int i = 0; i < nFillPoints; i++)
			{
				pWrite[0].begin = top;
				pWrite[0].end = top;
				pWrite[0].coverage = 0;
				pWrite[0].coverageInc = 0;

				pWrite[1].begin = bottom;
				pWrite[1].end = bottom;
				pWrite[1].coverage = 0;
				pWrite[1].coverageInc = 0;

				pWrite += pitch;
			}
		}

		// Calculate rest of the edge data for our traced points (not needed for the filled points)

		int nEdges = nFillPoints > 0 ? nTracePoints : nTracePoints-1;

		pWrite = pDest;
		for (int i = 0; i < nEdges; i++)
		{
			{
				int edgeBegin = pWrite[0].begin;
				int edgeEnd = pWrite[pitch].begin;
				int coverageStart = 0;

				if (edgeBegin > edgeEnd)
					swap(edgeBegin, edgeEnd);

				int coverageInc = (edgeEnd == edgeBegin) ? 0 : (65536 * 256) / (edgeEnd - edgeBegin);


				pWrite[0].begin = edgeBegin;
				pWrite[0].end = edgeEnd;
				pWrite[0].coverage = coverageStart;
				pWrite[0].coverageInc = coverageInc;
			}

			{
				int edgeBegin = pWrite[1].begin;
				int edgeEnd = pWrite[1+pitch].begin;
				if (edgeBegin > edgeEnd)
					swap(edgeBegin, edgeEnd);

				int coverageInc = (edgeEnd == edgeBegin) ? 0 : (65536 * 256) / (edgeEnd - edgeBegin);

				int coverageStart = 0;

				pWrite[1].begin = edgeBegin;
				pWrite[1].end = edgeEnd;
				pWrite[1].coverage = coverageStart;
				pWrite[1].coverageInc = coverageInc;
			}

			pWrite += pitch;
		}
	}

	//____ plotPixels() _________________________________________________

	void SoftGfxDevice::plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
	{
		const int pitch = m_canvasPitch;
		const int pixelBytes = m_canvasPixelBits / 8;

		PlotListOp_p pOp = s_plotListOpTab[(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];

		if (pOp == nullptr )
			return;

		for (int i = 0; i < m_nClipRects; i++)
			pOp(m_pClipRects[i], nCoords, pCoords, pColors, m_pCanvasPixels, pixelBytes, pitch, m_colTrans);
	}

	//____ setBlitSource() ____________________________________________________

	bool SoftGfxDevice::setBlitSource(Surface * pSource)
	{
		if (pSource == m_pBlitSource)
			return true;

		if (!pSource || !pSource->isInstanceOf(SoftSurface::TYPEINFO))
		{
			m_pBlitSource = nullptr;
			GfxDevice::m_pBlitSource = nullptr;
			return false;
		}

		SoftSurface * pSrcSurf = (SoftSurface*)pSource;

		if (!pSrcSurf->m_pData)
		{
			m_pBlitSource = nullptr;
			GfxDevice::m_pBlitSource = nullptr;
			return false;
		}

		m_pBlitSource = pSrcSurf;
		GfxDevice::m_pBlitSource = pSource;

		_updateBlitFunctions();
		return true;
	}

	//____ _rotScaleBlit() ____________________________________________________

	void SoftGfxDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter )
	{
		if (m_pBlitSource->isTiling())
		{
			auto oldOnePass = m_pComplexBlitOnePassOp;
			auto oldFirstPass = m_pComplexBlitFirstPassOp;
			m_pComplexBlitOnePassOp = m_pComplexTileBlitOnePassOp;
			m_pComplexBlitFirstPassOp = m_pComplexTileBlitFirstPassOp;
			GfxDevice::rotScaleBlit(dest, rotationDegrees, scale, srcCenter, destCenter);
			m_pComplexBlitOnePassOp = oldOnePass;
			m_pComplexBlitFirstPassOp = oldFirstPass;
		}
		else
		{
			m_bClipSource = true;
			GfxDevice::rotScaleBlit(dest, rotationDegrees, scale, srcCenter, destCenter);
			m_bClipSource = false;
		}
	}

	//____ tile() _____________________________________________________________

	void SoftGfxDevice::tile(const RectSPX& dest, CoordSPX shift)
	{
		auto oldOnePass = m_pSimpleBlitOnePassOp;
		auto oldFirstPass = m_pSimpleBlitFirstPassOp;
		m_pSimpleBlitOnePassOp = m_pSimpleTileBlitOnePassOp;
		m_pSimpleBlitFirstPassOp = m_pSimpleTileBlitFirstPassOp;
		GfxDevice::tile(dest, shift);
		m_pSimpleBlitOnePassOp = oldOnePass;
		m_pSimpleBlitFirstPassOp = oldFirstPass;
	}

	//____ flipTile() _________________________________________________________

	void SoftGfxDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
	{
		auto oldOnePass = m_pSimpleBlitOnePassOp;
		auto oldFirstPass = m_pSimpleBlitFirstPassOp;
		m_pSimpleBlitOnePassOp = m_pSimpleTileBlitOnePassOp;
		m_pSimpleBlitFirstPassOp = m_pSimpleTileBlitFirstPassOp;
		GfxDevice::flipTile(dest, flip, shift);
		m_pSimpleBlitOnePassOp = oldOnePass;
		m_pSimpleBlitFirstPassOp = oldFirstPass;
	}

	//____ scaleTile() ________________________________________________________

	void SoftGfxDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
	{
		auto oldOnePass = m_pComplexBlitOnePassOp;
		auto oldFirstPass = m_pComplexBlitFirstPassOp;
		m_pComplexBlitOnePassOp = m_pComplexTileBlitOnePassOp;
		m_pComplexBlitFirstPassOp = m_pComplexTileBlitFirstPassOp;
		GfxDevice::scaleTile(dest, scale, shift);
		m_pComplexBlitOnePassOp = oldOnePass;
		m_pComplexBlitFirstPassOp = oldFirstPass;
	}

	//____ scaleFlipTile() ____________________________________________________

	void SoftGfxDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
	{
		auto oldOnePass = m_pComplexBlitOnePassOp;
		auto oldFirstPass = m_pComplexBlitFirstPassOp;
		m_pComplexBlitOnePassOp = m_pComplexTileBlitOnePassOp;
		m_pComplexBlitFirstPassOp = m_pComplexTileBlitFirstPassOp;
		GfxDevice::scaleFlipTile(dest, scale, flip, shift);
		m_pComplexBlitOnePassOp = oldOnePass;
		m_pComplexBlitFirstPassOp = oldFirstPass;
	}

	//____ _canvasWasChanged() ________________________________________________

	void SoftGfxDevice::_canvasWasChanged()
	{
		Surface * pCanvasSurface = nullptr;
		
		if(m_canvas.pSurface)
			pCanvasSurface = m_canvas.pSurface;
		else if(m_canvas.ref != CanvasRef::None)
		{
			CanvasRef ref = m_canvas.ref;
			
			auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );
			
			if( it != m_definedCanvases.end() )
				pCanvasSurface = it->pSurface;
		}
		
		
		if (pCanvasSurface)
		{
            m_layerSurfaces[0] = pCanvasSurface;
			_renderLayerWasChanged();
		}
		else
		{
			// Clean up.

			if (m_pRenderLayerSurface)
			{
				m_pRenderLayerSurface->pullPixels(m_canvasPixelBuffer);
				m_pRenderLayerSurface->freePixelBuffer(m_canvasPixelBuffer);
			}

			m_pRenderLayerSurface = nullptr;
			m_pCanvasPixels = nullptr;
			m_canvasPixelBits = 0;
			m_canvasPitch = 0;

			_updateBlitFunctions();		// Good to have dummies in place when we are not allowed to blit.
		}
	}

	//____ _renderLayerWasChanged() ___________________________________________

	void SoftGfxDevice::_renderLayerWasChanged()
	{
		// Finish up previous render layer surface

		if (m_pRenderLayerSurface)
		{
			m_pRenderLayerSurface->pullPixels(m_canvasPixelBuffer);
			m_pRenderLayerSurface->freePixelBuffer(m_canvasPixelBuffer);
		}

		// Create our layer surface if needed

		bool	bClear = false;
		if (m_renderLayer > 0 && m_layerSurfaces[m_renderLayer] == nullptr)
		{
			m_layerSurfaces[m_renderLayer] = SoftSurface::create(m_canvas.size/64, m_pCanvasLayers->layerFormat(m_renderLayer));
			bClear = true;
		}

		// 

		m_pRenderLayerSurface = m_layerSurfaces[m_renderLayer];

		m_canvasPixelBuffer = m_pRenderLayerSurface->allocPixelBuffer();
		m_pRenderLayerSurface->pushPixels(m_canvasPixelBuffer);

		m_pCanvasPixels = m_canvasPixelBuffer.pPixels;
		m_canvasPitch = m_canvasPixelBuffer.pitch;

		m_canvasPixelBits = m_pRenderLayerSurface->pixelDescription()->bits;

		_updateTintSettings();
		_updateBlitFunctions();

		if (bClear)
			_clearRenderLayer();
	}

	//____ _transformBlit() [simple] ____________________________________

	void SoftGfxDevice::_transformBlit(const RectSPX& _dest, CoordSPX _src, const int simpleTransform[2][2])
	{
		// For this method, source and dest should be pixel aligned.

		// Clip and render the patches

		if (!_dest.intersectsWith(m_clipBounds))
			return;


		//TODO: Proper 26:6 support
		RectI dest = roundToPixels(_dest);

		const RectI& clip = dest;

		// Step forward _src by half a pixel, so we start from correct pixel.

		_src.x = _src.x + (simpleTransform[0][0] + simpleTransform[1][0])*32;
		_src.y = _src.y + (simpleTransform[0][1] + simpleTransform[1][1])*32;

		//

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI  patch = m_pClipRects[i]/64;

			CoordI src = _src/64;

			CoordI	patchOfs = patch.pos() - dest.pos();


			if ((clip.x > patch.x) || (clip.x + clip.w < patch.x + patch.w) ||
				(clip.y > patch.y) || (clip.y + clip.h < patch.y + patch.h))
			{

				if ((clip.x > patch.x + patch.w) || (clip.x + clip.w < patch.x) ||
					(clip.y > patch.y + patch.h) || (clip.y + clip.h < patch.y))
					continue;																					// Totally outside clip-rect.

				if (patch.x < clip.x)
				{
					int xDiff = clip.x - patch.x;
					patch.w -= xDiff;
					patch.x = clip.x;
					patchOfs.x += xDiff;
				}

				if (patch.y < clip.y)
				{
					int yDiff = clip.y - patch.y;
					patch.h -= yDiff;
					patch.y = clip.y;
					patchOfs.y += yDiff;
				}

				if (patch.x + patch.w > clip.x + clip.w)
					patch.w = (clip.x + clip.w) - patch.x;

				if (patch.y + patch.h > clip.y + clip.h)
					patch.h = (clip.y + clip.h) - patch.y;
			}

			//

			src.x += patchOfs.x * simpleTransform[0][0] + patchOfs.y * simpleTransform[1][0];
			src.y += patchOfs.x * simpleTransform[0][1] + patchOfs.y * simpleTransform[1][1];

			(this->*m_pSimpleBlitOp)(patch, src, simpleTransform, patch.pos());
		}
	}

	//____ _transformBlit() [complex] ____________________________________

	void SoftGfxDevice::_transformBlit(const RectSPX& _dest, CoordF _src, const float complexTransform[2][2])
	{
		// Clip and render the patches

		if (!_dest.intersectsWith(m_clipBounds))
			return;

		//TODO: Proper 26:6 support
		RectI dest = roundToPixels(_dest);

		const RectI& clip = dest;

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI  patch = m_pClipRects[i]/64;

			CoordF src = CoordF(_src)/64;

			CoordI	patchOfs = patch.pos() - dest.pos();


			if ((clip.x > patch.x) || (clip.x + clip.w < patch.x + patch.w) ||
				(clip.y > patch.y) || (clip.y + clip.h < patch.y + patch.h))
			{

				if ((clip.x > patch.x + patch.w) || (clip.x + clip.w < patch.x) ||
					(clip.y > patch.y + patch.h) || (clip.y + clip.h < patch.y))
					continue;																					// Totally outside clip-rect.

				if (patch.x < clip.x)
				{
					int xDiff = clip.x - patch.x;
					patch.w -= xDiff;
					patch.x = clip.x;
					patchOfs.x += xDiff;
				}

				if (patch.y < clip.y)
				{
					int yDiff = clip.y - patch.y;
					patch.h -= yDiff;
					patch.y = clip.y;
					patchOfs.y += yDiff;
				}

				if (patch.x + patch.w > clip.x + clip.w)
					patch.w = (clip.x + clip.w) - patch.x;

				if (patch.y + patch.h > clip.y + clip.h)
					patch.h = (clip.y + clip.h) - patch.y;
			}

			//

			src.x += patchOfs.x * complexTransform[0][0] + patchOfs.y * complexTransform[1][0];
			src.y += patchOfs.x * complexTransform[0][1] + patchOfs.y * complexTransform[1][1];

			// See if we can skip the expensive source clipping....

			bool bClipSource = false;
			if (m_bClipSource)
			{
				SizeF clipMax = static_cast<SizeF>(m_pBlitSource->m_size);

				if (m_pBlitSource->m_sampleMethod == SampleMethod::Bilinear)
					clipMax -= SizeF(1.f, 1.f);

				CoordF src2 = src;		// Source pixel to read for top-right destination corner.
				CoordF src3 = src;		// Source pixel to read for bottom-right destination corner.
				CoordF src4 = src;		// Source pixel to read for bottom-left destination corner.

				src2.x += (patch.w - 1) * complexTransform[0][0];
				src2.y += (patch.w - 1) * complexTransform[0][1];

				src3.x += (patch.w - 1) * complexTransform[0][0] + (patch.h - 1) * complexTransform[1][0];
				src3.y += (patch.w - 1) * complexTransform[0][1] + (patch.h - 1) * complexTransform[1][1];

				src4.x += (patch.h - 1) * complexTransform[1][0];
				src4.y += (patch.h - 1) * complexTransform[1][1];

				if (src.x < 0 || src.y < 0 || src.x >= clipMax.w || src.y >= clipMax.h ||
					src2.x < 0 || src2.y < 0 || src2.x > clipMax.w || src2.y > clipMax.h ||
					src3.x < 0 || src3.y < 0 || src3.x > clipMax.w || src3.y > clipMax.h ||
					src4.x < 0 || src4.y < 0 || src4.x > clipMax.w || src4.y > clipMax.h )
				{
					bClipSource = true;
				}
			}

			//

			(this->*m_pComplexBlitOp)(patch, src, complexTransform,patch.pos(),bClipSource);
		}
	}

	//____ _onePassSimpleBlit() _____________________________________________

	void SoftGfxDevice::_onePassSimpleBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2], CoordI patchPos)
	{
		const SoftSurface * pSource = m_pBlitSource;

		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitches;

		pitches.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
		pitches.dstX = dstPixelBytes;
		pitches.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitches.srcX*dest.w;
		pitches.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;
		uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + src.x * srcPixelBytes;

		m_pSimpleBlitOnePassOp(pSrc, pDst, pSource, pitches, dest.h, dest.w, m_colTrans, patchPos, simpleTransform);
	}

	//____ _twoPassSimpleBlit() _____________________________________________

	void SoftGfxDevice::_twoPassSimpleBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2], CoordI patchPos)
	{
		SoftSurface * pSource = m_pBlitSource;

		int srcPixelBytes = pSource->m_pixelDescription.bits / 8;
		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitchesPass1, pitchesPass2;

		pitchesPass1.srcX = srcPixelBytes * simpleTransform[0][0] + pSource->m_pitch * simpleTransform[0][1];
		pitchesPass1.dstX = 8;
		pitchesPass1.srcY = srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1] - pitchesPass1.srcX*dest.w;
		pitchesPass1.dstY = 0;

		pitchesPass2.srcX = 8;
		pitchesPass2.dstX = dstPixelBytes;
		pitchesPass2.srcY = 0;
		pitchesPass2.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		int chunkLines;

		if (dest.w >= 2048)
			chunkLines = 1;
		else if (dest.w*dest.h <= 2048)
			chunkLines = dest.h;
		else
			chunkLines = 2048 / dest.w;

		int memBufferSize = chunkLines * dest.w*8;

		uint8_t * pChunkBuffer = (uint8_t*) Base::memStackAlloc(memBufferSize);

		int line = 0;

		while (line < dest.h)
		{
			int thisChunkLines = min(dest.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y+line) * m_canvasPitch + dest.x * dstPixelBytes;
			uint8_t * pSrc = pSource->m_pData + src.y * pSource->m_pitch + line*(srcPixelBytes * simpleTransform[1][0] + pSource->m_pitch * simpleTransform[1][1]) + src.x * srcPixelBytes;
//			uint8_t * pSrc = pSource->m_pData + (src.y+line) * pSource->m_pitch + src.x * srcPixelBytes;

			m_pSimpleBlitFirstPassOp(pSrc, pChunkBuffer, pSource, pitchesPass1, thisChunkLines, dest.w, m_colTrans, { 0,0 }, simpleTransform);
			m_pBlitSecondPassOp(pChunkBuffer, pDst, pSource, pitchesPass2, thisChunkLines, dest.w, m_colTrans, patchPos, nullptr);

			patchPos.y += thisChunkLines;
			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _onePassComplexBlit() ____________________________________________

	void SoftGfxDevice::_onePassComplexBlit(const RectI& dest, CoordF pos, const float transformMatrix[2][2], CoordI patchPos, bool bClipSource)
	{
		const SoftSurface * pSource = m_pBlitSource;

		// We use 0,0 as pixel center in software renderer, not 0.5,0.5.

		if (pSource->m_sampleMethod == SampleMethod::Bilinear)
		{
//			pos.x -= 0.5f;
//			pos.y -= 0.5f;
		}

		int dstPixelBytes = m_canvasPixelBits / 8;

		uint8_t * pDst = m_pCanvasPixels + dest.y * m_canvasPitch + dest.x * dstPixelBytes;

		ComplexBlitOp_p op = bClipSource ? m_pComplexClipBlitOnePassOp : m_pComplexBlitOnePassOp;

		op(pSource, pos, transformMatrix, pDst, dstPixelBytes, m_canvasPitch - dstPixelBytes * dest.w, dest.h, dest.w, m_colTrans, patchPos);
	}


	//____ _twoPassComplexBlit() ____________________________________________

	void SoftGfxDevice::_twoPassComplexBlit(const RectI& dest, CoordF pos, const float transformMatrix[2][2], CoordI patchPos, bool bClipSource)
	{
		const SoftSurface * pSource = m_pBlitSource;

		// We use 0,0 as pixel center in software renderer, not 0.5,0.5.

		if (pSource->m_sampleMethod == SampleMethod::Bilinear)
		{
//			pos.x -= 0.5f;
//			pos.y -= 0.5f;
		}


		int dstPixelBytes = m_canvasPixelBits / 8;

		Pitches pitchesPass2;

		pitchesPass2.srcX = 8;
		pitchesPass2.dstX = dstPixelBytes;
		pitchesPass2.srcY = 0;
		pitchesPass2.dstY = m_canvasPitch - dstPixelBytes * dest.w;

		int chunkLines;

		if (dest.w >= 2048)
			chunkLines = 1;
		else if (dest.w*dest.h <= 2048)
			chunkLines = dest.h;
		else
			chunkLines = 2048 / dest.w;

		int memBufferSize = chunkLines * dest.w * 8;

		uint8_t * pChunkBuffer = (uint8_t*)Base::memStackAlloc(memBufferSize);

		ComplexBlitOp_p firstPassOp = bClipSource ? m_pComplexClipBlitFirstPassOp : m_pComplexBlitFirstPassOp;

		int line = 0;

		while (line < dest.h)
		{
			int thisChunkLines = min(dest.h - line, chunkLines);

			uint8_t * pDst = m_pCanvasPixels + (dest.y + line) * m_canvasPitch + dest.x * dstPixelBytes;

			firstPassOp(pSource, pos, transformMatrix, pChunkBuffer, 8, 0, thisChunkLines, dest.w, m_colTrans, { 0,0 });
			m_pBlitSecondPassOp(pChunkBuffer, pDst, pSource, pitchesPass2, thisChunkLines, dest.w, m_colTrans, patchPos, nullptr);

			pos.x += transformMatrix[1][0] * thisChunkLines;
			pos.y += transformMatrix[1][1] * thisChunkLines;

			patchPos.y += thisChunkLines;
			line += thisChunkLines;
		}

		Base::memStackRelease(memBufferSize);
	}

	//____ _dummySimpleBlit() _________________________________________________

	void SoftGfxDevice::_dummySimpleBlit(const RectI& dest, CoordI pos, const int simpleTransform[2][2], CoordI patchPos)
	{
		// Do nothing but prevent crashing or need to check for nullpointer ;)
	}

	//____ _dummyComplexBlit() ________________________________________________

	void SoftGfxDevice::_dummyComplexBlit(const RectI& dest, CoordF pos, const float matrix[2][2], CoordI patchPos, bool ClipSource)
	{
		// Do nothing but prevent crashing or need to check for nullpointer ;)
	}


	//____ _updateBlitFunctions() _____________________________________________

	void SoftGfxDevice::_updateBlitFunctions()
	{
		// Sanity checking...

		if (!m_pRenderLayerSurface || !m_pBlitSource || !m_pCanvasPixels || !m_pBlitSource->m_pData)
		{
			m_pSimpleBlitOp = &SoftGfxDevice::_dummySimpleBlit;
			m_pComplexBlitOp = &SoftGfxDevice::_dummyComplexBlit;
			return;
		}

		//

		SampleMethod		sampleMethod = m_pBlitSource->sampleMethod();
		PixelFormat		srcFormat = m_pBlitSource->m_pixelDescription.format;
		PixelFormat		dstFormat = m_pRenderLayerSurface->pixelFormat();

		// Add fallback back to two-pass rendering.

		m_pSimpleBlitOnePassOp = nullptr;
		m_pComplexBlitOnePassOp = nullptr;

		if (m_pRenderLayerSurface->pixelDescription()->bLinear && m_pBlitSource->pixelDescription()->bLinear)
		{
			m_pSimpleBlitFirstPassOp = s_moveTo_BGRA_8_linear_OpTab[(int)srcFormat][0];
			m_pSimpleTileBlitFirstPassOp = s_moveTo_BGRA_8_linear_OpTab[(int)srcFormat][1];
			m_pComplexBlitFirstPassOp = s_transformTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][0];
			m_pComplexClipBlitFirstPassOp = s_transformTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][1];
			m_pComplexTileBlitFirstPassOp = s_transformTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][2];
			m_pBlitSecondPassOp = s_pass2OpTab_fast8[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		}
		else
		{
			m_pSimpleBlitFirstPassOp = s_moveTo_internal_OpTab[(int)srcFormat][0];
			m_pSimpleTileBlitFirstPassOp = s_moveTo_internal_OpTab[(int)srcFormat][1];
			m_pComplexBlitFirstPassOp = s_transformTo_internal_OpTab[(int)srcFormat][(int)sampleMethod][0];
			m_pComplexClipBlitFirstPassOp = s_transformTo_internal_OpTab[(int)srcFormat][(int)sampleMethod][1];
			m_pComplexTileBlitFirstPassOp = s_transformTo_internal_OpTab[(int)srcFormat][(int)sampleMethod][2];
			m_pBlitSecondPassOp = s_pass2OpTab[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];
		}


		// Try to find a suitable one-pass operation

		if (srcFormat == PixelFormat::BGRA_8_linear && m_pRenderLayerSurface->pixelDescription()->bLinear )
			m_pSimpleBlitOnePassOp = s_pass2OpTab_fast8[(int)m_colTrans.mode][(int)m_blendMode][(int)m_pRenderLayerSurface->pixelFormat()];


		if (m_colTrans.mode == TintMode::None)
		{
			if (m_blendMode == BlendMode::Blend)
			{
				switch (dstFormat)
				{
					case PixelFormat::BGRA_8_linear:
					{
						m_pSimpleBlitOnePassOp = s_blendTo_BGRA_8_linear_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_blendTo_BGRA_8_linear_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformBlendTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexClipBlitOnePassOp = s_transformBlendTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexTileBlitOnePassOp = s_transformBlendTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}
					case PixelFormat::BGRA_8_sRGB:
					{
						m_pSimpleBlitOnePassOp = s_blendTo_BGRA_8_sRGB_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_blendTo_BGRA_8_sRGB_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexClipBlitOnePassOp = s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexTileBlitOnePassOp = s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}

					case PixelFormat::BGR_8_linear:
					case PixelFormat::BGRX_8_linear:
					{
						m_pSimpleBlitOnePassOp = s_blendTo_BGR_8_linear_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_blendTo_BGR_8_linear_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformBlendTo_BGR_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexClipBlitOnePassOp = s_transformBlendTo_BGR_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexTileBlitOnePassOp = s_transformBlendTo_BGR_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}

					case PixelFormat::BGR_8_sRGB:
					case PixelFormat::BGRX_8_sRGB:
					{
						m_pSimpleBlitOnePassOp = s_blendTo_BGR_8_sRGB_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_blendTo_BGR_8_sRGB_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformBlendTo_BGR_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexClipBlitOnePassOp = s_transformBlendTo_BGR_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexTileBlitOnePassOp = s_transformBlendTo_BGR_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}

					default:
						break;

				}
			}
			else if (m_blendMode == BlendMode::Replace)
			{
				switch (dstFormat)
				{
					case PixelFormat::BGRA_8_linear:
					{
						m_pSimpleBlitOnePassOp = s_moveTo_BGRA_8_linear_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_moveTo_BGRA_8_linear_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexClipBlitOnePassOp = s_transformTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexTileBlitOnePassOp = s_transformTo_BGRA_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}

					case PixelFormat::BGRA_8_sRGB:
					{
						m_pSimpleBlitOnePassOp = s_moveTo_BGRA_8_sRGB_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_moveTo_BGRA_8_sRGB_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformTo_BGRA_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexTileBlitOnePassOp = s_transformTo_BGRA_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexClipBlitOnePassOp = s_transformTo_BGRA_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}

					case PixelFormat::BGR_8_linear:
					case PixelFormat::BGRX_8_linear:
					{
						m_pSimpleBlitOnePassOp = s_moveTo_BGR_8_linear_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_moveTo_BGR_8_linear_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformTo_BGR_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexClipBlitOnePassOp = s_transformTo_BGR_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexTileBlitOnePassOp = s_transformTo_BGR_8_linear_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}

					case PixelFormat::BGR_8_sRGB:
					case PixelFormat::BGRX_8_sRGB:
					{
						m_pSimpleBlitOnePassOp = s_moveTo_BGR_8_sRGB_OpTab[(int)srcFormat][0];
						m_pSimpleTileBlitOnePassOp = s_moveTo_BGR_8_sRGB_OpTab[(int)srcFormat][1];
						m_pComplexBlitOnePassOp = s_transformTo_BGR_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][0];
						m_pComplexClipBlitOnePassOp = s_transformTo_BGR_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][1];
						m_pComplexTileBlitOnePassOp = s_transformTo_BGR_8_sRGB_OpTab[(int)srcFormat][(int)sampleMethod][2];
						break;
					}

					default:
						break;
				}
			}
		}

		// Decide on Proxy function depending on what blit operations we got.

		if (m_pSimpleBlitOnePassOp)
			m_pSimpleBlitOp = &SoftGfxDevice::_onePassSimpleBlit;
		else if (m_pSimpleBlitFirstPassOp && m_pBlitSecondPassOp)
			m_pSimpleBlitOp = &SoftGfxDevice::_twoPassSimpleBlit;
		else
			m_pSimpleBlitOp = &SoftGfxDevice::_dummySimpleBlit;

		if (m_pComplexBlitOnePassOp)
			m_pComplexBlitOp = &SoftGfxDevice::_onePassComplexBlit;
		else if (m_pComplexBlitFirstPassOp && m_pBlitSecondPassOp)
			m_pComplexBlitOp = &SoftGfxDevice::_twoPassComplexBlit;
		else
			m_pComplexBlitOp = &SoftGfxDevice::_dummyComplexBlit;
	}

	//____ _initTables() ___________________________________________________________

	void SoftGfxDevice::_initTables()
	{
		// Init sRGBtoLinearTab

		for (int i = 0; i <= 4096; i++)
			s_limit4096Tab[i] = 0;

		for (int i = 0; i <= 4096; i++)
			s_limit4096Tab[4097+i] = i;

		for (int i = 0; i <= 4096; i++)
			s_limit4096Tab[4097*2 + i] = 4096;

		// Init mulTab

		//TODO: Both of these causes artefacts, but in different ways. What to do?

		for (int i = 0; i < 256; i++)
			s_mulTab[i] = 65536 * i / 255;

//		for (int i = 0; i < 256; i++)
//			s_mulTab[i] = i * 256 + i + 1;

		// Init lineThicknessTable

		for( int i = 0 ; i < 17 ; i++ )
		{
			double b = i/16.0;
			m_lineThicknessTable[i] = (int) (Util::squareRoot( 1.0 + b*b ) * 65536);
		}

		// Clear Op tables

		for (int i = 0; i < BlendMode_size; i++)
		{
			for (int j = 0; j < PixelFormat_size; j++)
			{
				for (int k = 0; k < TintMode_size; k++)
				{
					s_fillOpTab[k][i][j] = nullptr;
					s_pass2OpTab[k][i][j] = nullptr;
					s_pass2OpTab_fast8[k][i][j] = nullptr;

				}

				s_plotOpTab[i][j] = nullptr;
				s_lineOpTab[i][j] = nullptr;
				s_clipLineOpTab[i][j] = nullptr;
				s_plotListOpTab[i][j] = nullptr;
				s_segmentOpTab[0][i][j] = nullptr;
				s_segmentOpTab[1][i][j] = nullptr;
			}
		}

		for (int i = 0; i < PixelFormat_size; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				s_moveTo_BGRA_8_linear_OpTab[i][j] = nullptr;
				s_moveTo_BGRA_8_sRGB_OpTab[i][j] = nullptr;
				s_moveTo_BGR_8_linear_OpTab[i][j] = nullptr;
				s_moveTo_BGR_8_sRGB_OpTab[i][j] = nullptr;

				s_blendTo_BGRA_8_linear_OpTab[i][j] = nullptr;
				s_blendTo_BGRA_8_sRGB_OpTab[i][j] = nullptr;
				s_blendTo_BGR_8_linear_OpTab[i][j] = nullptr;
				s_blendTo_BGR_8_sRGB_OpTab[i][j] = nullptr;
			}

			for (int j = 0; j < 3; j++)
			{

				s_transformTo_BGRA_8_linear_OpTab[i][0][j] = nullptr;
				s_transformTo_BGRA_8_linear_OpTab[i][1][j] = nullptr;

				s_transformTo_BGRA_8_sRGB_OpTab[i][0][j] = nullptr;
				s_transformTo_BGRA_8_sRGB_OpTab[i][1][j] = nullptr;

				s_transformTo_BGR_8_linear_OpTab[i][0][j] = nullptr;
				s_transformTo_BGR_8_linear_OpTab[i][1][j] = nullptr;

				s_transformTo_BGR_8_sRGB_OpTab[i][0][j] = nullptr;
				s_transformTo_BGR_8_sRGB_OpTab[i][1][j] = nullptr;

				s_transformBlendTo_BGRA_8_linear_OpTab[i][0][j] = nullptr;
				s_transformBlendTo_BGRA_8_linear_OpTab[i][1][j] = nullptr;

				s_transformBlendTo_BGRA_8_sRGB_OpTab[i][0][j] = nullptr;
				s_transformBlendTo_BGRA_8_sRGB_OpTab[i][1][j] = nullptr;

				s_transformBlendTo_BGR_8_linear_OpTab[i][0][j] = nullptr;
				s_transformBlendTo_BGR_8_linear_OpTab[i][1][j] = nullptr;

				s_transformBlendTo_BGR_8_sRGB_OpTab[i][0][j] = nullptr;
				s_transformBlendTo_BGR_8_sRGB_OpTab[i][1][j] = nullptr;
			}
		}

		_populateOpTab();


		if (true)
		{

		// Init Blit Pass 2 Operation Table (4096 version)

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] =_simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;


		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;


		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;


		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;


		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_sRGB, false>;

		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGRA_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;
		s_pass2OpTab[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::Undefined, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_sRGB, false>;

		// Init Blit Pass 2 Operation Table (256 version)

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::A_8, false>;


		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::A_8, false>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGRA_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_565_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::RGB_565_bigendian, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGRA_4_linear, false>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::A_8, false>;
/*

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Add, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Min, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Max, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;


		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Add, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Min, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Max, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::Flat][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::Flat, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;


		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Add, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Min, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Max, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientX][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientX, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;


		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Add, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Min, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Max, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientY, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;


		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Add, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Min, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Max, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;

		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGRA_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
		s_pass2OpTab_fast8[(int)TintMode::GradientXY][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::GradientXY, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
*/


		// Init straight move to internal Operation Table
		// (used as pass 1 Operation Table for straight blit)

		s_moveTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;
		s_moveTo_internal_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;

		// Tiled versions

		s_moveTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;
		s_moveTo_internal_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;


		// Init straight move to BGRA_8_linear Operation Table

		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0] =_simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0] =_simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0] =_simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0] =_simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0] =_simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0] =_simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0] =_simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0] =_simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;

		// Tiled versions

		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;
		s_moveTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;


		// Init straight blend to BGRA_8_linear Operation Table

		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, false>;

		// Tiled versions

		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;
		s_blendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, true>;


		// Init straight move to BGRA_8_sRGB Operation Table

		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, false>;

		// Tiled versions

		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;
		s_moveTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, true>;


		// Init straight blend to BGRA_8_sRGB Operation Table

		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, false>;

		// Tiled versions

		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;
		s_blendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, true>;

		// Init straight move to BGR_8_linear Operation Table

		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, false>;

		// Tiled versions

		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;
		s_moveTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, true>;


		// Init straight blend to BGR_8_linear Operation Table

		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, false>;

		// Tiled versions

		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;
		s_blendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, true>;


		// Init straight move to BGR_8_sRGB Operation Table

		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, false>;

		// Tiled versions

		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;
		s_moveTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, true>;



		// Init straight blend to BGR_8_sRGB Operation Table

		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, false>;

		// Tiled versions

		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1] = _simple_blit < PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1] = _simple_blit < PixelFormat::BGRA_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1] = _simple_blit < PixelFormat::BGR_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1] = _simple_blit < PixelFormat::BGR_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1] = _simple_blit < PixelFormat::BGR_565_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1] = _simple_blit < PixelFormat::RGB_565_bigendian, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1] = _simple_blit < PixelFormat::BGRA_4_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1] = _simple_blit < PixelFormat::CLUT_8_linear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1] = _simple_blit < PixelFormat::CLUT_8_sRGB, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;
		s_blendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1] = _simple_blit < PixelFormat::A_8, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, true>;



		// Init transform move to internal Operation Table

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		s_transformTo_internal_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;
		s_transformTo_internal_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;

		// Clipped versions

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		s_transformTo_internal_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;
		s_transformTo_internal_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;

		// Tiled versions

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;

		s_transformTo_internal_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;
		s_transformTo_internal_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;


		// Init transformMove to BGRA_8_linear Operation Table

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		// Clipped versions

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		// Tiled versions

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;


		// Init transformBlend to BGRA_8_linear Operation Table

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::None>;

		// Clipped versions

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;

		// Tiled versions

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_linear_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;


		// Init transform move to BGRA_8_sRGB Operation Table

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		// Clipped versions

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		// Tiled versions

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;


		// Init transformBlend to BGRA_8_sRGB Operation Table

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::None>;

		// Clipped versions

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Clip>;

		// Tiled versions

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGRA_8_sRGB_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGRA_8_sRGB, EdgeOp::Tile>;


		// Init transformMove to BGR_8_linear Operation Table

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::None>;

		// Clipped versions

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		// Tiled versions

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_linear, EdgeOp::Tile>;


		// Init transformBlend to BGR_8_linear Operation Table

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::None>;

		// Clipped versions

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Clip>;

		// Tiled versions

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_linear_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_linear, EdgeOp::Tile>;


		// Init transform move to BGR_8_sRGB Operation Table

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		// Clipped versions

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		// Tiled versions

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;


		// Init transformBlend to BGR_8_sRGB Operation Table

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][0] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][0] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][0] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][0] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][0] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][0] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][0] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][0] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][0] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1][0] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::None>;

		// Clipped versions

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][1] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][1] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][1] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][1] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][1] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][1] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][1] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][1] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][1] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1][1] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Clip>;

		// Tiled versions

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][0][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_linear][1][2] = _complex_blit < PixelFormat::BGRA_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][0][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_8_sRGB][1][2] = _complex_blit < PixelFormat::BGRA_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRX_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][0][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_linear][1][2] = _complex_blit < PixelFormat::BGR_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][0][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_8_sRGB][1][2] = _complex_blit < PixelFormat::BGR_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][0][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGR_565_linear][1][2] = _complex_blit < PixelFormat::BGR_565_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][0][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::RGB_565_bigendian][1][2] = _complex_blit < PixelFormat::RGB_565_bigendian, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][0][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::BGRA_4_linear][1][2] = _complex_blit < PixelFormat::BGRA_4_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][0][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_linear][1][2] = _complex_blit < PixelFormat::CLUT_8_linear, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][0][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::CLUT_8_sRGB][1][2] = _complex_blit < PixelFormat::CLUT_8_sRGB, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;

		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][0][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Nearest, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;
		s_transformBlendTo_BGR_8_sRGB_OpTab[(int)PixelFormat::A_8][1][2] = _complex_blit < PixelFormat::A_8, SampleMethod::Bilinear, TintMode::None, BlendMode::Blend, PixelFormat::BGR_8_sRGB, EdgeOp::Tile>;




		// Init Segments Operation Table

		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::A_8>;

		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::A_8>;


		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Add, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Min, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Max, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[0][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <0, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;


		// Versions with vertical tint

		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::A_8>;

		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_linear] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGRA_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_linear] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_linear] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGR_8_linear>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGR_565_linear] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGR_565_linear>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::RGB_565_bigendian] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::RGB_565_bigendian>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGRA_4_linear] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGRA_4_linear>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::A_8] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::A_8>;


		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Replace][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Replace, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Blend][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Blend, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Add][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Add, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Subtract][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Subtract, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Multiply][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Multiply, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Min][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Min, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Max][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Max, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Invert][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Invert, PixelFormat::BGR_8_sRGB>;

		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGRA_8_sRGB] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGRA_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGRX_8_sRGB] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
		s_segmentOpTab[1][(int)BlendMode::Morph][(int)PixelFormat::BGR_8_sRGB] = _draw_segment_strip <1, BlendMode::Morph, PixelFormat::BGR_8_sRGB>;
}

		
}

	//____ _scaleLineThickness() ___________________________________________________

	int SoftGfxDevice::_scaleLineThickness( float thickness, int slope )
	{
		slope = std::abs(slope);

		int scale = m_lineThicknessTable[slope>>12];

		if( slope < (1 << 16) )
		{
			int scale2 = m_lineThicknessTable[(slope>>12)+1];
			scale += ((scale2-scale)*(slope & 0xFFF)) >> 12;
		}

		return (int) (thickness * scale);
	}


} // namespace wg
