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

#include <wg_gfxstreamplayer.h>
#include <assert.h>

namespace wg
{

	const char GfxStreamPlayer::CLASSNAME[] = { "GfxStreamPlayer" };


	//____ create() ___________________________________________________________

	GfxStreamPlayer_p GfxStreamPlayer::create(GfxInStream& in, GfxDevice * pDevice, SurfaceFactory * pFactory)
	{
		return new GfxStreamPlayer(in, pDevice, pFactory);
	}

	//____ Constructor _____________________________________________________________

	GfxStreamPlayer::GfxStreamPlayer(GfxInStream& in, GfxDevice * pDevice, SurfaceFactory * pFactory)
	{
		m_pStream = in.ptr();
		m_pDevice = pDevice;
		m_pSurfaceFactory = pFactory;
	}

	//____ Destructor _________________________________________________________

	GfxStreamPlayer::~GfxStreamPlayer()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool GfxStreamPlayer::isInstanceOf(const char * pClassName) const
	{
		if (pClassName == CLASSNAME)
			return true;

		return Object::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * GfxStreamPlayer::className(void) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	GfxStreamPlayer_p GfxStreamPlayer::cast(Object * pObject)
	{
		if (pObject && pObject->isInstanceOf(CLASSNAME))
			return GfxStreamPlayer_p(static_cast<GfxStreamPlayer*>(pObject));

		return 0;
	}

	//____ isEmpty() __________________________________________________________

	bool GfxStreamPlayer::isEmpty() const
	{
		return m_pStream->isEmpty();
	}

	//____ peekChunk() ________________________________________________________

	GfxStream::Header GfxStreamPlayer::peekChunk() const
	{
		return m_pStream->peek();
	}

	//____ playAll() ___________________________________________________________

	void GfxStreamPlayer::playAll()
	{
		while (playChunk() == true);
	}

	//____ playChunk() ____________________________________________________________

	bool GfxStreamPlayer::playChunk()
	{
		GfxStream::Header header;

		*m_pStream >> header;

		switch (header.type)
		{

		case GfxChunkId::OutOfData:
			return false;

		case GfxChunkId::BeginRender:
			m_pDevice->beginRender();
			break;

		case GfxChunkId::EndRender:
			m_pDevice->endRender();
			break;

		case GfxChunkId::Fill:
		{
			Rect	rect;
			Color	col;

			*m_pStream >> rect;
			*m_pStream >> col;

			m_pDevice->fill(rect, col);
			break;
		}

		case GfxChunkId::SetCanvas:
			//TODO: Implement!
			break;

		case GfxChunkId::SetTintColor:
			//TODO: Implement!
			break;

		case GfxChunkId::SetBlendMode:
		{
			BlendMode	blendMode;
			*m_pStream >> blendMode;
			m_pDevice->setBlendMode(blendMode);
			break;
		}

		case GfxChunkId::DrawStraightLine:
		{
			Coord		begin;
			Orientation	orientation;
			uint16_t	length;
			Color		color;

			*m_pStream >> begin;
			*m_pStream >> orientation;
			*m_pStream >> length;
			*m_pStream >> color;

			m_pDevice->_drawStraightLine(begin, orientation, length, color);
			break;
		}

		case GfxChunkId::PlotPixels:
			//TODO: Implement!
			break;

		case GfxChunkId::ClipPlotPixels:
			//TODO: Implement!
			break;

		case GfxChunkId::DrawLine:
		{
			Coord	begin;
			Coord	end;
			Color	color;
			float	thickness;

			*m_pStream >> begin;
			*m_pStream >> end;
			*m_pStream >> color;
			*m_pStream >> thickness;

			m_pDevice->drawLine(begin, end, color, thickness);
			break;
		}

		case GfxChunkId::ClipDrawLine:
		{
			Rect	clip;
			Coord	begin;
			Coord	end;
			Color	color;
			float	thickness;

			*m_pStream >> clip;
			*m_pStream >> begin;
			*m_pStream >> end;
			*m_pStream >> color;
			*m_pStream >> thickness;

			m_pDevice->clipDrawLine(clip, begin, end, color, thickness);
			break;
		}

		case GfxChunkId::ClipDrawHorrWave:
			//TODO: Implement!
			break;

		case GfxChunkId::Blit:
			//TODO: Implement!
			break;

		case GfxChunkId::StretchBlitSubPixel:
			//TODO: Implement!
			break;

		case GfxChunkId::FillSubPixel:
		{
			RectF	rect;
			Color	col;

			*m_pStream >> rect;
			*m_pStream >> col;

			m_pDevice->fillSubPixel(rect, col);
			break;
		}

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

		default:
			//TODO: Signal error somehow.
			return false;
		}

		return true;
	}

	//____ playFrame() _________________________________________________________

	bool GfxStreamPlayer::playFrame()
	{
		GfxStream::Header	header = peekChunk();

		while (header.type != GfxChunkId::OutOfData)
		{
			playChunk();
			if (header.type == GfxChunkId::EndRender)
				return true;
			header = peekChunk();
		}
		return false;
	}


} //namespace wg