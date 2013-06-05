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
#include <wg_bitmapglyphs.h>
#include <wg_vectorglyphs.h>
#include <wg_knob.h>
#include <iostream>

#include <wg_surface_soft.h>
#include <wg_gfxdevice_soft.h>
#include <sdl_wglib.h>

extern std::ostream cout;

SDL_Surface *	initSDL( int w, int h );
bool			eventLoop( WgEventHandler * pHandler );
WgRootPanel * 		setupGUI( WgGfxDevice * pDevice );
void			printWidgetSizes();

void cbInitDrag( const WgEvent::Event* _pEvent, WgWidget * pWidget );
void cbDragWidget( const WgEvent::Event* _pEvent, WgWidget * pWidget );

void cbOpenModal( const WgEvent::Event* _pEvent, WgWidget * pWidget );
void cbCloseModal( const WgEvent::Event* _pEvent, WgWidget * pWidget );

void addResizablePanel( WgFlexPanel * pParent, WgWidget * pChild, WgEventHandler * pEventHandler );
void cbResize( const WgEvent::Event* _pEvent, void * _pFlexHook );


WgModalLayer * g_pModal = 0;

//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	// Dump some info
	
	printWidgetSizes();
	
	
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

	WgTextprop prop;

	prop.SetFont(pFont);
	prop.SetColor( WgColor::white );
	prop.SetSize(8);

	WgBase::SetDefaultTextprop( prop.Register() );


	WgRootPanel * pRoot = setupGUI( pGfxDevice );

	pRoot->FindWidget( WgCoord(10,10), WG_SEARCH_ACTION_TARGET );

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

//____ printWidgetSizes() _______________________________________________________

void printWidgetSizes()
{
	printf( "WgAnimPlayer: %d bytes\n", (int) sizeof(WgAnimPlayer) );
	printf( "WgButton: %d bytes\n", (int) sizeof(WgButton) );
	printf( "WgCheckBox: %d bytes\n", (int) sizeof(WgCheckBox) );
	printf( "WgLineEditor: %d bytes\n", (int) sizeof(WgLineEditor) );
	printf( "WgValueEditor: %d bytes\n", (int) sizeof(WgValueEditor) );
	printf( "WgFiller: %d bytes\n", (int) sizeof(WgFiller) );
	printf( "WgFlexPanel: %d bytes\n", (int) sizeof(WgFlexPanel) );
	printf( "WgFpsDisplay: %d bytes\n", (int) sizeof(WgFpsDisplay) );
	printf( "WgMenu: %d bytes\n", (int) sizeof(WgMenu) );
	printf( "WgMenuLayer: %d bytes\n", (int) sizeof(WgMenuLayer) );
	printf( "WgModalLayer: %d bytes\n", (int) sizeof(WgModalLayer) );
	printf( "WgImage: %d bytes\n", (int) sizeof(WgImage) );
	printf( "WgRadioButton: %d bytes\n", (int) sizeof(WgRadioButton) );
	printf( "WgRefreshButton: %d bytes\n", (int) sizeof(WgRefreshButton) );
	printf( "WgShaderCapsule: %d bytes\n", (int) sizeof(WgShaderCapsule) );
	printf( "WgScrollbar: %d bytes\n", (int) sizeof(WgScrollbar) );
	printf( "WgPackPanel: %d bytes\n", (int) sizeof(WgPackPanel) );
	printf( "WgStackPanel: %d bytes\n", (int) sizeof(WgStackPanel) );
//	printf( "WgTablePanel: %d bytes\n", (int) sizeof(WgTablePanel) );
	printf( "WgSizeCapsule: %d bytes\n", (int) sizeof(WgSizeCapsule) );
	printf( "WgTextDisplay: %d bytes\n", (int) sizeof(WgTextDisplay) );
	printf( "WgValueDisplay: %d bytes\n", (int) sizeof(WgValueDisplay) );
	printf( "WgListPanel: %d bytes\n", (int) sizeof(WgListPanel) );
	printf( "WgScrollPanel: %d bytes\n", (int) sizeof(WgScrollPanel) );
	printf( "\n" );
	printf( "WgFlexHook: %d bytes\n", (int) sizeof(WgFlexHook) );
	printf( "WgMenuHook: %d bytes\n", (int) sizeof(WgMenuHook) );
	printf( "WgModalHook: %d bytes\n", (int) sizeof(WgModalHook) );
	printf( "WgPackHook: %d bytes\n", (int) sizeof(WgPackHook) );
	printf( "WgStackHook: %d bytes\n", (int) sizeof(WgStackHook) );
//	printf( "WgTableHook: %d bytes\n", (int) sizeof(WgTableHook) );
	printf( "WgListHook: %d bytes\n", (int) sizeof(WgListHook) );
	printf( "WgScrollHook: %d bytes\n", (int) sizeof(WgScrollHook) );
}

