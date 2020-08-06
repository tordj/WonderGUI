
#ifdef WIN32
#    include <SDL.h>
#    include <SDL_image.h>
#    include <GL/glew.h>
#elif __APPLE__
#    include <SDL2/SDL.h>
#    include <SDL2_image/SDL_image.h>
#else
#    include <SDL2/SDL.h>
#    include <SDL2/SDL_image.h>
#   include <GL/glew.h>
#endif


#include <list>

#include <wondergui.h>
#include <gfxdevices/software/wg_softgfxdevice.h>
#include <gfxdevices/software/wg_softsurfacefactory.h>


#include <gfxdevices/stream/wg_streamgfxdevice.h>
#include <gfxdevices/stream/wg_streamsurfacefactory.h>


#include <gfxdevices/opengl/wg_glgfxdevice.h>
#include <gfxdevices/opengl/wg_glsurface.h>
#include <gfxdevices/opengl/wg_glsurfacefactory.h>

#include "device.h"
#include "gfxdevicetester.h"
#include "app.h"


bool		init_system( Rect windowGeo );
void		exit_system();

bool		process_system_events(const RootPanel_p& pRoot);
void		update_window_rects( const Rect * pRects, int nRects );

bool		init_wondergui();
void		exit_wondergui();


MouseButton translateSDLMouseButton(Uint8 button);


SizeI				g_windowSize;

SDL_Window *		g_pSDLWindow = nullptr;

Surface_p			g_pWindowSurface = nullptr;				// Set by init_system()
RootPanel_p			g_pRoot = nullptr;


//____ MyAppVisitor() ________________________________________________________

class MyAppVisitor : public AppVisitor
{
public:
	wg::RootPanel_p	rootPanel() override
	{
		return g_pRoot;
	}

	int64_t			time() override
	{
		return SDL_GetPerformanceCounter() * 1000 / SDL_GetPerformanceFrequency();
	}

	wg::Blob_p		loadBlob(const char* pPath) override;
	wg::Surface_p	loadSurface(const char* pPath, SurfaceFactory* pFactory, int flags = 0) override;

protected:
	void			convertSDLFormat(PixelDescription* pWGFormat, const SDL_PixelFormat* pSDLFormat);

};


//____ main() _________________________________________________________________

int main(int argc, char *argv[] )
{
	// Create app and visitor, make any app-specif initialization

	auto pApp = new GfxDeviceTester();
	auto pVisitor = new MyAppVisitor();

	// Get apps window size before we continue

	g_windowSize = pApp->startWindowSize();

	if (!init_system({ 20,20, g_windowSize }))
		return -1;
	
	if (!init_wondergui() )
		return -1;


	pApp->addTestDevice(new SoftwareDevice());
	pApp->addTestDevice(new OpenGLDevice());

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

	// App specific exit calls

	pApp->destroy_testdevices();

	//

	delete pApp;
	delete pVisitor;

	exit_wondergui();
	exit_system();
	return 0;
}


//____ init_wondergui() _______________________________________________________

bool init_wondergui()
{
	Base::init();

	Context_p pContext = Context::create();
    pContext->setScale(1.0);
    pContext->setSurfaceFactory(GlSurfaceFactory::create());
	pContext->setGammaCorrection(false);
	Base::setActiveContext(pContext);

	auto pGfxDevice = GlGfxDevice::create(g_windowSize*pContext->scale(), 0);

	g_pRoot = RootPanel::create(pGfxDevice);

//	g_pRoot->setDebugMode(true);

	return true;
}

//____ exit_wondergui() _______________________________________________________

void exit_wondergui()
{
	g_pRoot = nullptr;

	Base::exit();
}


//____ init_system() _______________________________________________________

