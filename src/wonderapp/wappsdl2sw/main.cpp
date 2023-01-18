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
#include <wondergui.h>

#ifdef WIN32
#	include <SDL.h>
#	include <SDL_image.h>
#	include <Windows.h>
#	include <libloaderapi.h>
#elif __APPLE__
#	include <SDL2/SDL.h>
#	include <SDL2_image/SDL_image.h>
#	include <dlfcn.h>
#else
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_image.h>
#	include <dlfcn.h>
#endif


#include <fstream>
#include <mutex>

#include <wg_softgfxdevice.h>
#include <wg_softsurfacefactory.h>
#include <wg_softkernels_default.h>

#include "window.h"
#include "tinyfiledialogs.h"

using namespace wg;

//____ MyAppVisitor ________________________________________________________

class MyAppVisitor : public WonderApp::Visitor
{
public:

	std::vector<std::string> programArguments() const override;

	int64_t			time() override
	{
		return SDL_GetPerformanceCounter() * 1000000 / SDL_GetPerformanceFrequency();
	}

	Blob_p 			loadBlob(const std::string& path) override;
	Surface_p 		loadSurface(const std::string& path, SurfaceFactory* pFactory, const Surface::Blueprint& bp = Surface::Blueprint() ) override;


	bool			notifyPopup(const std::string& title, const std::string& message, WonderApp::IconType iconType) override;

	WonderApp::DialogButton	messageBox(	const std::string& title, const std::string& message, WonderApp::DialogType dialogType,
										WonderApp::IconType iconType, WonderApp::DialogButton defaultButton = WonderApp::DialogButton::Undefined) override;

	std::string		inputBox( const std::string& title, const std::string& message, const std::string& defaultInput) override;

	std::string		saveFileDialog(	const std::string& title, const std::string& defaultPathAndFile,
									const std::vector<std::string>& filterPatterns,
									const std::string& singleFilterDescription) override;

	std::string		openFileDialog( const std::string& title, const std::string& defaultPathAndFile,
									const std::vector<std::string>& filterPatterns,
									const std::string& singleFilterDescription) override;

	std::vector<std::string> openMultiFileDialog(	const std::string& title, const std::string& defaultPathAndFile,
													const std::vector<std::string>& filterPatterns,
													const std::string& singleFilterDescription) override;

	std::string		selectFolderDialog(const std::string& title, const std::string& defaultPath) override;
	
	WonderApp::Window_p	createWindow(const WonderApp::Window::Blueprint& blueprint) override;

	WonderApp::LibId	openLibrary(const std::string& path) override;
	void*			loadSymbol(WonderApp::LibId lib, const std::string& symbol) override;
	bool			closeLibrary(WonderApp::LibId lib) override;


protected:
	void			convertSDLFormat(PixelDescription* pWGFormat, const SDL_PixelFormat* pSDLFormat);

};

//____ MyHostBridge ___________________________________________________________

class MyHostBridge : public HostBridge
{
public:
	bool		hidePointer() override;
	bool		showPointer() override;
	
	std::string	getClipboardText() override;
	bool		setClipboardText(const std::string& text) override;
};


//______________________________________________________________________________

bool		init_system();
void		exit_system();

bool		process_system_events();

bool		init_wondergui();
void		exit_wondergui();


MouseButton translateSDLMouseButton(Uint8 button);

Surface_p generateWindowSurface(SDL_Window* pWindow, int widht, int height);


float				g_scale;

GfxDevice_p			g_pGfxDevice;
SurfaceFactory_p	g_pSurfaceFactory;

MyHostBridge *		g_pHostBridge = nullptr;

std::vector<MyWindow_wp>	g_windows;

int			g_argc = 0;
char**		g_argv = nullptr;


static const char * iconNames[4] = { "info", "warning", "error", "question" };
static const char * dialogNames[4] = { "ok", "okcancel", "yesno", "yesnocancel" };


std::mutex g_winResizeEventMutex;


//____ main() _________________________________________________________________

