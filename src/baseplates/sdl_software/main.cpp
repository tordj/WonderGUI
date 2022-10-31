
#ifdef WIN32
#    include <SDL.h>
#    include <SDL_image.h>
#elif __APPLE__
#    include <SDL2/SDL.h>
#    include <SDL2_image/SDL_image.h>
#else
#    include <SDL2/SDL.h>
#    include <SDL2/SDL_image.h>
#endif


#include <wondergui.h>

#include <wg_softgfxdevice.h>
#include <wg_softsurfacefactory.h>

#include "baseplate.h"

#include "tinyfiledialogs.h"

using namespace wg;

extern void addGeneratedKernels( SoftGfxDevice * pDevice );


//____ MyAppVisitor ________________________________________________________

class MyAppVisitor : public WonderApp::Visitor
{
public:
	wg::RootPanel_p	rootPanel() override;
	
	int64_t			time() override
	{
		return SDL_GetPerformanceCounter() * 1000000 / SDL_GetPerformanceFrequency();
	}

	wg::Blob_p 		loadBlob(const std::string& path) override;
	wg::Surface_p 	loadSurface(const std::string& path, SurfaceFactory* pFactory, const Surface::Blueprint& bp = Surface::Blueprint() ) override;

	bool 			notifyPopup(char const * title, char const * message, WonderApp::IconType iconType) override;

	WonderApp::DialogButton	messageBox(char const * title, char const * message, WonderApp::DialogType dialogType,
									   WonderApp::IconType iconType, WonderApp::DialogButton defaultButton ) override;

	char *			inputBox(char const * title, char const * message, char const * defaultInput ) override;

	char *			saveFileDialog( char const * title, char const * defaultPathAndFile,
									int nbFilterPatterns,	char const * const * filterPatterns,
									char const * singleFilterDescription ) override;
	
	char * 			openFileDialog(	char const * title, char const * defaultPathAndFile,
									int nbFilterPatterns, char const * const * filterPatterns,
									char const * singleFilterDescription, bool bMultiSelect ) override;

