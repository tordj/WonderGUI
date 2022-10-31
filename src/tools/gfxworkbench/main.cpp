
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

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

#include <wg_softsurface.h>
#include <wg_softsurfacefactory.h>
#include <wg_softgfxdevice.h>

using namespace wg;

void 			translateEvents();
MouseButton 	translateMouseButton( uint8_t button );

bool			bQuit = false;	// Set to false by myButtonClickCallback() or translateEvents().

extern void addGeneratedKernels( SoftGfxDevice * pDevice );



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
	addGeneratedKernels(pGfxDevice);

	
	// First we load the 24-bit bmp containing the button graphics.
	// No error handling or such to keep this example short and simple.

	SDL_Surface * pSDLSurf = SDL_LoadBMP( "resources/simple_button.bmp" );
	SoftSurface_p pButtonSurface = SoftSurface::create( SizeI( pSDLSurf->w, pSDLSurf->h ), PixelFormat::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	SDL_FreeSurface(pSDLSurf);


	// Setup a bitmap font
/*
	SDL_Surface * pSDLFontSurf = IMG_Load( "resources/droid-20-ascii.png" );
	SoftSurface_p pFontSurface = SoftSurface::create( SizeI( pSDLFontSurf->w, pSDLFontSurf->h ), PixelFormat::BGRA_8, (unsigned char*) pSDLFontSurf->pixels, pSDLFontSurf->pitch, 0 );
	SDL_FreeSurface(pSDLFontSurf);
*/

	std::ifstream input("resources/droid_16_ascii_indexed.surf", std::ios::binary );
	auto pReader = SurfaceReader::create( WGBP(SurfaceReader, _.factory = SoftSurfaceFactory::create() ));
	Surface_p pFontSurface = pReader->readSurfaceFromStream(input);
	input.close();


	std::ifstream file("resources/droid_16_ascii_indexed.fnt");
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string fontSpec = buffer.str();


/*
	SDL_Surface * pSDLFontSurf = IMG_Load( "resources/test.png" );
	SoftSurface_p pFontSurface = SoftSurface::create( SizeI( pSDLFontSurf->w, pSDLFontSurf->h ), PixelFormat::BGRA_8, (unsigned char*) pSDLFontSurf->pixels, pSDLFontSurf->pitch, 0 );
	SDL_FreeSurface(pSDLFontSurf);

	std::ifstream file( "resources/test.fnt" );
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string fontSpec = buffer.str();
*/

	
	auto pFont = BitmapFont::create(pFontSurface, fontSpec.c_str() );
	
	//
	
	auto pPrinter = Printer::create();
	pPrinter->setFont(pFont);
	pPrinter->setGfxDevice(pGfxDevice);
	pPrinter->setCursorOrigo({0,20*64});
	
	
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

		pGfxDevice->fill(Color8::Green);

		
		pFont->setSize(10*64);
		
		pPrinter->setLineWidth(640*3*64);
		pPrinter->resetCursor();
		pPrinter->print("Hello from Printer!\n");
		pPrinter->printAligned(Placement::Center, "This text should be centered, one line down!");

		pPrinter->crlf();
		pPrinter->print("First part of text,");
		pPrinter->print(" second part of text on same line.");
		
		RectSPX box = { 20*64, 300*64, 640*64, 60*64 };

		pGfxDevice->fill(box, Color8::Black);
		
		pPrinter->printInBox(box, Placement::Center, "Multiline text that is\naligned inside a box.");
		
		pPrinter->crlf();
		pPrinter->print("Yet another line.\n");
		
		pPrinter->printAt(box.pos(), "A little text");
		
		pPrinter->print("And another (one).");

		pPrinter->setLineWidth(100*64);
		pPrinter->printWrapping("This not so very long texts should be wrapping automatically every 100 pixels.\nThis is line two.", 20*64);
		
		
/*
		for( int y = 0 ; y < 17 ; y++ )
		{
			for( int x = 0 ; x < 17 ; x++ )
			{
				pGfxDevice->fill( RectI((4+x*16)*64+y*4,(4+y*16)*64+x*4,10*64,10*64), HiColor(4096,0,0,4096) );
			}
		}

		pGfxDevice->fill( RectI((18*16)*64,0, 64*10, 64*300), HiColor(4096/16,0,0,4096) );
*/
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
