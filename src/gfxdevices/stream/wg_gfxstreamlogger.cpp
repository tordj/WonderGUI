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
#include <wg_gradient.h>
#include <wg_gfxdevice.h>

#include <assert.h>

namespace wg
{

	const TypeInfo GfxStreamLogger::TYPEINFO = { "GfxStreamLogger", &Object::TYPEINFO };


	//____ create() ___________________________________________________________

	GfxStreamLogger_p GfxStreamLogger::create(std::ostream& out)
	{
		return new GfxStreamLogger(out);
	}

	//____ constructor _____________________________________________________________

	GfxStreamLogger::GfxStreamLogger(std::ostream& out) : m_charStream(out), input(this)
	{
		out << std::boolalpha;

		m_pDecoder = GfxStreamDecoder::create();

	}

	//____ Destructor _________________________________________________________

	GfxStreamLogger::~GfxStreamLogger()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamLogger::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _object() _____________________________________________________________

	Object* GfxStreamLogger::_object()
	{
		return this;
	}

	const Object* GfxStreamLogger::_object() const
	{
		return this;
	}

	//____ _processStreamChunks() _____________________________________________

	void GfxStreamLogger::_processStreamChunks(const uint8_t* pBegin, const uint8_t* pEnd)
	{
		m_pDecoder->setInput(pBegin, pEnd);

		while (_logChunk() == true);
	}

	//____ _logChunk() ____________________________________________________________

