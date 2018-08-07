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
#include <wg_base.h>
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
		{
			uint16_t	surfaceId;
			*m_pStream >> surfaceId;
			m_pDevice->setCanvas(m_vSurfaces[surfaceId]);
			break;
		}

		case GfxChunkId::SetTintColor:
		{
			Color	col;
			*m_pStream >> col;
			m_pDevice->setTintColor(col);
			break;
		}

		case GfxChunkId::SetBlendMode:
		{
			BlendMode	blendMode;
			*m_pStream >> blendMode;
			m_pDevice->setBlendMode(blendMode);
			break;
		}

		case GfxChunkId::PlotPixels:
		{
			int nPixels = header.size / 8;

			int bufferSize = header.size*3/2;			// Pixels needs to be expanded, but not colors
			char * pBuffer = reinterpret_cast<char*>(Base::memStackAlloc(bufferSize));

			// Load all data to end of buffer

			*m_pStream >> GfxStream::DataChunk{ header.size, pBuffer + bufferSize - header.size };

			// Unpack Coordinates

			Coord * pDest = (Coord*)pBuffer;
			int16_t * pSrc = (int16_t*) (pBuffer + bufferSize - header.size);

			for (int i = 0; i < nPixels; i++)
			{
				pDest[i].x = *pSrc++;
				pDest[i].y = *pSrc++;
			}

			//

			m_pDevice->plotPixels(nPixels, (Coord*)pBuffer, (Color*)(pBuffer + header.size / 2));

			Base::memStackRelease(bufferSize);
			break;
		}

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

		case GfxChunkId::ClipDrawLine2:
		{
			Rect		clip;
			Coord		begin;
			Direction	dir;
			uint16_t	length;
			Color		color;
			float		thickness;

			*m_pStream >> clip;
			*m_pStream >> begin;
			*m_pStream >> dir;
			*m_pStream >> length;
			*m_pStream >> color;
			*m_pStream >> thickness;

			m_pDevice->clipDrawLine(clip, begin, dir, length, color, thickness);
			break;
		}

		case GfxChunkId::ClipDrawHorrWave:
			//TODO: Implement!
			break;

		case GfxChunkId::Blit:
		{
			uint16_t	surfaceId;
			Rect		source;
			Coord		dest;

			*m_pStream >> surfaceId;
			*m_pStream >> source;
			*m_pStream >> dest;

			m_pDevice->blit(m_vSurfaces[surfaceId], source, dest);
			break;
		}

		case GfxChunkId::StretchBlit:
		{
			uint16_t	surfaceId;
			RectF		source;
			Rect		dest;

			*m_pStream >> surfaceId;
			*m_pStream >> source;
			*m_pStream >> dest;

			m_pDevice->stretchBlit(m_vSurfaces[surfaceId], source, dest);
			break;
		}

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
		{
			uint16_t	surfaceId;
			PixelFormat	type;
			Size		size;

			*m_pStream >> surfaceId;
			*m_pStream >> type;
			*m_pStream >> size;

			Color * pClut = nullptr;

			if (header.size > 4096)
			{
				pClut = (Color*) Base::memStackAlloc(4096);
				*m_pStream >> GfxStream::DataChunk{ 4096, pClut };
			}

			if (m_vSurfaces.size() <= surfaceId)
				m_vSurfaces.resize(surfaceId + 1, nullptr);

			m_vSurfaces[surfaceId] = m_pSurfaceFactory->createSurface(size, type, 0, pClut);

			if (pClut)
				Base::memStackRelease(4096);

			break;
		}

		case GfxChunkId::DeleteSurface:
		{
			uint16_t	surfaceId;

			*m_pStream >> surfaceId;

			m_vSurfaces[surfaceId] = nullptr;
			break;
		}

		case GfxChunkId::SetSurfaceScaleMode:
		{
			uint16_t	surfaceId;
			ScaleMode	scaleMode;

			*m_pStream >> surfaceId;
			*m_pStream >> scaleMode;

			m_vSurfaces[surfaceId]->setScaleMode(scaleMode);
			break;
		}

		case GfxChunkId::BeginSurfaceUpdate:
		{
			uint16_t	surfaceId;
			Rect		rect;

			*m_pStream >> surfaceId;
			*m_pStream >> rect;

			m_pUpdatingSurface = m_vSurfaces[surfaceId];
			m_pWritePixels = m_pUpdatingSurface->lockRegion(AccessMode::WriteOnly, rect);
			break;
		}

		case GfxChunkId::SurfaceData:
		{
			*m_pStream >> GfxStream::DataChunk{ header.size, m_pWritePixels };
			m_pWritePixels += header.size;
			break;
		}

		case GfxChunkId::EndSurfaceUpdate:
		{
			m_pUpdatingSurface->unlock();
			m_pUpdatingSurface = nullptr;
			break;
		}

		case GfxChunkId::FillSurface:
		{
			uint16_t	surfaceId;
			Rect		region;
			Color		col;

			*m_pStream >> surfaceId;
			*m_pStream >> region;
			*m_pStream >> col;

			m_vSurfaces[surfaceId]->fill(col, region);
			break;
		}
		
		case GfxChunkId::CopySurface:
		{
			uint16_t	destSurfaceId;
			uint16_t	sourceSurfaceId;
			Rect		sourceRect;
			Coord		dest;

			*m_pStream >> destSurfaceId;
			*m_pStream >> sourceSurfaceId;
			*m_pStream >> sourceRect;
			*m_pStream >> dest;

			Surface * pDest	  = m_vSurfaces[destSurfaceId];
			Surface * pSource = m_vSurfaces[sourceSurfaceId];

			pDest->copyFrom(pSource, sourceRect, dest);
			break;
		}

		default:
			// We don't know how to handle this, so let's just skip it

			m_pStream->skip(header.size);
			break;
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