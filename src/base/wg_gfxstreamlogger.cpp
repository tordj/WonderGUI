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

			case GfxChunkId::Fill:
			{
				Rect	rect;
				Color	col;

				*m_pGfxStream >> rect;
				*m_pGfxStream >> col;

				m_charStream << "    dest        = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
				m_charStream << "    color       = " << (int) col.a << ", " << (int) col.r << ", " << (int) col.g << ", " << (int) col.b << std::endl;
				break;
			}

			case GfxChunkId::SetCanvas:
			{
				uint16_t	surfaceId;
				*m_pGfxStream >> surfaceId;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
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

	
			case GfxChunkId::PlotPixels:
			{
				m_pGfxStream->skip(header.size);

				m_charStream << "    number of pixels: " << header.size/8 << std::endl;
				break;
			}

			case GfxChunkId::DrawLine:
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


			case GfxChunkId::ClipDrawLine:
			{
				Rect	clip;
				Coord	begin;
				Coord	end;
				Color	color;
				float	thickness;

				*m_pGfxStream >> clip;
				*m_pGfxStream >> begin;
				*m_pGfxStream >> end;
				*m_pGfxStream >> color;
				*m_pGfxStream >> thickness;

				m_charStream << "    clip        = " << clip.x << ", " << clip.y << ", " << clip.w << ", " << clip.h << std::endl;
				m_charStream << "    begin       = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    end         = " << end.x << ", " << end.y << std::endl;
				m_charStream << "    color       = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				m_charStream << "    thickness   = " << thickness << std::endl;
				break;
			}

			case GfxChunkId::ClipDrawLine2:
			{
				Rect		clip;
				Coord		begin;
				Direction	dir;
				uint16_t	length;
				Color		color;
				float		thickness;

				*m_pGfxStream >> clip;
				*m_pGfxStream >> begin;
				*m_pGfxStream >> dir;
				*m_pGfxStream >> length;
				*m_pGfxStream >> color;
				*m_pGfxStream >> thickness;

				m_charStream << "    clip        = " << clip.x << ", " << clip.y << ", " << clip.w << ", " << clip.h << std::endl;
				m_charStream << "    begin       = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    direction   = " << toString(dir) << std::endl;
				m_charStream << "    length      = " << length << std::endl;
				m_charStream << "    color       = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				m_charStream << "    thickness   = " << thickness << std::endl;
				break;
			}


/*
			case GfxChunkId::ClipDrawHorrWave:
				//TODO: Implement!
				break;
*/
			case GfxChunkId::Blit:
			{
				uint16_t	surfaceId;
				Rect		source;
				Coord		dest;

				*m_pGfxStream >> surfaceId;
				*m_pGfxStream >> source;
				*m_pGfxStream >> dest;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    source      = " << source.x << ", " << source.y << ", " << source.w << ", " << source.h << std::endl;
				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				break;
			}

			case GfxChunkId::StretchBlit:
			{
				uint16_t	surfaceId;
				RectF		source;
				Coord		dest;

				*m_pGfxStream >> surfaceId;
				*m_pGfxStream >> source;
				*m_pGfxStream >> dest;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    source      = " << source.x << ", " << source.y << ", " << source.w << ", " << source.h << std::endl;
				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				break;
			}
			
			case GfxChunkId::FillSubPixel:
			{
				RectF	rect;
				Color	col;

				*m_pGfxStream >> rect;
				*m_pGfxStream >> col;

				m_charStream << "    dest        = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
				m_charStream << "    color       = " << (int)col.a << ", " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << std::endl;
				break;
			}

			case GfxChunkId::CreateSurface:
			{
				uint16_t	surfaceId;
				PixelType	type;
				Size		size;

				*m_pGfxStream >> surfaceId;
				*m_pGfxStream >> type;
				*m_pGfxStream >> size;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    type        = " << toString(type) << std::endl;
				m_charStream << "    size        = " << size.w << ", " << size.h << std::endl;
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


} //namespace wg