	bool GfxStreamLogger::_logChunk()
	{
		GfxStream::Header header;

		*m_pDecoder >> header;

		if (header.type == GfxChunkId::OutOfData)
			return false;


		switch (header.type)
		{
			case GfxChunkId::BeginRender:
				m_charStream << "BeginRender" << std::endl;
				break;

			case GfxChunkId::EndRender:
				m_charStream << "EndRender" << std::endl;
				break;

			case GfxChunkId::Flush:
				m_charStream << "Flush" << std::endl;
				break;

			case GfxChunkId::BeginCanvasUpdate:
			{
				m_charStream << "BeginCanvasUpdate" << std::endl;

				CanvasRef	canvasRef;
				uint16_t	surfaceId;
				int			nUpdateRects;
					
				*m_pDecoder >> canvasRef;
				*m_pDecoder >> surfaceId;
				*m_pDecoder >> nUpdateRects;

				if( surfaceId > 0 )
					m_charStream << "    canvasRef       = " << "[no toString() for canvasRef yet]" << std::endl;
				else
					m_charStream << "    surfaceId       = " << surfaceId << std::endl;

				_readPrintRects( "    clip rects     ", nUpdateRects );
				break;
			}
				
			case GfxChunkId::EndCanvasUpdate:
				m_charStream << "EndCanvasUpdate" << std::endl;
				break;

			case GfxChunkId::SetClipList:
			{
				m_charStream << "SetClipList" << std::endl;

				int		nRects = header.size / 16;
				m_charStream << "    number of rects: " << nRects << std::endl;

				_readPrintRects( "    clip rects     ", nRects );
				break;
			}

			case GfxChunkId::ResetClipList:
			{
				m_charStream << "ClearClipList" << std::endl;
				break;
			}

			case GfxChunkId::PushClipList:
			{
				m_charStream << "PushClipList" << std::endl;

				int		nRects = header.size / 16;
				m_charStream << "    number of rects: " << nRects << std::endl;

				_readPrintRects( "    clip rects     ", nRects );
				break;
			}

			case GfxChunkId::PopClipList:
			{
				m_charStream << "PopClipList" << std::endl;
				break;
			}

			case GfxChunkId::SetTintColor:
			{
				m_charStream << "SetTintColor" << std::endl;

				HiColor	col;
				*m_pDecoder >> col;

				_printColor( "    color       ", col );
				break;
			}

			case GfxChunkId::SetTintGradient:
			{
				m_charStream << "SetTintGradient" << std::endl;

				RectI rect;
				Gradient gradient;
				
				*m_pDecoder >> rect;
				*m_pDecoder >> gradient;

				_printRect(  "    rect         ", rect );
				m_charStream << "    isValid      " << gradient.isValid << std::endl;
				_printColor( "    topLeft      ", gradient.topLeft );
				_printColor( "    topRight     ", gradient.topRight );
				_printColor( "    bottomRight  ", gradient.bottomRight );
				_printColor( "    bottomLeft   ", gradient.bottomLeft );
				break;
			}

			case GfxChunkId::ClearTintGradient:
			{
				m_charStream << "ClearTintGradient" << std::endl;
				break;
			}

			case GfxChunkId::SetBlendMode:
			{
				m_charStream << "SetBlendMode" << std::endl;

				BlendMode	blendMode;
				*m_pDecoder >> blendMode;

				m_charStream << "    mode        = " << (int)blendMode << std::endl;
				break;
			}

			case GfxChunkId::SetBlitSource:
			{
				m_charStream << "SetBlitSource" << std::endl;

				uint16_t	surfaceId;
				*m_pDecoder >> surfaceId;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				break;
			}

			case GfxChunkId::SetMorphFactor:
			{
				m_charStream << "SetMorphFactor" << std::endl;

				float	factor;
				*m_pDecoder >> factor;

				m_charStream << "    factor      = " << factor << std::endl;
				break;
			}

			case GfxChunkId::SetRenderLayer:
			{
				m_charStream << "SetRenderLayer" << std::endl;

				uint16_t	layer;
				*m_pDecoder >> layer;

				m_charStream << "    layer       = " << layer << std::endl;
				break;
			}

			case GfxChunkId::Fill:
			{
				m_charStream << "Fill" << std::endl;

				HiColor	col;

				*m_pDecoder >> col;

				_printColor( "    color      ", col );
				break;
			}

			case GfxChunkId::FillRectI:
			{
				m_charStream << "FillRectI" << std::endl;

				RectI	rect;
				HiColor	col;

				*m_pDecoder >> rect;
				*m_pDecoder >> col;

				_printRect( "    dest       ", rect );
				_printColor( "    color      ", col );
				break;
			}

			case GfxChunkId::FillRectF:
			{
				m_charStream << "FillRectF" << std::endl;

				RectF	rect;
				HiColor	col;

				*m_pDecoder >> rect;
				*m_pDecoder >> col;

				_printRect( "    dest       ", rect );
				_printColor( "    color      ", col );
				break;
			}

			case GfxChunkId::PlotPixels:
			{
				m_charStream << "PlotPixels" << std::endl;

				m_pDecoder->skip(header.size);

				m_charStream << "    number of pixels: " << header.size/16 << std::endl;
				break;
			}

			case GfxChunkId::DrawLineFromTo:
			{
				m_charStream << "DrawLineFromTo" << std::endl;

				CoordI	begin;
				CoordI	end;
				HiColor	color;
				float	thickness;

				*m_pDecoder >> begin;
				*m_pDecoder >> end;
				*m_pDecoder >> color;
				*m_pDecoder >> thickness;

				m_charStream << "    begin       = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    end         = " << end.x << ", " << end.y << std::endl;
				
				_printColor( "    color      ", color );
				m_charStream << "    thickness   = " << thickness << std::endl;
				break;
			}


			case GfxChunkId::DrawLineStraight:
			{
				m_charStream << "DrawLineStraight" << std::endl;

				CoordI		begin;
				Direction	dir;
				int			length;
				HiColor		color;
				float		thickness;

				*m_pDecoder >> begin;
				*m_pDecoder >> dir;
				*m_pDecoder >> length;
				*m_pDecoder >> color;
				*m_pDecoder >> thickness;

				m_charStream << "    begin       = " << begin.x << ", " << begin.y << std::endl;
				m_charStream << "    direction   = " << toString(dir) << std::endl;
				m_charStream << "    length      = " << length << std::endl;

				_printColor( "    color      ", color );
				m_charStream << "    thickness   = " << thickness << std::endl;
				break;
			}

			case GfxChunkId::Blit:
			{
				m_charStream << "Blit" << std::endl;

				CoordI		dest;

				*m_pDecoder >> dest;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				break;
			}
				
			case GfxChunkId::BlitRectI:
			{
				m_charStream << "BlitRectI" << std::endl;

				CoordI		dest;
				RectI		source;

				*m_pDecoder >> dest;
				*m_pDecoder >> source;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				_printRect( "    source     ", source );
				break;
			}

			case GfxChunkId::FlipBlit:
			{
				m_charStream << "FlipBlit" << std::endl;

				CoordI		dest;
				GfxFlip		flip;

				*m_pDecoder >> dest;
				*m_pDecoder >> flip;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				m_charStream << "    flip        = " << toString(flip) << std::endl;

				break;
			}
				
			case GfxChunkId::FlipBlitRectI:
			{
				m_charStream << "FlipBlitRectI" << std::endl;

				CoordI		dest;
				RectI		source;
				GfxFlip		flip;

				*m_pDecoder >> dest;
				*m_pDecoder >> source;
				*m_pDecoder >> flip;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				_printRect( "    source     ", source );
				m_charStream << "    flip        = " << toString(flip) << std::endl;
				break;
			}

			case GfxChunkId::StretchBlit:
			{
				m_charStream << "StretchBlit" << std::endl;

				CoordI		dest;

				*m_pDecoder >> dest;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				break;
			}

			case GfxChunkId::StretchBlitRectI:
			{
				m_charStream << "StretchBlitRectI" << std::endl;

				CoordI		dest;
				RectF		source;

				*m_pDecoder >> dest;
				*m_pDecoder >> source;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				_printRect( "    source     ", source );
				break;
			}

			case GfxChunkId::StretchBlitRectF:
			{
				m_charStream << "StretchBlitRectF" << std::endl;

				CoordI		dest;
				RectF		source;

				*m_pDecoder >> dest;
				*m_pDecoder >> source;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				_printRect( "    source     ", source );
				break;
			}

			case GfxChunkId::StretchFlipBlit:
			{
				m_charStream << "StretchFlipBlit" << std::endl;

				CoordI		dest;
				GfxFlip		flip;

				*m_pDecoder >> dest;
				*m_pDecoder >> flip;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				m_charStream << "    flip        = " << toString(flip) << std::endl;
				break;
			}

			case GfxChunkId::StretchFlipBlitRectI:
			{
				m_charStream << "StretchFlipBlitRectI" << std::endl;

				CoordI		dest;
				RectF		source;
				GfxFlip		flip;

				*m_pDecoder >> dest;
				*m_pDecoder >> source;
				*m_pDecoder >> flip;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				_printRect(     "    source     ", source );
				m_charStream << "    flip        = " << toString(flip) << std::endl;
				break;
			}

			case GfxChunkId::StretchFlipBlitRectF:
			{
				m_charStream << "StretchFlipBlitRectF" << std::endl;

				CoordI		dest;
				RectF		source;
				GfxFlip		flip;

				*m_pDecoder >> dest;
				*m_pDecoder >> source;
				*m_pDecoder >> flip;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				_printRect( "    source     ", source );
				m_charStream << "    flip        = " << toString(flip) << std::endl;
				break;
			}

			case GfxChunkId::RotScaleBlit:
			{
				m_charStream << "RotScaleBlit" << std::endl;

				RectI		dest;
				float		rotationDegrees;
				float		scale;
				CoordF		srcCenter;
				CoordF		destCenter;
				
				*m_pDecoder >> dest;
				*m_pDecoder >> rotationDegrees;
				*m_pDecoder >> scale;
				*m_pDecoder >> srcCenter;
				*m_pDecoder >> destCenter;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				m_charStream << "    rotation    = " << rotationDegrees << " degrees" << std::endl;
				m_charStream << "    scale       = " << scale << std::endl;
				m_charStream << "    srcCenter   = " << srcCenter.x << ", " << srcCenter.y << std::endl;
				m_charStream << "    destCenter  = " << destCenter.x << ", " << destCenter.y << std::endl;

				break;
			}

			case GfxChunkId::Tile:
			{
				m_charStream << "Tile" << std::endl;

				RectI		dest;
				CoordI		shift;
				
				*m_pDecoder >> dest;
				*m_pDecoder >> shift;

				_printRect( "    dest       ", dest );
				m_charStream << "    shift       = " << shift.x << ", " << shift.y << std::endl;
				break;
			}

			case GfxChunkId::FlipTile:
			{
				m_charStream << "FlipTile" << std::endl;

				RectI		dest;
				GfxFlip		flip;
				CoordI		shift;
				
				*m_pDecoder >> dest;
				*m_pDecoder >> flip;
				*m_pDecoder >> shift;

				_printRect( "    dest       ", dest );
				m_charStream << "    flip        = " << toString(flip) << std::endl;
				m_charStream << "    shift       = " << shift.x << ", " << shift.y << std::endl;
				break;
			}

			case GfxChunkId::ScaleTile:
			{
				m_charStream << "ScaleTile" << std::endl;

				RectI		dest;
				float		scale;
				CoordI		shift;
				
				*m_pDecoder >> dest;
				*m_pDecoder >> scale;
				*m_pDecoder >> shift;

				_printRect( "    dest       ", dest );
				m_charStream << "    scale       = " << scale << std::endl;
				m_charStream << "    shift       = " << shift.x << ", " << shift.y << std::endl;
				break;
			}

			case GfxChunkId::ScaleFlipTile:
			{
				m_charStream << "ScaleFlipTile" << std::endl;

				RectI		dest;
				float		scale;
				GfxFlip		flip;
				CoordI		shift;
				
				*m_pDecoder >> dest;
				*m_pDecoder >> scale;
				*m_pDecoder >> flip;
				*m_pDecoder >> shift;

				_printRect( "    dest       ", dest );
				m_charStream << "    scale       = " << scale << std::endl;
				m_charStream << "    flip        = " << toString(flip) << std::endl;
				m_charStream << "    shift       = " << shift.x << ", " << shift.y << std::endl;
				break;
			}
				
			case GfxChunkId::DrawWave:
			{
				m_charStream << "DrawWave" << std::endl;
				m_charStream << "    no data printout implemented yet." << std::endl;
				m_pDecoder->skip(header.size);
				break;
			}

			case GfxChunkId::FlipDrawWave:
			{
				m_charStream << "FlipDrawWave" << std::endl;
				m_charStream << "    no data printout implemented yet." << std::endl;
				m_pDecoder->skip(header.size);
				break;
			}

			case GfxChunkId::DrawElipse:
			{
				m_charStream << "DrawElipse" << std::endl;

				RectSPX	canvas;
				spx 	thickness;
				HiColor	color;
				spx		outlineThickness;
				HiColor	outlineColor;

				*m_pDecoder >> canvas;
				*m_pDecoder >> thickness;
				*m_pDecoder >> color;
				*m_pDecoder >> outlineThickness;
				*m_pDecoder >> outlineColor;

				_printRect(     "    canvas          ", canvas );
				m_charStream << "    thickness        = " << thickness << std::endl;
				_printColor(    "    color           ", color );
				m_charStream << "    outlineThickness = " << outlineThickness << std::endl;
				_printColor(    "    outlineColor    ", outlineColor );
				break;
			}

			case GfxChunkId::DrawPieChart:
			{
				m_charStream << "DrawPieChart" << std::endl;

				float sliceSizes[32];
				HiColor sliceColors[32];
				
				RectI 	canvas;
				float 	start;
				int 	nSlices;
				float 	hubSize;
				HiColor hubColor;
				HiColor backColor;
				bool	bRectangular;
				
				*m_pDecoder >> canvas;
				*m_pDecoder >> start;
				*m_pDecoder >> nSlices;
				*m_pDecoder >> hubSize;
				*m_pDecoder >> hubColor;
				*m_pDecoder >> backColor;
				*m_pDecoder >> bRectangular;

				assert(nSlices <= 32);
				
				*m_pDecoder >> GfxStream::DataChunk{ nSlices*4, sliceSizes };
				*m_pDecoder >> GfxStream::DataChunk{ nSlices*8, sliceColors };

				_printRect(     "    canvas          ", canvas );
				m_charStream << "    start           = " << start << std::endl;
				m_charStream << "    nSlices         = " << nSlices << std::endl;
				m_charStream << "    hubSize         = " << hubSize << std::endl;
				_printColor(    "    hubColor       ", hubColor );
				_printColor(    "    backColor      ", backColor );
				m_charStream << "    rectangular     = " << bRectangular << std::endl;
				
				for( int i = 0 ; i < nSlices ; i++ )
				{
					HiColor& col = sliceColors[i];
					
					m_charStream << "    slice " << (i+1) << "    size: " << sliceSizes[i] << "    color: " << (int)col.a << ", " << (int)col.r << ", " << (int)col.g << ", " << (int)col.b << std::endl;
				}
				break;
			}

			case GfxChunkId::DrawSegments:
			{
				m_charStream << "DrawSegments" << std::endl;
				m_charStream << "    no data printout implemented yet." << std::endl;
				m_pDecoder->skip(header.size);
				break;
			}

			case GfxChunkId::FlipDrawSegments:
			{
				m_charStream << "FlipDrawSegments" << std::endl;
				m_charStream << "    no data printout implemented yet." << std::endl;
				m_pDecoder->skip(header.size);
				break;
			}
				
			case GfxChunkId::BlitNinePatch:
			{
				m_charStream << "BlitNinePatch" << std::endl;

				RectI 		dstRect;
				BorderI 	dstFrame;
				NinePatch 	patch;
				int			scale;
					
				*m_pDecoder >> dstRect;
				*m_pDecoder >> dstFrame;

				*m_pDecoder >> patch.block;
				*m_pDecoder >> patch.frame;

				*m_pDecoder >> patch.rigidPartXOfs;
				*m_pDecoder >> patch.rigidPartXLength;
				*m_pDecoder >> patch.rigidPartXSections;

				*m_pDecoder >> patch.rigidPartYOfs;
				*m_pDecoder >> patch.rigidPartYLength;
				*m_pDecoder >> patch.rigidPartYSections;

				*m_pDecoder >> scale;

				_printRect(     "    dstRect        ", dstRect );
				_printBorder(   "    dstRect        ", dstFrame );
				_printRect(     "    patch.block    ", patch.block );
				_printBorder(   "    patch.frame    ", patch.frame );
				m_charStream << "    (rigid parts not printed)" << std::endl;
				m_charStream << "    scale          = " << scale << std::endl;
				break;
			}
				
			case GfxChunkId::EdgeSamples:
			{
				m_charStream << "EdgeSamples" << std::endl;

				m_pDecoder->skip(header.size);
				m_charStream << "    nSamples    = " << (int)header.size/2 << std::endl;
				break;
			}

			case GfxChunkId::CreateSurface:
			{
				m_charStream << "CreateSurface" << std::endl;

				uint16_t	surfaceId;
				Surface::Blueprint	bp;

				*m_pDecoder >> surfaceId;
				*m_pDecoder >> bp.canvas;
				*m_pDecoder >> bp.dynamic;
				*m_pDecoder >> bp.format;
				*m_pDecoder >> bp.id;
				*m_pDecoder >> bp.mipmap;
				*m_pDecoder >> bp.sampleMethod;
				*m_pDecoder >> bp.scale;
				*m_pDecoder >> bp.size;
				*m_pDecoder >> bp.tiling;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				m_charStream << "    canvas      = " << bp.canvas << std::endl;
				m_charStream << "    dynamic     = " << bp.dynamic << std::endl;
				m_charStream << "    format      = " << toString(bp.format) << std::endl;
				m_charStream << "    id          = " << bp.id << std::endl;
				m_charStream << "    mipmap      = " << bp.mipmap << std::endl;
				m_charStream << "    sampleMethod= " << toString(bp.sampleMethod) << std::endl;
				m_charStream << "    scale       = " << bp.scale << std::endl;
				m_charStream << "    size        = " << bp.size.w << ", " << bp.size.h << std::endl;
				m_charStream << "    tiling      = " << bp.tiling << std::endl;
				break;
			}

			case GfxChunkId::BeginSurfaceUpdate:
			{
				m_charStream << "BeginSurfaceUpdate" << std::endl;

				uint16_t	surfaceId;
				RectI		region;

				*m_pDecoder >> surfaceId;
				*m_pDecoder >> region;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				_printRect( "    region     ", region );
				break;
			}

			case GfxChunkId::SurfacePixels:
			{
				m_charStream << "SurfacePixels" << std::endl;

				m_pDecoder->skip(header.size);

				m_charStream << "    size: " << header.size << " bytes." << std::endl;
				break;
			}

			case GfxChunkId::EndSurfaceUpdate:
			{
				m_charStream << "EndSurfaceUpdate" << std::endl;

				break;
			}

			case GfxChunkId::FillSurface:
			{
				m_charStream << "FillSurface" << std::endl;

				uint16_t	surfaceId;
				RectI		region;
				HiColor		col;

				*m_pDecoder >> surfaceId;
				*m_pDecoder >> region;
				*m_pDecoder >> col;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				_printRect( "    region     ", region );
				_printColor("    color      ", col );
				break;
			}

			case GfxChunkId::CopySurface:
			{
				m_charStream << "CopySurface" << std::endl;

				uint16_t	destSurfaceId;
				uint16_t	sourceSurfaceId;
				RectI		sourceRect;
				CoordI		dest;

				*m_pDecoder >> destSurfaceId;
				*m_pDecoder >> sourceSurfaceId;
				*m_pDecoder >> sourceRect;
				*m_pDecoder >> dest;

				m_charStream << "    destSurface = " << destSurfaceId << std::endl;
				m_charStream << "    srcSurface  = " << sourceSurfaceId << std::endl;
				_printRect( "    source     ", sourceRect );
				m_charStream << "    dest        = " << dest.x << ", " << dest.y << std::endl;
				break;
			}

			case GfxChunkId::DeleteSurface:
			{
				m_charStream << "DeleteSurface" << std::endl;

				uint16_t	surfaceId;
				*m_pDecoder >> surfaceId;

				m_charStream << "    surfaceId   = " << surfaceId << std::endl;
				break;
			}


/*
			case GfxChunkId::SimpleTransformBlit:
			{
				RectI		dest;
				CoordI		src;
				int			transform[2][2];

				*m_pDecoder >> dest;
				*m_pDecoder >> src;
				*m_pDecoder >> transform;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << ", " << dest.w << ", " << dest.h << std::endl;
				m_charStream << "    src         = " << src.x << ", " << src.y << std::endl;
				m_charStream << "    transform   = [ " << transform[0][0] << ", " << transform[0][1] << " ]" << std::endl;
				m_charStream << "                  [ " << transform[1][0] << ", " << transform[1][1] << " ]" << std::endl;
				break;
			}

			case GfxChunkId::ComplexTransformBlit:
			{
				RectI		dest;
				CoordF		src;
				float		transform[2][2];

				*m_pDecoder >> dest;
				*m_pDecoder >> src;
				*m_pDecoder >> transform;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << ", " << dest.w << ", " << dest.h << std::endl;
				m_charStream << "    src         = " << src.x << ", " << src.y << std::endl;
				m_charStream << "    transform   = [ " << transform[0][0] << ", " << transform[0][1] << " ]" << std::endl;
				m_charStream << "                  [ " << transform[1][0] << ", " << transform[1][1] << " ]" << std::endl;
				break;
			}

			case GfxChunkId::TransformDrawSegments:
			{
				RectI		dest;
				uint16_t	nSegments;
				uint16_t	nEdgeStrips;
				int			transform[2][2];


				(*m_pDecoder) >> dest;
				(*m_pDecoder) >> nSegments;
				(*m_pDecoder) >> nEdgeStrips;
				(*m_pDecoder) >> transform;

				m_charStream << "    dest        = " << dest.x << ", " << dest.y << ", " << dest.w << ", " << dest.h << std::endl;
				m_charStream << "    nSegments   = " << nSegments << std::endl;
				m_charStream << "    nEdgeStrips = " << nEdgeStrips << std::endl;
				m_charStream << "    transform   = [ " << transform[0][0] << ", " << transform[0][1] << " ]" << std::endl;
				m_charStream << "                  [ " << transform[1][0] << ", " << transform[1][1] << " ]" << std::endl;


				m_charStream << "    seg colors  = ";
				for (int i = 0; i < nSegments; i++)
				{
					HiColor color;
					(*m_pDecoder) >> color;

					m_charStream << "{ " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << " } ";

				}
				m_charStream << std::endl;
				break;
			}
*/

			default:
			{
				m_charStream << "Unknown Chunk Type: " << (int) header.type << std::endl;
				m_charStream << "    size: " << (int)header.size << std::endl;

				m_pDecoder->skip(header.size);
				break;
			}
		}

		return true;
	}

