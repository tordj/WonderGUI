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
#ifndef WG_CANVASSTACK_DOT_H
#define WG_CANVASSTACK_DOT_H
#pragma once

#include <wg_capsule.h>
#include <wg3_surface.h>
#include <wg_patches.h>
#include <vector>

//____ CanvasStack ________________________________________________________

class WgCanvasStack : public WgCapsule
{
public:

    WgCanvasStack();
    ~WgCanvasStack();

    const char *Type( void ) const override;
    static const char * GetClass();
    WgWidget * NewOfMyType() const override { return new WgCanvasStack(); };


    void		SetPreferredSize(WgSize preferred);
    WgSize		PreferredPixelSize() const override;
    int			MatchingPixelWidth(int height) const override;
    int			MatchingPixelHeight(int height) const override;


    void		SetCanvases(int nbCanvases);
    wg::Surface_p Canvas(int idx) const;
    void		ForceRedraw();

    void		SetCreateFunc(const std::function<wg::Surface_p(int, WgSize)>& func);
    void		SetClearFunc(const std::function<void(wg::GfxDevice*, std::vector<wg::Surface_p>&)>& func);
    void		SetCombineFunc(const std::function<void(wg::GfxDevice*, wg::Surface*, std::vector<wg::Surface_p>&)>& func);

    wg::Skin_p	RedirectSkin(wg::Skin * pSkin, int canvasIdx);
    WgCapsule*  RedirectWidget(WgWidget * pWidget, int canvasIdx);

protected:

    void        _renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches ) override;
    void		_onCloneContent(const WgWidget * _pOrg) override;
    void		_onNewSize(const WgSize& size) override;

    void        _onRenderRequested() override;
    void        _onRenderRequested(const WgRect& rect) override;

private:

    wg::Surface_p	_createCanvas(int canvasIdx, wg::SizeI size);
    void		_clearCanvases(wg::GfxDevice * pDevice);
    void		_combineCanvases(wg::GfxDevice * pDevice);

    void		_regenCanvases(int nbCanvases, wg::SizeI canvasSize);

    WgSize      _contentPaddingSize() const { return m_pSkin ? _skinContentPadding(m_pSkin,m_scale) : WgSize(); }
    WgRect      _contentRect() const { return m_pSkin ? _skinContentRect(m_pSkin,m_size,m_state,m_scale) : WgRect(m_size); }

    WgSize					m_preferred;                // Preferred size in points.
    WgSize                  m_size;
    bool					m_bRendering = false;

    std::vector<wg::Surface_p>	m_canvases;
    wg::Surface_p				m_pEndCanvas;
    WgPatches					m_patches;			// Patches from internal request render calls.

    std::function<wg::Surface_p(int, WgSize)>								m_createFunc;
    std::function<void(wg::GfxDevice*, std::vector<wg::Surface_p>&)>				m_clearFunc;
    std::function<void(wg::GfxDevice*, wg::Surface*, std::vector<wg::Surface_p>&)>	m_combineFunc;

        //____ CustomSkin _____________________________________________________

        class CustomSkin : public wg::Skin
        {
        public:
            CustomSkin(WgCanvasStack * pHolder, wg::Skin * pSkin, int canvasIdx);

            const wg::TypeInfo&		typeInfo(void) const override;
            const static wg::TypeInfo	TYPEINFO;

            wg::Size    minSize() const override;
            wg::Size    preferredSize() const override;

            wg::Size    sizeForContent(const wg::Size& contentSize) const override;
            wg::Border  contentPadding(wg::State state) const override;
            wg::Size    contentPaddingSize() const override;
            wg::Coord   contentOfs(wg::State state) const override;
            wg::Rect    contentRect(const wg::Rect& canvas, wg::State state) const override;

            bool    isOpaque(wg::State state) const override;
            bool    isOpaque(const wg::Rect& rect, const wg::Size& canvasSize, wg::State state) const override;

            bool    isStateIdentical(wg::State state, wg::State comparedTo, float fraction = 1.f) const override;

            bool    markTest(const wg::Coord& ofs, const wg::Rect& canvas, wg::State state, int opacityTreshold, float fraction = 1.f ) const override;
            void    render(wg::GfxDevice * pDevice, const wg::Rect& canvas, wg::State state, float fraction = 1.f) const override;

            wg::Rect    fractionChangeRect(const wg::Rect& canvas, wg::State state, float oldFraction, float newFraction) const override;


        protected:

            WgWidgetWeakPtr	m_pStack;
            wg::Skin_p		m_pSkin;
            int				m_canvasIdx;
        };


    //____ CustomCapsule __________________________________________________

    class CustomCapsule : public WgCapsule
    {
    public:
        CustomCapsule() {};
        CustomCapsule(WgCanvasStack * pHolder, int canvasIdx);

        const char *        Type( void ) const override;

//      const TypeInfo&	typeInfo(void) const override;
//      const static TypeInfo	TYPEINFO;

        virtual WgWidget* NewOfMyType() const override { return new CustomCapsule(); };
    protected:

        void            _renderPatches( wg::GfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches ) override;

    private:
        WgWidgetWeakPtr	m_pStack;
        int				m_canvasIdx = 0;
    };
};

#endif //WG_CANVASSTACK_DOT_H