int main(int argc, char *argv[] )
{
	g_argc = argc;
	g_argv = argv;

	// Create app and visitor, make any app-specif initialization

	auto pApp = WonderApp::create();
	auto pVisitor = new MyAppVisitor();

	// Get apps window size before we continue

	g_scale = 1.f;


	if (!init_system() )
		return -1;
	
	if (!init_wondergui() )
		return -1;

	// Initialize the app

	bool bContinue = pApp->init( pVisitor );

	// Main loop

	while (bContinue)
	{
		// Handle system events

		bContinue = process_system_events();

		if (bContinue)
		{
			// Don't collide with eventWatcher.
			
			const std::lock_guard<std::mutex> lock(g_winResizeEventMutex);

			// Uppdate the app

			bContinue = pApp->update();

			// Dispatch messages

			Base::msgRouter()->dispatch();

			// Periodic update

			Base::update(SDL_GetPerformanceCounter() * 1000000 / SDL_GetPerformanceFrequency());

			// Remove any windows pointers for erased windows.

			g_windows.erase(std::remove(g_windows.begin(), g_windows.end(), nullptr), g_windows.end());


			// Render. We do this outside the app since we might want to 
			// handle updated rectangles in a system specific way.

			for (auto& pWin : g_windows )
			{
				pWin->render();		
			}

			// Sleep for a while

			SDL_Delay(10);
		}
	}

	// Exit the app

	pApp->exit();

	//

	delete pVisitor;

	exit_wondergui();
	exit_system();
	return 0;
}


//____ init_wondergui() _______________________________________________________

bool init_wondergui()
{
	g_pHostBridge = new MyHostBridge();
	
	Base::init(g_pHostBridge);


	auto pGfxDevice = SoftGfxDevice::create();
	addDefaultSoftKernels( pGfxDevice );

	auto pSurfaceFactory = SoftSurfaceFactory::create();
	
	Context_p pContext = Context::create();
    pContext->setSurfaceFactory(pSurfaceFactory);
	pContext->setGfxDevice(pGfxDevice);
	pContext->setGammaCorrection(true);
	Base::setActiveContext(pContext);

	InputHandler_p pInput = Base::inputHandler();
	
	pInput->mapKey(SDLK_LEFT, Key::Left);
	pInput->mapKey(SDLK_RIGHT, Key::Right);
	pInput->mapKey(SDLK_UP, Key::Up);
	pInput->mapKey(SDLK_DOWN, Key::Down);
	pInput->mapKey(SDLK_BACKSPACE, Key::Backspace);
	pInput->mapKey(SDLK_DELETE, Key::Delete);
	pInput->mapKey(SDLK_END, Key::End);
	pInput->mapKey(SDLK_ESCAPE, Key::Escape);
	pInput->mapKey(SDLK_HOME, Key::Home);
	pInput->mapKey(SDLK_PAGEDOWN, Key::PageDown);
	pInput->mapKey(SDLK_PAGEUP, Key::PageUp);
	pInput->mapKey(SDLK_RETURN, Key::Return);
	pInput->mapKey(SDLK_SPACE, Key::Space);
	pInput->mapKey(SDLK_TAB, Key::Tab);
	pInput->mapKey(SDLK_F1, Key::F1);
	pInput->mapKey(SDLK_F2, Key::F2);
	pInput->mapKey(SDLK_F3, Key::F3);
	pInput->mapKey(SDLK_F4, Key::F4);
	pInput->mapKey(SDLK_F5, Key::F5);
	pInput->mapKey(SDLK_F6, Key::F6);
	pInput->mapKey(SDLK_F7, Key::F7);
	pInput->mapKey(SDLK_F8, Key::F8);
	pInput->mapKey(SDLK_F9, Key::F9);
	pInput->mapKey(SDLK_F10, Key::F10);
	pInput->mapKey(SDLK_F11, Key::F11);
	pInput->mapKey(SDLK_F12, Key::F12);


	pInput->mapKey(SDLK_LCTRL, Key::Control);
	pInput->mapKey(SDLK_RCTRL, Key::Control);

	pInput->mapKey(SDLK_LSHIFT, Key::Shift);
	pInput->mapKey(SDLK_RSHIFT, Key::Shift);

	pInput->mapKey(SDLK_LALT, Key::Alt);
	pInput->mapKey(SDLK_RALT, Key::Alt);

	pInput->mapKey(SDLK_LGUI, Key::Super);
	pInput->mapKey(SDLK_RGUI, Key::Super);

	
	pInput->mapKey(SDLK_KP_ENTER, Key::Return);


#ifdef __APPLE__
	pInput->mapCommand(SDLK_x, ModKeys::Super, EditCmd::Cut);
	pInput->mapCommand(SDLK_c, ModKeys::Super, EditCmd::Copy);
	pInput->mapCommand(SDLK_v, ModKeys::Super, EditCmd::Paste);

	pInput->mapCommand(SDLK_a, ModKeys::Super, EditCmd::SelectAll);

	pInput->mapCommand(SDLK_z, ModKeys::Super, EditCmd::Undo);
	pInput->mapCommand(SDLK_z, ModKeys::SuperShift, EditCmd::Redo);
#else
	pInput->mapCommand(SDLK_x, ModKeys::Ctrl, EditCmd::Cut);
	pInput->mapCommand(SDLK_c, ModKeys::Ctrl, EditCmd::Copy);
	pInput->mapCommand(SDLK_v, ModKeys::Ctrl, EditCmd::Paste);

	pInput->mapCommand(SDLK_a, ModKeys::Ctrl, EditCmd::SelectAll);

	pInput->mapCommand(SDLK_z, ModKeys::Ctrl, EditCmd::Undo);
	pInput->mapCommand(SDLK_z, ModKeys::CtrlShift, EditCmd::Redo);
#endif
	
	pInput->mapCommand(SDLK_ESCAPE, ModKeys::None, EditCmd::Escape);

	
	
//	g_pRoot->setDebugMode(true);

	return true;
}

