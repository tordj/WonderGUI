#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


#include <wondergui.h>
#include <wg_bitmapglyphs.h>
#include <wg_vectorglyphs.h>
#include <wg_knob.h>
#include <iostream>

#include <wg_softsurface.h>
#include <wg_softsurfacefactory.h>
#include <wg_softgfxdevice.h>
#include <sdl_wglib.h>

extern std::ostream cout;

SDL_Surface *	initSDL( int w, int h );
bool			eventLoop( const WgEventHandlerPtr& pHandler );
WgRootPanelPtr	setupGUI( const WgGfxDevicePtr& pDevice );
void			printWidgetSizes();

void cbInitDrag( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget );
void cbDragWidget( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget );

void cbOpenModal( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget );
void cbCloseModal( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget );

void addResizablePanel( const WgFlexPanelPtr& pParent, const WgWidgetPtr& pChild, const WgEventHandlerPtr& pEventHandler );
void cbResize( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget );


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

	WgSoftSurfacePtr pCanvas = WgSoftSurface::Create( WgSize(640,480), WG_PIXEL_ARGB_8, (unsigned char *) pScreen->pixels, pScreen->pitch );
	WgSoftGfxDevicePtr pGfxDevice = WgSoftGfxDevice::Create( pCanvas );
	pGfxDevice->SetBilinearFiltering( true );

	// Load TTF-font
/*
	WgVectorGlyphs::SetSurfaceFactory( new WgSDLSurfaceFactory() );

	char	ttfname[] = { "a.ttf" };

	int size = fileSize( ttfname );
	char * pFontFile = (char*) loadFile( ttfname );
	WgVectorGlyphsPtr pGlyphs = WgVectorGlyphs::Create( pFontFile , size, 0 );

	WgFontPtr pFont = WgFont::Create();
	pFont->SetDefaultVectorGlyphs( pGlyphs );
*/
	// Load bitmap font

	WgFontPtr pFont = sdl_wglib::LoadBitmapFont( "../resources/anuvverbubbla_8x8.png", "../resources/anuvverbubbla_8x8.fnt", WgSoftSurfaceFactory::Create() );

	// Load and setup cursor

	WgSurfacePtr pCursorImg = sdl_wglib::LoadSurface("../resources/cursors.png", WgSoftSurfaceFactory::Create() );

	WgGfxAnimPtr pCursorEOL = WgGfxAnim::Create();
	pCursorEOL->SetSize( WgSize(8,8) );
	pCursorEOL->AddFrames(pCursorImg, WgCoord(0,0), WgSize(2,1), 200 );
	pCursorEOL->SetPlayMode( WG_FORWARD_LOOPING );

	WgGfxAnimPtr pCursorINS = WgGfxAnim::Create();
	pCursorINS->SetSize( WgSize(8,8) );
	pCursorINS->AddFrames( pCursorImg, WgCoord(0,8), WgSize(2,1), 200 );
	pCursorINS->SetPlayMode( WG_FORWARD_LOOPING );

	WgCursorPtr pCursor = WgCursor::Create();
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


	WgRootPanelPtr pRoot = setupGUI( pGfxDevice );

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

	g_pModal = 0;
	pRoot = 0;
	pGfxDevice = 0;

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
	printf( "WgPopupLayer: %d bytes\n", (int) sizeof(WgPopupLayer) );
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
//	printf( "WgListPanel: %d bytes\n", (int) sizeof(WgListPanel) );
	printf( "WgScrollPanel: %d bytes\n", (int) sizeof(WgScrollPanel) );
	printf( "\n" );
	printf( "WgFlexHook: %d bytes\n", (int) sizeof(WgFlexHook) );
	printf( "WgPopupHook: %d bytes\n", (int) sizeof(WgPopupHook) );
	printf( "WgModalHook: %d bytes\n", (int) sizeof(WgModalHook) );
	printf( "WgPackHook: %d bytes\n", (int) sizeof(WgPackHook) );
	printf( "WgStackHook: %d bytes\n", (int) sizeof(WgStackHook) );
//	printf( "WgTableHook: %d bytes\n", (int) sizeof(WgTableHook) );
//	printf( "WgListHook: %d bytes\n", (int) sizeof(WgListHook) );
	printf( "WgScrollHook: %d bytes\n", (int) sizeof(WgScrollHook) );
}

//____ setupGUI() ______________________________________________________________

