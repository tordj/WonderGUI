#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

#include <SDL_image.h>

#include <wondergui.h>
#include <wg_surface_sdl.h>
#include <wg_gfxdevice_sdl.h>
#include <wg_eventlogger.h>

#include <iostream>

extern std::ostream cout;

WgSurface * 	loadSurface( const char * path );
SDL_Surface *	initSDL( int w, int h );
bool			eventLoop( WgEventHandler * pHandler );


//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	// Init SDL

	SDL_Surface * pScreen = initSDL(640,480);
	if(!pScreen )
		return 1;

	IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG );

	// Init WonderGUI

	WgBase::Init();
	WgBase::InitFreeType();

	WgSurfaceSDL * pCanvas = new WgSurfaceSDL( pScreen );
	WgGfxDevice * pGfxDevice = new WgGfxDeviceSDL( pCanvas );
	WgInputDevice * pInputDevice = new WgInputDevice();

	WgRoot * pRoot = new WgRoot( pGfxDevice, pInputDevice );

	WgEventHandler * pEventHandler = new WgEventHandler( 0, pRoot );

	WgEventLogger * pEventLogger = new WgEventLogger( std::cout );
	pEventHandler->AddCallback( pEventLogger );

	// Load images and specify blocks

	WgSurface * pBackImg = loadSurface("What-Goes-Up-3.bmp");
	WgBlockSetPtr pBackBlock = pBackImg->defineBlockSet( WgRect(0,0,pBackImg->Width(),pBackImg->Height()), WgBorders(0), WgBorders(0), WG_TILE_ALL );

	WgSurface * pFlagImg = loadSurface("cb2.bmp");
	WgBlockSetPtr pFlagBlock = pFlagImg->defineBlockSet( WgRect(0,0,pFlagImg->Width(),pFlagImg->Height()), WgBorders(0), WgBorders(0), 0 );

	WgSurface * pBlocksImg = loadSurface("blocks.png");
	WgBlockSetPtr pButtonBlock = pBlocksImg->defineBlockSet( WgHorrTile4( WgRect(0,0,8*4+6,8), 2), WgBorders(3), WgBorders(2), WG_OPAQUE );


	//

	WgGizmoPixmap * pBackground = new WgGizmoPixmap();
	pBackground->SetSource( pBackBlock );

	//

	WgGizmoFlexGeo * pFlex = new WgGizmoFlexGeo();
	WgFlexHook * pHook = pFlex->AddGizmo( pBackground );

	pHook->SetAnchored( WG_NORTHWEST, WG_SOUTHEAST );

	pRoot->SetGizmo(pFlex);

	//

	WgGizmoButton * pButton = new WgGizmoButton();
	pButton->SetSource( pButtonBlock );

	pHook = pFlex->AddGizmo( pButton, WgRect(0,0,100,100), WG_NORTHWEST );

	//

	WgGizmoPixmap * pFlag1= new WgGizmoPixmap();
	pFlag1->SetSource( pFlagBlock );

	pHook = pFlex->AddGizmo( pFlag1, WgCord(0,0), WG_CENTER );



	WgGizmoPixmap * pFlag2= new WgGizmoPixmap();
	pFlag2->SetSource( pFlagBlock );

	pHook = pFlex->AddGizmo( pFlag2, WgCord(100,100), WG_CENTER );


    // program main loop

    while (eventLoop( pEventHandler ))
    {

        // DRAWING STARTS HERE

		pRoot->Render( WgRect(0,0,pCanvas->Width(),pCanvas->Height()) );


        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(pScreen);

        // Pause for a while

        SDL_Delay(10);

    } // end main loop


	// Exit WonderGUI

	delete pRoot;
	delete pGfxDevice;
	delete pInputDevice;
	delete pCanvas;
	delete pBackImg;
	delete pFlagImg;

	WgBase::Exit();

	IMG_Quit();

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}

//____ initSDL() ______________________________________________________________

SDL_Surface * initSDL( int w, int h )
{
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 0;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    SDL_Surface* pScreen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !pScreen )
    {
        printf("Unable to set %dx%d video: %s\n", w, h, SDL_GetError());
        return 0;
    }

	return pScreen;
}

//____ eventLoop() ____________________________________________________________

bool eventLoop( WgEventHandler * pHandler )
{
	static int	prevTicks = 0;

	int ticks = SDL_GetTicks();
	pHandler->QueueEvent( WgEvent::Tick( ticks - prevTicks ) );
	prevTicks = ticks;

   // message processing loop
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		// check for messages
		switch (event.type)
		{
			// exit if the window is closed
			case SDL_QUIT:
				return false;

			// check for keypresses
			case SDL_KEYDOWN:
			{
				// exit if ESCAPE is pressed
				if (event.key.keysym.sym == SDLK_ESCAPE)
					return false;
				break;
			}

			case	SDL_MOUSEMOTION:
			{
				pHandler->QueueEvent( WgEvent::PointerMove( WgCord( event.motion.x, event.motion.y ) ) );
				break;
			}

			case	SDL_MOUSEBUTTONDOWN:
				if(event.button.button == 4 )
					pHandler->QueueEvent( WgEvent::WheelRoll( 1, 120 ) );
				else if(event.button.button == 5)
					pHandler->QueueEvent( WgEvent::WheelRoll( 1, -120 ) );
				else
				{
//					pHandler->QueueEvent( WgEvent::PointerMove( WgCord( event.button.x, event.button.y )) );
					pHandler->QueueEvent( WgEvent::ButtonPress( event.button.button ) );
				}
				break;

			case	SDL_MOUSEBUTTONUP:
//				pHandler->QueueEvent( WgEvent::PointerMove( WgCord( event.button.x, event.button.y ) ));
				if( event.button.button != 4 && event.button.button != 5 )
				pHandler->QueueEvent( WgEvent::ButtonRelease( event.button.button ) );
				break;


		} // end switch
	} // end of message processing

	pHandler->ProcessEvents();

	return true;
}


//____ loadSurface() __________________________________________________________

WgSurface * loadSurface( const char * path )
{
    // load an image
    SDL_Surface* bmp = IMG_Load(path);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", IMG_GetError());
        return 0;
    }

	return new WgSurfaceSDL(bmp);

}