//____ exit_wondergui() _______________________________________________________

void exit_wondergui()
{
	g_pGfxDevice = nullptr;

	Base::exit();
	delete g_pHostBridge;
	g_pHostBridge = nullptr;
}

//____ eventWatcher() _________________________________________________________

int eventWatcher(void * pNull, SDL_Event* pEvent)
{
	//WARNING! This function could potentially run in another thread!
	
	if (pEvent->type == SDL_WINDOWEVENT && pEvent->window.event == SDL_WINDOWEVENT_RESIZED)
	{
		const std::lock_guard<std::mutex> lock(g_winResizeEventMutex);

		for( int i = 0 ; i < g_windows.size() ; i++ )
		{
			if( g_windows[i] && g_windows[i]->SDLWindowId() == pEvent->window.windowID )
			{
				MyWindow * pWindow = g_windows[i];

				auto pWindowSurface = generateWindowSurface(pWindow->SDLWindow(), pEvent->window.data1, pEvent->window.data2);
				pWindow->rootPanel()->setCanvas(pWindowSurface);
				pWindow->render();
				break;
			}
		}
	}
	return 0;
}

//____ init_system() _______________________________________________________

bool init_system()
{
	// initialize SDL video
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return false;
	}

	SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");

	SDL_AddEventWatch(eventWatcher, nullptr);
	
	// make sure SDL cleans up before exit
	atexit(SDL_Quit);
 
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	return true;
}

//____ exit_system() __________________________________________________________

void exit_system()
{
//	SDL_DelEventWatch(eventWatcher, nullptr);

	IMG_Quit();
}

//____ _generateWindowSurface() _______________________________________________

Surface_p generateWindowSurface(SDL_Window* pWindow, int width, int height )
{
	SDL_Surface* pWinSurf = SDL_GetWindowSurface(pWindow);
	if (pWinSurf == nullptr)
	{
		//		printf("Unable to get window SDL Surface: %s\n", SDL_GetError());
		return nullptr;
	}

	PixelFormat format = PixelFormat::Undefined;

	switch (pWinSurf->format->BitsPerPixel)
	{
	case 32:
		format = PixelFormat::BGRX_8;
		break;
	case 24:
		format = PixelFormat::BGR_8;
		break;
	default:
	{
		printf("Unsupported pixelformat of SDL Surface!\n");
		return nullptr;
	}
	}

	Blob_p pCanvasBlob = Blob::create(pWinSurf->pixels, 0);
	auto pWindowSurface = SoftSurface::create(SizeI(width, height), format, pCanvasBlob, pWinSurf->pitch);

	return pWindowSurface;
}

