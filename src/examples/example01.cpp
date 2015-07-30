/*
 * This example uses SDL2 to setup a minimal framework for WonderGUI and
 * create a clickable button that closes the window. It has no
 * dependencies except SDL2, WonderGUI and WgSoft.
 * 
 * The intention is to show how WonderGUI easily can be integrated  with
 * any development framework with some simple bindings and pure software
 * rendering. Although SDL2 is used, no WonderGUI code outside this file
 * is dependent on SDL in anyway.
 * 
 * In order to keep the example small, we are making some assumptions,
 * for example that the screen surface is either 24 or 32 bits graphics
 * with the RGB elements in the same order as WgSoftSurface keeps them,
 * which is usually the case on normal PC's. In environments where this
 * is not the case, either specific WgSurface and WgGfxDevice classes
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

void 			translateEvents( WgMsgRouter_p pMsgRouter );
WgMouseButton 	translateMouseButton( Uint8 button );
void 			updateWindowRects( WgRootPanel_p pRoot, SDL_Window * pWindow );
void 			myButtonClickCallback( const WgMsg_p& pMsg );

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

	WgBase::init();

	// The software renderer needs a WgSoftSurface as its canvas,
	// so we wrap the SDL WindowSurface into a WgSoftSurface.

	WgPixelType type = WG_PIXEL_UNKNOWN;

	if( pWinSurf->format->BitsPerPixel == 32 )
		type = WG_PIXEL_ARGB_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		type = WG_PIXEL_RGB_8;
		
	WgSoftSurface_p pCanvas = WgSoftSurface::create( WgSize(pWinSurf->w,pWinSurf->h), type, (unsigned char*) pWinSurf->pixels, pWinSurf->pitch, 0 );

	// Wg create the GfxDevice that will be used for all rendering, providing
	// it our canvas to draw up.

	WgSoftGfxDevice_p pGfxDevice = WgSoftGfxDevice::create( pCanvas );

	// We create a RootPanel. This is responsible for rendering the
	// tree of child widgets connected to it and handle their events.
	// We provide it the GfxDevice to use for rendering.

	WgRootPanel_p pRoot = WgRootPanel::create( pGfxDevice );

	//------------------------------------------------------
	// Setup a simple GUI consisting of a filled background and 
	// a button using scaled bitmaps.
	//------------------------------------------------------
	
	// First we load the 24-bit bmp containing the button graphics.
	// No error handling or such to keep this example short and simple.

	SDL_Surface * pSDLSurf = SDL_LoadBMP( "../../../resources/simple_button.bmp" );
	WgSoftSurface_p pButtonSurface = WgSoftSurface::create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );

	// First we create and add a FlexPanel to the RootPanel.
	// The RootPanel can only take one child, but the FlexPanel
	// provides simple and powerful ways to layout multiple children.

	WgFlexPanel_p pFlexPanel = WgFlexPanel::create();
	pRoot->setWidget(pFlexPanel);

	// Now we create the background using the simplest widget
	// type, the Filler and add it to the FlexPanel, making
	// it stretch from the north-west to the south-east corners
	// of the FlexPanel.

	WgFiller_p pBackground = WgFiller::create();
	pBackground->setSkin( WgColorSkin::create(WgColor::aqua) );
	pFlexPanel->addWidget(pBackground, WG_NORTHWEST, WG_SOUTHEAST);

	// Now we create the button, using a clickable skin built from
	// the BMP with the button graphics. createClickableFromSurface()
	// expects the surface to be divided horizontally in four equal
	// sections containing the graphics for the button states NORMAL,
	// HOVERED, PRESSED and DISABLED. We specify that there are no
	// spacing between these four graphics blocks and that they have 
	// three pixel thick borders around that should not stretch.
	// When adding it to the FlexPanel we specify its geometry in
	// pixels and that it should be centered.

	WgButton_p pButton = WgButton::create();
	pButton->setSkin( WgBlockSkin::createClickableFromSurface( pButtonSurface, 0, WgBorder(3) ) );
	pFlexPanel->addWidget( pButton, WgRect(0,0,80,33), WG_CENTER );

	// Finally we add a callback to the click-event of the button.

	pRoot->msgRouter()->AddCallback( WgMsgFilter::select(), myButtonClickCallback );
	

	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	while( !bQuit ) 
	{
		// Loop through SDL events, translate them to WonderGUI events
		// and process them.
		
		translateEvents( pRoot->msgRouter() );

		// Let WonderGUI render any updated/dirty regions of the screen.

		SDL_LockSurface(pWinSurf);
		pRoot->render();
		SDL_UnlockSurface(pWinSurf);

		// Make SDL update any screen regions redrawn by WonderGUI.

		updateWindowRects( pRoot, pWin );

		SDL_Delay(20);
    }

	// Exit WonderGUI

	WgBase::exit();

	// Exit SDL

	SDL_Quit();

    return 0;
}

//____ translateEvents() ___________________________________________________________

void translateEvents( WgMsgRouter_p pMsgRouter )
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

	pMsgRouter->post( WgTickMsg::create(tickDiff) );

	// Process all the SDL events in a loop

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		
		switch( e.type )
		{
			case SDL_QUIT:
				bQuit = true;
				break;
				
			case SDL_MOUSEMOTION:
				pMsgRouter->post( WgMouseMoveMsg::create( WgCoord(e.motion.x,e.motion.y)) );
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				pMsgRouter->post( WgMousePressMsg::create( translateMouseButton(e.button.button)));
				break;

			case SDL_MOUSEBUTTONUP:
				pMsgRouter->post( WgMouseReleaseMsg::create( translateMouseButton(e.button.button)));
				break;
				
			default:
				break;
		}
	}
	
	pMsgRouter->dispatch();	
}

//____ translateMouseButton() __________________________________________________
//
// Translate SDL mouse button enums to WonderGUI equivalents.
//
WgMouseButton translateMouseButton( Uint8 button )
{
	switch( button )
	{
		case SDL_BUTTON_LEFT:
			return WG_BUTTON_LEFT;
		case SDL_BUTTON_MIDDLE:
			return WG_BUTTON_MIDDLE;
		case SDL_BUTTON_RIGHT:
			return WG_BUTTON_RIGHT;
		case SDL_BUTTON_X1:
			return WG_BUTTON_X1;
		case SDL_BUTTON_X2:
			return WG_BUTTON_X2;
		default:
			return WG_BUTTON_NONE;
	}
}

//____ updateWindowRects() _____________________________________________________
//
// Updates the rectangles of the SDL Window that WonderGUI has modified.
//
void updateWindowRects( WgRootPanel_p pRoot, SDL_Window * pWindow )
{	
	int nRects = pRoot->nbUpdatedRects();
	if( nRects == 0 )
		return;
	
	const WgRect * pUpdatedRects = pRoot->firstUpdatedRect();
	SDL_Rect * pSDLRects = (SDL_Rect*) WgBase::memStackAlloc( sizeof(SDL_Rect) * nRects );

	for( int i = 0 ; i < nRects ; i++ )
	{
		pSDLRects[i].x = pUpdatedRects[i].x;
		pSDLRects[i].y = pUpdatedRects[i].y;
		pSDLRects[i].w = pUpdatedRects[i].w;
		pSDLRects[i].h = pUpdatedRects[i].h;
	}

	SDL_UpdateWindowSurfaceRects( pWindow, pSDLRects, nRects );

	WgBase::memStackRelease( sizeof(SDL_Rect) * nRects );
}

//____ myButtonClickCallback() _________________________________________________

void myButtonClickCallback( const WgMsg_p& pMsg )
{
	bQuit = true;
}