WgRootPanelPtr setupGUI( const WgGfxDevicePtr& pDevice )
{
	WgResDBPtr pDB = sdl_wglib::LoadStdWidgets( "../resources/blocks.png", WgSoftSurfaceFactory::Create() );
	if( !pDB )
		return 0;

	WgRootPanelPtr pRoot = WgRootPanel::Create( pDevice );

	WgEventHandlerPtr pEventHandler = pRoot->EventHandler();

	WgEventLogger * pEventLogger = new WgEventLogger( std::cout );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_POSITION );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_REPEAT );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_PRESS );
	pEventLogger->IgnoreAllEvents();
	pEventLogger->LogMouseButtonEvents();
	pEventHandler->AddCallback( pEventLogger );


	// Load images

	WgSoftSurfaceFactoryPtr pFactory = WgSoftSurfaceFactory::Create();

	WgSurfacePtr pBackImg = sdl_wglib::LoadSurface("../resources/What-Goes-Up-3.bmp", pFactory );

	WgSurfacePtr pFlagImg = sdl_wglib::LoadSurface("cb2.bmp", pFactory );

	WgSurfacePtr pSplashImg = sdl_wglib::LoadSurface("../resources/splash.png", pFactory );

	WgSurfacePtr pBigImg = sdl_wglib::LoadSurface("../resources/frog.jpg", pFactory );

	// MenuPanel

	WgPopupLayerPtr pPopupLayer = WgPopupLayer::Create();
	pRoot->SetWidget( pPopupLayer );

	// Bottom Flex

	WgFlexPanelPtr pBottom = WgFlexPanel::Create();
	pPopupLayer->SetBaseWidget( pBottom );
	pBottom->SetSkin( WgColorSkin::Create( WgColor::black ) );

	// Main Flex

	WgFlexPanelPtr pFlex = WgFlexPanel::Create();
	pBottom->AddWidget( pFlex, WG_NORTHWEST, WgCoord(), WG_SOUTHEAST, WgCoord(), WgBorders(10) );

	// Background

//	WgImage * pBackground = new WgImage();
//	pBackground->SetSource( pBackBlock );
//	WgFlexHook * pHook = pFlex->AddWidget( pBackground );
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
		pShader->SetWidget( pStack );

		WgImage * pBg = (WgImage*) pDB->CloneWidget( "plate" );
		pStack->AddWidget( pBg );

		WgShaderCapsule * pShader2 = new WgShaderCapsule();
		pShader2->SetBlendMode(WG_BLENDMODE_ADD);
		pShader2->SetColor( WgColor(0xFFFFFFFF) );
		WgStackHook * pHook = pStack->AddWidget( pShader2 );
		pHook->SetSizePolicy( WgStackHook::SCALE );
		pHook->SetOrigo( WG_CENTER );
		pHook->SetBorders( WgBorders(2) );


		WgImage * pSplash= new WgImage();
		pSplash->SetSource( pSplashBlock );
		pShader2->SetWidget( pSplash );

		addResizablePanel( pFlex, pShader, pEventHandler );
	}
*/

