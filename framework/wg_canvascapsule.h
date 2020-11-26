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

#ifndef WG_CANVASCAPSULE_DOT_H
#define WG_CANVASCAPSULE_DOT_H

#ifndef WG_CAPSULE_DOT_H
#    include <wg_capsule.h>
#endif

#include <wg3_surfacefactory.h>

#include <wg_patches.h>

#include <wg_color.h>

class WgCanvasCapsule : public WgCapsule
{
public:
	WgCanvasCapsule();
	~WgCanvasCapsule();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgCanvasCapsule(); };

//    void                SetSkin(wg::Skin * pSkin);        // Method added separately to those widgets that support skin so far.

	void                SetSurfaceFactory(wg::SurfaceFactory * pFactory);
	wg::SurfaceFactory_p SurfaceFactory() const { return m_pFactory; }

	void                StartFade(WgColor destination, int ms);
	void                StopFade();

    void                SetCanvasFillColor(WgColor color);
    inline WgColor      CanvasFillColor() const { return m_canvasFillColor; }
    
	void                SetColor( const WgColor& color);
	void                SetTintMode( WgTintMode mode );
	void                SetBlendMode( WgBlendMode mode );

	inline WgColor      Color() { return m_tintColor; }
	inline WgBlendMode  BlendMode() { return m_blendMode; }
	inline WgTintMode   TintMode() { return m_tintMode; }

	WgWidget * 			FindWidget(const WgCoord& ofs, WgSearchMode mode);

protected:
	void            _onEvent(const WgEvent::Event * pEvent, WgEventHandler * pHandler);
	void            _renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches );
	void            _onRender(wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window);
	void            _onCloneContent( const WgWidget * _pOrg );
	WgBlendMode     _getBlendMode() const;

	void            _onNewSize(const WgSize& size);

	void            _onRenderRequested();
	void            _onRenderRequested(const WgRect& rect);

	void _onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip );

	void _onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode );


private:
	WgColor            m_tintColor;
	WgColor            m_fadeStartColor;
	WgColor            m_fadeEndColor;
    WgColor            m_canvasFillColor = WgColor::Transparent;
	int                m_fadeTime;
	int                m_fadeTimeCounter;

	WgBlendMode        m_blendMode;
	WgTintMode         m_tintMode;

	wg::SurfaceFactory_p m_pFactory;
	wg::Surface_p       m_pCanvas;
	WgPatches           m_dirtyPatches;            // Dirty patches on our own canvas.
};

#endif //WG_CANVASCAPSULE_DOT_H
