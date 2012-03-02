#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif


#include <wondergui.h>
#include <wg_eventlogger.h>
#include <wg_bitmapglyphs.h>
#include <wg_vectorglyphs.h>
#include <wg_textprop.h>
#include <wg_gizmo_stack.h>
#include <iostream>

#include <wg_surface_soft.h>
#include <wg_gfxdevice_soft.h>
#include <sdl_wglib.h>

extern std::ostream cout;

SDL_Surface *	initSDL( int w, int h );
bool			eventLoop( WgEventHandler * pHandler );
WgRoot * 		setupGUI( WgGfxDevice * pDevice );


void cbInitDrag( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );
void cbDragGizmo( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );

void cbOpenModal( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );
void cbCloseModal( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );

void addResizableContainer( WgGizmoFlexGeo * pParent, WgGizmo * pChild, WgEventHandler * pEventHandler );
void cbResize( const WgEvent::Event* _pEvent, void * _pFlexHook );


WgGizmoModal * g_pModal = 0;

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
	sdl_wglib::MapKeys();

//	WgBase::InitFreeType();

	// Setup gfxdevice and gui

	WgSurfaceSoft * pCanvas = new WgSurfaceSoft( WgSize(640,480), WG_PIXEL_ARGB_8, (unsigned char *) pScreen->pixels, pScreen->pitch );
	WgGfxDeviceSoft * pGfxDevice = new WgGfxDeviceSoft( pCanvas );
	pGfxDevice->SetBilinearFiltering( true );

	// Load TTF-font
/*
	WgVectorGlyphs::SetSurfaceFactory( new WgSurfaceFactorySDL() );

	char	ttfname[] = { "a.ttf" };

	int size = fileSize( ttfname );
	char * pFontFile = (char*) loadFile( ttfname );
	WgVectorGlyphs * pGlyphs = new WgVectorGlyphs( pFontFile , size, 0 );

	WgFont * pFont = new WgFont();
	pFont->SetDefaultVectorGlyphs( pGlyphs );
*/
	// Load bitmap font

	WgFont * pFont = sdl_wglib::LoadBitmapFont( "../resources/anuvverbubbla_8x8.png", "../resources/anuvverbubbla_8x8.fnt", WgSurfaceFactorySoft() );

	// Load and setup cursor

	WgSurface * pCursorImg = sdl_wglib::LoadSurface("../resources/cursors.png", WgSurfaceFactorySoft() );

	WgGfxAnim * pCursorEOL = new WgGfxAnim();
	pCursorEOL->SetSize( WgSize(8,8) );
	pCursorEOL->AddFrames(pCursorImg, WgCoord(0,0), WgSize(2,1), 200 );
	pCursorEOL->SetPlayMode( WG_FORWARD_LOOPING );

	WgGfxAnim * pCursorINS = new WgGfxAnim();
	pCursorINS->SetSize( WgSize(8,8) );
	pCursorINS->AddFrames( pCursorImg, WgCoord(0,8), WgSize(2,1), 200 );
	pCursorINS->SetPlayMode( WG_FORWARD_LOOPING );

	WgCursor * pCursor = new WgCursor();
	pCursor->SetAnim(WgCursor::EOL, pCursorEOL);
	pCursor->SetAnim(WgCursor::INS, pCursorINS);
	pCursor->SetAnim(WgCursor::OVR, pCursorEOL);
	pCursor->SetBearing(WgCursor::EOL, WgCoord(0,-8));
	pCursor->SetBearing(WgCursor::INS, WgCoord(0,-8));
	pCursor->SetBearing(WgCursor::OVR, WgCoord(0,-8));

	WgBase::SetDefaultCursor( pCursor );

	// Set default textprop

	WgTextProp prop;

	prop.SetFont(pFont);
	prop.SetColor( WgColor::white );
	prop.SetSize(8);

	WgBase::SetDefaultTextProp( prop.Register() );


	WgRoot * pRoot = setupGUI( pGfxDevice );

   // program main loop

    while (eventLoop( pRoot->EventHandler() ))
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


	// Exit WonderGUI

	delete pRoot;
	delete pGfxDevice;

	WgBase::Exit();

	IMG_Quit();

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}


//____ setupGUI() ______________________________________________________________

WgRoot * setupGUI( WgGfxDevice * pDevice )
{
	WgResDB * pDB = sdl_wglib::LoadStdGizmos( "../resources/blocks.png", WgSurfaceFactorySoft() );
	if( !pDB )
		return 0;

	WgRoot * pRoot = new WgRoot( pDevice );

	WgEventHandler * pEventHandler = pRoot->EventHandler();

	WgEventLogger * pEventLogger = new WgEventLogger( std::cout );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_POSITION );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSEBUTTON_REPEAT );
	pEventLogger->IgnoreEvent( WG_EVENT_BUTTON_PRESS );