//____ setupGUI() ______________________________________________________________

WgRootPanel * setupGUI( WgGfxDevice * pDevice )
{
	WgResDB * pDB = sdl_wglib::LoadStdWidgets( "../resources/blocks.png", WgSurfaceFactorySoft() );
	if( !pDB )
		return 0;

	WgRootPanel * pRoot = new WgRootPanel( pDevice );

	WgEventHandler * pEventHandler = pRoot->EventHandler();

	WgEventLogger * pEventLogger = new WgEventLogger( std::cout );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_POSITION );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSEBUTTON_REPEAT );
	pEventLogger->IgnoreEvent( WG_EVENT_BUTTON_PRESS );
//	pEventLogger->IgnoreAllEvents();
//	pEventLogger->LogMouseButtonEvents();
	pEventHandler->AddCallback( pEventLogger );


	// Load images

	WgSurface * pBackImg = sdl_wglib::LoadSurface("../resources/What-Goes-Up-3.bmp", WgSurfaceFactorySoft() );

	WgSurface * pFlagImg = sdl_wglib::LoadSurface("cb2.bmp", WgSurfaceFactorySoft() );

	WgSurface * pSplashImg = sdl_wglib::LoadSurface("../resources/splash.png", WgSurfaceFactorySoft() );

	WgSurface * pBigImg = sdl_wglib::LoadSurface("../resources/frog.jpg", WgSurfaceFactorySoft() );

	// MenuPanel

	WgMenuLayer * pMenuLayer = new WgMenuLayer();
	pRoot->SetChild( pMenuLayer );

	// Bottom Flex

	WgFlexPanel * pBottom = new WgFlexPanel();
	pMenuLayer->SetBase( pBottom );
	pBottom->SetSkin( WgColorSkin::Create( WgColor::black ) );

	// Main Flex

	WgFlexPanel * pFlex = new WgFlexPanel();
	pBottom->AddChild( pFlex, WG_NORTHWEST, WG_SOUTHEAST, WgBorders(10) );

	// Background

//	WgImage * pBackground = new WgImage();
//	pBackground->SetSource( pBackBlock );
//	WgFlexHook * pHook = pFlex->AddChild( pBackground );
//	pHook->SetAnchored( WG_NORTHWEST, WG_SOUTHEAST );

	WgBlockSkinPtr pBgSkin = WgBlockSkin::Create();
	pBgSkin->SetSurface(pBackImg);
	pBgSkin->SetBlockGeo( pBackImg->Size() );
	pBgSkin->SetStateBlock( WG_STATE_NORMAL, WgCoord(0,0) );
	pBgSkin->SetTiledCenter(true);
	pFlex->SetSkin( pBgSkin );


	//