/*
	{
		WgStackPanel * pStack = new WgStackPanel();

		WgImage * pBg = (WgImage*) pDB->CloneWidget( "plate" );
		pStack->AddWidget( pBg );

		WgListPanel * pVBox = new WgListPanel();
		WgStackHook * pHook = pStack->AddWidget( pVBox );
		pHook->SetPadding( WgBorders(4) );

		for( int i = 0 ; i < 10 ; i++ )
		{
			WgValueDisplay * pV = new WgValueDisplay();
			pV->SetValue( 1000 + i );
			pVBox->AddWidget( pV );
		}
		

//		WgValueEditor * pValue = new WgValueEditor();
//		pVBox->AddWidget( pValue );

//		WgScrollbar * pScrollbar = (WgScrollbar*) pDB->CloneWidget( "hscrollbar" );
//		pScrollbar->SetScrollbarTarget(pValue);
//		pVBox->AddWidget( pScrollbar );

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

	{
		WgMenuPtr pSubMenu1_1 = WgMenu::Cast(pDB->CloneWidget( "menu" ));
		pSubMenu1_1->AddItem( new WgMenuEntry( 1, WgString("ENTRY 11"), WgString("Help text for entry 11"), WgSkinPtr(), 0 ));
		pSubMenu1_1->AddItem( new WgMenuEntry( 2, WgString("ENTRY 12"), WgString("Help text for entry 12"), WgSkinPtr(), 0 ));
		
		WgMenuPtr pSubMenu1 = WgMenu::Cast(pDB->CloneWidget( "menu" ));
		pSubMenu1->AddItem( new WgMenuEntry( 3, WgString("ENTRY 7"), WgString("Help text for entry 7"), WgSkinPtr(), 0 ));
		pSubMenu1->AddItem( new WgMenuEntry( 4, WgString("ENTRY 8"), WgString("Help text for entry 8"), WgSkinPtr(), 0 ));
		pSubMenu1->AddItem( new WgMenuSeparator() );
		pSubMenu1->AddItem( new WgMenuSubMenu( 5, WgString("SUBMENU 1_1"), WgString("Help text for submenu 1_1"), WgSkinPtr(), 0, pSubMenu1_1.GetRealPtr() ) );

		WgMenuPtr pSubMenu2 = WgMenu::Cast(pDB->CloneWidget( "menu" ));
		pSubMenu2->AddItem( new WgMenuEntry( 6, WgString("ENTRY 9"), WgString("Help text for entry 9"), WgSkinPtr(), 0 ));
		pSubMenu2->AddItem( new WgMenuSeparator() );
		pSubMenu2->AddItem( new WgMenuEntry( 7, WgString("ENTRY 10"), WgString("Help text for entry 10"), WgSkinPtr(), 0 ));

		WgMenuPtr pMenu = WgMenu::Cast(pDB->CloneWidget( "menu" ));
		pMenu->AddItem( new WgMenuEntry( 8, WgString("ENTRY 1"), WgString("Help text for entry 1"), WgSkinPtr(), 0 ));
		pMenu->AddItem( new WgMenuSeparator() );
		pMenu->AddItem( new WgMenuEntry( 9, WgString("ENTRY 2"), WgString("Help text for entry 2"), WgSkinPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( 10, WgString("ENTRY 3"), WgString("Help text for entry 3"), WgSkinPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( 11, WgString("ENTRY 4"), WgString("Help text for entry 4"), WgSkinPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( 12, WgString("ENTRY 5"), WgString("Help text for entry 5"), WgSkinPtr(), 0 ));
		pMenu->AddItem( new WgMenuEntry( 13, WgString("ENTRY 6"), WgString("Help text for entry 6"), WgSkinPtr(), 0 ));

		pMenu->AddItem( new WgMenuSubMenu( 14, WgString("SUBMENU 1"), WgString("Help text for submenu 1"), WgSkinPtr(), 0, pSubMenu1.GetRealPtr() ) );
		pMenu->AddItem( new WgMenuSubMenu( 15, WgString("SUBMENU 2"), WgString("Help text for submenu 2"), WgSkinPtr(), 0, pSubMenu2.GetRealPtr() ) );

		pFlex->AddWidget(pMenu);

//		addResizablePanel( pFlex, pMenu, pEventHandler );
		pMenu->GrabFocus();

//		pMenuPanel->OpenMenu( pMenu, WgRect(10,10,100,10), WG_SOUTHWEST );

	}

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
		WgSurface * pImg = sdl_wglib::LoadSurface("../resources/blocks.png", WgSoftSurfaceFactory() );

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
		pFlex->AddWidget( pCovered, WgRect( 10,10,20,20 ) );

		g_pCovered = pCovered;

		WgFiller * pCovering = new WgFiller();
		pCovering->SetColors( WgColorset::Create( WgColor::wheat ) );
		WgFlexHook * pHook = pFlex->AddWidget( pCovering, WgRect( 0,0,50,50 ) );
		pHook->SetVisible(false);
	}
*/

	// Modal panel
/*
	g_pModal = new WgModalLayer();
	g_pModal->SetBase( pFlex );

	pRoot->SetWidget(g_pModal);


	// Modal button

	WgButton * pModalButton = (WgButton*) pDB->CloneWidget( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonClick(pModalButton, 1), cbCloseModal, pModalButton );

	//

	WgButton * pButton = (WgButton*) pDB->CloneWidget( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pButton, 1), cbOpenModal, pModalButton );

	WgFlexHook * pHook = pFlex->AddWidget( pButton, WgRect(0,0,100,100), WG_NORTHWEST );
*/
	//
/*
	WgImage * pFlag1= new WgImage();
	pFlag1->SetSource( pSplashBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pFlag1, 1), cbInitDrag, pFlag1 );

	WgFlexHook * pHook = pFlex->AddWidget( pFlag1, WgCoord(0,0), WG_CENTER );



	WgImage * pFlag2= new WgImage();
	pFlag2->SetSource( pFlagBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pFlag1, 1), cbDragWidget, pFlag1 );

	pHook = pFlex->AddWidget( pFlag2, WgCoord(100,100), WG_CENTER );

	//
*/

//	WgPackPanelPtr pVBox = WgPackPanel::Create();
//	pVBox->SetOrientation( WG_VERTICAL );
//	pFlex->AddWidget( pVBox, WgCoord(50,50), WG_NORTHWEST );


//	WgImage * pFlag3 = new WgImage();
//	pFlag3->SetSource( pFlagBlock );
//	WgImage * pFlag4= new WgImage();
//	pFlag4->SetSource( pFlagBlock );

