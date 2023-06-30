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

#include <wg_gfxstreamdevice.h>
#include <wg_gfxstreamsurface.h>
#include <wg_gfxstreamsurfacefactory.h>
#include <wg_gfxstreamedgemap.h>
#include <wg_gfxstreamedgemapfactory.h>
#include <wg_gfxbase.h>
#include <assert.h>
#include <math.h>
#include <algorithm>

using namespace std;

namespace wg
{
	const TypeInfo GfxStreamDevice::TYPEINFO = { "GfxStreamDevice", &GfxDevice::TYPEINFO };


	inline int _spxMask( const CoordI& c )
	{
		const int add = (1 << 21);
		return (c.x+add) | (c.y+add);
	}

	inline int _spxMask( const SizeI& s )
	{
		return s.w | s.h;
	}

	inline int _spxMask( const RectI& r )
	{
		const int add = (1 << 21);
		return (r.x+add) | (r.y+add) | r.w | r.h;
	}

	inline int _spxMask( spx v )
	{
		const int add = (1 << 21);
		return (v+add);
	}



	inline int _spxMask( const spx * pBegin, const spx * pEnd )
	{
		const int add = (1 << 21);
		int result = 0;

		while( pBegin < pEnd )
			result |= (* pBegin++) + add;

		return result;
	}

	inline GfxStream::SpxFormat _evaluateMask( int spxMask )
	{
		if( (spxMask & 0xFFDFC03F) == 0 )
			return GfxStream::SpxFormat::Uint8_int;						// Fits in uint8_t without binals.

		if( (spxMask & 0xFFC0003F) == 0 )
			return GfxStream::SpxFormat::Int16_int;						// Fits in int16_t without binals.

		if( (spxMask & 0xFFDF0000) == 0 )
			return GfxStream::SpxFormat::Uint16_dec;						// Fits in uint16_t with binals.

		return GfxStream::SpxFormat::Int32_dec;
	}

	inline GfxStream::SpxFormat _spxFieldFormat( const spx * pBegin, const spx * pEnd )
	{
		const int add = (1 << 21);
		int spxMask = 0;

		while( pBegin < pEnd )
		{
			int value = * pBegin++;
			spxMask |= value + add;
		}
			
		if( (spxMask & 0xFFDFC03F) == 0 )
			return GfxStream::SpxFormat::Uint8_int;						// Fits in uint8_t without binals.
		
		if( (spxMask & 0xFFDF0000) == 0 )
			return GfxStream::SpxFormat::Uint16_dec;					// Fits in uint16_t with binals.

		if( (spxMask & 0xFFC0003F) == 0 )
			return GfxStream::SpxFormat::Int16_int;						// Fits in int16_t without binals.
		
		return GfxStream::SpxFormat::Int32_dec;							// We need 32 bits.
	}


	//____ create() _______________________________________________________________

	GfxStreamDevice_p GfxStreamDevice::create( GfxStreamEncoder * pEncoder )
	{
		GfxStreamDevice_p p(new GfxStreamDevice( pEncoder ));
		return p;
	}


	//____ constructor _____________________________________________________________