/*
	{
		WgShaderCapsule * pShader = new WgShaderCapsule();
		pShader->SetBlendMode(WG_BLENDMODE_MULTIPLY);
		pShader->SetColor( WgColor(0xFF0000FF) );

		WgStackPanel * pStack = new WgStackPanel();
		pShader->SetChild( pStack );

		WgImage * pBg = (WgImage*) pDB->CloneWidget( "plate" );
		pStack->AddChild( pBg );

		WgShaderCapsule * pShader2 = new WgShaderCapsule();
		pShader2->SetBlendMode(WG_BLENDMODE_ADD);
		pShader2->SetColor( WgColor(0xFFFFFFFF) );
		WgStackHook * pHook = pStack->AddChild( pShader2 );
		pHook->SetSizePolicy( WgStackHook::SCALE );
		pHook->SetOrigo( WG_CENTER );
		pHook->SetBorders( WgBorders(2) );


		WgImage * pSplash= new WgImage();
		pSplash->SetSource( pSplashBlock );
		pShader2->SetChild( pSplash );

		addResizablePanel( pFlex, pShader, pEventHandler );
	}
*/

/*
	{
		WgStackPanel * pStack = new WgStackPanel();

		WgImage * pBg = (WgImage*) pDB->CloneWidget( "plate" );
		pStack->AddChild( pBg );

		WgListPanel * pVBox = new WgListPanel();
		WgStackHook * pHook = pStack->AddChild( pVBox );
		pHook->SetPadding( WgBorders(4) );

		for( int i = 0 ; i < 10 ; i++ )
		{
			WgValueDisplay * pV = new WgValueDisplay();
			pV->SetValue( 1000 + i );
			pVBox->AddChild( pV );
		}
		

//		WgValueEditor * pValue = new WgValueEditor();
//		pVBox->AddChild( pValue );

//		WgScrollbar * pScrollbar = (WgScrollbar*) pDB->CloneWidget( "hscrollbar" );
//		pScrollbar->SetScrollbarTarget(pValue);
//		pVBox->AddChild( pScrollbar );

		addResizablePanel( pFlex, pStack, pEventHandler );

//		pValue->SetValue( 100 );
//		pValue->GrabFocus();
	}
*/
/*
	{
		WgKnob * pKnob = new WgKnob();
		addResizablePanel( pFlex, pKnob, pEventHandler );
	}
*/
	// Test menus and MenuPanel.
/*
	{
		WgMenu * pSubMenu1_1 = (WgMenu*) pDB->CloneWidget( "menu" );
		pSubMenu1_1->AddItem( new WgMenuEntry( WgString("ENTRY 11"), WgString("Help text for entry 11"), WgBlocksetPtr(), 0 ));
		pSubMenu1_1->AddItem( new WgMenuEntry( WgString("ENTRY 12"), WgString("Help text for entry 12"), WgBlocksetPtr(), 0 ));
		
		WgMenu * pSubMenu1 = (WgMenu*) pDB->CloneWidget( "menu" );
		pSubMenu1->AddItem( new WgMenuEntry( WgString("ENTRY 7"), WgString("Help text for entry 7"), WgBlocksetPtr(), 0 ));
		pSubMenu1->AddItem( new WgMenuEntry( WgString("ENTRY 8"), WgString("Help text for entry 8"), WgBlocksetPtr(), 0 ));
		pSubMenu1->AddItem( new WgMenuSeparator() );
		pSubMenu1->AddItem( new WgMenuSubMenu( WgString("SUBMENU 1_1"), WgString("Help text for submenu 1_1"), WgBlocksetPtr(), 0, pSubMenu1_1 ) );

		WgMenu * pSubMenu2 = (WgMenu*) pDB->CloneWidget( "menu" );
		pSubMenu2->AddItem( new WgMenuEntry( WgString("ENTRY 9"), WgString("Help text for entry 9"), WgBlocksetPtr(), 0 ));
		pSubMenu2->AddItem( new WgMenuSeparator() );
		pSubMenu2->AddItem( new WgMenuEntry( WgString("ENTRY 10"), WgString("Help text for entry 10"), WgBlocksetPtr(), 0 ));

		WgMenu * pMenu = (WgMenu*) pDB->CloneWidget( "menu" );
		pMenu->AddItem( new WgMenuEntry( WgString("ENTRY 1"), WgString("Help text for entry 1"), WgBlocksetPtr(), 0 ));
		pMenu->AddItem( new WgMenuSeparator() );
		pMenu->AddItem( new WgMenuEntry( WgString("ENTRY 2"), WgString("Help text for entry 2"), WgBlocksetPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( WgString("ENTRY 3"), WgString("Help text for entry 3"), WgBlocksetPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( WgString("ENTRY 4"), WgString("Help text for entry 4"), WgBlocksetPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( WgString("ENTRY 5"), WgString("Help text for entry 5"), WgBlocksetPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( WgString("ENTRY 6"), WgString("Help text for entry 6"), WgBlocksetPtr(), 0 ));

		pMenu->AddItem( new WgMenuSubMenu( WgString("SUBMENU 1"), WgString("Help text for submenu 1"), WgBlocksetPtr(), 0, pSubMenu1 ) );
		pMenu->AddItem( new WgMenuSubMenu( WgString("SUBMENU 2"), WgString("Help text for submenu 2"), WgBlocksetPtr(), 0, pSubMenu2 ) );

		addResizablePanel( pFlex, pMenu, pEventHandler );
		pMenu->GrabFocus();

//		pMenuPanel->OpenMenu( pMenu, WgRect(10,10,100,10), WG_SOUTHWEST );
	}
*/
	// Test menubar and menus
