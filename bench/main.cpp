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
#include <wg_surface_sdl.h>
#include <wg_gfxdevice_sdl.h>
#include <wg_eventlogger.h>
#include <wg_bitmapglyphs.h>
#include <wg_textprop.h>

#include <iostream>

extern std::ostream cout;

int fileSize( const char * pPath );
void * loadFile( const char * pPath );

WgSurface * 	loadSurface( const char * path );
SDL_Surface *	initSDL( int w, int h );
bool			eventLoop( WgEventHandler * pHandler );

void cbInitDrag( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );
void cbDragGizmo( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );

void cbOpenModal( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );
void cbCloseModal( const WgEvent::Event* _pEvent, WgGizmo * pGizmo );

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
	WgBase::InitFreeType();

	WgSurfaceSDL * pCanvas = new WgSurfaceSDL( pScreen );
	WgGfxDevice * pGfxDevice = new WgGfxDeviceSDL( pCanvas );

	WgRoot * pRoot = new WgRoot( pGfxDevice );
	pRoot->SetGeo(WgRect(0,0,640,480));

	WgEventHandler * pEventHandler = pRoot->EventHandler();

	WgEventLogger * pEventLogger = new WgEventLogger( std::cout );
	pEventLogger->IgnoreEvent( WG_EVENT_POINTER_PLACED );
	pEventLogger->IgnoreEvent( WG_EVENT_POINTER_MOVE );
//	pEventLogger->IgnoreAllEvents();
//	pEventLogger->LogButtonEvents();
	pEventHandler->AddCallback( pEventLogger );

	pEventHandler->MapKey( WG_KEY_SHIFT, SDLK_LSHIFT );
	pEventHandler->MapKey( WG_KEY_SHIFT, SDLK_RSHIFT );
	pEventHandler->MapKey( WG_KEY_CONTROL, SDLK_LCTRL );
	pEventHandler->MapKey( WG_KEY_CONTROL, SDLK_RCTRL );
	pEventHandler->MapKey( WG_KEY_ALT, SDLK_LALT );
	pEventHandler->MapKey( WG_KEY_ALT, SDLK_RALT );


	pEventHandler->MapKey( WG_KEY_LEFT, SDLK_LEFT );
	pEventHandler->MapKey( WG_KEY_RIGHT, SDLK_RIGHT );
	pEventHandler->MapKey( WG_KEY_UP, SDLK_UP );
	pEventHandler->MapKey( WG_KEY_DOWN, SDLK_DOWN );

	pEventHandler->MapKey( WG_KEY_HOME, SDLK_HOME );
	pEventHandler->MapKey( WG_KEY_END, SDLK_END );
	pEventHandler->MapKey( WG_KEY_PAGEUP, SDLK_PAGEUP );
	pEventHandler->MapKey( WG_KEY_PAGEDOWN, SDLK_PAGEDOWN );

	pEventHandler->MapKey( WG_KEY_RETURN, SDLK_RETURN );
	pEventHandler->MapKey( WG_KEY_BACKSPACE, SDLK_BACKSPACE );
	pEventHandler->MapKey( WG_KEY_DELETE, SDLK_DELETE );
	pEventHandler->MapKey( WG_KEY_TAB, SDLK_TAB );
	pEventHandler->MapKey( WG_KEY_ESCAPE, SDLK_ESCAPE );



	// Load font

	WgSurface * pFontImg = loadSurface("anuvverbubbla_8x8.png");
	char * pFontSpec = (char*) loadFile( "anuvverbubbla_8x8.fnt" );

	WgBitmapGlyphs * pGlyphs = new WgBitmapGlyphs( pFontImg, pFontSpec );

	WgFont * pFont = new WgFont();
	pFont->SetBitmapGlyphs( pGlyphs, WG_STYLE_NORMAL, 8 );

	WgSurface * pCursorImg = loadSurface("cursors.png");

	WgGfxAnim * pCursorEOL = new WgGfxAnim();
	pCursorEOL->setHeight(8);
	pCursorEOL->setWidth(8);
	pCursorEOL->addHorrTiledFrames(2, pCursorImg, 0, 0, 200 );
	pCursorEOL->SetPlayMode( WG_FORWARD_LOOPING );

	WgGfxAnim * pCursorINS = new WgGfxAnim();
	pCursorINS->setHeight(8);
	pCursorINS->setWidth(8);
	pCursorINS->addHorrTiledFrames(2, pCursorImg, 0, 8, 200 );
	pCursorINS->SetPlayMode( WG_FORWARD_LOOPING );

	WgCursor * pCursor = new WgCursor();
	pCursor->setAnim(WgCursor::EOL, pCursorEOL);
	pCursor->setAnim(WgCursor::INS, pCursorINS);
	pCursor->setAnim(WgCursor::OVR, pCursorEOL);
	pCursor->setBearing(WgCursor::EOL, WgCoord(0,-8));
	pCursor->setBearing(WgCursor::INS, WgCoord(0,-8));
	pCursor->setBearing(WgCursor::OVR, WgCoord(0,-8));

	// Set default textprop

	WgTextProp prop;

	prop.SetFont(pFont);
	prop.SetSize(8);

	WgBase::SetDefaultTextProp( prop.Register() );

	// Load images and specify blocks

	WgSurface * pBackImg = loadSurface("What-Goes-Up-3.bmp");
	WgBlockSetPtr pBackBlock = pBackImg->defineBlockSet( WgRect(0,0,pBackImg->Width(),pBackImg->Height()), WgBorders(0), WgBorders(0), 0, WG_TILE_ALL );

	WgSurface * pFlagImg = loadSurface("cb2.bmp");
	WgBlockSetPtr pFlagBlock = pFlagImg->defineBlockSet( WgRect(0,0,pFlagImg->Width(),pFlagImg->Height()), WgBorders(0), WgBorders(0), 0, 0 );

	WgSurface * pBlocksImg = loadSurface("blocks.png");
	WgBlockSetPtr pButtonBlock = pBlocksImg->defineBlockSet( WgHorrTile4( WgRect(0,0,8*4+6,8), 2), WgBorders(3), WgBorders(2), 0, WG_OPAQUE );


	// Background

	WgGizmoPixmap * pBackground = new WgGizmoPixmap();
	pBackground->SetSource( pBackBlock );

	// Main Flex

	WgGizmoFlexGeo * pFlex = new WgGizmoFlexGeo();
	WgFlexHook * pHook = pFlex->AddChild( pBackground );

	pHook->SetAnchored( WG_NORTHWEST, WG_SOUTHEAST );


	// Modal container

	g_pModal = new WgGizmoModal();
	g_pModal->SetBase( pFlex );

	pRoot->SetChild(g_pModal);


	// Modal button

	WgGizmoButton * pModalButton = new WgGizmoButton();
	pModalButton->SetSource( pButtonBlock );

	pEventHandler->AddCallback( WgEventFilter::ButtonClick(pModalButton, 1), cbCloseModal, pModalButton );

	//

	WgGizmoButton * pButton = new WgGizmoButton();
	pButton->SetSource( pButtonBlock );

	pHook = pFlex->AddChild( pButton, WgRect(0,0,100,100), WG_NORTHWEST );

	pEventHandler->AddCallback( WgEventFilter::ButtonPress(pButton, 1), cbOpenModal, pModalButton );

	//

	WgGizmoPixmap * pFlag1= new WgGizmoPixmap();
	pFlag1->SetSource( pFlagBlock );

	pHook = pFlex->AddChild( pFlag1, WgCoord(0,0), WG_CENTER );



	WgGizmoPixmap * pFlag2= new WgGizmoPixmap();
	pFlag2->SetSource( pFlagBlock );

	pHook = pFlex->AddChild( pFlag2, WgCoord(100,100), WG_CENTER );


	pEventHandler->AddCallback( WgEventFilter::ButtonPress(pFlag1, 1), cbInitDrag, pFlag1 );
	pEventHandler->AddCallback( WgEventFilter::ButtonDrag(pFlag1, 1), cbDragGizmo, pFlag1 );

	//


	WgVBoxLayout * pVBox = new WgVBoxLayout();
