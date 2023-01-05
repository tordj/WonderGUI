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
#pragma once

#include <wonderapp.h>
#include <wondergui.h>

//____ MyWindow _______________________________________________________________

class MyWindow;
typedef	wg::StrongPtr<MyWindow>		MyWindow_p;
typedef	wg::WeakPtr<MyWindow>		MyWindow_wp;


class MyWindow : public WonderApp::Window
{
public:
	static MyWindow_p		create(const Blueprint& blueprint);

	bool			setTitle(std::string& title) override;
	std::string		title() const override { return m_title; }

	void			render();

	Uint32			SDLWindowId() { return SDL_GetWindowID(m_pSDLWindow); }

protected:
	MyWindow(const std::string& title, wg::RootPanel* pRootPanel, const wg::Rect& geo, SDL_Window* pSDLWindow);
	~MyWindow() {}

	wg::Rect	_updateWindowGeo(const wg::Rect& geo) override;

	std::string		m_title;
	SDL_Window*		m_pSDLWindow;
};
