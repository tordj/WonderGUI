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

#include <wonderapp.h>
#include <sdlwindow_metal.h>

#include <SDL2/SDL.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <wg_base.h>

#include <wg_metalsurface.h>
#include <wg_metalsurfacefactory.h>
#include <wg_metalgfxdevice.h>



using namespace wg;


const TypeInfo SDLWindowMetal::TYPEINFO = { "SDLMetalWindow", &SDLWindow::TYPEINFO };

//int SDLWindowGL::s_windowCounter = 0;
//bool SDLWindowGL::s_bInitialized = false;

//____ create() _______________________________________________________________

SDLWindow_p SDLWindow::create(const Blueprint& blueprint)
{
    wg::Rect geo;

    geo = { blueprint.pos + Coord(4,20), blueprint.size};

    uint32_t flags = SDL_WINDOW_METAL;

    if (blueprint.resizable)
        flags |= SDL_WINDOW_RESIZABLE;

    
    SDL_Window* pSDLWindow = SDL_CreateWindow(blueprint.title.c_str(), geo.x, geo.y, geo.w, geo.h, flags);
    if (pSDLWindow == NULL)
        return nullptr;

    if( !blueprint.minSize.isEmpty() )
        SDL_SetWindowMinimumSize(pSDLWindow,blueprint.minSize.w, blueprint.minSize.h);

    if( !blueprint.maxSize.isEmpty() )
        SDL_SetWindowMaximumSize(pSDLWindow,blueprint.maxSize.w, blueprint.maxSize.h);


    SDL_Renderer *renderer = SDL_CreateRenderer(pSDLWindow, -1, SDL_RENDERER_PRESENTVSYNC);
        const CAMetalLayer *swapchain = (__bridge CAMetalLayer *)SDL_RenderGetMetalLayer(renderer);
        const id<MTLDevice> gpu = swapchain.device;
    
   MetalGfxDevice::setMetalDevice(gpu);
   auto pDevice = MetalGfxDevice::create();
   Base::setDefaultGfxDevice(pDevice);

   auto pFactory = MetalSurfaceFactory::create();
   Base::setDefaultSurfaceFactory(pFactory);

    
    wg_static_cast<MetalGfxDevice_p>(Base::defaultGfxDevice())->setDefaultCanvas(nullptr, {int(geo.w),int(geo.h)}, PixelFormat::BGRA_8_linear);
    auto pRootPanel = RootPanel::create(CanvasRef::Default, nullptr);

    SDLWindowMetal_p pWindow = new SDLWindowMetal(blueprint.title, pRootPanel, geo, pSDLWindow, renderer);

    //TODO: This is ugly. It should be handled when windows gets focused.

    Base::inputHandler()->setFocusedWindow(pRootPanel);

//    SDLWindowGL::s_bInitialized = true;
    return pWindow;
}


//____ SDLWindowMetal constructor ___________________________________________________

SDLWindowMetal::SDLWindowMetal(const std::string& title, wg::RootPanel* pRootPanel, const wg::Rect& geo, SDL_Window* pSDLWindow, SDL_Renderer * pRenderer)
    : SDLWindow(title, pRootPanel, geo, pSDLWindow),
    m_pSDLRenderer(pRenderer)
{
//    s_windowCounter++;
}

//____ typeInfo() _________________________________________________________

const wg::TypeInfo& SDLWindowMetal::typeInfo(void) const
{
    return TYPEINFO;
}

//____ onWindowSizeUpdated() _________________________________________________

void SDLWindowMetal::onWindowSizeUpdated( int width, int height )
{
    m_geo.w = width;
    m_geo.h = height;
    
    wg_static_cast<MetalGfxDevice_p>(Base::defaultGfxDevice())->setDefaultCanvas(nullptr, SizeSPX(width, height), wg::PixelFormat::BGRA_8_linear, 64);

    m_pRootPanel->setCanvas(CanvasRef::Default);
}

//____ render() _______________________________________________________________

void SDLWindowMetal::render()
{
    // We use a single context for all our windows since we otherwise would
    // need to have a separate GlGfxDevice for each context to make things work on Windows.

    const CAMetalLayer *swapchain = (__bridge CAMetalLayer *)SDL_RenderGetMetalLayer(m_pSDLRenderer);
    const id<MTLCommandQueue> queue = [swapchain.device newCommandQueue];
	const id<MTLDevice> gpu = swapchain.device;

	
    @autoreleasepool {
        id<CAMetalDrawable> surface = [swapchain nextDrawable];

//        MTLClearColor color = MTLClearColorMake(0, 0, 0, 0);

        MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
//        pass.colorAttachments[0].clearColor = color;
        pass.colorAttachments[0].loadAction  = MTLLoadActionLoad;
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.colorAttachments[0].texture = surface.texture;

  
//		wg_static_cast<MetalGfxDevice_p>(Base::defaultGfxDevice())->setMetalDevice(gpu);
        wg_static_cast<MetalGfxDevice_p>(Base::defaultGfxDevice())->setDefaultCanvas(pass, {int(m_geo.w),int(m_geo.h)}, wg::PixelFormat::BGRA_8_linear);

        wg_static_cast<MetalGfxDevice_p>(Base::defaultGfxDevice())->autopresent(surface);
        
		m_pRootPanel->addDirtyPatch({0,0,int(m_geo.w)*64,int(m_geo.h)*64});
		
        m_pRootPanel->render();
/*
        id<MTLCommandBuffer> buffer = [queue commandBuffer];
        id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:pass];
        [encoder endEncoding];
        [buffer presentDrawable:surface];
        [buffer commit];
*/
        
    }
    
    
    

    //TODO: Just update the dirty rectangles!
/*
 
    int nRects = m_pRootPanel->nbUpdatedRects();
    if( nRects == 0 )
        return;

    const RectSPX * pUpdatedRects = m_pRootPanel->firstUpdatedRect();
    SDL_Rect * pSDLRects = (SDL_Rect*) Base::memStackAlloc( sizeof(SDL_Rect) * nRects );

    for( int i = 0 ; i < nRects ; i++ )
    {
        pSDLRects[i].x = pUpdatedRects[i].x/64;
        pSDLRects[i].y = pUpdatedRects[i].y/64;
        pSDLRects[i].w = pUpdatedRects[i].w/64;
        pSDLRects[i].h = pUpdatedRects[i].h/64;
    }

    SDL_UpdateWindowSurfaceRects( m_pSDLWindow, pSDLRects, nRects );

    Base::memStackFree( sizeof(SDL_Rect) * nRects );
 */
 
//    SDL_UpdateWindowSurface(m_pSDLWindow);
}
