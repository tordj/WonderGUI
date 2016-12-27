/*
 * This example uses SDL2 to setup a minimal framework for WonderGUI and
 * create a clickable button that closes the window. It has no
 * dependencies except SDL2 and WonderGUI.
 * 
 * The intention is to show how WonderGUI easily can be integrated  with
 * any development framework with some simple bindings and pure software
 * rendering. Although SDL2 is used, no WonderGUI code outside this file
 * is dependent on SDL in anyway.
 * 
 * In order to keep the example small, we are making some assumptions,
 * for example that the screen surface is either 24 or 32 bits graphics
 * with the RGB elements in the same order as SoftSurface keeps them,
 * which is usually the case on normal PC's. In environments where this
 * is not the case, either specific Surface and GfxDevice classes
 * will need to be provided or graphics needs to be rendered to an interim
 * surface from which blocks can be copied and translated to the output
 * format.
 * 
 * If you want to use WonderGUI with SDL2, there are better examples to use
 * as a template. Use the SDL2 specific Surface/GfxDevice classes that
 * better integrate with SDL2's video API, giving you better performance
 * and transparent support for images and windows of other pixel formats.
 * 
 * Also note that in order to keep this example short, it doesn't provide a full
 * initialization of WonderGUI. No font is initialized and no keyboard events
 * are translated. See other examples for a more full initialization of WonderGUI.
 */

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
	
void 			translateEvents( RootPanel_p pRoot );
MouseButton 	translateMouseButton( uint8_t button );
void 			updateWindowRects( RootPanel_p pRoot, SDL_Window * pWindow );
void 			myButtonClickCallback( const Msg_p& pMsg );

bool			bQuit = false;	// Set to false by myButtonClickCallback() or translateEvents().

//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 320, height = 240;
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, 0);

	SDL_Surface * pWinSurf = SDL_GetWindowSurface( pWin );
	
	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------

	// First we need to initialize the base system

	Base::init();

	// The software renderer needs a SoftSurface as its canvas,
	// so we wrap the SDL WindowSurface into a SoftSurface.

	PixelType type = PixelType::Unknown;

	if( pWinSurf->format->BitsPerPixel == 32 )
		type = PixelType::BGRA_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		type = PixelType::BGR_8;

	Blob_p pCanvasBlob = Blob::create( pWinSurf->pixels, 0);	
	SoftSurface_p pCanvas = SoftSurface::create( Size(pWinSurf->w,pWinSurf->h), type, pCanvasBlob, pWinSurf->pitch );
		

	// Wg create the GfxDevice that will be used for all rendering, providing
	// it our canvas to draw up.

	SoftGfxDevice_p pGfxDevice = SoftGfxDevice::create( pCanvas );

	// We create a RootPanel. This is responsible for rendering the
	// tree of child widgets connected to it and handle their events.
	// We provide it the GfxDevice to use for rendering.

	RootPanel_p pRoot = RootPanel::create( pGfxDevice );

	//------------------------------------------------------
	// Setup a simple GUI consisting of a filled background and 
	// a button using scaled bitmaps.
	//------------------------------------------------------
	
	// First we load the 24-bit bmp containing the button graphics.
	// No error handling or such to keep this example short and simple.

	SDL_Surface * pSDLSurf = SDL_LoadBMP( "../../../resources/simple_button.bmp" );
	SoftSurface_p pButtonSurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );

	// First we create and add a FlexPanel to the RootPanel.
	// The RootPanel can only take one child, but the FlexPanel
	// provides simple and powerful ways to layout multiple children.

	FlexPanel_p pFlexPanel = FlexPanel::create();
	pRoot->setWidget(pFlexPanel);

	// Now we create the background using the simplest widget
	// type, the Filler and add it to the FlexPanel, making
	// it stretch from the north-west to the south-east corners
	// of the FlexPanel.

	Filler_p pBackground = Filler::create();
	pBackground->setSkin( ColorSkin::create(Color::Aqua) );
	pFlexPanel->addWidget(pBackground, Origo::NorthWest, Origo::SouthEast);

	// Now we create the button, using a clickable skin built from
	// the BMP with the button graphics. createClickableFromSurface()
	// expects the surface to be divided horizontally in four equal
	// sections containing the graphics for the button states NORMAL,
	// HOVERED, PRESSED and DISABLED. We specify that there are no
	// spacing between these four graphics blocks and that they have 
	// three pixel thick borders around that should not stretch.
	// When adding it to the FlexPanel we specify its geometry in
	// pixels and that it should be centered.

	Button_p pButton = Button::create();
	pButton->setSkin( BlockSkin::createClickableFromSurface( pButtonSurface, 0, Border(3) ) );
	pFlexPanel->addWidget( pButton, Rect(0,0,80,33), Origo::Center );

	// Finally we add a callback to the click-event of the button.

	Base::msgRouter()->addRoute( MsgType::Select, MsgFunc::create( myButtonClickCallback ) );
	

	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	while( !bQuit ) 
	{
		// Loop through SDL events, translate them to WonderGUI events
		// and process them.
		
		translateEvents( pRoot );

		// Let WonderGUI render any updated/dirty regions of the screen.

		SDL_LockSurface(pWinSurf);
		pRoot->render();
		SDL_UnlockSurface(pWinSurf);

		// Make SDL update any screen regions redrawn by WonderGUI.

		updateWindowRects( pRoot, pWin );

		SDL_Delay(20);
	}

	// Exit WonderGUI

	Base::exit();

	// Exit SDL

	SDL_Quit();

	return 0;
}

//____ translateEvents() ___________________________________________________________

void translateEvents( RootPanel_p pRoot )
{
	// WonderGUI needs Tick-messages to keep track of time passed for things such
	// key-repeat, double-click detection, animations etc.  So we create one
	// and post it.
	
	static unsigned int oldTicks = 0;
	
	unsigned int ticks = SDL_GetTicks();
	int tickDiff;

	if( oldTicks == 0 )
		tickDiff = 0;
	else
		tickDiff = (int) (ticks - oldTicks);		
	oldTicks = ticks;


	Base::msgRouter()->post( TickMsg::create(ticks, tickDiff) );

	// Process all the SDL events in a loop

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		
		switch( e.type )
		{
			case SDL_QUIT:
				bQuit = true;
				break;
				
			case SDL_MOUSEMOTION:
				Base::inputHandler()->setPointer( pRoot, Coord(e.motion.x,e.motion.y));
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

//____ updateWindowRects() _____________________________________________________
//
// Updates the rectangles of the SDL Window that WonderGUI has modified.
//
void updateWindowRects( RootPanel_p pRoot, SDL_Window * pWindow )
{	
	int nRects = pRoot->nbUpdatedRects();
	if( nRects == 0 )
		return;
	
	const Rect * pUpdatedRects = pRoot->firstUpdatedRect();
	SDL_Rect * pSDLRects = (SDL_Rect*) Base::memStackAlloc( sizeof(SDL_Rect) * nRects );

	for( int i = 0 ; i < nRects ; i++ )
	{
		pSDLRects[i].x = pUpdatedRects[i].x;
		pSDLRects[i].y = pUpdatedRects[i].y;
		pSDLRects[i].w = pUpdatedRects[i].w;
		pSDLRects[i].h = pUpdatedRects[i].h;
	}

	SDL_UpdateWindowSurfaceRects( pWindow, pSDLRects, nRects );

	Base::memStackRelease( sizeof(SDL_Rect) * nRects );
}

//____ myButtonClickCallback() _________________________________________________

void myButtonClickCallback( const Msg_p& pMsg )
{
	bQuit = true;
}