/*
	{
		WgMenubar * pMenubar = WgCast<WgMenubar>(pDB->CloneWidget("menubar"));

		WgMenu * pFileMenu = (WgMenu*) pDB->CloneWidget( "menu" );
		pFileMenu->AddItem( new WgMenuEntry( WgString("Open..."), WgString("Open a file"), WgBlocksetPtr(), 0 ));
		pFileMenu->AddItem( new WgMenuEntry( WgString("Save"), WgString("Save the file"), WgBlocksetPtr(), 0 ));
		pFileMenu->AddItem( new WgMenuSeparator() );
		pFileMenu->AddItem( new WgMenuEntry( WgString("Quit"), WgString("Quit this program"), WgBlocksetPtr(), 0 ));
		pMenubar->AddMenu( "FILE", pFileMenu );
	
		WgMenu * pHelpMenu = (WgMenu*) pDB->CloneWidget( "menu" );
		pHelpMenu->AddItem( new WgMenuEntry( WgString("Help"), WgString("Get some help"), WgBlocksetPtr(), 0 ));
		pHelpMenu->AddItem( new WgMenuSeparator() );
		pHelpMenu->AddItem( new WgMenuEntry( WgString("About"), WgString("About this program"), WgBlocksetPtr(), 0 ));
		pMenubar->AddMenu( "HELP", pHelpMenu );

		addResizablePanel( pFlex, pMenubar, pEventHandler );
	}
*/
/*
	// Test oscilloscope

	{
		WgSurface * pImg = sdl_wglib::LoadSurface("../resources/blocks.png", WgSurfaceFactorySoft() );

		WgBlocksetPtr pMarkerBlock = WgBlockset::CreateFromRect( pImg, WgRect(1,120,8,8) );

		WgOscilloscope * pOsc = new WgOscilloscope();

		pOsc->SetBackground( WgColorSkin::Create( WgColor(0,0,96) ) );

		float grid[] = {-1.f,-0.5f,0.f,0.5f,1.f};
		pOsc->SetHGridLines( 5, grid );
		pOsc->SetVGridLines( 5, grid );
		pOsc->SetGridColor( WgColor(64,64,64) );

		pOsc->SetMarkerGfx( pMarkerBlock );
		pOsc->AddMarker( 30, 0.f );


		float points[256];

		for( int i = 0 ; i < 256 ; i++ )
		{
			points[i] = (float) sin(i/25.f)*0.90f;
		}

		pOsc->SetLinePoints(256,points);


		addResizablePanel( pFlex, pOsc, pEventHandler );
	}
*/
	// Test combobox
