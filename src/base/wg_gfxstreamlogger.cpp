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

#include <wg_gfxstreamlogger.h>
#include <assert.h>

namespace wg
{

	const char GfxStreamLogger::CLASSNAME[] = { "GfxStreamLogger" };


	//____ create() ___________________________________________________________

	GfxStreamLogger_p GfxStreamLogger::create(GfxInStream& in, std::ostream& out)
	{
		return new GfxStreamLogger(in, out);
	}

	//____ Constructor _____________________________________________________________

	GfxStreamLogger::GfxStreamLogger(GfxInStream& in, std::ostream& out) : m_charStream(out)
	{
		m_pGfxStream = in.ptr();
	}

	//____ Destructor _________________________________________________________

	GfxStreamLogger::~GfxStreamLogger()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool GfxStreamLogger::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return Object::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * GfxStreamLogger::className(void) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	GfxStreamLogger_p GfxStreamLogger::cast(Object * pObject)
	{
		if (pObject && pObject->isInstanceOf(CLASSNAME))
			return GfxStreamLogger_p(static_cast<GfxStreamLogger*>(pObject));

		return 0;
	}

	//____ isEmpty() __________________________________________________________

	bool GfxStreamLogger::isEmpty() const
	{
		return m_pGfxStream->isEmpty();
	}

	//____ peekChunk() ________________________________________________________

	GfxStream::Header GfxStreamLogger::peekChunk() const
	{
		return m_pGfxStream->peek();
	}

	//____ logAll() ___________________________________________________________

	void GfxStreamLogger::logAll()
	{
		while (logChunk() == true);
	}

	//____ logChunk() ____________________________________________________________