//____ update_window_rects() __________________________________________________
/*
void update_window_rects(const Rect* pRects, int nRects)
{
	if (nRects == 0)
		return;

	std::vector<SDL_Rect>	rects;

	for (int i = 0; i < nRects; i++)
	{
		SDL_Rect r = { (int) pRects[i].x, (int) pRects[i].y, (int) pRects[i].w, (int) pRects[i].h };
		rects.push_back(r);
	}

	SDL_UpdateWindowSurfaceRects(g_pSDLWindow, &rects.front(), nRects);
}
*/


//____ process_system_events() ________________________________________________

bool process_system_events()
{
/*
	SDL_Event e;
	while (SDL_PollEvent(&e)) {

		switch (e.type)
		{
		case SDL_QUIT:
			return false;
		default:
			break;
		}
	}
*/

	// Process all the SDL events in a loop

	InputHandler_p pInput = Base::inputHandler();

	SDL_Event e;
	while (SDL_PollEvent(&e)) {

		switch (e.type)
		{
		case SDL_QUIT:
			return false;

		case SDL_MOUSEMOTION:
		{
			RootPanel* pRootPanel = nullptr;

			for (auto& wpWindow : g_windows)
			{
				if (wpWindow == nullptr)
					continue;

				MyWindow* pWin = wpWindow;
				if (pWin->SDLWindowId() == e.motion.windowID)
				{
					pRootPanel = pWin->rootPanel();
					break;
				}

			}

			pInput->setPointer(pRootPanel, Coord(e.motion.x, e.motion.y));
			break;
		}

		case SDL_MOUSEBUTTONDOWN:
			pInput->setButton(translateSDLMouseButton(e.button.button), true);
			break;

		case SDL_MOUSEBUTTONUP:
			pInput->setButton(translateSDLMouseButton(e.button.button), false);
			break;

		case SDL_MOUSEWHEEL:
		{
			bool bInvertScroll = false;
			Coord distance(e.wheel.x, e.wheel.y);
			if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			{
				bInvertScroll = true;
				distance *= -1;
			}

			if (e.wheel.y != 0)
				pInput->setWheelRoll(1, distance.y, bInvertScroll);

			if (e.wheel.x != 0)
				pInput->setWheelRoll(2, distance.x, bInvertScroll);
			break;
		}

		case SDL_KEYDOWN:
		{
			pInput->setKey(e.key.keysym.sym, true);
			break;
		}

		case SDL_KEYUP:
		{
			pInput->setKey(e.key.keysym.sym, false);
			break;
		}

		case SDL_TEXTINPUT:
			pInput->putText(e.text.text);
			break;

		case SDL_WINDOWEVENT:
		{
			MyWindow* pWindow = nullptr;

			for (auto& pWin : g_windows)
			{
				if (pWin && e.window.windowID == pWin->SDLWindowId())
				{
					pWindow = pWin;
					break;
				}
			}
			if (pWindow == nullptr)
				break;

			switch (e.window.event)
			{
				// See https://stackoverflow.com/questions/32294913/getting-continuous-window-resize-event-in-sdl-2
				// for how to do continous resize.

				case SDL_WINDOWEVENT_SIZE_CHANGED:		// Called for all in-between sizes.			
				{
					break;
				}

				case SDL_WINDOWEVENT_RESIZED:			// Called for final size.
				{
					Size size(e.window.data1, e.window.data2);

					if (size != pWindow->rootPanel()->geo().size())
					{
						auto pWindowSurface = generateWindowSurface(pWindow->SDLWindow(), e.window.data1, e.window.data2 );
						pWindow->rootPanel()->setCanvas(pWindowSurface);
					}
					break;
				}

				default:
					break;
			}
			break;
		}



		default:
			break;
		}
	}

	return true;
}

