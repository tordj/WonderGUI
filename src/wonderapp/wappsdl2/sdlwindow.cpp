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
#include <sdlwindow.h>

#ifdef WIN32
#    include <SDL.h>
#    include <SDL_image.h>
#    include <Windows.h>
#    include <libloaderapi.h>
#elif __APPLE__
#    include <SDL2/SDL.h>
#    include <SDL2_image/SDL_image.h>
#    include <dlfcn.h>
#else
#    include <SDL2/SDL.h>
#    include <SDL2/SDL_image.h>
#    include <dlfcn.h>
#endif


using namespace wg;


const TypeInfo SDLWindow::TYPEINFO = { "SDLWindow", &Window::TYPEINFO };

//____ constructor ___________________________________________________

SDLWindow::SDLWindow(const std::string& title, wg::RootPanel* pRootPanel, const wg::Rect& geo, SDL_Window* pSDLWindow)
    : Window(pRootPanel, geo)
{
    m_pSDLWindow = pSDLWindow;
}

//____ destructor _____________________________________________________

SDLWindow::~SDLWindow()
{
    SDL_DestroyWindow(m_pSDLWindow);
}

//____ typeInfo() _________________________________________________________

const wg::TypeInfo& SDLWindow::typeInfo(void) const
{
    return TYPEINFO;
}

//___ setTitle() ______________________________________________________________

bool SDLWindow::setTitle(std::string& title)
{
    SDL_SetWindowTitle(m_pSDLWindow, title.c_str());
    return true;
}

//____ setIcon() ______________________________________________________________

bool SDLWindow::setIcon(Surface* pIcon)
{
    //TODO: IMPLEMENT!

    return false;
}

//____ title() ________________________________________________________________

std::string SDLWindow::title() const
{
    return std::string(SDL_GetWindowTitle(m_pSDLWindow));
}


//____ render() _______________________________________________________________

void SDLWindow::render()
{
    m_pRootPanel->render();

    //TODO: Just update the dirty rectangles!

    SDL_UpdateWindowSurface(m_pSDLWindow);
}

//____ SDLWindowId() __________________________________________________________

uint32_t SDLWindow::SDLWindowId()
{
    return SDL_GetWindowID(m_pSDLWindow);
}


//____ _updateWindowGeo() _____________________________________________________

Rect SDLWindow::_updateWindowGeo(const Rect& geo)
{
    //TODO: Calculate and update position as well.

    SDL_SetWindowSize(m_pSDLWindow, geo.w, geo.h);
    return m_geo;
}