//	WgButtonPtr pButton2 = WgButton::Cast(pDB->CloneWidget( "button" ));
//	pButton2->Label()->Set( "BUTTON TEXT" );
//	pVBox->AddWidget(pButton2);

//	pVBox->AddWidget(pFlag3);
//	pVBox->AddWidget(pFlag4);


//	WgSizeCapsule * pSizeCapsule = new WgSizeCapsule();
//	pSizeCapsule->SetMaxSize( WgSize(150,600) );
//	pSizeCapsule->SetWidget( pVBox );
//	WgFlexHook * p = pFlex->AddWidget( pVBox, WgCoord(50,50), WG_NORTHWEST );
//	p->SetSizePolicy( WG_BOUND, WG_DEFAULT );
//	p->SetSize( WgSize(150,10 ) );

//	WgTextDisplay * pText1 = new WgTextDisplay();
//	pText1->SetText("TEXTA1");
//	pText1->SetEditMode(WG_TEXT_EDITABLE);
//	pVBox->AddWidget(pText1);

//	WgTextDisplayPtr pText2 = WgTextDisplay::Create();
//	pText2->Text()->Set("TEXTB234ABC sajfas kjfaljsras kjasdfkasd kajfd fkajfa fkdjfa dfasfda asdkfj Hej");
//	pText2->SetEditMode(WG_TEXT_EDITABLE);
//	pVBox->AddWidget(pText2);

//	pText1->GrabFocus();

	// Radiobuttons test

//	pVBox->AddWidget( pDB->CloneWidget( "radiobutton" ) );
//	pVBox->AddWidget( pDB->CloneWidget( "radiobutton" ) );

//	pFlex->AddWidget( pDB->CloneWidget( "radiobutton" ) );
//	pFlex->AddWidget( pDB->CloneWidget( "radiobutton" ) );

//	pVBox->SetRadioGroup(true);

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

bool eventLoop( const WgEventHandlerPtr& pHandler )
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
		sdl_wglib::TranslateEvent( pHandler, event );
	}

	sdl_wglib::EndEvents( pHandler );

	return true;
}



WgCoord dragStartPos;

//____ cbInitDrag() ___________________________________________________________

void cbInitDrag( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget )
{
	WgFlexHook * pHook = static_cast<WgFlexHook*>(WgWidget::Cast(pWidget)->Hook().GetRealPtr());


	dragStartPos = pHook->FloatOfs();
	printf( "DRAG START!\n" );
}

//____ cbDragWidget() __________________________________________________________

void cbDragWidget( const WgEventPtr& _pEvent, const WgObjectPtr& pObject )
{
	WgWidgetPtr pWidget = WgWidget::Cast(pObject);

	if( _pEvent->Type() != WG_EVENT_MOUSE_DRAG || !pWidget->Parent() )
		return;

	const WgMouseDragEventPtr pEvent = WgMouseDragEvent::Cast(_pEvent);

	WgCoord	dragDistance = pEvent->DraggedTotal();

	WgCoord	ofs = dragStartPos + dragDistance;

//	printf( "AccDistance: %d, %d\n", dragDistance.x, dragDistance.y );
	printf( "ofs: %d, %d   start: %d %d   distance: %d, %d\n", ofs.x, ofs.y, dragStartPos.x, dragStartPos.y, dragDistance.x, dragDistance.y );

	WgFlexHook * pHook = static_cast<WgFlexHook*>(pWidget->Hook().GetRealPtr());
	pHook->SetOfs(dragStartPos+dragDistance);
}

//____ cbOpenModal() __________________________________________________________

void cbOpenModal( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget )
{
	g_pModal->AddModalWidget( WgWidget::Cast(pWidget), WgCoord(), WG_SOUTHEAST );
}

//____ cbCloseModal() __________________________________________________________

void cbCloseModal( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget )
{
	g_pModal->RemoveWidget( WgWidget::Cast(pWidget));
}

//____ cbResizeWidget() _________________________________________________________

void cbResize( const WgEventPtr& _pEvent, const WgObjectPtr& pWidget )
{
	WgFlexHook * pHook =   static_cast<WgFlexHook*>( WgWidget::Cast(pWidget)->Hook().GetRealPtr() );
	const WgMouseDragEventPtr pEvent = WgMouseDragEvent::Cast(_pEvent);

	WgCoord dragged = pEvent->DraggedNow();

	pHook->SetSize( pHook->Size() + WgSize(dragged.x,dragged.y) );
}



//____ addResizablePanel() _________________________________________________

void addResizablePanel( const WgFlexPanelPtr& pParent, const WgWidgetPtr& pChild, const WgEventHandlerPtr& pEventHandler )
{
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pChild, 2), cbResize, pChild );

	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pChild, 3), cbInitDrag, pChild );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pChild, 3), cbDragWidget, pChild );
}





