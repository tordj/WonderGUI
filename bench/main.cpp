#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include <wondergui.h>
#include <wg_surface_sdl.h>
#include <wg_gfxdevice_sdl.h>


SDL_Surface *	initSDL( int w, int h );
bool	eventLoop();


//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	// Init SDL

	SDL_Surface * pScreen = initSDL(640,480);
	if(!pScreen )
		return 1;

	// Init WonderGUI

	WgBase::Init();
	WgBase::InitFreeType();

	WgSurfaceSDL * pCanvas = new WgSurfaceSDL( pScreen );
	WgGfxDevice * pGfxDevice = new WgGfxDeviceSDL( pCanvas );
	WgInputDevice * pInputDevice = new WgInputDevice();

	WgRoot * pRoot = new WgRoot( pGfxDevice, pInputDevice );

    // load an image
    SDL_Surface* bmp = SDL_LoadBMP("cb2.bmp");
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        return 1;
    }


	WgSurfaceSDL * pImage = new WgSurfaceSDL(bmp);
	WgBlockSetPtr pImageBlock = pImage->defineBlockSet( WgRect(0,0,bmp->w,bmp->h), WgBorders(0), WgBorders(0), 0 );

	//

	WgGizmoPixmap * pBackground = new WgGizmoPixmap();
	pBackground->SetSource( pImageBlock );

	pRoot->SetGizmo(pBackground);

    // program main loop

    while (eventLoop())
    {

        // DRAWING STARTS HERE

		pRoot->Render( WgRect(0,0,pCanvas->Width(),pCanvas->Height()) );


        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(pScreen);
    } // end main loop


	// Exit WonderGUI

	delete pRoot;
	delete pGfxDevice;
	delete pInputDevice;
	delete pCanvas;
	delete pImage;

	WgBase::Exit();

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

bool eventLoop()
{
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
		} // end switch
	} // end of message processing

	return true;
}