	//____ readPrintPatches() _________________________________________________

	int GfxStreamLogger::_readPrintPatches()
	{
		uint16_t	nPatches;
		RectI		patch;

		*m_pDecoder >> nPatches;

		if (nPatches == 0)
		{
			m_charStream << "    zero patches!" << std::endl;
			return 0;
		}

		*m_pDecoder >> patch;
		m_charStream << "    patches     = " << patch.x << ", " << patch.y << ", " << patch.w << ", " << patch.h << std::endl;

		for (int i = 1; i < nPatches; i++)
		{
			*m_pDecoder >> patch;
			m_charStream << "                  " << patch.x << ", " << patch.y << ", " << patch.w << ", " << patch.h << std::endl;
		}

		return nPatches;
	}

	//____ readPrintRects() _________________________________________________

	void GfxStreamLogger::_readPrintRects( const char * label, int nRects )
	{
		RectI		rect;

		if (nRects == 0)
		{
			m_charStream << label << " = none" << std::endl;
			return;
		}

		*m_pDecoder >> rect;
		m_charStream << label << " = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;

		for (int i = 1; i < nRects; i++)
		{
			*m_pDecoder >> rect;
			m_charStream << "                  " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
		}
	}

	//____ _printRect() _________________________________________________

	void GfxStreamLogger::_printRect( const char * header, const RectI& rect )
	{
		m_charStream << header << " = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
	}

	void GfxStreamLogger::_printRect( const char * header, const RectF& rect )
	{
		m_charStream << header << " = " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
	}

	//____ _printColor() _________________________________________________

	void GfxStreamLogger::_printColor( const char * header, HiColor color )
	{
		m_charStream << header << " = " << (int)color.a << ", " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << std::endl;
	}

	//____ _printBorder() _________________________________________________

	void GfxStreamLogger::_printBorder( const char * header, const BorderI& border )
	{
		m_charStream << header << " = " << border.top << ", " << border.right << ", " << border.bottom << ", " << border.left << std::endl;
	}

	void GfxStreamLogger::_printBorder(const char* header, const Border& border)
	{
		m_charStream << header << " = " << border.top << ", " << border.right << ", " << border.bottom << ", " << border.left << std::endl;
	}


} //namespace wg
