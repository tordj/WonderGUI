
#include <stdio.h>

#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#include <wondergui.h>
#include <wg_surface_soft.h>
#include <wg_gfxdevice_soft.h>
#include <sdl_wglib.h>

int fileSize( const char * pPath );
void * loadFile( const char * pPath );

WgSurface * 	loadSurface( const char * path );
SDL_Surface *	initSDL( int w, int h );
bool			eventLoop( WgEventHandler * pHandler );
WgRoot * 		setupGUI( WgGfxDevice * pDevice );


//____ main() __________________________________________________________________

int main(int argc, char **argv)
{
	// Init SDL

	SDL_Surface * pScreen = initSDL(1024,640);
	if(!pScreen )
		return 1;

	IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF );

	// Init WonderGUI

	WgBase::Init();

	WgSurfaceSoft * pCanvas = new WgSurfaceSoft( WgSize(1024,640), WG_PIXEL_RGBA_8 );
	WgGfxDevice * pGfxDevice = new WgGfxDeviceSoft( pCanvas );

	WgRoot * pRoot = new WgRoot( pGfxDevice );
	pRoot->SetGeo(WgRect(0,0,1024,640));

	WgEventHandler * pEventHandler = pRoot->EventHandler();

	sdl_wglib::MapKeys();

	// Load bitmap font

	WgSurface * pFontImg = sdl_wglib::LoadSurface("../resources/anuvverbubbla_8x8.png", WgSurfaceFactorySoft() );
	pFontImg->defineBlockSet( WgRect(0,0,10,10), WgBorders(0), WgBorders(0), 0, WG_TILE_ALL );	//dummy!!!

	char * pFontSpec = (char*) loadFile( "../resources/anuvverbubbla_8x8.fnt" );

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


//____ setupGUI() ______________________________________________________________

WgRoot * setupGUI( WgGfxDevice * pDevice )
{
	WgResDB * pDB = LoadStdGizmos( "../resources/blocks.png" );
	if( !pDB )
		return 0;

	WgRoot * pRoot = new WgRoot( pGfxDevice );


	return pRoot;
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
	sdl_wglib::BeginEvents( pHandler );

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
			}
		}
		sdl_wglib::TranslateEvent( event );
	}

	sdl_wglib::EndEvents();

	return true;
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