bool init_system( Rect windowGeo )
{
	// initialize SDL video
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return false;
	}

	// make sure SDL cleans up before exit
	atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    
	SDL_Window * pWin = SDL_CreateWindow("GfxDevice TestApp", windowGeo.x.px(), windowGeo.y.px(), windowGeo.w.px(), windowGeo.h.px(), SDL_WINDOW_OPENGL /*| SDL_WINDOW_ALLOW_HIGHDPI*/);
	if( pWin == nullptr )
	{
		printf("Unable to create SDL window: %s\n", SDL_GetError());
		return false;
	}
    
	SDL_GL_CreateContext(pWin);

//	SDL_GL_SetSwapInterval(1);

	//	SDL_Renderer * pRenderer = SDL_CreateRenderer(pWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

#ifdef WIN32  
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
#endif

	glDrawBuffer(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();


//    GlGfxDevice_p pGfxDevice = GlGfxDevice::create(windowGeo.size());
//    GlGfxDevice::create( {100,100} );

/*
	SDL_Surface * pWinSurf = SDL_GetWindowSurface(pWin);
	if (pWinSurf == nullptr)
	{
		printf("Unable to get window SDL Surface: %s\n", SDL_GetError());
		return false;
	}

	PixelFormat format = PixelFormat::Unknown;

	switch (pWinSurf->format->BitsPerPixel)
	{
		case 32:
			format = PixelFormat::BGRA_8;
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

    GlGfxDevice_p pGfxDevice = GlGfxDevice::create(windowGeo.size());

    
	g_pSDLWindow = pWin;

	Blob_p pCanvasBlob = Blob::create(pWinSurf->pixels, 0);
	g_pWindowSurface = SoftSurface::create(SizeI(pWinSurf->w, pWinSurf->h), format, pCanvasBlob, pWinSurf->pitch);
*/
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
		SDL_Rect r = { pRects[i].x.px(), pRects[i].y.px(), pRects[i].w.px(), pRects[i].h.px() };
		rects.push_back(r);
	}

	SDL_UpdateWindowSurfaceRects(g_pSDLWindow, &rects.front(), nRects);
}


//____ process_system_events() ________________________________________________

bool process_system_events(const RootPanel_p& pRoot)
{
	// Process all the SDL events in a loop

	InputHandler_p pInput = Base::inputHandler();

	SDL_Event e;
	while (SDL_PollEvent(&e)) {

		switch (e.type)
		{
		case SDL_QUIT:
			return false;

		case SDL_MOUSEMOTION:
			pInput->setPointer(pRoot, Coord(MU::fromPX(e.motion.x), MU::fromPX(e.motion.y)));
			break;

		case SDL_MOUSEBUTTONDOWN:
			pInput->setButton(translateSDLMouseButton(e.button.button), true);
			break;

		case SDL_MOUSEBUTTONUP:
			pInput->setButton(translateSDLMouseButton(e.button.button), false);
			break;

		case SDL_MOUSEWHEEL:
		{
			Coord distance(e.wheel.x, e.wheel.y);
			if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
				distance *= -1;

			pInput->setWheelRoll(1, distance);
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

//____ convertSDLFormat() ______________________________________________________

void MyAppVisitor::convertSDLFormat(PixelDescription* pWGFormat, const SDL_PixelFormat* pSDLFormat)
{
	pWGFormat->format = PixelFormat::Custom;
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

Blob_p MyAppVisitor::loadBlob(const char* pPath)
{
	FILE* fp;

#ifdef WIN32
	errno_t err = fopen_s(&fp, pPath, "rb");
#else
	fp = fopen(pPath, "rb");
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

Surface_p MyAppVisitor::loadSurface(const char* pPath, SurfaceFactory* pFactory, int flags)
{
	PixelDescription format;

	auto pSDLSurf = IMG_Load(pPath);
	convertSDLFormat(&format, pSDLSurf->format);

	PixelFormat px;
	Color* pClut = nullptr;

	Color clut[256];

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

	auto pSurface = pFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), px, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &format, flags, pClut);
	SDL_FreeSurface(pSDLSurf);
	return pSurface;
}


