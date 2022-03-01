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

#ifndef	WG_GFXSTREAMPLAYER_DOT_H
#define	WG_GFXSTREAMPLAYER_DOT_H
#pragma once

#include <wg_object.h>
#include <wg_gfxstream.h>
#include <wg_cgfxoutstream.h>
#include <wg_gfxstreamdecoder.h>
#include <wg_gfxdevice.h>
#include <wg_patches.h>

#include <vector>
#include <tuple>

namespace wg
{

	class GfxStreamPlayer;
	typedef	StrongPtr<GfxStreamPlayer>	GfxStreamPlayer_p;
	typedef	WeakPtr<GfxStreamPlayer>	GfxStreamPlayer_wp;

	class GfxStreamPlayer : public Object, protected CGfxOutStream::Holder
	{
	public:

		//.____ Creation __________________________________________

		static GfxStreamPlayer_p	create(GfxDevice * pDevice, SurfaceFactory * pFactory);

		//.____ Components _______________________________________

		CGfxOutStream		input;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Misc _____________________________________________________

		void	setStoreDirtyRects(bool bStore);
		void	setMaxDirtyRects(int max);

		std::tuple<int, const RectI*> dirtyRects(CanvasRef canvas);
		void	clearDirtyRects();

	protected:
		GfxStreamPlayer(GfxDevice * pDevice, SurfaceFactory * pFactory);
		~GfxStreamPlayer();

		Object* _object() override;
		const Object* _object() const override;

		void	_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd) override;

		bool	_playChunk();

		RectI *	_pushClipList(int nRects);
		RectI *	_setClipList(int nRects);
		void	_popClipList();

		GfxStreamDecoder_p	m_pDecoder;
		GfxDevice_p			m_pDevice;
		SurfaceFactory_p	m_pSurfaceFactory;

		std::vector<Surface_p>	m_vSurfaces;

		Surface_p			m_pUpdatingSurface;
		PixelBuffer			m_pixelBuffer;
		uint8_t *			m_pWritePixels;
		int					m_surfaceBytesLeft = 0;

		// For multi-chunk drawing operations (DrawSegments, FlipDrawSegments, DrawWave and FlipDrawWave), telling which one we are receiving edge samples for.
		
		GfxChunkId			m_drawTypeInProgress = GfxChunkId::OutOfData;
		
		// Temporary storage for DrawSegments and FlipDrawSegments.

		struct SegmentInfo
		{
			RectI		dest;
			int			nSegments;
			HiColor *	pSegmentColors;
			int			nEdgeStrips;
			int *		pEdgeStrips;
			int			edgeStripPitch;
			GfxFlip		flip;				// Only used when m_drawTypeInProgress == flipDrawSegments
			TintMode	tintMode;
			
		};

		// Temporary storage for DrawWave and FlipDrawWave.
		
		struct WaveInfo
		{
			RectI		dest;
			WaveLine	topBorder;
			WaveLine	bottomBorder;
			HiColor		frontFill;
			HiColor		backFill;
			GfxFlip		flip;			// Onlu used when m_drawTypeInProgress == flipDrawWave.
		};
		
		SegmentInfo		m_seg;
		WaveInfo		m_wave;

		char *	m_pTempBuffer = nullptr;
		int		m_bytesLoaded;
		int		m_bufferSize;

		struct ClipRectsBuffer
		{
			RectI*	pRects;
			int		nRects;
			int		capacity;
		};

		constexpr static int c_clipListBufferSize = 512;

		std::vector<int>	m_clipListSizes;		// Number of rects for each clipList pushed.

		ClipRectsBuffer		m_clipListBuffer = { nullptr, 0, 0 };
		int					m_currentClipListSize = 0;

		std::vector<ClipRectsBuffer>	m_clipListBufferStack;

		bool				m_bStoreDirtyRects = false;
		int					m_maxDirtyRects = 64;
		PatchesSPX			m_dirtyRects[CanvasRef_size];
	};

}

#endif // WG_GFXSTREAMPLAYER_DOT_H