	bool GfxStreamLogger::logChunk()
	{
		GfxStream::Header header;

		*m_pGfxStream >> header;

		if (header.type == GfxChunkId::OutOfData)
			return false;

		if (header.type <= GfxChunkId_max)
			m_charStream << toString(header.type) << std::endl;
		else
			m_charStream << "Unknown Chunk Type: " << (int) header.type << std::endl;

		switch (header.type)
		{
			case GfxChunkId::BeginRender:
				break;

			case GfxChunkId::EndRender:
				break;

			case GfxChunkId::SetCanvas:
			{
				uint16_t	surfaceId;
				*m_pGfxStream >> surfaceId;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				break;
			}

			case GfxChunkId::SetClip:
			{
				int		nRects = header.size / 8;
				m_charStream << "    number of rects: " << nRects << std::endl;

				Rect	rect;
				for (int i = 0; i < nRects; i++)
				{
					*m_pGfxStream >> rect;
					m_charStream << "    clip rects     = [" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << "]" << std::endl;
				}
				break;
			}

			case GfxChunkId::SetTintColor:
			{
				Color	col;
				*m_pGfxStream >> col;

				m_charStream << "    color       = " << (int)col.a << ", " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << std::endl;
				break;
			}

			case GfxChunkId::SetBlendMode:
			{
				BlendMode	blendMode;
				*m_pGfxStream >> blendMode;

				m_charStream << "    mode        = " << (int)blendMode << std::endl;
				break;
			}

			case GfxChunkId::SetBlitSource:
			{
				uint16_t	surfaceId;
				*m_pGfxStream >> surfaceId;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				break;
			}

			case GfxChunkId::Fill:
			{
				Rect	rect;
				Color	col;

				*m_pGfxStream >> rect;
				*m_pGfxStream >> col;

				m_charStream << "    dest        = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
				m_charStream << "    color       = " << (int)col.a << ", " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << std::endl;
				break;
			}

			case GfxChunkId::FillSubpixel:
			{
				RectF	rect;
				Color	col;

				*m_pGfxStream >> rect;
				*m_pGfxStream >> col;

				m_charStream << "    dest        = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
				m_charStream << "    color       = " << (int)col.a << ", " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << std::endl;
				break;
			}

			case GfxChunkId::PlotPixels:
			{
				m_pGfxStream->skip(header.size);

				m_charStream << "    number of pixels: " << header.size/8 << std::endl;
				break;
			}

			case GfxChunkId::DrawLineFromTo:
			{
				Coord	begin;
				Coord	end;
				Color	color;
				float	thickness;

				*m_pGfxStream >> begin;
				*m_pGfxStream >> end;
				*m_pGfxStream >> color;
				*m_pGfxStream >> thickness;

				m_charStream << "    begin       = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    end         = " << end.x << ", " << end.y << std::endl;
				m_charStream << "    color       = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				m_charStream << "    thickness   = " << thickness << std::endl;
				break;
			}


			case GfxChunkId::DrawLineStraight:
			{
				Coord		begin;
				Direction	dir;
				uint16_t	length;
				Color		color;
				float		thickness;

				*m_pGfxStream >> begin;
				*m_pGfxStream >> dir;
				*m_pGfxStream >> length;
				*m_pGfxStream >> color;
				*m_pGfxStream >> thickness;

				m_charStream << "    begin       = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    direction   = " << toString(dir) << std::endl;
				m_charStream << "    length      = " << length << std::endl;
				m_charStream << "    color       = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				m_charStream << "    thickness   = " << thickness << std::endl;
				break;
			}

			case GfxChunkId::Blit:
			{
				Coord		dest;
				Rect		source;

				*m_pGfxStream >> dest;
				*m_pGfxStream >> source;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				m_charStream << "    source      = " << source.x << ", " << source.y << ", " << source.w << ", " << source.h << std::endl;
				break;
			}

			case GfxChunkId::StretchBlit:
			{
				Coord		dest;
				RectF		source;

				*m_pGfxStream >> dest;
				*m_pGfxStream >> source;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				m_charStream << "    source      = " << source.x << ", " << source.y << ", " << source.w << ", " << source.h << std::endl;
				break;
			}

			case GfxChunkId::SimpleTransformBlit:
			{
				Rect		dest;
				Coord		src;
				int			transform[2][2];

				*m_pGfxStream >> dest;
				*m_pGfxStream >> src;
				*m_pGfxStream >> transform;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << ", " << dest.w << ", " << dest.h << std::endl;
				m_charStream << "    src         = " << src.x << ", " << src.y << std::endl;
				m_charStream << "    transform   = [ " << transform[0][0] << ", " << transform[0][1] << " ]" << std::endl;
				m_charStream << "                  [ " << transform[1][0] << ", " << transform[1][1] << " ]" << std::endl;
				break;
			}

			case GfxChunkId::ComplexTransformBlit:
			{
				Rect		dest;
				CoordF		src;
				float		transform[2][2];

				*m_pGfxStream >> dest;
				*m_pGfxStream >> src;
				*m_pGfxStream >> transform;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << ", " << dest.w << ", " << dest.h << std::endl;
				m_charStream << "    src         = " << src.x << ", " << src.y << std::endl;
				m_charStream << "    transform   = [ " << transform[0][0] << ", " << transform[0][1] << " ]" << std::endl;
				m_charStream << "                  [ " << transform[1][0] << ", " << transform[1][1] << " ]" << std::endl;
				break;
			}

			case GfxChunkId::TransformDrawSegments:
			{
				Rect		dest;
				uint16_t	nSegments;
				uint16_t	nEdgeStrips;
				int			transform[2][2];


				(*m_pGfxStream) >> dest;
				(*m_pGfxStream) >> nSegments;
				(*m_pGfxStream) >> nEdgeStrips;
				(*m_pGfxStream) >> transform;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << ", " << dest.w << ", " << dest.h << std::endl;
				m_charStream << "    nSegments   = " << nSegments << std::endl;
				m_charStream << "    nEdgeStrips = " << nEdgeStrips << std::endl;
				m_charStream << "    transform   = [ " << transform[0][0] << ", " << transform[0][1] << " ]" << std::endl;
				m_charStream << "                  [ " << transform[1][0] << ", " << transform[1][1] << " ]" << std::endl;


				m_charStream << "    seg colors  = ";
				for (int i = 0; i < nSegments; i++)
				{
					Color color;
					(*m_pGfxStream) >> color;

					m_charStream << "{ " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << " } ";

				}
				m_charStream << std::endl;
				break;
			}

			case GfxChunkId::EdgeSamples:
			{
				m_pGfxStream->skip(header.size);
				m_charStream << "    nSamples    = " << (int)header.size/2 << std::endl;
				break;
			}

			case GfxChunkId::CreateSurface:
			{
				uint16_t	surfaceId;
				PixelFormat	type;
				Size		size;
				uint16_t	flags;

				*m_pGfxStream >> surfaceId;
				*m_pGfxStream >> type;
				*m_pGfxStream >> size;
				*m_pGfxStream >> flags;


				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    type        = " << toString(type) << std::endl;
				m_charStream << "    size        = " << size.w << ", " << size.h << std::endl;
				m_charStream << "    flags       = " << flags << std::endl;
				break;
			}

			case GfxChunkId::DeleteSurface:
			{
				uint16_t	surfaceId;
				*m_pGfxStream >> surfaceId;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				break;
			}

			case GfxChunkId::SetSurfaceScaleMode:
			{
				uint16_t	surfaceId;
				ScaleMode	scaleMode;

				*m_pGfxStream >> surfaceId;
				*m_pGfxStream >> scaleMode;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    scaleMode   = " << toString(scaleMode) << std::endl;
				break;
			}

			case GfxChunkId::BeginSurfaceUpdate:
			{
				uint16_t	surfaceId;
				Rect		region;

				*m_pGfxStream >> surfaceId;
				*m_pGfxStream >> region;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    region      = " << region.x << ", " << region.y << ", " << region.w << ", " << region.h << std::endl;
				break;
			}

			case GfxChunkId::SurfaceData:
			{
				m_pGfxStream->skip(header.size);

				m_charStream << "    size: " << header.size << " bytes." << std::endl;
				break;
			}

			case GfxChunkId::EndSurfaceUpdate:
			{
				break;
			}

			case GfxChunkId::FillSurface:
			{
				uint16_t	surfaceId;
				Rect		region;
				Color		col;

				*m_pGfxStream >> surfaceId;
				*m_pGfxStream >> region;
				*m_pGfxStream >> col;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    region      = " << region.x << ", " << region.y << ", " << region.w << ", " << region.h << std::endl;
				m_charStream << "    color       = " << (int)col.a << ", " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << std::endl;
				break;
			}

			case GfxChunkId::CopySurface:
			{
				uint16_t	destSurfaceId;
				uint16_t	sourceSurfaceId;
				Rect		sourceRect;
				Coord		dest;

				*m_pGfxStream >> destSurfaceId;
				*m_pGfxStream >> sourceSurfaceId;
				*m_pGfxStream >> sourceRect;
				*m_pGfxStream >> dest;

				m_charStream << "    destSurface = " << destSurfaceId << std::endl;
				m_charStream << "    srcSurface  = " << sourceSurfaceId << std::endl;
				m_charStream << "    source      = " << sourceRect.x << ", " << sourceRect.y << ", " << sourceRect.w << ", " << sourceRect.h << std::endl;
				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				break;
			}

			default:
			{
				m_pGfxStream->skip(header.size);

				m_charStream << "    size: " << (int)header.size << std::endl;
				break;
			}
		}

		return true;
	}

	//____ logFrame() _________________________________________________________

	bool GfxStreamLogger::logFrame()
	{
		GfxStream::Header	header = peekChunk();

		while (header.type != GfxChunkId::OutOfData)
		{
			logChunk();
			if (header.type == GfxChunkId::EndRender)
				return true;
		}
		return false;
	}

	//____ readPrintPatches() _________________________________________________

	int GfxStreamLogger::_readPrintPatches()
	{
		uint16_t	nPatches;
		Rect		patch;

		*m_pGfxStream >> nPatches;

		if (nPatches == 0)
		{
			m_charStream << "    zero patches!" << std::endl;
			return 0;
		}

		*m_pGfxStream >> patch;
		m_charStream << "    patches     = " << patch.x << ", " << patch.y << ", " << patch.w << ", " << patch.h << std::endl;

		for (int i = 1; i < nPatches; i++)
		{
			*m_pGfxStream >> patch;
			m_charStream << "                  " << patch.x << ", " << patch.y << ", " << patch.w << ", " << patch.h << std::endl;
		}

		return nPatches;
	}



} //namespace wg