/*
	{
		WgCombobox * pCombobox = WgCast<WgCombobox>(pDB->CloneWidget("combobox"));

		WgMenu * pComboMenu = (WgMenu*) pDB->CloneWidget( "menu" );
		pComboMenu->AddItem( new WgMenuEntry( 1, WgString("Alfa"), WgString("First"), WgBlocksetPtr(), 0 ) );
		pComboMenu->AddItem( new WgMenuEntry( 2, WgString("Beta"), WgString("Second"), WgBlocksetPtr(), 0 ) );
		pCombobox->SetMenu( pComboMenu );

		addResizablePanel( pFlex, pCombobox, pEventHandler );

	}
*/

	// Test view
/*
	{
		WgScrollPanel * pView = (WgScrollPanel*) pDB->CloneWidget( "view" );

		WgImage * pImage = new WgImage();
		pImage->SetSource( pBigBlock );
		pView->SetContent( pImage );
		pView->SetOverlayScrollbars(true);
		((WgPanelHook*)pView->HScrollbar()->Hook())->SetPadding( 10 );
		((WgPanelHook*)pView->VScrollbar()->Hook())->SetPadding( 10 );

		((WgPanelHook*)pView->Content()->Hook())->SetPadding( 50 );

		addResizablePanel( pFlex, pView, pEventHandler );
	}
*/

	// Test overlapping non-visible
/*
	{
		WgFiller * pCovered = new WgFiller();
		pCovered->SetColors( WgColorset::Create( WgColor::aliceblue ) );
		pFlex->AddChild( pCovered, WgRect( 10,10,20,20 ) );

		g_pCovered = pCovered;

		WgFiller * pCovering = new WgFiller();
		pCovering->SetColors( WgColorset::Create( WgColor::wheat ) );
		WgFlexHook * pHook = pFlex->AddChild( pCovering, WgRect( 0,0,50,50 ) );
		pHook->SetVisible(false);
	}
*/

	// Modal panel
/*
	g_pModal = new WgModalLayer();
	g_pModal->SetBase( pFlex );

	pRoot->SetChild(g_pModal);


	// Modal button

	WgButton * pModalButton = (WgButton*) pDB->CloneWidget( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonClick(pModalButton, 1), cbCloseModal, pModalButton );

	//

	WgButton * pButton = (WgButton*) pDB->CloneWidget( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pButton, 1), cbOpenModal, pModalButton );

	WgFlexHook * pHook = pFlex->AddChild( pButton, WgRect(0,0,100,100), WG_NORTHWEST );
*/
	//
/*
	WgImage * pFlag1= new WgImage();
	pFlag1->SetSource( pSplashBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pFlag1, 1), cbInitDrag, pFlag1 );

	WgFlexHook * pHook = pFlex->AddChild( pFlag1, WgCoord(0,0), WG_CENTER );



	WgImage * pFlag2= new WgImage();
	pFlag2->SetSource( pFlagBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pFlag1, 1), cbDragWidget, pFlag1 );

	pHook = pFlex->AddChild( pFlag2, WgCoord(100,100), WG_CENTER );

	//
*/

	WgListPanel * pVBox = new WgListPanel();
//	pFlex->AddChild( pVBox, WgCoord(50,50), WG_NORTHWEST );


//	WgImage * pFlag3 = new WgImage();
//	pFlag3->SetSource( pFlagBlock );
//	WgImage * pFlag4= new WgImage();
//	pFlag4->SetSource( pFlagBlock );

	WgButton * pButton2 = (WgButton*) pDB->CloneWidget( "button" );
	pButton2->SetText( "BUTTON TEXT" );
	pVBox->AddChild(pButton2);

//	pVBox->AddChild(pFlag3);
//	pVBox->AddChild(pFlag4);


//	WgSizeCapsule * pSizeCapsule = new WgSizeCapsule();
//	pSizeCapsule->SetMaxSize( WgSize(150,600) );
//	pSizeCapsule->SetChild( pVBox );
	WgFlexHook * p = pFlex->AddChild( pVBox, WgCoord(50,50), WG_NORTHWEST );
	p->SetSizePolicy( WG_BOUND, WG_DEFAULT );
	p->SetSize( WgSize(150,10 ) );