//	pFlex->AddChild( pVBox, WgCoord(50,50), WG_NORTHWEST );


	WgGizmoPixmap * pFlag3 = new WgGizmoPixmap();
	pFlag3->SetSource( pFlagBlock );
	WgGizmoPixmap * pFlag4= new WgGizmoPixmap();
	pFlag4->SetSource( pFlagBlock );

	WgGizmoButton * pButton2 = new WgGizmoButton();
	pButton2->SetSource( pButtonBlock );
	pButton2->SetText( "STANDARD BUTTON" );

	pVBox->AddChild(pButton2);

	pVBox->AddChild(pFlag3);
//	pVBox->AddChild(pFlag4);

	pHook = pFlex->AddChild( pVBox, WgCoord(50,50), WG_NORTHWEST );
	pHook->SetMaxSize( WgSize(120, INT_MAX) );

	//

	WgGizmoTabOrder * pTabOrder = new WgGizmoTabOrder();
	pVBox->AddChild(pTabOrder);

	WgVBoxLayout * pTabBox = new WgVBoxLayout();
	pTabOrder->SetChild(pTabBox);

	WgGizmoText * pText1 = new WgGizmoText();
	pText1->SetText("TEXTA1");
	pText1->SetEditMode(WG_TEXT_EDITABLE);
	pText1->SetCursor(pCursor);
	pTabBox->AddChild(pText1);

	WgGizmoText * pText2 = new WgGizmoText();
	pText2->SetText("TEXTB234ABC sajfas kjfaljsras kjasdfkasd kajfd fkajfa fkdjfa dfasfda asdkfj");
	pText2->SetEditMode(WG_TEXT_EDITABLE);
	pText2->SetCursor(pCursor);
	pTabBox->AddChild(pText2);

	pText1->GrabFocus();

	pTabOrder->AddToTabOrder(pText1);
	pTabOrder->AddToTabOrder(pText2);
	

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
				pHandler->QueueEvent( new WgEvent::PointerMove( WgCoord( event.motion.x, event.motion.y ) ) );
				break;
			}

			case	SDL_MOUSEBUTTONDOWN:
				if(event.button.button == 4 )
					pHandler->QueueEvent( new WgEvent::WheelRoll( 1, 120 ) );
				else if(event.button.button == 5)
					pHandler->QueueEvent( new WgEvent::WheelRoll( 1, -120 ) );
				else
				{
//					pHandler->QueueEvent( WgEvent::PointerMove( WgCoord( event.button.x, event.button.y )) );
					pHandler->QueueEvent( new WgEvent::ButtonPress( event.button.button ) );
				}
				break;

			case	SDL_MOUSEBUTTONUP:
//				pHandler->QueueEvent( WgEvent::PointerMove( WgCoord( event.button.x, event.button.y ) ));
				if( event.button.button != 4 && event.button.button != 5 )
				pHandler->QueueEvent( new WgEvent::ButtonRelease( event.button.button ) );
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
	if( _pEvent->Type() != WG_EVENT_BUTTON_DRAG || !pGizmo->ParentX() )
		return;

	const WgEvent::ButtonDrag* pEvent = static_cast<const WgEvent::ButtonDrag*>(_pEvent);

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


