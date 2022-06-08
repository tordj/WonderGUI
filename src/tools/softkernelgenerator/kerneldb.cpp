#include "kerneldb.h"

using namespace wg;
using namespace std;

KernelDB::KernelDB()
{
	reset();
}

KernelDB::~KernelDB()
{

}

KernelDB::CustomBlitSpec::CustomBlitSpec()
{
	for (auto& e : blitTypes)
		e = false;

	for (auto& e : tintModes)
		e = false;

	for (auto& e : blendModes)
		e = false;

	for (auto& e : sourceFormats)
		e = false;

	for (auto& e : destFormats)
		e = false;

}




//____ setTintMode() __________________________________________________________

void KernelDB::setTintMode(TintMode mode, bool bOn)
{
	if (mode >= TintMode_min && mode <= TintMode_max && mode != TintMode::None)
		m_tintModes[int(mode)] = bOn;
}

//____ setBlendMode() _________________________________________________________

void KernelDB::setBlendMode(BlendMode mode, bool bOn)
{
	if (mode >= BlendMode_min && mode <= BlendMode_max && 
		mode != BlendMode::Undefined && mode != BlendMode::Ignore && 
		mode != BlendMode::Blend && mode != BlendMode::Replace )
		m_blendModes[int(mode)] = bOn;
}

//____ setSrcFormat() _________________________________________________________

void KernelDB::setSrcFormat(PixelFormat format, bool bOn)
{
	if (format >= PixelFormat_min && format <= PixelFormat_max &&
		format != PixelFormat::Undefined && format != PixelFormat::Custom &&
		format != PixelFormat::BGRA_8 && format != PixelFormat::BGRX_8 &&
		format != PixelFormat::BGR_8 && format != PixelFormat::CLUT_8 )
		m_srcFormats[int(format)] = bOn;
}

//____ setDestFormat() ________________________________________________________

void KernelDB::setDestFormat(PixelFormat format, bool bOn)
{
	if (format >= PixelFormat_min && format <= PixelFormat_max &&
		format != PixelFormat::Undefined && format != PixelFormat::Custom &&
		format != PixelFormat::BGRA_8 && format != PixelFormat::BGRX_8 &&
		format != PixelFormat::BGR_8 && format != PixelFormat::CLUT_8 &&
		format != PixelFormat::CLUT_8_linear && format != PixelFormat::CLUT_8_sRGB )
		m_destFormats[int(format)] = bOn;
}

//____ countKernels() _________________________________________________________

KernelDB::KernelCount KernelDB::countKernels()
{
	int nTintModes = 0;
	int nBlendModes = 0;
	int nSourceFormats = 0;
	int nDestFormats = 0;

	for (auto& e : m_tintModes)
		if (e) nTintModes++;

	for (auto& e : m_blendModes)
		if (e) nBlendModes++;
	
	for (auto& e : m_srcFormats)
		if (e) nSourceFormats++;

	for (auto& e : m_destFormats)
		if (e) nDestFormats++;

	// Remove duplicates that use the same kernel

	if (m_srcFormats[int(PixelFormat::BGR_8_linear)] && m_srcFormats[int(PixelFormat::BGRX_8_linear)])
		nSourceFormats--;

	if (m_srcFormats[int(PixelFormat::BGR_8_sRGB)] && m_srcFormats[int(PixelFormat::BGRX_8_sRGB)])
		nSourceFormats--;

	if (m_destFormats[int(PixelFormat::BGR_8_linear)] && m_destFormats[int(PixelFormat::BGRX_8_linear)])
		nDestFormats--;

	if (m_destFormats[int(PixelFormat::BGR_8_sRGB)] && m_destFormats[int(PixelFormat::BGRX_8_sRGB)])
		nDestFormats--;



	// Fill in values for draw kernels and fundamental blit kernels.

	KernelCount count;

	count.plot = nBlendModes * nDestFormats;
	count.fill = nTintModes * nBlendModes * nDestFormats;
	count.line = nBlendModes * nDestFormats;
	count.clipLine = nBlendModes * nDestFormats;
	count.plotList = nBlendModes * nDestFormats;
	count.segment = nBlendModes * nDestFormats * ((m_tintModes[int(TintMode::GradientY)] || m_tintModes[int(TintMode::GradientXY)]) ? 2 : 1);

	count.pass1blits_straight		= nSourceFormats * 2;
	count.pass1blits_straight_fast8	= nSourceFormats * 2;

	count.pass1blits_transform		= nSourceFormats * 2 * 3;
	count.pass1blits_transform_fast8 = nSourceFormats * 2 * 3;

	count.pass2blits = nTintModes * nBlendModes * nDestFormats;
	count.pass2blits_fast8 = nTintModes * nBlendModes * nDestFormats;		// Room for improvement?

	// Fill in values for transform custom kernels

	for (int i = 0; i < m_customBlits.size() ; i++ )
		count.customBlits += countEntryKernels(i);


	return count;
}


