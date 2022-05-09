#include "kerneldb.h"

using namespace wg;
using namespace std;

KernelDB::KernelDB()
{
	_resetDB();
}

KernelDB::~KernelDB()
{

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


//____ _resetDB() _____________________________________________________________

void KernelDB::_resetDB()
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

	// Clear all optimized blits (specifics will be set later)

	int simpleBlitMapSize = TintMode_size * BlendMode_size * PixelFormat_size * PixelFormat_size * 2;
	memset(m_simpleBlitMap, 0, simpleBlitMapSize);

	int complexBlitMapSize = TintMode_size * BlendMode_size * PixelFormat_size * PixelFormat_size * 2 * 3;
	memset(m_complexBlitMap, 0, complexBlitMapSize);

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
	
	for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
	{
		if (m_srcFormats[srcFormat])
		{
			for (int normTile = 0; normTile < 2; normTile++)
			{
				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGRA_8_linear)][normTile] = true;
				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGRA_8_sRGB)][normTile] = true;

				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGR_8_linear)][normTile] = true;
				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGR_8_sRGB)][normTile] = true;

				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGRA_8_linear)][normTile] = true;
				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGRA_8_sRGB)][normTile] = true;

				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGR_8_linear)][normTile] = true;
				m_simpleBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGR_8_sRGB)][normTile] = true;
			}
		}
	}

	// Set optimized ComplexBlit methods

	for (int srcFormat = 0; srcFormat < PixelFormat_size; srcFormat++)
	{
		if (m_srcFormats[srcFormat])
		{
			for (int sampleMethod = 0; sampleMethod < 2; sampleMethod++)
			{
				for (int normClipTile = 0; normClipTile < 2; normClipTile++)
				{
					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGRA_8_linear)][sampleMethod][normClipTile] = true;
					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGRA_8_sRGB)][sampleMethod][normClipTile] = true;

					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGR_8_linear)][sampleMethod][normClipTile] = true;
					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Replace)][srcFormat][int(PixelFormat::BGR_8_sRGB)][sampleMethod][normClipTile] = true;

					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGRA_8_linear)][sampleMethod][normClipTile] = true;
					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGRA_8_sRGB)][sampleMethod][normClipTile] = true;

					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGR_8_linear)][sampleMethod][normClipTile] = true;
					m_complexBlitMap[int(TintMode::None)][int(BlendMode::Blend)][srcFormat][int(PixelFormat::BGR_8_sRGB)][sampleMethod][normClipTile] = true;
				}
			}
		}
	}

}