	GfxStreamDevice::GfxStreamDevice(GfxStreamEncoder* pEncoder)
	{
		m_pEncoder = pEncoder;
		m_bRendering = false;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ProtocolVersion, GfxStream::SpxFormat::Int32_dec, 2 };
        (*m_pEncoder) << (uint16_t) 0x0100;
	}

	//____ Destructor ______________________________________________________________

	GfxStreamDevice::~GfxStreamDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GfxStreamDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

    //____ canvas() ___________________________________________________________

    const CanvasInfo GfxStreamDevice::canvas(CanvasRef ref) const
    {
        auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](const CanvasInfo& entry) { return (ref == entry.ref); });

        if (it != m_definedCanvases.end())
            return *it;

        return m_dummyCanvas;
    }

	//____ surfaceType() _______________________________________________________

	const TypeInfo& GfxStreamDevice::surfaceType( void ) const
	{
		return GfxStreamSurface::TYPEINFO;
	}

	//____ defineCanvas() ____________________________________________________

	bool GfxStreamDevice::defineCanvas( CanvasRef ref, GfxStreamSurface * pSurface )
	{ 
        auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](CanvasInfo& entry) { return (ref == entry.ref); });

        if (it == m_definedCanvases.end())
        {
            if (pSurface)
                m_definedCanvases.push_back(CanvasInfo(ref, pSurface, pSurface->pixelSize() * 64, pSurface->pixelFormat(), pSurface->scale()));
        }
        else
        {
            if (pSurface)
            {
                it->pSurface = pSurface;
                it->size = pSurface->pixelSize() * 64;
                it->scale = pSurface->scale();
            }
            else
            {
                m_definedCanvases.erase(it);
            }
        }

        return true;
    }

	bool GfxStreamDevice::defineCanvas( CanvasRef ref, const SizeI& size, PixelFormat format, int scale )
	{
	
		auto it = std::find_if( m_definedCanvases.begin(), m_definedCanvases.end(), [ref] (CanvasInfo& entry) { return (ref == entry.ref); } );

		if( it == m_definedCanvases.end() )
		{
			if( !size.isEmpty() )
				m_definedCanvases.push_back( CanvasInfo( ref, nullptr, size * 64, format, scale ) );
		}
		else
		{
			if( !size.isEmpty() )
			{
				it->pSurface = nullptr;
				it->size = size * 64;
                it->scale = scale;
			}
			else
			{
				m_definedCanvases.erase(it);
			}
		}

		return true;
	}

	//____ encodeCanvasList() ______________________________________________________

	void GfxStreamDevice::encodeCanvasList()
	{
		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::CanvasList, GfxStream::SpxFormat::Int32_dec, (uint16_t)(m_definedCanvases.size()*12) };

		(*m_pEncoder) << (uint16_t) m_definedCanvases.size();
		
		for( auto& canvas : m_definedCanvases )
		{
			(*m_pEncoder) << (uint16_t) canvas.ref;
			(*m_pEncoder) << canvas.size;
			(*m_pEncoder) << (uint16_t) canvas.scale;
		}		
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p GfxStreamDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = GfxStreamSurfaceFactory::create(m_pEncoder);

		return m_pSurfaceFactory;
	}

	//____ edgemapFactory() ______________________________________________________

	EdgemapFactory_p GfxStreamDevice::edgemapFactory()
	{
		if( !m_pEdgemapFactory )
			m_pEdgemapFactory = GfxStreamEdgemapFactory::create(m_pEncoder);

		return m_pEdgemapFactory;
	}


    //____ setClipList() _________________________________________________________

    bool GfxStreamDevice::setClipList(int nRectangles, const RectSPX * pRectangles)
    {
        GfxDevice::setClipList(nRectangles, pRectangles);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetClipList, GfxStream::SpxFormat::Int32_dec, (uint16_t)(nRectangles * 16) };
        (*m_pEncoder) << GfxStream::WriteBytes{ nRectangles * 16, pRectangles };
        return true;
    }

    //____ resetClipList() _______________________________________________________

    void GfxStreamDevice::resetClipList()
    {
        GfxDevice::resetClipList();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ResetClipList, GfxStream::SpxFormat::Int32_dec, 0 };
    }

    //____ pushClipList() ________________________________________________________

    bool GfxStreamDevice::pushClipList(int nRectangles, const RectSPX* pRectangles)
    {
        GfxDevice::pushClipList(nRectangles, pRectangles);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PushClipList, GfxStream::SpxFormat::Int32_dec, (uint16_t)(nRectangles * 16) };
        (*m_pEncoder) << GfxStream::WriteBytes{ nRectangles * 16, pRectangles };
        return true;
    }

    //____ popClipList() __________________________________________________________

    bool GfxStreamDevice::popClipList()
    {
        GfxDevice::popClipList();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PopClipList, GfxStream::SpxFormat::Int32_dec, 0};
        return true;
    }

    //____ setTintColor() __________________________________________________________

    void GfxStreamDevice::setTintColor( HiColor color )
    {
		if( color == m_tintColor )
			return;
		
        GfxDevice::setTintColor(color);
    }

    //____ setTintGradient() _______________________________________________________

    void GfxStreamDevice::setTintGradient(const RectSPX& rect, const Gradient& gradient)
    {
		if( m_bTintGradient && rect == m_tintGradientRect && m_tintGradient == gradient )
			return;
		
        GfxDevice::setTintGradient(rect, gradient);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetTintGradient, GfxStream::SpxFormat::Int32_dec, 16 + 34 };
        (*m_pEncoder) << rect;
        (*m_pEncoder) << gradient;
    }

    //____ clearTintGradient() _____________________________________________________

    void GfxStreamDevice::clearTintGradient()
    {
		if( m_bTintGradient == false )
			return;
		
        GfxDevice::clearTintGradient();
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::ClearTintGradient, GfxStream::SpxFormat::Int32_dec, 0 };
    }

    //____ setBlendMode() __________________________________________________________

    bool GfxStreamDevice::setBlendMode( BlendMode blendMode )
    {
        if( blendMode < BlendMode_min || blendMode > BlendMode_max )
        {
            GfxBase::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Not a valid blendMode", this, &TYPEINFO, __func__, __FILE__, __LINE__);
            return false;
        }

		if( blendMode == m_blendMode )
			return true;
		
        GfxDevice::setBlendMode(blendMode);
		return true;
    }

    //____ setBlitSource() _______________________________________________________

    bool GfxStreamDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || !pSource->isInstanceOf(GfxStreamSurface::TYPEINFO))
        {
            GfxBase::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Surface is not a GfxStreamSurface", this, &TYPEINFO, __func__, __FILE__, __LINE__);
            return false;
        }

		if( pSource == m_pBlitSource )
			return true;
		
        GfxDevice::setBlitSource(pSource);
        return true;
    }

    //____ setMorphFactor() ______________________________________________________

    void GfxStreamDevice::setMorphFactor(float factor)
    {
		if( factor == m_morphFactor )
			return;
		
        GfxDevice::setMorphFactor(factor);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetMorphFactor, GfxStream::SpxFormat::Int32_dec, 4 };
        (*m_pEncoder) << factor;
    }

	//____ setFixedBlendColor() __________________________________________________

	void GfxStreamDevice::setFixedBlendColor( HiColor color )
	{
		if( color == m_fixedBlendColor )
			return;
		
		GfxDevice::setFixedBlendColor(color);
		
		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::SetFixedBlendColor, GfxStream::SpxFormat::Int32_dec, 8 };
		(*m_pEncoder) << color;
	}

    //____ setRenderLayer() _______________________________________________________

    void GfxStreamDevice::setRenderLayer(int layer)
    {
		if( layer == m_renderLayer )
			return;
		
        GfxDevice::setRenderLayer(layer);        
    }

	//____ beginRender() ___________________________________________________________

	bool GfxStreamDevice::beginRender()
	{
		if( m_bRendering == true )
			return false;

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::BeginRender, GfxStream::SpxFormat::Int32_dec, 0 };

        GfxDevice::setBlitSource(nullptr);                 // We don't optimize blit source between frames.
		m_pStreamedBlitSource = nullptr;
		
		m_bRendering = true;
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool GfxStreamDevice::endRender()
	{
		if( m_bRendering == false )
			return false;

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::EndRender, GfxStream::SpxFormat::Int32_dec, 0 };
		m_pEncoder->flush();

		m_bRendering = false;
		return true;
	}

    //____ flush() ___________________________________________________________________

    void GfxStreamDevice::flush()
    {
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Flush, GfxStream::SpxFormat::Int32_dec, 0 };
        m_pEncoder->flush();
    }

    //____ endCanvasUpdate() _________________________________________________________

    void GfxStreamDevice::endCanvasUpdate()
    {
		auto& renderStates = m_stateStack.back();
		
		m_streamedBlendMode = renderStates.blendMode;
		m_streamedRenderLayer = renderStates.renderLayer;
		m_streamedTintColor = renderStates.tintColor;
		
		m_stateStack.pop_back();
		
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::EndCanvasUpdate, GfxStream::SpxFormat::Int32_dec, 0 };
		GfxDevice::endCanvasUpdate();
    }

    //____ fill() __________________________________________________________________

    void GfxStreamDevice::fill( HiColor _col )
    {
        if( _col.a  == 0 )
            return;

		_streamStatesIfUpdated();

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::Fill, GfxStream::SpxFormat::Int32_dec, 8 };
        (*m_pEncoder) << _col;

        return;
    }

    void GfxStreamDevice::fill( const RectSPX& _rect, HiColor _col )
    {
		if ((_col.a == 0 || m_tintColor.a == 0) && (m_blendMode == BlendMode::Blend || m_blendMode == BlendMode::Add || m_blendMode == BlendMode::Subtract ) )
			return;
		
        if( _rect.w < 1 || _rect.h < 1 )
            return;

		if( _clippedOut(_rect) )
			return;
		
		_streamStatesIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(_rect) );

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FillRect, spxFormat, GfxStream::spxSize(spxFormat)*4 + 8 };
        (*m_pEncoder) << _rect;
        (*m_pEncoder) << _col;

        return;
    }

    //____ plotPixels() ________________________________________________________

    void GfxStreamDevice::plotPixels(int nCoords, const CoordSPX * pCoords, const HiColor * pColors)
    {
        // Each pixel is 8 + 8 bytes: int32_t x, int32_t y, HiColor
        // All coordinates comes first, then all colors.

        //TODO: Optimize. Clip coordinates before we send them.
        //TODO: Optimize. Remove binals from pixels, store as 16-bit values.

        if (nCoords == 0)
            return;

		_streamStatesIfUpdated();
		
        int maxChunkCoords = (int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)) / 16;

        int chunkCoords = min(nCoords, maxChunkCoords);

        while (nCoords > 0)
        {
            (*m_pEncoder) << GfxStream::Header{ GfxChunkId::PlotPixels, GfxStream::SpxFormat::Int32_dec, (uint16_t)(chunkCoords * 16) };
            (*m_pEncoder) << GfxStream::WriteBytes{ chunkCoords * 8, pCoords };
            (*m_pEncoder) << GfxStream::WriteBytes{ chunkCoords * 8, pColors };

            nCoords -= chunkCoords;
            pCoords += chunkCoords;
            pColors += chunkCoords;
            chunkCoords = min(nCoords, maxChunkCoords);
        }
    }

    //____ drawLine() __________________________________________________________

    void GfxStreamDevice::drawLine(CoordSPX begin, CoordSPX end, HiColor color, spx thickness)
    {
		//TODO: ClipOut!!!

		_streamStatesIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(begin ) | _spxMask(end) | _spxMask(thickness) );
		
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawLineFromTo, spxFormat, (((GfxStream::spxSize(spxFormat)*5)+1)&0xFFFE) + 8  };
        (*m_pEncoder) << begin;
        (*m_pEncoder) << end;
        (*m_pEncoder) << color;
        (*m_pEncoder) << GfxStream::SPX(thickness);
		m_pEncoder->align();
    }

    void GfxStreamDevice::drawLine(CoordSPX begin, Direction dir, spx length, HiColor col, spx thickness)
    {
		RectSPX clip;
		switch(dir)
		{
			case Direction::Left:
				clip = {begin.x - length, begin.y - thickness/2 -1, length, thickness + 2};
				break;
			case Direction::Right:
				clip = {begin.x, begin.y - thickness/2 -1, length, thickness + 2};
				break;
			case Direction::Down:
				clip = {begin.x - thickness/2 - 1, begin.y, thickness + 2, length};
				break;
			case Direction::Up:
				clip = {begin.x - thickness/2 - 1, begin.y - length, thickness + 2, length};
				break;

		}
		if( _clippedOut( clip ) )
			return;

		_streamStatesIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(begin ) | _spxMask(length) | _spxMask(thickness) );
		
		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawLineStraight, spxFormat, GfxStream::spxSize(spxFormat)*4 + 10 };
        (*m_pEncoder) << begin;
        (*m_pEncoder) << dir;
        (*m_pEncoder) << GfxStream::SPX(length);
        (*m_pEncoder) << GfxStream::SPX(thickness);
		(*m_pEncoder) << col;
    }

    //____ blit() __________________________________________________________________

    void GfxStreamDevice::blit(CoordSPX dest)
    {
		if( _clippedOut( {dest,m_pBlitSource->pixelSize()} ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();
		
		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(dest ) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::Blit, spxFormat, GfxStream::spxSize(spxFormat)*2 };
        (*m_pEncoder) << dest;
    }

    void GfxStreamDevice::blit(CoordSPX dest, const RectSPX& src)
    {
        if (src.w < 1 || src.h < 1)
            return;

		if( _clippedOut( {dest,src.w,src.h} ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(src) );
		
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BlitRect, spxFormat, GfxStream::spxSize(spxFormat)*6 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << src;
    }

    //____ flipBlit() _________________________________________________________

    void GfxStreamDevice::flipBlit(CoordSPX dest, GfxFlip flip )
    {
		//TODO: ClipOut!!!

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();
		
		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(dest ) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipBlit, spxFormat, GfxStream::spxSize(spxFormat)*2 + 2 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
    }

    void GfxStreamDevice::flipBlit(CoordSPX dest, const RectSPX& src, GfxFlip flip )
    {
		//TODO: ClipOut!!!

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(src) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipBlitRect, spxFormat, GfxStream::spxSize(spxFormat)*6 + 2 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << src;
        (*m_pEncoder) << flip;
    }

    //____ stretchBlit() ___________________________________________________

    void GfxStreamDevice::stretchBlit(const RectSPX& dest)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(dest ) );

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlit, spxFormat, GfxStream::spxSize(spxFormat)*4 };
        (*m_pEncoder) << dest;
    }

    void GfxStreamDevice::stretchBlit(const RectSPX& dest, const RectSPX& source)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(source) );

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRect, spxFormat, GfxStream::spxSize(spxFormat)*8 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
    }

    //____ stretchFlipBlit() ___________________________________________________

    void GfxStreamDevice::stretchFlipBlit(const RectSPX& dest, GfxFlip flip)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(dest ) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchFlipBlit, spxFormat, GfxStream::spxSize(spxFormat)*4+2 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
    }

    void GfxStreamDevice::stretchFlipBlit(const RectSPX& dest, const RectSPX& source, GfxFlip flip)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(source) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::StretchBlitRect, spxFormat, GfxStream::spxSize(spxFormat)*8+2 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << source;
        (*m_pEncoder) << flip;
    }

	//____ precisionBlit() _______________________________________________________

	void GfxStreamDevice::precisionBlit(const RectSPX& dest, const RectF& srcSPX)
	{
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(dest ) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::PrecisionBlit, spxFormat, GfxStream::spxSize(spxFormat)*4 + 16 };
		(*m_pEncoder) << dest;
		(*m_pEncoder) << srcSPX;
	}

	//____ transformBlit() _______________________________________________________

	void GfxStreamDevice::transformBlit(const RectSPX& dest, CoordF srcSPX, const float transform[2][2])
	{
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(dest ) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::PrecisionBlit, spxFormat, GfxStream::spxSize(spxFormat)*4 + 24 };
		(*m_pEncoder) << dest;
		(*m_pEncoder) << srcSPX;
		(*m_pEncoder) << transform[0][0];
		(*m_pEncoder) << transform[0][1];
		(*m_pEncoder) << transform[1][0];
		(*m_pEncoder) << transform[1][1];
	}

    //____ rotScaleBlit() ___________________________________________________________

    void GfxStreamDevice::rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter, CoordF destCenter)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask( _spxMask(dest ) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::RotScaleBlit, spxFormat, GfxStream::spxSize(spxFormat)*4 + 24 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << rotationDegrees;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << srcCenter;
        (*m_pEncoder) << destCenter;
    }

    //____ tile() _____________________________________________________________________

    void GfxStreamDevice::tile(const RectSPX& dest, CoordSPX shift)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(shift) );
		
		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::Tile, spxFormat, GfxStream::spxSize(spxFormat)*6 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << shift;
    }

    void GfxStreamDevice::flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(shift) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipTile, spxFormat, GfxStream::spxSize(spxFormat)*6 + 2 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << shift;
    }

    void GfxStreamDevice::scaleTile(const RectSPX& dest, float scale, CoordSPX shift)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(shift) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::ScaleTile, spxFormat, GfxStream::spxSize(spxFormat)*6 + 4 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << shift;
    }

    void GfxStreamDevice::scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest ) | _spxMask(shift) );

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::ScaleFlipTile, spxFormat, GfxStream::spxSize(spxFormat)*6 + 6 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << scale;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << shift;
    }

    //____ drawWave() ___________________________________________________________________

    void GfxStreamDevice::drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();

		int size = 16 + 20 + 20 + 8 + 8;
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawWave, GfxStream::SpxFormat::Int32_dec, (uint16_t) size };
        (*m_pEncoder) << dest;

        (*m_pEncoder) << pTopBorder->length;
        (*m_pEncoder) << pTopBorder->thickness;
        (*m_pEncoder) << pTopBorder->color;
        (*m_pEncoder) << pTopBorder->hold;

        (*m_pEncoder) << pBottomBorder->length;
        (*m_pEncoder) << pBottomBorder->thickness;
        (*m_pEncoder) << pBottomBorder->color;
        (*m_pEncoder) << pBottomBorder->hold;

        (*m_pEncoder) << frontFill;
        (*m_pEncoder) << backFill;

        if( pTopBorder->length > 0 )
            _streamEdgeSamples( pTopBorder->length, 1, 1, pTopBorder->pWave );
        
        if( pBottomBorder->length > 0 )
            _streamEdgeSamples( pBottomBorder->length, 1, 1, pBottomBorder->pWave );
    }

    //____ flipDrawWave() ___________________________________________________________________

    void GfxStreamDevice::flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();

		uint16_t size = 16 + 20 + 20 + 8 + 8 + 2;
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipDrawWave, GfxStream::SpxFormat::Int32_dec, size };
        (*m_pEncoder) << dest;

        (*m_pEncoder) << pTopBorder->length;
        (*m_pEncoder) << pTopBorder->thickness;
        (*m_pEncoder) << pTopBorder->color;
        (*m_pEncoder) << pTopBorder->hold;

        (*m_pEncoder) << pBottomBorder->length;
        (*m_pEncoder) << pBottomBorder->thickness;
        (*m_pEncoder) << pBottomBorder->color;
        (*m_pEncoder) << pBottomBorder->hold;

        (*m_pEncoder) << frontFill;
        (*m_pEncoder) << backFill;
        (*m_pEncoder) << flip;

        if( pTopBorder->length > 0 )
            _streamEdgeSamples( pTopBorder->length, 1, 1, pTopBorder->pWave );
        
        if( pBottomBorder->length > 0 )
            _streamEdgeSamples( pBottomBorder->length, 1, 1, pBottomBorder->pWave );
    }

    //____ _streamEdgeSamples() ______________________________________________________

    void GfxStreamDevice::_streamEdgeSamples( int nLines, int samplesPerLine, int linePitch, const int * pSamples )
    {
        int allocSize = 0;
        
		if( samplesPerLine != linePitch )
        {
            allocSize = nLines * samplesPerLine * 4;

            int * pSampleBuffer = (int*) GfxBase::memStackAlloc(allocSize);
            int * wp = pSampleBuffer;

            for (int line = 0; line < nLines; line++)
            {
                for (int i = 0; i < samplesPerLine; i++)
                    *wp++ = pSamples[i];
                pSamples += linePitch;
            }

            pSamples = pSampleBuffer;
        }
        
        int	nSamples = nLines * samplesPerLine;

		auto spxFormat = _spxFieldFormat( pSamples, pSamples + nSamples );
		int spxSize = GfxStream::spxSize(spxFormat);
		
		int maxSamplesPerChunk = (GfxStream::c_maxBlockSize - sizeof(GfxStream::Header)) / spxSize;

        while (nSamples > 0)
        {
            uint16_t chunkSamples = min(nSamples, maxSamplesPerChunk);

            (*m_pEncoder) << GfxStream::Header{ GfxChunkId::EdgeSamples, spxFormat, (uint16_t) ((chunkSamples*spxSize+1)&0xFFFE) };
            (*m_pEncoder) << GfxStream::WriteSpxArray{ chunkSamples, spxFormat, pSamples, nullptr };

            pSamples += chunkSamples;
            nSamples -= chunkSamples;
        }
		m_pEncoder->align();
		
        if( allocSize > 0 )
            GfxBase::memStackFree(allocSize);

    }

    //____ _drawElipse() _______________________________________________________________

    void GfxStreamDevice::drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness, HiColor outlineColor )
    {
		if( _clippedOut( canvas ) )
			return;

		_streamStatesIfUpdated();

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawElipse, GfxStream::SpxFormat::Int32_dec, 40 };
        (*m_pEncoder) << canvas;
        (*m_pEncoder) << thickness;
        (*m_pEncoder) << color;
        (*m_pEncoder) << outlineThickness;
        (*m_pEncoder) << outlineColor;
    }

    //____ drawPieChart() ______________________________________________________________

    void GfxStreamDevice::drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize, HiColor hubColor, HiColor backColor, bool bRectangular)
    {
		if( _clippedOut( canvas ) )
			return;

		_streamStatesIfUpdated();

		uint16_t size = 16 + 4 + 4 + 4 + 8 + 8 + 2 + nSlices*(4+8);
        
        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawPieChart, GfxStream::SpxFormat::Int32_dec, size };
        (*m_pEncoder) << canvas;
        (*m_pEncoder) << start;
        (*m_pEncoder) << nSlices;
        (*m_pEncoder) << hubSize;
        (*m_pEncoder) << hubColor;
        (*m_pEncoder) << backColor;
        (*m_pEncoder) << bRectangular;
        
        (*m_pEncoder) << GfxStream::WriteBytes{ nSlices*4, pSliceSizes };
        (*m_pEncoder) << GfxStream::WriteBytes{ nSlices*8, pSliceColors };
    }

    //____ drawSegments() _________________________________________________________________

    void GfxStreamDevice::drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode )
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();

        int nColors;
        switch( tintMode )
        {
            default:
            case TintMode::None:
            case TintMode::Flat:
                nColors = nSegments;
                break;
            case TintMode::GradientX:
            case TintMode::GradientY:
                nColors = nSegments*2;
                break;
            case TintMode::GradientXY:
                nColors = nSegments*4;
                break;
        }

        uint16_t size = 16 + 2 + 2 + 2 + nColors*8;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawSegments, GfxStream::SpxFormat::Int32_dec, size };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << (uint16_t) nSegments;
        (*m_pEncoder) << (uint16_t) nEdgeStrips;
        (*m_pEncoder) << tintMode;

        (*m_pEncoder) << GfxStream::WriteBytes{ nColors*8, pSegmentColors };

        _streamEdgeSamples( nEdgeStrips, nSegments-1, edgeStripPitch, pEdgeStrips );
    }

    //____ flipDrawSegments() _______________________________________________________

    void GfxStreamDevice::flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode)
    {
		if( _clippedOut( dest ) )
			return;

		_streamStatesIfUpdated();

		int nColors;
        switch( tintMode )
        {
            default:
            case TintMode::None:
            case TintMode::Flat:
                nColors = nSegments;
                break;
            case TintMode::GradientX:
            case TintMode::GradientY:
                nColors = nSegments*2;
                break;
            case TintMode::GradientXY:
                nColors = nSegments*4;
                break;
        }

        uint16_t size = 16 + 2 + 2 + 2 + 2 + nColors*8;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipDrawSegments, GfxStream::SpxFormat::Int32_dec, size };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << (uint16_t) nSegments;
        (*m_pEncoder) << (uint16_t) nEdgeStrips;
        (*m_pEncoder) << flip;
        (*m_pEncoder) << tintMode;

        (*m_pEncoder) << GfxStream::WriteBytes{ nColors*8, pSegmentColors };

        _streamEdgeSamples( nEdgeStrips, nSegments-1, edgeStripPitch, pEdgeStrips );
    }

	//____ drawEdgemap() __________________________________________________________

	void GfxStreamDevice::drawEdgemap(CoordSPX dest, Edgemap * _pEdgemap )
	{
        auto pEdgemap = dynamic_cast<GfxStreamEdgemap*>(_pEdgemap);
        if (!pEdgemap)
        {
            //TODO: Errorhandling

            return;
        }

		if (_clippedOut({dest,pEdgemap->pixelSize()*64}))
            return;

        _streamStatesIfUpdated();

        GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest));

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::DrawEdgemap, spxFormat, GfxStream::spxSize(spxFormat) * 4 + 2 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << pEdgemap->m_inStreamId;
	}

	//____ flipDrawEdgemap() ______________________________________________________

	void GfxStreamDevice::flipDrawEdgemap(CoordSPX dest, Edgemap * _pEdgemap, GfxFlip flip)
	{
		//TODO: ClipOut!!!
		
        auto pEdgemap = dynamic_cast<GfxStreamEdgemap*>(_pEdgemap);
        if (!pEdgemap)
        {
            //TODO: Errorhandling

            return;
        }

//        if (_clippedOut(dest))
//            return;

        _streamStatesIfUpdated();

        GfxStream::SpxFormat spxFormat = _evaluateMask(_spxMask(dest));

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::FlipDrawEdgemap, spxFormat, GfxStream::spxSize(spxFormat) * 4 + 2 + 2 };
        (*m_pEncoder) << dest;
        (*m_pEncoder) << pEdgemap->m_inStreamId;
        (*m_pEncoder) << flip;
    }

    //.____ blitNinePatch() ___________________________________________

    void GfxStreamDevice::blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale)
    {
		if( _clippedOut( dstRect ) )
			return;

		_streamStatesIfUpdated();
		_streamBlitSourceIfUpdated();

		uint16_t size = 16 + 16 + ( 16 + 8 + 10 + 10 ) + 4;

        (*m_pEncoder) << GfxStream::Header{ GfxChunkId::BlitNinePatch, GfxStream::SpxFormat::Int32_dec, size };
        (*m_pEncoder) << dstRect;
        (*m_pEncoder) << dstFrame;

        (*m_pEncoder) << patch.block;
        (*m_pEncoder) << patch.frame;

        (*m_pEncoder) << patch.rigidPartXOfs;
        (*m_pEncoder) << patch.rigidPartXLength;
        (*m_pEncoder) << patch.rigidPartXSections;

        (*m_pEncoder) << patch.rigidPartYOfs;
        (*m_pEncoder) << patch.rigidPartYLength;
        (*m_pEncoder) << patch.rigidPartYSections;

        (*m_pEncoder) << scale;
    }

	//.____ _clippedOut() ________________________________________________

	bool GfxStreamDevice::_clippedOut(const RectSPX& rect)
	{
		if( !rect.isOverlapping(m_clipBounds) )
			return true;
		
		if( m_nClipRects < 1 )
			return false;
		
		for( int i = 0 ; i < m_nClipRects ; i++ )
			if( rect.isOverlapping(m_pClipRects[i]) )
				return false;

		return true;
	}


    //.____ _beginCanvasUpdate() __________________________________________

    bool GfxStreamDevice::_beginCanvasUpdate(CanvasRef canvasRef, Surface * pCanvasSurface, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
    {
		if( pLayers )
		{
			// No support for canvas layers yet!
			
			return false;
		}

		m_stateStack.push_back({ m_streamedBlendMode, m_streamedTintColor, m_streamedRenderLayer});
		
		
		if( !GfxDevice::_beginCanvasUpdate(canvasRef, pCanvasSurface, nUpdateRects, pUpdateRects, pLayers, startLayer) )
		{
			m_stateStack.pop_back();
			return false;
		}

		m_streamedBlendMode = m_blendMode;
		m_streamedRenderLayer = m_renderLayer;
		m_streamedTintColor = m_tintColor;

		
		uint16_t size = 2 + 1 + 1 + nUpdateRects * 16;

		(*m_pEncoder) << GfxStream::Header{ GfxChunkId::BeginCanvasUpdate, {}, size };
		(*m_pEncoder) << (pCanvasSurface ? static_cast<GfxStreamSurface*>(pCanvasSurface)->m_inStreamId : (uint16_t) 0);
		(*m_pEncoder) << canvasRef;
		(*m_pEncoder) << (uint8_t) 0;				// Dummy, was nUpdateRects;
		(*m_pEncoder) << GfxStream::WriteBytes{ nUpdateRects*16, pUpdateRects };

        return true;
    }

	//____ _canvasWasChanged() ___________________________________________________

    void GfxStreamDevice::_canvasWasChanged()
    {
	}

    void GfxStreamDevice::_renderLayerWasChanged()
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void GfxStreamDevice::_transformBlitSimple(const RectI& dest, CoordI src, const int simpleTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

    void GfxStreamDevice::_transformBlitComplex(const RectI& dest, BinalCoord src, const binalInt complexTransform[2][2])
    {
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
    }

	//____ _transformDrawSegments() ______________________________________

	void GfxStreamDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        //This method should never be called, but is pure virtual in super class.
        
        assert(false);
	}


} // namespace wg

