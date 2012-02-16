
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
	sdl_wglib::MapKeys();

	// Load bitmap font

	WgFont * pFont = sdl_wglib::LoadBitmapFont( "../resources/anuvverbubbla_8x8.png", "../resources/anuvverbubbla_8x8.fnt", WgSurfaceFactorySoft() );

	// Set default textprop

	WgTextProp prop;

	prop.SetFont(pFont);
	prop.SetColor( WgColor::white );
	prop.SetSize(8);

	WgBase::SetDefaultTextProp( prop.Register() );

	// Setup gfxdevice and gui

	WgSurfaceSoft * pCanvas = new WgSurfaceSoft( WgSize(1024,640), WG_PIXEL_ARGB_8, (unsigned char *) pScreen->pixels, pScreen->pitch );
	WgGfxDeviceSoft * pDevice = new WgGfxDeviceSoft( pCanvas );
	pDevice->SetBilinearFiltering( true );


	WgRoot * pRoot = setupGUI(pDevice);
	if( !pRoot )
	{
		delete pDevice;
		return -1;
	}

	WgEventHandler * pEventHandler = pRoot->EventHandler();

   // program main loop

    while (eventLoop( pEventHandler ))
    {

		// GET DIRTY RECTS

		int nDirtyRects;
		SDL_Rect	dirtyRects[100];

		if( pRoot->NbDirtyRects() <= 100 )
		{
			nDirtyRects = pRoot->NbDirtyRects();
			for( int i = 0 ; i < nDirtyRects ; i++ )
			{
				const WgRect * pR = pRoot->FirstDirtyRect() + i;

				dirtyRects[i].x = pR->x;
				dirtyRects[i].y = pR->y;
				dirtyRects[i].w = pR->w;
				dirtyRects[i].h = pR->h;
			}
		}
		else
		{
			nDirtyRects = 1;

			const WgRect r = pRoot->Geo();

			dirtyRects[0].x = r.x;
			dirtyRects[0].y = r.y;
			dirtyRects[0].w = r.w;
			dirtyRects[0].h = r.h;
		}


        // DRAWING STARTS HERE

		SDL_LockSurface( pScreen );
		pRoot->Render();
		SDL_UnlockSurface( pScreen );

        // DRAWING ENDS HERE

        // finally, update the screen :)

		SDL_UpdateRects( pScreen, nDirtyRects, dirtyRects);


        // Pause for a while

        SDL_Delay(10);

    } // end main loop


	WgBase::Exit();
	IMG_Quit();

	return 0;
}


//____ setupGUI() ______________________________________________________________

WgRoot * setupGUI( WgGfxDevice * pDevice )
{
	WgResDB * pDB = sdl_wglib::LoadStdGizmos( "../resources/blocks.png", WgSurfaceFactorySoft() );
	if( !pDB )
		return 0;

	WgRoot * pRoot = new WgRoot( pDevice );

	WgGizmoFlexGeo * pMainContainer = new WgGizmoFlexGeo();

	int hAnchorLeft = pMainContainer->AddAnchor( 0.f, 0.f, WgCoord(0,100) );
	int hAnchorRight = pMainContainer->AddAnchor( 1.f, 0.f, WgCoord(0,100) );

	pRoot->SetChild(pMainContainer);



	WgGizmoStack * pPanelStack = new WgGizmoStack();
	pMainContainer->AddChild( pPanelStack, WG_NORTHWEST, hAnchorRight );

//	WgGizmo * pCheckeredBack = pDB->CloneGizmo( "bg_blue_gradient" );
	WgGizmo * pCheckeredBack = pDB->CloneGizmo( "bg_checkered_grey" );
	pMainContainer->AddChild( pCheckeredBack, hAnchorLeft, WG_SOUTHEAST );


	WgGizmo * pPanelBack = pDB->CloneGizmo( "plate" );
	pPanelStack->AddChild( pPanelBack );

	WgGizmoFill * pFill = new WgGizmoFill();
	pFill->SetColor( WgColor(255,0,0) );
	pMainContainer->AddChild( pFill, WgRect(100,100,100,100) );

	WgGizmoFill * pFill2 = new WgGizmoFill();
	pFill2->SetColor( WgColor(0,255,0) );
	pMainContainer->AddChild( pFill2, WgRect(200,100,100,100) );

	WgGizmoFill * pFill3 = new WgGizmoFill();
	pFill3->SetColor( WgColor(0,0,255) );
	pMainContainer->AddChild( pFill3, WgRect(300,100,100,100) );

	WgGizmoText * pText = new WgGizmoText();
	pText->SetTextProperties( WgBase::GetDefaultTextProp() );
	pText->SetText( "TESTING" );
	pPanelStack->AddChild( pText );

	WgGizmo * pSlider = pDB->CloneGizmo( "hdragbar" );
	WgStackHook * pDragbarHook = pPanelStack->AddChild( pSlider );
//	pDragbarHook->SetOrientation( WG_SOUTH );
	pDragbarHook->SetBorders( WgBorders(0,0,81,0) );

	//

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
    SDL_Surface* pScreen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
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


