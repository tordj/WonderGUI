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
#include <sdlwindow_gl.h>

#ifdef WIN32
#    include <SDL.h>
#    include <Windows.h>
#   include <GL/glew.h>
#elif __APPLE__
#    include <SDL2/SDL.h>
#else
#    include <SDL2/SDL.h>
#   include <GL/glew.h>
#endif

#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>
#include <wg_glgfxdevice.h>



using namespace wg;


const TypeInfo SDLWindowGL::TYPEINFO = { "SDLOpenGLWindow", &SDLWindow::TYPEINFO };

int SDLWindowGL::s_windowCounter = 0;


//____ create() _______________________________________________________________

SDLWindow_p SDLWindow::create(const Blueprint& blueprint)
{
    Rect geo;

    geo = { blueprint.pos + Coord(4,20), blueprint.size};

    uint32_t flags = SDL_WINDOW_OPENGL;

    if (blueprint.resizable)
        flags |= SDL_WINDOW_RESIZABLE;

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDLWindowGL::s_windowCounter > 0 );
    
    SDL_Window* pSDLWindow = SDL_CreateWindow(blueprint.title.c_str(), geo.x, geo.y, geo.w, geo.h, flags);
    if (pSDLWindow == NULL)
        return nullptr;

    if( !blueprint.minSize.isEmpty() )
        SDL_SetWindowMinimumSize(pSDLWindow,blueprint.minSize.w, blueprint.minSize.h);

    if( !blueprint.maxSize.isEmpty() )
        SDL_SetWindowMaximumSize(pSDLWindow,blueprint.maxSize.w, blueprint.maxSize.h);

    
    SDL_GLContext glContext = SDL_GL_CreateContext(pSDLWindow);
    
    SDL_GL_MakeCurrent( pSDLWindow, glContext );
/*
    glDrawBuffer(GL_FRONT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
*/
    
    auto pDevice = Base::defaultGfxDevice();
    if( !pDevice )
    {
        pDevice = GlGfxDevice::create();
        Base::setDefaultGfxDevice(pDevice);
        
        auto pFactory = GlSurfaceFactory::create();
        Base::setDefaultSurfaceFactory(pFactory);
    }
    
    wg_static_cast<GlGfxDevice_p>(pDevice)->setDefaultCanvas({int(geo.w)*64,int(geo.h)*64});
    auto pRootPanel = RootPanel::create(CanvasRef::Default, nullptr);

    SDLWindowGL_p pWindow = new SDLWindowGL(blueprint.title, pRootPanel, geo, pSDLWindow, glContext);

    //TODO: This is ugly. It should be handled when windows gets focused.

    Base::inputHandler()->setFocusedWindow(pRootPanel);

    return pWindow;
}


//____ SDLWindowSW constructor ___________________________________________________

SDLWindowGL::SDLWindowGL(const std::string& title, wg::RootPanel* pRootPanel, const wg::Rect& geo, SDL_Window* pSDLWindow, SDL_GLContext glContext)
    : SDLWindow(title, pRootPanel, geo, pSDLWindow),
    m_SDLGLContext(glContext)
{
    s_windowCounter++;
}

//____ typeInfo() _________________________________________________________

const wg::TypeInfo& SDLWindowGL::typeInfo(void) const
{
    return TYPEINFO;
}

//____ onWindowSizeUpdated() _________________________________________________

void SDLWindowGL::onWindowSizeUpdated( int width, int height )
{
    m_geo.w = width;
    m_geo.h = height;
    
    wg_static_cast<GlGfxDevice_p>(Base::defaultGfxDevice())->setDefaultCanvas(SizeSPX(width * 64, height * 64), 64);

    m_pRootPanel->setCanvas(CanvasRef::Default);
}

//____ render() _______________________________________________________________

void SDLWindowGL::render()
{
    SDL_GL_MakeCurrent( m_pSDLWindow, m_SDLGLContext );
    
    wg_static_cast<GlGfxDevice_p>(Base::defaultGfxDevice())->setDefaultCanvas({int(m_geo.w)*64,int(m_geo.h)*64});

    
    m_pRootPanel->render();

    //TODO: Just update the dirty rectangles!

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

//    SDL_UpdateWindowSurface(m_pSDLWindow);
}