//____ translateSDLMouseButton() __________________________________________________
//
// Translate SDL mouse button enums to WonderGUI equivalents.
//
MouseButton translateSDLMouseButton(Uint8 button)
{
	switch (button)
	{
	default:
	case SDL_BUTTON_LEFT:
		return MouseButton::Left;
	case SDL_BUTTON_MIDDLE:
		return MouseButton::Middle;
	case SDL_BUTTON_RIGHT:
		return MouseButton::Right;
	case SDL_BUTTON_X1:
		return MouseButton::X1;
	case SDL_BUTTON_X2:
		return MouseButton::X2;
	}
}

//____ convertSDLFormat() ______________________________________________________

void MyAppVisitor::convertSDLFormat(PixelDescription* pWGFormat, const SDL_PixelFormat* pSDLFormat)
{
	pWGFormat->format = PixelFormat::Undefined;
	pWGFormat->bits = pSDLFormat->BitsPerPixel;
	pWGFormat->bIndexed = (pSDLFormat->palette != nullptr);

	pWGFormat->R_mask = pSDLFormat->Rmask;
	pWGFormat->G_mask = pSDLFormat->Gmask;
	pWGFormat->B_mask = pSDLFormat->Bmask;
	pWGFormat->A_mask = pSDLFormat->Amask;

	pWGFormat->R_shift = pSDLFormat->Rshift;
	pWGFormat->G_shift = pSDLFormat->Gshift;
	pWGFormat->B_shift = pSDLFormat->Bshift;
	pWGFormat->A_shift = pSDLFormat->Ashift;

	pWGFormat->R_loss = pSDLFormat->Rloss;
	pWGFormat->G_loss = pSDLFormat->Gloss;
	pWGFormat->B_loss = pSDLFormat->Bloss;
	pWGFormat->A_loss = pSDLFormat->Aloss;

	pWGFormat->R_bits = 8 - pSDLFormat->Rloss;
	pWGFormat->G_bits = 8 - pSDLFormat->Gloss;
	pWGFormat->B_bits = 8 - pSDLFormat->Bloss;
	pWGFormat->A_bits = 8 - pSDLFormat->Aloss;

}

//____ programArguments() _________________________________________________

std::vector<std::string> MyAppVisitor::programArguments() const
{
	std::vector<std::string>	args;

	for (int i = 1; i < g_argc; i++)
		args.push_back(g_argv[i]);

	return args;
}

//____ loadBlob() _________________________________________________________

Blob_p MyAppVisitor::loadBlob(const std::string& path)
{
	FILE* fp;

#ifdef WIN32
	errno_t err = fopen_s(&fp, path.c_str(), "rb");
#else
	fp = fopen(path.c_str(), "rb");
#endif
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	int size = (int)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	Blob_p pBlob = Blob::create(size);

	int nRead = (int)fread(pBlob->data(), 1, size, fp);
	fclose(fp);

	if (nRead < size)
		return 0;

	return pBlob;
}

//____ loadSurface() ______________________________________________________

Surface_p MyAppVisitor::loadSurface(const std::string& path, SurfaceFactory* pFactory, const Surface::Blueprint& _bp)
{
	if (path.rfind(".surf") == path.size() - 5 || path.rfind(".srf") == path.size() - 4)
	{
		std::ifstream input(path, std::ios::binary);
		if (!input.good())
			return nullptr;

		auto pReader = SurfaceReader::create({ .factory = Base::activeContext()->surfaceFactory() });
		Surface_p pSurface = pReader->readSurfaceFromStream(input);
		input.close();
		return pSurface;
	}
	else
	{
		Surface::Blueprint bp = _bp;

		PixelDescription format;

		auto pSDLSurf = IMG_Load(path.c_str());
		if (pSDLSurf == NULL)
			return nullptr;
		convertSDLFormat(&format, pSDLSurf->format);

		PixelFormat px;
		Color8* pClut = nullptr;

		Color8 clut[256];

		if (format.bIndexed)
		{
			px = PixelFormat::CLUT_8;

			for (int i = 0; i < 256; i++)
			{
				clut[i].r = pSDLSurf->format->palette->colors[i].r;
				clut[i].g = pSDLSurf->format->palette->colors[i].g;
				clut[i].b = pSDLSurf->format->palette->colors[i].b;
				clut[i].a = pSDLSurf->format->palette->colors[i].a;
			}
			pClut = clut;
		}

		else if (format.A_bits > 0)
			px = PixelFormat::BGRA_8;
		else
			px = PixelFormat::BGR_8;

		if (!pFactory)
			pFactory = Base::activeContext()->surfaceFactory();

		bp.format = px;
		bp.clut = pClut;
		bp.size = { pSDLSurf->w, pSDLSurf->h };

		auto pSurface = pFactory->createSurface(bp, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &format);
		SDL_FreeSurface(pSDLSurf);
		return pSurface;
	}
	
	
}