//____ countEntryKernels() ____________________________________________________

int KernelDB::countEntryKernels( int entry )
{
	auto& spec = m_customBlits[entry];

	int blitAndTile = 0;
	int nBlitTypes = 0;
	int nTintModes = 0;
	int nBlendModes = 0;
	int nSourceFormats = 0;
	int nDestFormats = 0;


	for (int i = 0; i < TintMode_size; i++)
		if (spec.tintModes[i] && m_tintModes[i])
			nTintModes++;

	for (int i = 0; i < BlendMode_size; i++)
		if (spec.blendModes[i] && m_blendModes[i])
			nBlendModes++;

	for (int i = 0; i < PixelFormat_size; i++)
		if (spec.sourceFormats[i] && m_srcFormats[i])
			nSourceFormats++;

	for (int i = 0; i < PixelFormat_size; i++)
		if (spec.destFormats[i] && m_destFormats[i])
			nDestFormats++;

	for (int i = 0; i < BlitType_size; i++)
		if (spec.blitTypes[i])
			nBlitTypes++;

	// Remove duplicates that use the same kernel

	if (m_srcFormats[int(PixelFormat::BGR_8_linear)] && m_srcFormats[int(PixelFormat::BGRX_8_linear)] && spec.sourceFormats[int(PixelFormat::BGR_8_linear)] && spec.sourceFormats[int(PixelFormat::BGRX_8_linear)])
		nSourceFormats--;

	if (m_srcFormats[int(PixelFormat::BGR_8_sRGB)] && m_srcFormats[int(PixelFormat::BGRX_8_sRGB)] && spec.sourceFormats[int(PixelFormat::BGR_8_sRGB)] && spec.sourceFormats[int(PixelFormat::BGRX_8_sRGB)])
		nSourceFormats--;

	if (m_destFormats[int(PixelFormat::BGR_8_linear)] && m_destFormats[int(PixelFormat::BGRX_8_linear)] && spec.destFormats[int(PixelFormat::BGR_8_linear)] && spec.destFormats[int(PixelFormat::BGRX_8_linear)])
		nDestFormats--;

	if (m_destFormats[int(PixelFormat::BGR_8_sRGB)] && m_destFormats[int(PixelFormat::BGRX_8_sRGB)] && spec.destFormats[int(PixelFormat::BGR_8_sRGB)] && spec.destFormats[int(PixelFormat::BGRX_8_sRGB)])
		nDestFormats--;


	return  nBlitTypes * nTintModes * nBlendModes * nSourceFormats * nDestFormats;
}


//____ generateSource() _______________________________________________________