	char *			selectFolderDialog( char const * title, char const * defaultPath) override;
	
	
	
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

bool		init_system( Rect windowGeo, float scale );
void		exit_system();

bool		process_system_events(const RootPanel_p& pRoot);
void		update_window_rects( const Rect * pRects, int nRects );

bool		init_wondergui();
void		exit_wondergui();


MouseButton translateSDLMouseButton(Uint8 button);


Size				g_windowPointSize;
SizeI				g_windowPixelSize;
float				g_scale;

SDL_Window *		g_pSDLWindow = nullptr;
PixelFormat			g_pixelFormat = PixelFormat::Undefined;

Surface_p			g_pWindowSurface = nullptr;				// Set by init_system()
RootPanel_p			g_pRoot = nullptr;

MyHostBridge *		g_pHostBridge = nullptr;

static const char * iconNames[4] = { "info", "warning", "error", "question" };
static const char * dialogNames[4] = { "ok", "okcancel", "yesno", "yesnocancel" };




//____ main() _________________________________________________________________

int main(int argc, char *argv[] )
{
	// Create app and visitor, make any app-specif initialization

	auto pApp = WonderApp::create();
	auto pVisitor = new MyAppVisitor();

	// Get apps window size before we continue

	g_windowPointSize = pApp->startWindowSize();
	g_scale = 1.f;
	g_windowPixelSize = SizeI(g_windowPointSize * g_scale);


	if (!init_system({ 20,20, g_windowPointSize }, g_scale ))
		return -1;
	
	if (!init_wondergui() )
		return -1;

	// Initialize the app

	bool bContinue = pApp->init( pVisitor );

	// Main loop

	while (bContinue)
	{
		// Handle system events

		bContinue = process_system_events(g_pRoot);

		if (bContinue)
		{
			// Uppdate the app

			bContinue = pApp->update();

			// Dispatch messages

			Base::msgRouter()->dispatch();

			// Periodic update

			Base::update(SDL_GetPerformanceCounter() * 1000000 / SDL_GetPerformanceFrequency());

			// Render. We do this outside the app since we might want to 
			// handle updated rectangles in a system specific way.

			g_pRoot->render();
			SDL_UpdateWindowSurface(g_pSDLWindow);

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
	addGeneratedKernels( pGfxDevice );

	auto pSurfaceFactory = SoftSurfaceFactory::create();
	
	Context_p pContext = Context::create();
    pContext->setSurfaceFactory(pSurfaceFactory);
	pContext->setGfxDevice(pGfxDevice);
	pContext->setGammaCorrection(true);
	Base::setActiveContext(pContext);


	SDL_Surface* pWinSurf = SDL_GetWindowSurface(g_pSDLWindow);

	Blob_p pCanvasBlob = Blob::create(pWinSurf->pixels, 0);
	g_pWindowSurface = SoftSurface::create(SizeI(pWinSurf->w, pWinSurf->h), g_pixelFormat, pCanvasBlob, pWinSurf->pitch);


	pGfxDevice->defineCanvas( CanvasRef::Default, static_cast<SoftSurface*>(g_pWindowSurface.rawPtr()));

	g_pRoot = RootPanel::create( CanvasRef::Default, pGfxDevice);

	InputHandler_p pInput = Base::inputHandler();
	
	pInput->setFocusedWindow(g_pRoot);

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
	g_pRoot = nullptr;

	Base::exit();
	delete g_pHostBridge;
	g_pHostBridge = nullptr;
}


//____ init_system() _______________________________________________________

bool init_system( Rect windowGeo, float scale )
{
	// initialize SDL video
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return false;
	}

	// make sure SDL cleans up before exit
	atexit(SDL_Quit);
 
	SDL_Window * pWin = SDL_CreateWindow("GfxDevice TestApp", windowGeo.x, windowGeo.y, windowGeo.w, windowGeo.h , SDL_WINDOW_SHOWN /* | SDL_WINDOW_ALLOW_HIGHDPI */ );
	if( pWin == nullptr )
	{
		printf("Unable to create SDL window: %s\n", SDL_GetError());
		return false;
	}
    
	SDL_Surface * pWinSurf = SDL_GetWindowSurface(pWin);
	if (pWinSurf == nullptr)
	{
		printf("Unable to get window SDL Surface: %s\n", SDL_GetError());
		return false;
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
			return false;
		}
	}
    
	g_pSDLWindow = pWin;
	g_pixelFormat = format;

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	return true;
}

//____ exit_system() __________________________________________________________

void exit_system()
{
	IMG_Quit();
}

//____ update_window_rects() __________________________________________________

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


//____ process_system_events() ________________________________________________

bool process_system_events(const RootPanel_p& pRoot)
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
			pInput->setPointer(pRoot, Coord(e.motion.x, e.motion.y));
			break;

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

//____ rootPanel() _____________________________________________________________

wg::RootPanel_p	MyAppVisitor::rootPanel()
{
	return g_pRoot;
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
	Surface::Blueprint bp = _bp;


	PixelDescription format;

	auto pSDLSurf = IMG_Load(path.c_str());
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

//____ notifyPopup() __________________________________________________________

bool MyAppVisitor::notifyPopup(char const * title, char const * message, WonderApp::IconType iconType)
{
	if( iconType == WonderApp::IconType::Question )
		return false;
	
	tinyfd_notifyPopup( title, message, iconNames[int(iconType)]);
	return true;
}

//____ messageBox() ___________________________________________________________

WonderApp::DialogButton	MyAppVisitor::messageBox(char const * title, char const * message, WonderApp::DialogType dialogType,
							 WonderApp::IconType iconType, WonderApp::DialogButton defaultButton )
{
	int defaultButtonInt;
	
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
			if( defaultButton == WonderApp::DialogButton::Undefined)
				defaultButton = WonderApp::DialogButton::Cancel;
			
			if( defaultButton != WonderApp::DialogButton::Ok && defaultButton != WonderApp::DialogButton::Cancel )
				goto wrongDefaultButton;
			break;
		}

		case::WonderApp::DialogType::YesNo:
		{
			if( defaultButton == WonderApp::DialogButton::Undefined)
				defaultButton = WonderApp::DialogButton::No;
			
			if( defaultButton != WonderApp::DialogButton::Yes && defaultButton != WonderApp::DialogButton::No )
				goto wrongDefaultButton;
			break;
		}

		case::WonderApp::DialogType::YesNoCancel:
		{
			if( defaultButton == WonderApp::DialogButton::Undefined)
				defaultButton = WonderApp::DialogButton::Cancel;
			
			if( defaultButton != WonderApp::DialogButton::Yes && defaultButton != WonderApp::DialogButton::No &&
				defaultButton != WonderApp::DialogButton::Cancel )
				goto wrongDefaultButton;
			break;
		}
	}
	
	{
		int retval = tinyfd_messageBox( title, message, dialogNames[int(dialogType)], iconNames[int(iconType)], defaultButtonInt );
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

char * MyAppVisitor::inputBox(char const * title, char const * message, char const * defaultInput )
{
	return tinyfd_inputBox( title, message, defaultInput );
}

//____ saveFileDialog() _______________________________________________________

char * MyAppVisitor::saveFileDialog( char const * title, char const * defaultPathAndFile,
								int nbFilterPatterns,	char const * const * filterPatterns,
								char const * singleFilterDescription )
{
	return tinyfd_saveFileDialog( title, defaultPathAndFile, nbFilterPatterns, filterPatterns, singleFilterDescription );
}

//____ openFileDialog() _______________________________________________________

char * MyAppVisitor::openFileDialog( char const * title, char const * defaultPathAndFile,
								int nbFilterPatterns, char const * const * filterPatterns,
								char const * singleFilterDescription, bool bMultiSelection )
{
	return tinyfd_openFileDialog( title, defaultPathAndFile, nbFilterPatterns, filterPatterns, singleFilterDescription, bMultiSelection );
}

//____ selectFolderDialog() ___________________________________________________

char * MyAppVisitor::selectFolderDialog( char const * title, char const * defaultPath)
{
	return tinyfd_selectFolderDialog( title, defaultPath );

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