//____ notifyPopup() __________________________________________________________

bool MyAppVisitor::notifyPopup(const std::string& title, const std::string& message, WonderApp::IconType iconType)
{
	if( iconType == WonderApp::IconType::Question )
		return false;
	
	tinyfd_notifyPopup( title.c_str(), message.c_str(), iconNames[int(iconType)]);
	return true;
}

//____ messageBox() ___________________________________________________________

WonderApp::DialogButton	MyAppVisitor::messageBox(	const std::string& title, const std::string& message, 
													WonderApp::DialogType dialogType, WonderApp::IconType iconType, 
													WonderApp::DialogButton defaultButton)
{
	int defaultButtonInt = 0;
	
	switch( dialogType )
	{
		case::WonderApp::DialogType::Ok:
		{
			if( defaultButton != WonderApp::DialogButton::Ok && defaultButton != WonderApp::DialogButton::Undefined )
				goto wrongDefaultButton;
			
			defaultButtonInt = 1;
			break;
		}

		case::WonderApp::DialogType::OkCancel:
		{
			if (defaultButton == WonderApp::DialogButton::Ok)
				defaultButtonInt = 1;
			else if (defaultButton != WonderApp::DialogButton::Cancel && defaultButton != WonderApp::DialogButton::Undefined)
				goto wrongDefaultButton;
			break;
		}

		case::WonderApp::DialogType::YesNo:
		{
			if (defaultButton == WonderApp::DialogButton::Yes)
				defaultButtonInt = 1;
			else if (defaultButton != WonderApp::DialogButton::No && defaultButton != WonderApp::DialogButton::Undefined)
				goto wrongDefaultButton;
			break;
		}

		case::WonderApp::DialogType::YesNoCancel:
		{
			if (defaultButton == WonderApp::DialogButton::Yes)
				defaultButtonInt = 1;
			else if (defaultButton == WonderApp::DialogButton::No)
				defaultButtonInt = 2;
			else if( defaultButton != WonderApp::DialogButton::Cancel && defaultButton != WonderApp::DialogButton::Undefined )
				goto wrongDefaultButton;
			break;
		}
	}
	
	{
		int retval = tinyfd_messageBox( title.c_str(), message.c_str(), dialogNames[int(dialogType)], iconNames[int(iconType)], defaultButtonInt);
		WonderApp::DialogButton selectedButton;
		
		switch( dialogType )
		{
			case::WonderApp::DialogType::Ok:
			{
				selectedButton = WonderApp::DialogButton::Ok;
				break;
			}

			case::WonderApp::DialogType::OkCancel:
			{
				selectedButton = retval == 1 ? WonderApp::DialogButton::Ok : WonderApp::DialogButton::Cancel;
				break;
			}

			case::WonderApp::DialogType::YesNo:
			{
				selectedButton = retval == 1 ? WonderApp::DialogButton::Yes : WonderApp::DialogButton::No;
				break;
			}

			default:
			case::WonderApp::DialogType::YesNoCancel:
			{
				if( retval == 0 )
					selectedButton = WonderApp::DialogButton::No;
				else if( retval == 1 )
					selectedButton = WonderApp::DialogButton::Yes;
				else
					selectedButton = WonderApp::DialogButton::Cancel;
				break;
			}
		}
		
		return selectedButton;
	}
	

wrongDefaultButton:
	
	Base::handleError(ErrorSeverity::Serious, ErrorCode::InvalidParam, "Default button isn't a button of specified dialog type", nullptr, nullptr, __func__, __FILE__, __LINE__);
	
	return WonderApp::DialogButton::Undefined;

}