bool KernelDB::generateSource(std::ostream& out)
{
	char temp[4096];


	bool bHasLinearSource = m_srcFormats[(int)PixelFormat::BGRA_4_linear] ||
							m_srcFormats[(int)PixelFormat::BGRA_8_linear] ||
							m_srcFormats[(int)PixelFormat::BGRX_8_linear] ||
							m_srcFormats[(int)PixelFormat::BGR_565_linear] ||
							m_srcFormats[(int)PixelFormat::BGR_8_linear] ||
							m_srcFormats[(int)PixelFormat::CLUT_8_linear] ||
							m_srcFormats[(int)PixelFormat::RGB_565_bigendian] ||
							m_srcFormats[(int)PixelFormat::A_8];

	bool bHasLinearDest =   m_destFormats[(int)PixelFormat::BGRA_4_linear] ||
							m_destFormats[(int)PixelFormat::BGRA_8_linear] ||
							m_destFormats[(int)PixelFormat::BGRX_8_linear] ||
							m_destFormats[(int)PixelFormat::BGR_565_linear] ||
							m_destFormats[(int)PixelFormat::BGR_8_linear] ||
							m_destFormats[(int)PixelFormat::RGB_565_bigendian] ||
							m_destFormats[(int)PixelFormat::A_8];

	bool bHasSRGBSource =	m_srcFormats[(int)PixelFormat::BGRA_8_sRGB] ||
							m_srcFormats[(int)PixelFormat::BGRX_8_sRGB] ||
							m_srcFormats[(int)PixelFormat::BGR_8_sRGB] ||
							m_srcFormats[(int)PixelFormat::CLUT_8_sRGB];

	bool bHasSRGBDest =		m_destFormats[(int)PixelFormat::BGRA_8_sRGB] ||
							m_destFormats[(int)PixelFormat::BGRX_8_sRGB] ||
							m_destFormats[(int)PixelFormat::BGR_8_sRGB];

	bool bUseFast8Blits = bHasLinearSource && bHasLinearDest;

	bool bUseSRGBBlits = bHasSRGBSource || bHasSRGBDest;


	// Print out initial stanza

	out << "#include <wg_softgfxdevice.h>" << endl;
	out << "#include <wg_softkernelstanza.impl.h>" << endl;
	out << endl;
	out << "void wg::SoftGfxDevice::_populateOpTab()" << endl;
	out << "{" << endl;
	out << "    _initStanzaTables();" << endl << endl;


	// Print out the plot kernels.

	for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
	{
		if (m_blendModes[blendMode]) 
		{
			for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
			{
				if (m_destFormats[destFormat])				
				{
					auto pBlend = toString((BlendMode)blendMode);
					auto pFormat = toString((PixelFormat)destFormat);

					sprintf(temp, "s_plotOpTab[(int)BlendMode::%s][(int)PixelFormat::%s] = _plot<BlendMode::%s, TintMode::None, PixelFormat::%s>;\n",
							pBlend, pFormat, pBlend, pFormat);

					out << temp;
				}
			}
			out << endl;
		}
	}

	// Print out the fill kernels.

	for (int tintMode = 0; tintMode < TintMode_size; tintMode++)
	{
		if (m_tintModes[tintMode])
		{
			for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
			{
				if (m_blendModes[blendMode])
				{
					for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
					{
						if (m_destFormats[destFormat])
						{
							auto pBlend = toString((BlendMode)blendMode);
							auto pFormat = toString((PixelFormat)destFormat);
							auto pTint = toString((TintMode)tintMode);

							sprintf(temp, "s_fillOpTab[(int)TintMode::%s][(int)BlendMode::%s][(int)PixelFormat::%s] = _fill<TintMode::%s,BlendMode::%s, PixelFormat::%s>;\n",
								pTint, pBlend, pFormat, pTint, pBlend, pFormat);

							out << temp;
						}
					}
				}
			}
			out << endl;
		}
	}

	// Print out the line kernels.

	for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
	{
		if (m_blendModes[blendMode])
		{
			for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
			{
				if (m_destFormats[destFormat])
				{
					auto pBlend = toString((BlendMode)blendMode);
					auto pFormat = toString((PixelFormat)destFormat);

					sprintf(temp, "s_lineOpTab[(int)BlendMode::%s][(int)PixelFormat::%s] = _draw_line<BlendMode::%s, TintMode::None, PixelFormat::%s>;\n",
						pBlend, pFormat, pBlend, pFormat);

					out << temp;
				}
			}
			out << endl;
		}
	}

	// Print out the clip line kernels.

	for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
	{
		if (m_blendModes[blendMode])
		{
			for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
			{
				if (m_destFormats[destFormat])
				{
					auto pBlend = toString((BlendMode)blendMode);
					auto pFormat = toString((PixelFormat)destFormat);

					sprintf(temp, "s_clipLineOpTab[(int)BlendMode::%s][(int)PixelFormat::%s] = _clip_draw_line<BlendMode::%s, TintMode::None, PixelFormat::%s>;\n",
						pBlend, pFormat, pBlend, pFormat);

					out << temp;
				}
			}
			out << endl;
		}
	}

	// Print out the plot list kernels.

	for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
	{
		if (m_blendModes[blendMode])
		{
			for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
			{
				if (m_destFormats[destFormat])
				{
					auto pBlend = toString((BlendMode)blendMode);
					auto pFormat = toString((PixelFormat)destFormat);

					sprintf(temp, "s_plotListOpTab[(int)BlendMode::%s][(int)PixelFormat::%s] = _plot_list<BlendMode::%s, TintMode::None, PixelFormat::%s>;\n",
						pBlend, pFormat, pBlend, pFormat);

					out << temp;
				}
			}
			out << endl;
		}
	}

	// Print out the segment kernels.

	for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
	{
		if (m_blendModes[blendMode])
		{
			for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
			{
				if (m_destFormats[destFormat])
				{
					auto pBlend = toString((BlendMode)blendMode);
					auto pFormat = toString((PixelFormat)destFormat);

					if (m_tintModes[(int)TintMode::None] || m_tintModes[(int)TintMode::Flat] || m_tintModes[(int)TintMode::GradientX])
					{
						sprintf(temp, "s_segmentOpTab[0][(int)BlendMode::%s][(int)PixelFormat::%s] = _draw_segment_strip<0, BlendMode::%s, PixelFormat::%s>;\n",
										pBlend, pFormat, pBlend, pFormat);
					}

					out << temp;

					if (m_tintModes[(int)TintMode::GradientY] || m_tintModes[(int)TintMode::GradientXY])
					{
						sprintf(temp, "s_segmentOpTab[1][(int)BlendMode::%s][(int)PixelFormat::%s] = _draw_segment_strip<1, BlendMode::%s, PixelFormat::%s>;\n",
							pBlend, pFormat, pBlend, pFormat);
					}

					out << temp;
				}
			}
			out << endl;
		}
	}

	// Print out the blit pass 2 kernels - sRGB version.

	if (bUseSRGBBlits)
	{
		for (int tintMode = 0; tintMode < TintMode_size; tintMode++)
		{
			if (m_tintModes[tintMode])
			{
				for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
				{
					if (m_blendModes[blendMode])
					{
						for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
						{
							if (m_destFormats[destFormat])
							{
								auto pBlend = toString((BlendMode)blendMode);
								auto pFormat = toString((PixelFormat)destFormat);
								auto pTint = toString((TintMode)tintMode);

								sprintf(temp, "s_pass2OpTab[(int)TintMode::%s][(int)BlendMode::%s][(int)PixelFormat::%s] = _straight_blit<PixelFormat::Undefined, TintMode::%s, BlendMode::%s, PixelFormat::%s, false>;\n",
									pTint, pBlend, pFormat, pTint, pBlend, pFormat);

								out << temp;
							}
						}
					}
				}
				out << endl;
			}
		}
	}

	// Print out the blit pass 2 kernels - fast 8 version.

	if (bUseFast8Blits)
	{
		for (int tintMode = 0; tintMode < TintMode_size; tintMode++)
		{
			if (m_tintModes[tintMode])
			{
				for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
				{
					if (m_blendModes[blendMode])
					{
						for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
						{
							if (m_destFormats[destFormat])
							{
								auto pBlend = toString((BlendMode)blendMode);
								auto pFormat = toString((PixelFormat)destFormat);
								auto pTint = toString((TintMode)tintMode);

								sprintf(temp, "s_pass2OpTab_fast8[(int)TintMode::%s][(int)BlendMode::%s][(int)PixelFormat::%s] = _straight_blit<PixelFormat::BGRA_8_linear, TintMode::%s, BlendMode::%s, PixelFormat::%s, false>;\n",
									pTint, pBlend, pFormat, pTint, pBlend, pFormat);

								out << temp;
							}
						}
					}
				}
				out << endl;
			}
		}
	}
	else
	{
		// We still need to support 8 bit straight blits (no tint, no blend).

		for (int destFormat = 0; destFormat < PixelFormat_size; destFormat++)
		{
			if (m_destFormats[destFormat])
			{
				auto pFormat = toString((PixelFormat)destFormat);
	
				sprintf(temp, "s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::%s] = _straight_blit<PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::%s, false>;\n",
					pFormat, pFormat);

				out << temp;
			}
		}
		out << endl;
	}

	// print out the simple blit pass 1 kernels

	for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
	{
		if (m_srcFormats[srcFormat] )
		{
			auto pFormat = toString((PixelFormat)srcFormat);

			sprintf(temp, "s_moveTo_internal_OpTab[(int)PixelFormat::%s][0] = _straight_blit<PixelFormat::%s, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;\n",
				pFormat, pFormat);
			out << temp;

			sprintf(temp, "s_moveTo_internal_OpTab[(int)PixelFormat::%s][1] = _straight_blit<PixelFormat::%s, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;\n",
				pFormat, pFormat);
			out << temp;
		}
	}
	out << endl;

	// print out fast8 version of simple blit pass 1 kernels

	if (bUseFast8Blits)
	{
		for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
		{
			if (m_srcFormats[srcFormat])
			{
				auto pFormat = toString((PixelFormat)srcFormat);

				sprintf(temp, "s_moveTo_internal_fast8_OpTab[(int)PixelFormat::%s][0] = _straight_blit<PixelFormat::%s, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, false>;\n",
					pFormat, pFormat);
				out << temp;

				sprintf(temp, "s_moveTo_internal_fast8_OpTab[(int)PixelFormat::%s][1] = _straight_blit<PixelFormat::%s, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, true>;\n",
					pFormat, pFormat);
				out << temp;
			}
		}
		out << endl;
	}


	// print out the complex blit pass 1 kernels

	for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
	{
		if (m_srcFormats[srcFormat])
		{
			auto pFormat = toString((PixelFormat)srcFormat);

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][0][0] = _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;\n",
				pFormat, pFormat);
			out << temp;

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][0][1] = _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;\n",
				pFormat, pFormat);
			out << temp;

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][0][2] = _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;\n",
				pFormat, pFormat);
			out << temp;

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][1][0] = _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;\n",
				pFormat, pFormat);
			out << temp;

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][1][1] = _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;\n",
				pFormat, pFormat);
			out << temp;

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][1][2] = _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;\n",
				pFormat, pFormat);
			out << temp;
		}
	}
	out << endl;

	// print out fast8 version of the complex blit pass 1 kernels

	if (bUseFast8Blits)
	{
		for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
		{
			if (m_srcFormats[srcFormat])
			{
				auto pFormat = toString((PixelFormat)srcFormat);

				sprintf(temp, "s_transformTo_internal_fast8_OpTab[(int)PixelFormat::%s][0][0] = _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;\n",
					pFormat, pFormat);
				out << temp;

				sprintf(temp, "s_transformTo_internal_fast8_OpTab[(int)PixelFormat::%s][0][1] = _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;\n",
					pFormat, pFormat);
				out << temp;

				sprintf(temp, "s_transformTo_internal_fast8_OpTab[(int)PixelFormat::%s][0][2] = _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;\n",
					pFormat, pFormat);
				out << temp;

				sprintf(temp, "s_transformTo_internal_fast8_OpTab[(int)PixelFormat::%s][1][0] = _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::None>;\n",
					pFormat, pFormat);
				out << temp;

				sprintf(temp, "s_transformTo_internal_fast8_OpTab[(int)PixelFormat::%s][1][1] = _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Clip>;\n",
					pFormat, pFormat);
				out << temp;

				sprintf(temp, "s_transformTo_internal_fast8_OpTab[(int)PixelFormat::%s][1][2] = _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::BGRA_8_linear, EdgeOp::Tile>;\n",
					pFormat, pFormat);
				out << temp;
			}
		}
		out << endl;

	}

	// Print end of method

	out << "}" << endl;


	// Print out straight blit kernel list


	out << "SoftGfxDevice::StraightBlitKernelEntry SoftGfxDevice::s_straightBlitKernels[] = {" << endl;

	for (auto& entry : m_customBlits)
	{
		for (int tintMode = 0; tintMode < TintMode_size; tintMode++)
		{
			for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
			{
				for (int srcFmt = 0; srcFmt < PixelFormat_size; srcFmt++)
				{
					for (int dstFmt = 0; dstFmt < PixelFormat_size; dstFmt++)
					{
						if (entry.tintModes[tintMode] && entry.blendModes[blendMode] && entry.sourceFormats[srcFmt] && entry.destFormats[dstFmt])
						{
							if (entry.blitTypes[(int)BlitType::StraightBlit])
							{
								sprintf(temp, "    { PixelFormat::%s, false, TintMode::%s, BlendMode::%s, PixelFormat::%s, _straight_blit<PixelFormat::%s, TintMode::%s, BlendMode::%s, PixelFormat::%s, false> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));
								
								out << temp;
							}

							if (entry.blitTypes[(int)BlitType::StraightTile])
							{
								sprintf(temp, "    { PixelFormat::%s, false, TintMode::%s, BlendMode::%s, PixelFormat::%s, _straight_blit<PixelFormat::%s, TintMode::%s, BlendMode::%s, PixelFormat::%s, false> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));

								out << temp;
							}

						}

					}
				}
			}
		}
	}

	out << "    { PixelFormat::Undefined, false, TintMode::None, BlendMode::Undefined, PixelFormat::Undefined, nullptr }" << endl;
	out << "};" << endl << endl;

	// Print out transform blit kernel list


	out << "SoftGfxDevice::TransformBlitKernelEntry SoftGfxDevice::s_transformBlitKernels[] = {" << endl;

	for (auto& entry : m_customBlits)
	{
		for (int tintMode = 0; tintMode < TintMode_size; tintMode++)
		{
			for (int blendMode = 0; blendMode < BlendMode_size; blendMode++)
			{
				for (int srcFmt = 0; srcFmt < PixelFormat_size; srcFmt++)
				{
					for (int dstFmt = 0; dstFmt < PixelFormat_size; dstFmt++)
					{
						if (entry.tintModes[tintMode] && entry.blendModes[blendMode] && entry.sourceFormats[srcFmt] && entry.destFormats[dstFmt])
						{
							if (entry.blitTypes[(int)BlitType::TransformBlitNearest])
							{
								sprintf(temp, "    { PixelFormat::%s, SampleMethod::Nearest, 0, TintMode::%s, BlendMode::%s, PixelFormat::%s, _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::%s, BlendMode::%s, PixelFormat::%s, EdgeOp::None> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));
								
								out << temp;
							}

							if (entry.blitTypes[(int)BlitType::TransformBlitBilinear])
							{
								sprintf(temp, "    { PixelFormat::%s, SampleMethod::Bilinear, 0, TintMode::%s, BlendMode::%s, PixelFormat::%s, _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::%s, BlendMode::%s, PixelFormat::%s, EdgeOp::None> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));
								
								out << temp;
							}


							if (entry.blitTypes[(int)BlitType::TransformClipBlitNearest])
							{
								sprintf(temp, "    { PixelFormat::%s, SampleMethod::Nearest, 1, TintMode::%s, BlendMode::%s, PixelFormat::%s, _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::%s, BlendMode::%s, PixelFormat::%s, EdgeOp::Clip> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));
								
								out << temp;
							}

							if (entry.blitTypes[(int)BlitType::TransformClipBlitBilinear])
							{
								sprintf(temp, "    { PixelFormat::%s, SampleMethod::Bilinear, 1, TintMode::%s, BlendMode::%s, PixelFormat::%s, _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::%s, BlendMode::%s, PixelFormat::%s, EdgeOp::Clip> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));
								
								out << temp;
							}

							if (entry.blitTypes[(int)BlitType::TransformTileNearest])
							{
								sprintf(temp, "    { PixelFormat::%s, SampleMethod::Nearest, 2, TintMode::%s, BlendMode::%s, PixelFormat::%s, _transform_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::%s, BlendMode::%s, PixelFormat::%s, EdgeOp::Tile> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));
								
								out << temp;
							}

							if (entry.blitTypes[(int)BlitType::TransformClipBlitBilinear])
							{
								sprintf(temp, "    { PixelFormat::%s, SampleMethod::Bilinear, 2, TintMode::%s, BlendMode::%s, PixelFormat::%s, _transform_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::%s, BlendMode::%s, PixelFormat::%s, EdgeOp::Tile> },\n",
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt),
									toString((PixelFormat)srcFmt),
									toString((TintMode)tintMode),
									toString((BlendMode)blendMode),
									toString((PixelFormat)dstFmt));
								
								out << temp;
							}

						}

					}
				}
			}
		}
	}

	out << "    { PixelFormat::Undefined, SampleMethod::Nearest, 0, TintMode::None, BlendMode::Undefined, PixelFormat::Undefined, nullptr }" << endl;
	out << "};" << endl << endl;

	//


	out << "SoftGfxDevice::StraightBlitKernelEntry* SoftGfxDevice::_getStraightBlitKernels() const { return s_straightBlitKernels; }" << endl;
	out << "SoftGfxDevice::TransformBlitKernelEntry* SoftGfxDevice::_getTransformBlitKernels() const { return s_transformBlitKernels; }" << endl;

	return true;
}


//____ clear() _____________________________________________________________

void KernelDB::clear()
{
	// Set all BlitTypes, TintModes and BlendModes

	for (bool& b : m_tintModes)
		b = false;

	for (bool& b : m_blendModes)
		b = false;

	m_blendModes[int(BlendMode::Replace)] = true;
	m_blendModes[int(BlendMode::Blend)] = true;

	m_tintModes[int(TintMode::None)] = true;

	// Clear all Source and Dest formats (specifics will be set later).

	for (bool& b : m_srcFormats)
		b = false;

	for (bool& b : m_destFormats)
		b = false;

	// Clear all custom blits.

	m_customBlits.clear();
}


//____ _reset() _____________________________________________________________

void KernelDB::reset()
{
	// Set all TintModes and BlendModes

	for (bool& b : m_tintModes)
		b = true;

	for (bool& b : m_blendModes)
		b = true;

	m_blendModes[int(BlendMode::Undefined)] = false;
	m_blendModes[int(BlendMode::Ignore)] = false;


	// Clear all Source and Dest formats (specifics will be set later).

	for (bool& b : m_srcFormats)
		b = false;

	for (bool& b : m_destFormats)
		b = false;

	// Set standard source formats

	m_srcFormats[int(PixelFormat::BGR_8_sRGB)] = true;
	m_srcFormats[int(PixelFormat::BGR_8_linear)] = true;
	m_srcFormats[int(PixelFormat::BGRX_8_sRGB)] = true;
	m_srcFormats[int(PixelFormat::BGRX_8_linear)] = true;
	m_srcFormats[int(PixelFormat::BGRA_8_sRGB)] = true;
	m_srcFormats[int(PixelFormat::BGRA_8_linear)] = true;
	m_srcFormats[int(PixelFormat::BGRA_4_linear)] = true;
	m_srcFormats[int(PixelFormat::BGR_565_linear)] = true;
	m_srcFormats[int(PixelFormat::CLUT_8_sRGB)] = true;
	m_srcFormats[int(PixelFormat::CLUT_8_linear)] = true;
	m_srcFormats[int(PixelFormat::RGB_565_bigendian)] = true;
	m_srcFormats[int(PixelFormat::A_8)] = true;

	// Set standard destination formats

	m_destFormats[int(PixelFormat::BGR_8_sRGB)] = true;
	m_destFormats[int(PixelFormat::BGR_8_linear)] = true;
	m_destFormats[int(PixelFormat::BGRX_8_sRGB)] = true;
	m_destFormats[int(PixelFormat::BGRX_8_linear)] = true;
	m_destFormats[int(PixelFormat::BGRA_8_sRGB)] = true;
	m_destFormats[int(PixelFormat::BGRA_8_linear)] = true;
	m_destFormats[int(PixelFormat::BGRA_4_linear)] = true;
	m_destFormats[int(PixelFormat::BGR_565_linear)] = true;
	m_destFormats[int(PixelFormat::RGB_565_bigendian)] = true;
	m_destFormats[int(PixelFormat::A_8)] = true;

	// Set custom blit methods
	{
		m_customBlits.clear();

		CustomBlitSpec spec;

		for (bool& b : spec.blitTypes)
			b = true;

		spec.tintModes[int(TintMode::None)] = true;

		spec.blendModes[int(BlendMode::Blend)] = true;
		spec.blendModes[int(BlendMode::Replace)] = true;

		spec.destFormats[int(PixelFormat::BGRA_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGRA_8_sRGB)] = true;

		spec.destFormats[int(PixelFormat::BGRX_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGRX_8_sRGB)] = true;

		spec.destFormats[int(PixelFormat::BGR_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGR_8_sRGB)] = true;

		for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
		{
			if (m_srcFormats[srcFormat])
				spec.sourceFormats[srcFormat] = true;
		}

		m_customBlits.push_back(spec);
	}

}
