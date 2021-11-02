

#include <cstdlib>

#ifdef WIN32
#	include <SDL.h>
#else
#	include <SDL2/SDL.h>
#endif
#include <wondergui.h>

#include <wg_softsurface.h>
#include <wg_softgfxdevice.h>

using namespace wg;

void 			translateEvents();
MouseButton 	translateMouseButton( uint8_t button );

bool			bQuit = false;	// Set to false by myButtonClickCallback() or translateEvents().

//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 640*3, height = 480*3;
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, 0);

	SDL_Surface * pWinSurf = SDL_GetWindowSurface( pWin );

	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------


	Base::init(nullptr);

	PixelFormat format = PixelFormat::Undefined;

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



	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	while( !bQuit )
	{
		// Loop through SDL events, translate them to WonderGUI events
		// and process them.

		translateEvents();

		// Let WonderGUI render any updated/dirty regions of the screen.

		SDL_LockSurface(pWinSurf);

		pGfxDevice->beginRender();
		pGfxDevice->beginCanvasUpdate(pCanvas);		

		pGfxDevice->fill(HiColor::Black);

		for( int y = 0 ; y < 17 ; y++ )
		{
			for( int x = 0 ; x < 17 ; x++ )
			{
				pGfxDevice->fill( RectI((4+x*16)*64+y*4,(4+y*16)*64+x*4,10*64,10*64), HiColor(4096,0,0,4096) );
			}
		}

		pGfxDevice->fill( RectI((18*16)*64,0, 64*10, 64*300), HiColor(4096/16,0,0,4096) );

		pGfxDevice->endCanvasUpdate();
		pGfxDevice->endRender();

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

void translateEvents()
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
//				Base::inputHandler()->setPointer( pRoot, Coord(MU::fromPX(e.motion.x), MU::fromPX(e.motion.y)));
				break;

			case SDL_MOUSEBUTTONDOWN:
//				Base::inputHandler()->setButton( translateMouseButton(e.button.button), true );
				break;

			case SDL_MOUSEBUTTONUP:
//				Base::inputHandler()->setButton( translateMouseButton(e.button.button), false );
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