//____ inputBox() _____________________________________________________________

std::string MyAppVisitor::inputBox(const std::string& title, const std::string& message, const std::string& defaultInput)
{
	return tinyfd_inputBox( title.c_str(), message.c_str(), defaultInput.c_str() );
}

//____ saveFileDialog() _______________________________________________________

std::string MyAppVisitor::saveFileDialog(	const std::string& title, const std::string& defaultPathAndFile,
											const std::vector<std::string>& filterPatterns,
											const std::string& singleFilterDescription)
{
	const char* pPatternPointers[64];
	const char** pPatterns = filterPatterns.empty() ? NULL : pPatternPointers;

	for (int i = 0; i < filterPatterns.size(); i++)
		pPatternPointers[i] = filterPatterns[i].c_str();

	auto pResult = tinyfd_saveFileDialog( title.c_str(), defaultPathAndFile.c_str(), filterPatterns.size(), pPatterns, singleFilterDescription.c_str() );
	
	if(pResult == NULL)
		return std::string();
	else
		return std::string(pResult);
}

//____ openFileDialog() _______________________________________________________

std::string MyAppVisitor::openFileDialog(	const std::string& title, const std::string& defaultPathAndFile,
											const std::vector<std::string>& filterPatterns,
											const std::string& singleFilterDescription)
{
	const char* pPatternPointers[64];
	const char** pPatterns = filterPatterns.empty() ? NULL : pPatternPointers;

	for (int i = 0; i < filterPatterns.size(); i++)
		pPatternPointers[i] = filterPatterns[i].c_str();

	auto pResult = tinyfd_openFileDialog( title.c_str(), defaultPathAndFile.c_str(), filterPatterns.size(), pPatterns, singleFilterDescription.c_str(), false);

	if (pResult == NULL)
		return std::string();
	else
		return std::string(pResult);
}

//____ openMultiFileDialog() _______________________________________________________

std::vector<std::string> MyAppVisitor::openMultiFileDialog(	const std::string& title, 
															const std::string& defaultPathAndFile,
															const std::vector<std::string>& filterPatterns,
															const std::string& singleFilterDescription)
{
	const char* pPatternPointers[64];
	const char** pPatterns = filterPatterns.empty() ? NULL : pPatternPointers;

	for (int i = 0; i < filterPatterns.size(); i++)
		pPatternPointers[i] = filterPatterns[i].c_str();

	auto pResult = tinyfd_openFileDialog(title.c_str(), defaultPathAndFile.c_str(), filterPatterns.size(), pPatterns, singleFilterDescription.c_str(), true);

	if (pResult == NULL)
		return std::vector<std::string>();
	else
	{
		std::vector<std::string> paths;

		char* pBeg = pResult;
		while (*pBeg != 0)
		{
			char* pEnd = pBeg;
			while (*pEnd != 0 && *pEnd != '|')
				pEnd++;

			paths.push_back(std::string(pBeg, pEnd - pBeg));

			pBeg = pEnd;
			if (*pBeg == '|')
				pBeg++;
		}

		return paths;
	}
}

//____ selectFolderDialog() ___________________________________________________

std::string MyAppVisitor::selectFolderDialog(const std::string& title, const std::string& defaultPath)
{
	return tinyfd_selectFolderDialog( title.c_str(), defaultPath.c_str());

}

//____ createWindow() _________________________________________________________

WonderApp::Window_p MyAppVisitor::createWindow(const WonderApp::Window::Blueprint& blueprint)
{
	auto pWindow =  MyWindow::create(blueprint);

	g_windows.push_back(pWindow.rawPtr());

	return pWindow;
}

//____ openLibrary() __________________________________________________________

