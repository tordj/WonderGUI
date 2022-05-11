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

KernelDB::StraightBlitSpec::StraightBlitSpec()
{
	for (auto& e : tintModes)
		e = false;

	for (auto& e : blendModes)
		e = false;

	for (auto& e : sourceFormats)
		e = false;

	for (auto& e : destFormats)
		e = false;

	for (auto& e : blitAndTile)
		e = false;
}

KernelDB::TransformBlitSpec::TransformBlitSpec()
{
	for (auto& e : tintModes)
		e = false;

	for (auto& e : blendModes)
		e = false;

	for (auto& e : sourceFormats)
		e = false;

	for (auto& e : destFormats)
		e = false;

	for (auto& e : sampleMethods)
		e = false;

	for (auto& e : blitClipAndTile)
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
		format != PixelFormat::BGR_8 && format != PixelFormat::CLUT_8)
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




	KernelCount count;

	count.plot = nBlendModes * nDestFormats;
	count.fill = nTintModes * nBlendModes * nDestFormats;
	count.line = nBlendModes * nDestFormats;
	count.clipLine = nBlendModes * nDestFormats;
	count.plotList = nBlendModes * nDestFormats;
	count.segment = nBlendModes * nDestFormats * ((m_tintModes[int(TintMode::GradientY)] || m_tintModes[int(TintMode::GradientXY)]) ? 2 : 1);

	count.pass1blits		= nSourceFormats * 2;
	count.pass1blits_fast8	= nSourceFormats * 2;

	count.pass2blits = nTintModes * nBlendModes * nDestFormats;
	count.pass2blits_fast8 = nTintModes * nBlendModes * nDestFormats;		// Room for improvement?

	return count;
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

							sprintf(temp, "s_fillOpTab[(int)TintMode::%s][(int)BlendMode::%s][(int)PixelFormat::%s] = _fill<TintMode::%s,BlendMode::%s, TintMode::None, PixelFormat::%s>;\n",
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

								sprintf(temp, "s_pass2OpTab[(int)TintMode::%s][(int)BlendMode::%s][(int)PixelFormat::%s] = _simple_blit<PixelFormat::Undefined, TintMode::%s, BlendMode::%s, PixelFormat::%s, false>;\n",
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

								sprintf(temp, "s_pass2OpTab_fast8[(int)TintMode::%s][(int)BlendMode::%s][(int)PixelFormat::%s] = _simple_blit<PixelFormat::BGRA_8_linear, TintMode::%s, BlendMode::%s, PixelFormat::%s, false>;\n",
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
	
				sprintf(temp, "s_pass2OpTab_fast8[(int)TintMode::None][(int)BlendMode::Replace][(int)PixelFormat::%s] = _simple_blit<PixelFormat::BGRA_8_linear, TintMode::None, BlendMode::Replace, PixelFormat::%s, false>;\n",
					pFormat, pFormat);

				out << temp;
			}
		}
		out << endl;
	}

	// print out the simple blit pass 1 kernels

	for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
	{
		if (m_srcFormats[srcFormat])
		{
			auto pFormat = toString((PixelFormat)srcFormat);

			sprintf(temp, "s_moveTo_internal_OpTab[(int)PixelFormat::%s][0] = _simple_blit<PixelFormat::%s, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, false>;\n",
				pFormat, pFormat);
			sprintf(temp, "s_moveTo_internal_OpTab[(int)PixelFormat::%s][1] = _simple_blit<PixelFormat::%s, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, true>;\n",
				pFormat, pFormat);

			out << temp;
		}
	}
	out << endl;

	// print out the complex blit pass 1 kernels

	for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
	{
		if (m_srcFormats[srcFormat])
		{
			auto pFormat = toString((PixelFormat)srcFormat);

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][0][0] = _complex_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;\n",
				pFormat, pFormat);
			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][0][1] = _complex_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;\n",
				pFormat, pFormat);
			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][0][2] = _complex_blit<PixelFormat::%s, SampleMethod::Nearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;\n",
				pFormat, pFormat);

			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][1][0] = _complex_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::None>;\n",
				pFormat, pFormat);
			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][1][1] = _complex_blit<PixelFormat::%s, SampleMethod::Bilinear, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Clip>;\n",
				pFormat, pFormat);
			sprintf(temp, "s_transformTo_internal_OpTab[(int)PixelFormat::%s][1][2] = _complex_blit<PixelFormat::%s, SampleMethod::BilinearNearest, TintMode::None, BlendMode::Replace, PixelFormat::Undefined, EdgeOp::Tile>;\n",
				pFormat, pFormat);

			out << temp;
		}
	}
	out << endl;

	return true;
}


//____ clear() _____________________________________________________________

void KernelDB::clear()
{
	// Set all TintModes and BlendModes

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

	// Set optimized SimpleBlit methods
	{
		StraightBlitSpec spec;

		spec.blitAndTile[0] = true;
		spec.blitAndTile[1] = true;

		spec.tintModes[int(TintMode::None)] = true;
		spec.blendModes[int(BlendMode::Blend)] = true;
		spec.blendModes[int(BlendMode::Replace)] = true;

		spec.destFormats[int(PixelFormat::BGRA_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGRA_8_sRGB)] = true;

		spec.destFormats[int(PixelFormat::BGR_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGR_8_sRGB)] = true;

		spec.destFormats[int(PixelFormat::BGRX_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGRX_8_sRGB)] = true;

		for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
		{
			if (m_srcFormats[srcFormat])
				spec.sourceFormats[srcFormat] = true;
		}

		m_straightBlits.push_back(spec);
	}

	// Set optimized ComplexBlit methods

	{
		TransformBlitSpec spec;

		spec.blitClipAndTile[0] = true;
		spec.blitClipAndTile[1] = true;
		spec.blitClipAndTile[2] = true;

		spec.tintModes[int(TintMode::None)] = true;
		spec.blendModes[int(BlendMode::Blend)] = true;
		spec.blendModes[int(BlendMode::Replace)] = true;

		spec.sampleMethods[int(SampleMethod::Nearest)] = true;
		spec.sampleMethods[int(SampleMethod::Bilinear)] = true;

		spec.destFormats[int(PixelFormat::BGRA_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGRA_8_sRGB)] = true;

		spec.destFormats[int(PixelFormat::BGR_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGR_8_sRGB)] = true;

		spec.destFormats[int(PixelFormat::BGRX_8_linear)] = true;
		spec.destFormats[int(PixelFormat::BGRX_8_sRGB)] = true;

		for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
		{
			if (m_srcFormats[srcFormat])
				spec.sourceFormats[srcFormat] = true;
		}

		m_transformBlits.push_back(spec);
	}
}
