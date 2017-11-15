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

		switch (header.type)
		{

		case GfxChunkId::OutOfData:
			return false;

		case GfxChunkId::BeginRender:

			m_charStream << "BeginRender";
			break;

		case GfxChunkId::EndRender:

			m_charStream << "EndRender";
			break;

		case GfxChunkId::Fill:
		{
			Rect	rect;
			Color	col;

			*m_pGfxStream >> rect;
			*m_pGfxStream >> col;

			m_charStream << "Fill";
			m_charStream << "    dest = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h;
			m_charStream << "    color = " << col.a << ", " << col.r << ", " << col.g << ", " << col.b;
			break;
		}
/*
				SetCanvas,
				SetTintColor,
				SetBlendMode,
				ClipDrawHorrLine,
				ClipDrawVertLine,
				PlotPixels,
				ClipPlotPixels,
				DrawLine,
				ClipDrawLine,
				ClipDrawHorrWave,
				Blit,
				StretchBlitSubPixel,
				FillSubPixel,

				CreateSurface,
				SetSurfaceScaleMode,
				UpdateSurfaceRegion,
				FillSurface,
				CopySurface
*/
		default:
			//TODO: Signal error somehow.
			return false;

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