WonderApp::LibId MyAppVisitor::openLibrary(const std::string& path)
{
#ifdef WIN32
	return (void*)LoadLibraryA(path.c_str());
#else														// Apple and Linux
	std::string fullPath = "lib" + path + ".dylib";
	return dlopen(fullPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif
}

//____ loadSymbol() __________________________________________________________

void* MyAppVisitor::loadSymbol(WonderApp::LibId lib, const std::string& symbol)
{
#ifdef WIN32
	return GetProcAddress((HMODULE)lib, symbol.c_str());
#else														// Apple and Linux
	return dlsym(lib, symbol.c_str());
#endif
}

//____ closeLibrary() _________________________________________________________

bool MyAppVisitor::closeLibrary(WonderApp::LibId lib)
{
#ifdef WIN32
	return FreeLibrary((HMODULE)lib);
#else														// Apple and Linux
	return (dlclose(lib) == 0);
#endif
}

//____ hidePointer() __________________________________________________________

bool MyHostBridge::hidePointer()
{
	if( SDL_ShowCursor(SDL_DISABLE) == SDL_DISABLE )
		return true;
	
	return false;
}

//____ showPointer() __________________________________________________________

bool MyHostBridge::showPointer()
{
	if( SDL_ShowCursor(SDL_ENABLE) == SDL_ENABLE )
		return true;
	
	return false;
}

//____ getClipboardText() _____________________________________________________

std::string MyHostBridge::getClipboardText()
{
	char * pText = SDL_GetClipboardText();
	std::string text = pText;
	SDL_free(pText);
	return text;
}

//____ setClipboardText() _____________________________________________________

bool MyHostBridge::setClipboardText(const std::string& text )
{
	int retval = SDL_SetClipboardText( text.c_str() );
	if( retval != 0 )
	{
		//TODO: Error handling!

		return false;
	}
	
	return true;
}

//____ create() _______________________________________________________________

MyWindow_p MyWindow::create(const Blueprint& blueprint)
{
	Rect geo;

	geo = { blueprint.pos + Coord(4,20), blueprint.size};

	uint32_t flags = 0;

	if (blueprint.resizable)
		flags |= SDL_WINDOW_RESIZABLE;


	SDL_Window* pSDLWindow = SDL_CreateWindow(blueprint.title.c_str(), geo.x, geo.y, geo.w, geo.h, flags);
	if (pSDLWindow == NULL)
		return nullptr;

	if( !blueprint.minSize.isEmpty() )
		SDL_SetWindowMinimumSize(pSDLWindow,blueprint.minSize.w, blueprint.minSize.h);

	if( !blueprint.maxSize.isEmpty() )
		SDL_SetWindowMaximumSize(pSDLWindow,blueprint.maxSize.w, blueprint.maxSize.h);

	auto pWindowSurface = generateWindowSurface(pSDLWindow, geo.w, geo.h);
	if (pWindowSurface == nullptr)
		return nullptr;

	auto pRootPanel = RootPanel::create(pWindowSurface, g_pGfxDevice);

	MyWindow_p pWindow = new MyWindow(blueprint.title, pRootPanel, geo, pSDLWindow);

	//TODO: This is ugly. It should be handled when windows gets focused.

	Base::inputHandler()->setFocusedWindow(pRootPanel);

	return pWindow;
}


//____ MyWindow constructor ___________________________________________________

MyWindow::MyWindow(const std::string& title, wg::RootPanel* pRootPanel, const wg::Rect& geo, SDL_Window* pSDLWindow)
	: Window(pRootPanel, geo)
{
	m_pSDLWindow = pSDLWindow;
}


//___ setTitle() ______________________________________________________________

bool MyWindow::setTitle(std::string& title)
{
	SDL_SetWindowTitle(m_pSDLWindow, title.c_str());
	return true;
}

//____ setIcon() ______________________________________________________________

bool MyWindow::setIcon(Surface* pIcon)
{
	//TODO: IMPLEMENT!

	return false;
}


//____ title() ________________________________________________________________

std::string MyWindow::title() const
{
	return std::string(SDL_GetWindowTitle(m_pSDLWindow));
}


//____ render() _______________________________________________________________

void MyWindow::render()
{
	m_pRootPanel->render();

	//TODO: Just update the dirty rectangles!

	SDL_UpdateWindowSurface(m_pSDLWindow);
}



//____ _updateWindowGeo() _____________________________________________________

Rect MyWindow::_updateWindowGeo(const Rect& geo)
{
	//TODO: Calculate and update position as well.

	SDL_SetWindowSize(m_pSDLWindow, geo.w, geo.h);
	return m_geo;
}

