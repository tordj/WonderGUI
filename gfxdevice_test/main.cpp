#include <stdio.h>

#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#include <wondergui.h>

int fileSize( const char * pPath );
void * loadFile( const char * pPath );

WgSurface * 	loadSurface( const char * path );
SDL_Surface *	initSDL( int w, int h );
bool			eventLoop( WgEventHandler * pHandler );


//____ main() __________________________________________________________________

int main(int argc, char **argv)
{
	// Init SDL

	SDL_Surface * pScreen = initSDL(1024,800);
	if(!pScreen )
		return 1;

	IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG );

	// Init WonderGUI

	WgBase::Init();

	WgSurfaceSDL * pCanvas = new WgSurfaceSDL( pScreen );
	WgGfxDevice * pGfxDevice = new WgGfxDeviceSDL( pCanvas );

	WgRoot * pRoot = new WgRoot( pGfxDevice );
	pRoot->SetGeo(WgRect(0,0,1024,800));

	WgEventHandler * pEventHandler = pRoot->EventHandler();

	WgBase::MapKey( WG_KEY_SHIFT, SDLK_LSHIFT );
	WgBase::MapKey( WG_KEY_SHIFT, SDLK_RSHIFT );
	WgBase::MapKey( WG_KEY_CONTROL, SDLK_LCTRL );
	WgBase::MapKey( WG_KEY_CONTROL, SDLK_RCTRL );
	WgBase::MapKey( WG_KEY_ALT, SDLK_LALT );
	WgBase::MapKey( WG_KEY_ALT, SDLK_RALT );
	
	WgBase::MapKey( WG_KEY_LEFT, SDLK_LEFT );
	WgBase::MapKey( WG_KEY_RIGHT, SDLK_RIGHT );
	WgBase::MapKey( WG_KEY_UP, SDLK_UP );
	WgBase::MapKey( WG_KEY_DOWN, SDLK_DOWN );

	WgBase::MapKey( WG_KEY_HOME, SDLK_HOME );
	WgBase::MapKey( WG_KEY_END, SDLK_END );
	WgBase::MapKey( WG_KEY_PAGEUP, SDLK_PAGEUP );
	WgBase::MapKey( WG_KEY_PAGEDOWN, SDLK_PAGEDOWN );

	WgBase::MapKey( WG_KEY_RETURN, SDLK_RETURN );
	WgBase::MapKey( WG_KEY_BACKSPACE, SDLK_BACKSPACE );
	WgBase::MapKey( WG_KEY_DELETE, SDLK_DELETE );
	WgBase::MapKey( WG_KEY_TAB, SDLK_TAB );
	WgBase::MapKey( WG_KEY_ESCAPE, SDLK_ESCAPE );
	
	// Load bitmap font

	WgSurface * pFontImg = loadSurface("anuvverbubbla_8x8.png");
	char * pFontSpec = (char*) loadFile( "anuvverbubbla_8x8.fnt" );

	WgBitmapGlyphs * pGlyphs = new WgBitmapGlyphs( pFontImg, pFontSpec );

	WgFont * pFont = new WgFont();
	pFont->SetBitmapGlyphs( pGlyphs, WG_STYLE_NORMAL, 8 );

	// Set default textprop

	WgTextProp prop;

	prop.SetFont(pFont);
	prop.SetColor( WgColor::white );
	prop.SetSize(8);

	WgBase::SetDefaultTextProp( prop.Register() );

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
	
	SDL_EnableUNICODE(true);

	return pScreen;
}

//____ eventLoop() ____________________________________________________________

bool eventLoop( WgEventHandler * pHandler )
{
	static int	prevTicks = 0;

	int ticks = SDL_GetTicks();
	pHandler->QueueEvent( new WgEvent::Tick( ticks - prevTicks ) );
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

				pHandler->QueueEvent( new WgEvent::KeyPress( event.key.keysym.sym ) );
				if( event.key.keysym.unicode != 0 )
					pHandler->QueueEvent( new WgEvent::Character( event.key.keysym.unicode ) );
				break;
			}

			case SDL_KEYUP:
			{
				pHandler->QueueEvent( new WgEvent::KeyRelease( event.key.keysym.sym ) );
				break;
			}

			case	SDL_MOUSEMOTION:
			{
				pHandler->QueueEvent( new WgEvent::MouseMove( WgCoord( event.motion.x, event.motion.y ) ) );
				break;
			}

			case	SDL_MOUSEBUTTONDOWN:
				if(event.button.button == 4 )
					pHandler->QueueEvent( new WgEvent::MouseWheelRoll( 1, 120 ) );
				else if(event.button.button == 5)
					pHandler->QueueEvent( new WgEvent::MouseWheelRoll( 1, -120 ) );
				else
				{
//					pHandler->QueueEvent( WgEvent::MouseMove( WgCoord( event.button.x, event.button.y )) );
					pHandler->QueueEvent( new WgEvent::MouseButtonPress( event.button.button ) );
				}
				break;

			case	SDL_MOUSEBUTTONUP:
//				pHandler->QueueEvent( WgEvent::MouseMove( WgCoord( event.button.x, event.button.y ) ));
				if( event.button.button != 4 && event.button.button != 5 )
				pHandler->QueueEvent( new WgEvent::MouseButtonRelease( event.button.button ) );
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

//____ fileSize() _____________________________________________________________

int fileSize( const char * pPath )
{
	FILE * fp = fopen( pPath, "rb" );
	if( !fp )
		return 0;

	fseek( fp, 0, SEEK_END );
	int size = ftell(fp);
	fseek( fp, 0, SEEK_SET );
	fclose( fp );

	return size;
}

//____ loadFile() _____________________________________________________________

void * loadFile( const char * pPath )
{
	FILE * fp = fopen( pPath, "rb" );
	if( !fp )
		return 0;

	fseek( fp, 0, SEEK_END );
	int size = ftell(fp);
	fseek( fp, 0, SEEK_SET );

	char * pMem = (char*) malloc( size+1 );
	pMem[size] = 0;
	int nRead = fread( pMem, 1, size, fp );
	fclose( fp );

	if( nRead < size )
	{
		free( pMem );
		return 0;
	}

	return pMem;

}