//	pEventLogger->IgnoreAllEvents();
//	pEventLogger->LogMouseButtonEvents();
	pEventHandler->AddCallback( pEventLogger );


	// Load images and specify blocks

	WgSurface * pBackImg = sdl_wglib::LoadSurface("../resources/What-Goes-Up-3.bmp", WgSurfaceFactorySoft() );
	WgBlockSetPtr pBackBlock = pBackImg->defineBlockSet( WgRect(0,0,pBackImg->Width(),pBackImg->Height()), WgBorders(0), WgBorders(0), 0, WG_TILE_ALL );

	WgSurface * pFlagImg = sdl_wglib::LoadSurface("cb2.bmp", WgSurfaceFactorySoft() );
	WgBlockSetPtr pFlagBlock = pFlagImg->defineBlockSet( WgRect(0,0,pFlagImg->Width(),pFlagImg->Height()), WgBorders(0), WgBorders(0), 0, 0 );

	WgSurface * pSplashImg = sdl_wglib::LoadSurface("../resources/splash.png", WgSurfaceFactorySoft() );
	WgBlockSetPtr pSplashBlock = pSplashImg->defineBlockSet( WgRect(0,0,pSplashImg->Width(),pSplashImg->Height()), WgBorders(0), WgBorders(0), 0, 0 );

	// Background

	WgGizmoPixmap * pBackground = new WgGizmoPixmap();
	pBackground->SetSource( pBackBlock );

	// Main Flex

	WgGizmoFlexGeo * pFlex = new WgGizmoFlexGeo();
	WgFlexHook * pHook = pFlex->AddChild( pBackground );

	pHook->SetAnchored( WG_NORTHWEST, WG_SOUTHEAST );

	//
/*
	{
		WgGizmoShader * pShader = new WgGizmoShader();
		pShader->SetBlendMode(WG_BLENDMODE_ADD);
		pShader->SetColor( WgColor(0xFFFFFFFF) );

		WgGizmoStack * pStack = new WgGizmoStack();
		pShader->SetChild( pStack );

		WgGizmoPixmap * pBg = new WgGizmoPixmap();
		pBg->SetSource( pButtonBlock );
		pStack->AddChild( pBg );

		WgGizmoPixmap * pSplash= new WgGizmoPixmap();
		pSplash->SetSource( pSplashBlock );
		WgStackHook * pHook = pStack->AddChild( pSplash );
		pHook->SetSizePolicy( WgStackHook::SCALE );
		pHook->SetOrientation( WG_CENTER );
		pHook->SetBorders( WgBorders(2) );


		addResizableContainer( pFlex, pShader, pEventHandler );
	}
*/
	// Modal container

	g_pModal = new WgGizmoModal();
	g_pModal->SetBase( pFlex );

	pRoot->SetChild(g_pModal);


	// Modal button

	WgGizmoButton * pModalButton = (WgGizmoButton*) pDB->CloneGizmo( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonClick(pModalButton, 1), cbCloseModal, pModalButton );

	//

	WgGizmoButton * pButton = (WgGizmoButton*) pDB->CloneGizmo( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pButton, 1), cbOpenModal, pModalButton );

	pHook = pFlex->AddChild( pButton, WgRect(0,0,100,100), WG_NORTHWEST );

	//

	WgGizmoPixmap * pFlag1= new WgGizmoPixmap();
	pFlag1->SetSource( pSplashBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pFlag1, 1), cbInitDrag, pFlag1 );

	pHook = pFlex->AddChild( pFlag1, WgCoord(0,0), WG_CENTER );



	WgGizmoPixmap * pFlag2= new WgGizmoPixmap();
	pFlag2->SetSource( pFlagBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pFlag1, 1), cbDragGizmo, pFlag1 );

	pHook = pFlex->AddChild( pFlag2, WgCoord(100,100), WG_CENTER );

	//


	WgGizmoVBox * pVBox = new WgGizmoVBox();
//	pFlex->AddChild( pVBox, WgCoord(50,50), WG_NORTHWEST );


	WgGizmoPixmap * pFlag3 = new WgGizmoPixmap();
	pFlag3->SetSource( pFlagBlock );
	WgGizmoPixmap * pFlag4= new WgGizmoPixmap();
	pFlag4->SetSource( pFlagBlock );

	WgGizmoButton * pButton2 = (WgGizmoButton*) pDB->CloneGizmo( "button" );
	pButton2->SetText( "BUTTON TEXT" );

	pVBox->AddChild(pButton2);

	pVBox->AddChild(pFlag3);
