

#include <cstdlib>

#ifdef WIN32
#	include <SDL.h>
#else
#	include <SDL2/SDL.h>
#endif

#include <wg_base.h>
#include <wg_inputhandler.h>
#include <wg_msgrouter.h>

#include <wg_softsurface.h>
#include <wg_softgfxdevice.h>

#include <wg_rootpanel.h>
#include <wg_lambdapanel.h>
#include <wg_filler.h>
#include <wg_staticcolorskin.h>

#include <wg_boxskin.h>
#include <wg_blockskin.h>


using namespace wg;

void 			translateEvents(RootPanel * pRoot);
MouseButton 	translateMouseButton( uint8_t button );

bool			bQuit = false;	// Set to false by myButtonClickCallback() or translateEvents().

int				rootScale = 64;

//____ drawFills() _____________________________________________________________

void drawFills(GfxDevice_p pGfxDevice, Surface_p pCanvas)
{
	pGfxDevice->beginRender();
	pGfxDevice->beginCanvasUpdate(pCanvas);

	pGfxDevice->fill(HiColor::Black);

	for (int y = 0; y < 17; y++)
	{
		for (int x = 0; x < 17; x++)
		{
			pGfxDevice->fill(RectI((4 + x * 16) * 64 + y * 4, (4 + y * 16) * 64 + x * 4, 10 * 64, 10 * 64), HiColor(4096, 0, 0, 4096));
		}
	}

	pGfxDevice->fill(RectI((18 * 16) * 64, 0, 64 * 10, 64 * 300), HiColor(4096 / 16, 0, 0, 4096));

	pGfxDevice->endCanvasUpdate();
	pGfxDevice->endRender();
}


//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 640, height = 480;
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, 0);

	SDL_Surface * pWinSurf = SDL_GetWindowSurface( pWin );

	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------


	Base::init(nullptr);

	PixelFormat format = PixelFormat::Unknown;

	if( pWinSurf->format->BitsPerPixel == 32 )
		format = PixelFormat::BGRA_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		format = PixelFormat::BGR_8;

	Blob_p pCanvasBlob = Blob::create( pWinSurf->pixels, 0);
	SoftSurface_p pCanvas = SoftSurface::create( SizeI(pWinSurf->w,pWinSurf->h), format, pCanvasBlob, pWinSurf->pitch );


	SoftGfxDevice_p pGfxDevice = SoftGfxDevice::create();

	// First we load the 24-bit bmp containing the button graphics.
	// No error handling or such to keep this example short and simple.

	SDL_Surface * pSDLSurf = SDL_LoadBMP( "resources/simple_button.bmp" );
	SoftSurface_p pButtonSurface = SoftSurface::create( SizeI( pSDLSurf->w, pSDLSurf->h ), PixelFormat::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	SDL_FreeSurface(pSDLSurf);

	// 

	RootPanel_p pRoot = RootPanel::create(pCanvas, pGfxDevice);

	pRoot->setScale(rootScale);


	//

	auto pBaseLambda = LambdaPanel::create();
	pBaseLambda->skin = StaticColorSkin::create(Color8::Beige);



	auto pFiller = Filler::create();
	auto pSkin = BoxSkin::create(3, {	{StateEnum::Normal, Color8::Blue, Color8::Salmon}, 
										{StateEnum::Hovered, Color8::LightBlue, Color8::LightSalmon},
										{StateEnum::Pressed, Color8::DarkBlue, Color8::DarkSalmon} });

	pFiller->skin = pSkin;

	pBaseLambda->slots.pushBack(pFiller, [](Widget* pWidget, Size parentSize) { return Rect(10,10,100,50); });

	auto pFiller2 = Filler::create();
	auto pSkin2 = BlockSkin::create(pButtonSurface, { StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Disabled }, 0, Axis::X );

	pFiller2->skin = pSkin2;

	pBaseLambda->slots.pushBack(pFiller2, [](Widget* pWidget, Size parentSize) { return Rect(10, 100, 100, 50); });

	pRoot->slot = pBaseLambda;

	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	while( !bQuit )
	{
		// Loop through SDL events, translate them to WonderGUI events
		// and process them.

		translateEvents(pRoot);

		// Let WonderGUI render any updated/dirty regions of the screen.

		SDL_LockSurface(pWinSurf);

		pRoot->render();


//		drawFills(pGfxDevice, pCanvas);

		SDL_UnlockSurface(pWinSurf);
		SDL_UpdateWindowSurface( pWin );
		SDL_Delay(20);
	}

	// Cleanup. We should null all our smartpointers so that all objects are
	// deleted before calling Base::exit().

	pCanvas = nullptr;
	pCanvasBlob = nullptr;

	// Exit WonderGUI

	Base::exit();

	// Exit SDL

	SDL_Quit();

	return 0;
}

//____ translateEvents() ___________________________________________________________

void translateEvents(RootPanel * pRoot)
{
	// Process all the SDL events in a loop.
	// In this example we only use mouse input, but typically you
	// would also need to translate keyboard events.

	SDL_Event e;
	while(SDL_PollEvent(&e)) {

		switch( e.type )
		{
			case SDL_QUIT:
				bQuit = true;
				break;

			case SDL_MOUSEMOTION:
				Base::inputHandler()->setPointer( pRoot, Coord(e.motion.x, e.motion.y)*64/rootScale);
				break;

			case SDL_MOUSEBUTTONDOWN:
				Base::inputHandler()->setButton( translateMouseButton(e.button.button), true );
				break;

			case SDL_MOUSEBUTTONUP:
				Base::inputHandler()->setButton( translateMouseButton(e.button.button), false );
				break;

			default:
				break;
		}
	}

	Base::msgRouter()->dispatch();
}

//____ translateMouseButton() __________________________________________________
//
// Translate SDL mouse button enums to WonderGUI equivalents.
//
MouseButton translateMouseButton( uint8_t button )
{
	switch( button )
	{
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
		default:
			return MouseButton::None;
	}
}