//	WgTextDisplay * pText1 = new WgTextDisplay();
//	pText1->SetText("TEXTA1");
//	pText1->SetEditMode(WG_TEXT_EDITABLE);
//	pVBox->AddChild(pText1);

	WgTextDisplay * pText2 = new WgTextDisplay();
	pText2->SetText("TEXTB234ABC sajfas kjfaljsras kjasdfkasd kajfd fkajfa fkdjfa dfasfda asdkfj Hej");
	pText2->SetEditMode(WG_TEXT_EDITABLE);
	pVBox->AddChild(pText2);

//	pText1->GrabFocus();

	// Radiobuttons test

//	pVBox->AddChild( pDB->CloneWidget( "radiobutton" ) );
//	pVBox->AddChild( pDB->CloneWidget( "radiobutton" ) );

//	pFlex->AddChild( pDB->CloneWidget( "radiobutton" ) );
//	pFlex->AddChild( pDB->CloneWidget( "radiobutton" ) );

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
//				if (event.key.keysym.sym == SDLK_ESCAPE)
//					return false;
			}
		}
		sdl_wglib::TranslateEvent( event );
	}

	sdl_wglib::EndEvents();

	return true;
}



WgCoord dragStartPos;

//____ cbInitDrag() ___________________________________________________________

void cbInitDrag( const WgEvent::Event* _pEvent, WgWidget * pWidget )
{
	WgFlexHook * pHook = static_cast<WgFlexHook*>(pWidget->Hook());


	dragStartPos = pHook->FloatOfs();
	printf( "DRAG START!\n" );
}

//____ cbDragWidget() __________________________________________________________

void cbDragWidget( const WgEvent::Event* _pEvent, WgWidget * pWidget )
{
	if( _pEvent->Type() != WG_EVENT_MOUSEBUTTON_DRAG || !pWidget->Parent() )
		return;

	const WgEvent::MouseButtonDrag* pEvent = static_cast<const WgEvent::MouseButtonDrag*>(_pEvent);

	WgCoord	dragDistance = pEvent->DraggedTotal();

	WgCoord	ofs = dragStartPos + dragDistance;

//	printf( "AccDistance: %d, %d\n", dragDistance.x, dragDistance.y );
	printf( "ofs: %d, %d   start: %d %d   distance: %d, %d\n", ofs.x, ofs.y, dragStartPos.x, dragStartPos.y, dragDistance.x, dragDistance.y );

	WgFlexHook * pHook = static_cast<WgFlexHook*>(pWidget->Hook());
	pHook->SetOfs(dragStartPos+dragDistance);
}

//____ cbOpenModal() __________________________________________________________

void cbOpenModal( const WgEvent::Event* _pEvent, WgWidget * pWidget )
{
	g_pModal->AddModal( pWidget, WgCoord(), WG_SOUTHEAST );
}

//____ cbCloseModal() __________________________________________________________

void cbCloseModal( const WgEvent::Event* _pEvent, WgWidget * pWidget )
{
	g_pModal->ReleaseChild(pWidget);
}

//____ cbResizeWidget() _________________________________________________________

void cbResize( const WgEvent::Event* _pEvent, void * _pFlexHook )
{
	WgFlexHook * pHook = static_cast<WgFlexHook*>(_pFlexHook);
	const WgEvent::MouseButtonDrag* pEvent = static_cast<const WgEvent::MouseButtonDrag*>(_pEvent);

	WgCoord dragged = pEvent->DraggedNow();

	pHook->SetSize( pHook->Size() + WgSize(dragged.x,dragged.y) );
}



//____ addResizablePanel() _________________________________________________

void addResizablePanel( WgFlexPanel * pParent, WgWidget * pChild, WgEventHandler * pEventHandler )
{
	WgHook * pHook = pParent->AddChild( pChild );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pChild, 2), cbResize, pHook );


	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pChild, 3), cbInitDrag, pChild );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pChild, 3), cbDragWidget, pChild );
}





