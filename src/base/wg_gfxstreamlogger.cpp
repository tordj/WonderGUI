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
		m_pWorkArea = new char[GfxStream::c_maxBlockSize];
	}

	//____ Destructor _________________________________________________________

	GfxStreamLogger::~GfxStreamLogger()
	{
		delete[] m_pWorkArea;
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

		switch (header.type)
		{

			case GfxChunkId::OutOfData:
				return false;

			case GfxChunkId::BeginRender:

				m_charStream << "BeginRender" << std::endl;
				break;

			case GfxChunkId::EndRender:

				m_charStream << "EndRender" << std::endl;
				break;

			case GfxChunkId::Fill:
			{
				Rect	rect;
				Color	col;

				*m_pGfxStream >> rect;
				*m_pGfxStream >> col;

				m_charStream << "Fill" << std::endl;
				m_charStream << "    dest = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
				m_charStream << "    color = " << (int) col.a << ", " << (int) col.r << ", " << (int) col.g << ", " << (int) col.b << std::endl;
				break;
			}
	/*
			case GfxChunkId::SetCanvas:
				//TODO: Implement!
				break;

			case GfxChunkId::SetTintColor:
				//TODO: Implement!
				break;
	*/
			case GfxChunkId::SetBlendMode:
			{
				BlendMode	blendMode;
				*m_pGfxStream >> blendMode;

				m_charStream << "SetBlendMode" << std::endl;
				m_charStream << "    mode = " << (int)blendMode << std::endl;
				break;
			}

			case GfxChunkId::ClipDrawHorrLine:
			{
				Rect	clip;
				Coord	begin;
				int		length;
				Color	color;

				*m_pGfxStream >> clip;
				*m_pGfxStream >> begin;
				*m_pGfxStream >> length;
				*m_pGfxStream >> color;

				m_charStream << "ClipDrawHorrLine" << std::endl;
				m_charStream << "    clip = " << clip.x << ", " << clip.y << ", " << clip.w << ", " << clip.h << std::endl;
				m_charStream << "    begin = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    length = " << length << std::endl;
				m_charStream << "    color = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				break;
			}

			case GfxChunkId::ClipDrawVertLine:
			{
				Rect	clip;
				Coord	begin;
				int		length;
				Color	color;

				*m_pGfxStream >> clip;
				*m_pGfxStream >> begin;
				*m_pGfxStream >> length;
				*m_pGfxStream >> color;

				m_charStream << "ClipDrawVertLine" << std::endl;
				m_charStream << "    clip = " << clip.x << ", " << clip.y << ", " << clip.w << ", " << clip.h << std::endl;
				m_charStream << "    begin = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    length = " << length << std::endl;
				m_charStream << "    color = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				break;
			}
	/*
			case GfxChunkId::PlotPixels:
				//TODO: Implement!
				break;

			case GfxChunkId::ClipPlotPixels:
				//TODO: Implement!
				break;
	*/
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

				m_charStream << "DrawLine" << std::endl;
				m_charStream << "    begin = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    end = " << end.x << ", " << end.y << std::endl;
				m_charStream << "    color = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				m_charStream << "    thickness = " << thickness << std::endl;
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

				m_charStream << "ClipDrawLine" << std::endl;
				m_charStream << "    clip = " << clip.x << ", " << clip.y << ", " << clip.w << ", " << clip.h << std::endl;
				m_charStream << "    begin = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    end = " << end.x << ", " << end.y << std::endl;
				m_charStream << "    color = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
				m_charStream << "    thickness = " << thickness << std::endl;
				break;
			}

	/*
			case GfxChunkId::ClipDrawHorrWave:
				//TODO: Implement!
				break;

			case GfxChunkId::Blit:
				//TODO: Implement!
				break;

			case GfxChunkId::StretchBlitSubPixel:
				//TODO: Implement!
				break;
	*/
			case GfxChunkId::FillSubPixel:
			{
				RectF	rect;
				Color	col;

				*m_pGfxStream >> rect;
				*m_pGfxStream >> col;

				m_charStream << "Fill" << std::endl;
				m_charStream << "    dest = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
				m_charStream << "    color = " << (int)col.a << ", " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << std::endl;
				break;
			}
	/*
			case GfxChunkId::CreateSurface:
				//TODO: Implement!
				break;

			case GfxChunkId::SetSurfaceScaleMode:
				//TODO: Implement!
				break;

			case GfxChunkId::BeginSurfaceUpdate:
				//TODO: Implement!
				break;

				case GfxChunkId::SurfaceData:
				//TODO: Implement!
				break;
	
			case GfxChunkId::EndSurfaceUpdate:
				//TODO: Implement!
				break;

			case GfxChunkId::FillSurface:
				//TODO: Implement!
				break;

			case GfxChunkId::CopySurface:
				//TODO: Implement!
				break;
	*/
			default:
			{
				m_charStream << "Unknown Chunk!!!!!!!!!!!!!!!!!" << std::endl;
				m_charStream << "    type = " << (int)header.type << std::endl;
				m_charStream << "    size = " << (int)header.size << std::endl;

				*m_pGfxStream >> GfxStream::DataChunk{ header.size, m_pWorkArea };
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