//	pVBox->AddChild(pFlag4);

	pHook = pFlex->AddChild( pVBox, WgCoord(50,50), WG_NORTHWEST );
	pHook->SetMaxSize( WgSize(120, INT_MAX) );

	//

	WgGizmoTabOrder * pTabOrder = new WgGizmoTabOrder();
	pVBox->AddChild(pTabOrder);

	WgGizmoVBox * pTabBox = new WgGizmoVBox();
	pTabOrder->SetChild(pTabBox);
//	pVBox->AddChild(pTabBox);

	WgGizmoText * pText1 = new WgGizmoText();
	pText1->SetText("TEXTA1");
	pText1->SetEditMode(WG_TEXT_EDITABLE);
	pTabBox->AddChild(pText1);

	WgGizmoText * pText2 = new WgGizmoText();
	pText2->SetText("TEXTB234ABC sajfas kjfaljsras kjasdfkasd kajfd fkajfa fkdjfa dfasfda asdkfj");
	pText2->SetEditMode(WG_TEXT_EDITABLE);
	pTabBox->AddChild(pText2);

	pText1->GrabFocus();

//	pTabOrder->AddToTabOrder(pText1);
//	pTabOrder->AddToTabOrder(pText2);

	// Radiobuttons test

	pVBox->AddChild( pDB->CloneGizmo( "radiobutton" ) );
	pVBox->AddChild( pDB->CloneGizmo( "radiobutton" ) );

	pFlex->AddChild( pDB->CloneGizmo( "radiobutton" ) );
	pFlex->AddChild( pDB->CloneGizmo( "radiobutton" ) );

	pVBox->SetRadioGroup(true);

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



WgCoord dragStartPos;

//____ cbInitDrag() ___________________________________________________________

void cbInitDrag( const WgEvent::Event* _pEvent, WgGizmo * pGizmo )
{
	WgFlexHook * pHook = static_cast<WgFlexHook*>(pGizmo->Hook());


	dragStartPos = pHook->FloatOfs();
	printf( "DRAG START!\n" );
}

//____ cbDragGizmo() __________________________________________________________

void cbDragGizmo( const WgEvent::Event* _pEvent, WgGizmo * pGizmo )
{
	if( _pEvent->Type() != WG_EVENT_MOUSEBUTTON_DRAG || !pGizmo->ParentX() )
		return;

	const WgEvent::MouseButtonDrag* pEvent = static_cast<const WgEvent::MouseButtonDrag*>(_pEvent);

	WgCoord	dragDistance = pEvent->DraggedTotal();

	WgCoord	ofs = dragStartPos + dragDistance;

//	printf( "AccDistance: %d, %d\n", dragDistance.x, dragDistance.y );
	printf( "ofs: %d, %d   start: %d %d   distance: %d, %d\n", ofs.x, ofs.y, dragStartPos.x, dragStartPos.y, dragDistance.x, dragDistance.y );

	WgFlexHook * pHook = static_cast<WgFlexHook*>(pGizmo->Hook());
	pHook->SetOfs(dragStartPos+dragDistance);
}

//____ cbOpenModal() __________________________________________________________

void cbOpenModal( const WgEvent::Event* _pEvent, WgGizmo * pGizmo )
{
	g_pModal->AddModal( pGizmo, WgCoord(), WG_SOUTHEAST );
}

//____ cbCloseModal() __________________________________________________________

void cbCloseModal( const WgEvent::Event* _pEvent, WgGizmo * pGizmo )
{
	g_pModal->ReleaseChild(pGizmo);
}

//____ cbResizeGizmo() _________________________________________________________

void cbResize( const WgEvent::Event* _pEvent, void * _pFlexHook )
{
	WgFlexHook * pHook = static_cast<WgFlexHook*>(_pFlexHook);
	const WgEvent::MouseButtonDrag* pEvent = static_cast<const WgEvent::MouseButtonDrag*>(_pEvent);

	WgCoord dragged = pEvent->DraggedNow();

	pHook->SetSize( pHook->Size() + WgSize(dragged.x,dragged.y) );
}



//____ addResizableContainer() _________________________________________________

void addResizableContainer( WgGizmoFlexGeo * pParent, WgGizmo * pChild, WgEventHandler * pEventHandler )
{
	WgHook * pHook = pParent->AddChild( pChild );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pChild, 2), cbResize, pHook );


	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pChild, 3), cbInitDrag, pChild );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pChild, 3), cbDragGizmo, pChild );
}





