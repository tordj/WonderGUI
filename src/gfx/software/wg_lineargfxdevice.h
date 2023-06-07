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
#ifndef WG_LINEOUTGFXDEVICE_DOT_H
#define WG_LINEOUTGFXDEVICE_DOT_H
#pragma once

#include <wg_softgfxdevice.h>

namespace wg
{


	class LinearGfxDevice;
	typedef	StrongPtr<LinearGfxDevice>	LinearGfxDevice_p;
	typedef	WeakPtr<LinearGfxDevice>	LinearGfxDevice_wp;

	class LinearGfxDevice : public SoftGfxDevice
	{
	public:

		struct Segment
		{
			RectI		rect;			// In pixels, not spx!
			uint8_t *	pBuffer;
			int 		pitch;
		};
		
		//.____ Creation __________________________________________

		static LinearGfxDevice_p	create( std::function<void*(CanvasRef ref, int nBytes)> beginCanvasRender,
										   std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> endCanvasRender );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Misc _______________________________________________________

		bool		defineCanvas( CanvasRef ref, const SizeSPX size, PixelFormat pixelFormat, int scale = 64 );

		using SoftGfxDevice::canvas;
		const CanvasInfo canvas(CanvasRef ref) const override;


		//.____ State _________________________________________________

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;
		
		void    endCanvasUpdate() override;

		using 	SoftGfxDevice::fill;
		void	fill(const RectSPX& rect, HiColor col) override;

/*

		void    plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors) override;

		void	drawLine(CoordSPX begin, CoordSPX end, HiColor color, spx thickness = 64) override;
		void	drawLine(CoordSPX begin, Direction dir, spx length, HiColor col, spx thickness = 64) override;

		bool	setBlitSource(Surface * pSource) override;
		void	transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2]) override;
		void	rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f,0.5f }, CoordF destCenter = { 0.5f,0.5f } ) override;

		void	tile(const RectSPX& dest, CoordSPX shift = { 0,0 }) override;
		void	flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift = { 0,0 }) override;

		void	scaleTile(const RectSPX& dest, float scale, CoordSPX shift = { 0,0 }) override;
		void	scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift = { 0,0 }) override;

		void	drawWaveform(CoordSPX dest, Waveform * pWaveform ) override;
		void	flipDrawWaveform(CoordSPX dest, Waveform * pWaveform, GfxFlip flip) override;
*/


		//.____ Internal _____________________________________________________

		

	protected:
		LinearGfxDevice( std::function<void*(CanvasRef ref, int nBytes)> beginCanvasRender,
						 std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> endCanvasRender );
		~LinearGfxDevice();

		void	_canvasWasChanged() override;
		void 	_clipListWasChanged() override;

		bool 	_beginCanvasUpdate(CanvasRef ref, Surface * pCanvas, int nUpdateRects,
								   const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer) override;

		
		std::function<void*(CanvasRef ref, int nBytes)> m_beginCanvasRenderCallback;
		std::function<void(CanvasRef ref, int nSegments, const Segment * pSegments)> m_endCanvasRenderCallback;

		CanvasInfo					m_canvasDefinitions[CanvasRef_size];

		std::vector<Segment>		m_canvasSegments;
		std::vector<Segment>		m_clipSegments;

		Segment *					m_pClipSegments;
		int							m_nClipSegments;
		
		int							m_canvasPixelBytes;
		int							m_nbSurfaceCanvases = 0;
		

	};


} // namespace wg
#endif //WG_LINEOUTGFXDEVICE_DOT_H

