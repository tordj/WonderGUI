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
WgRootPanel_p	setupGUI( const WgGfxDevice_p& pDevice );
void			printWidgetSizes();

void cbInitDrag( const WgEventPtr& _pEvent, const WgObject_p& pWidget );
void cbDragWidget( const WgEventPtr& _pEvent, const WgObject_p& pWidget );

void cbOpenModal( const WgEventPtr& _pEvent, const WgObject_p& pWidget );
void cbCloseModal( const WgEventPtr& _pEvent, const WgObject_p& pWidget );

void addResizablePanel( const WgFlexPanel_p& pParent, const WgWidget_p& pChild, const WgEventHandlerPtr& pEventHandler );
void cbResize( const WgEventPtr& _pEvent, const WgObject_p& pWidget );
void cbIncreaseEntryPadding( const WgEventPtr& _pEvent, const WgObject_p& pWidget );
void cbAddEntryText( const WgEventPtr& _pEvent, const WgObject_p& pWidget );

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

	WgBase::init();
	sdl_wglib::MapKeys();

//	WgBase::initFreeType();

	// Setup gfxdevice and gui

	WgSoftSurface_p pCanvas = WgSoftSurface::create( WgSize(640,480), WG_PIXEL_ARGB_8, (unsigned char *) pScreen->pixels, pScreen->pitch, 0 );
	WgSoftGfxDevice_p pGfxDevice = WgSoftGfxDevice::create( pCanvas );
	pGfxDevice->setBilinearFiltering( true );

	// Load TTF-font
/*
	WgVectorGlyphs::setSurfaceFactory( new WgSDLSurfaceFactory() );

	char	ttfname[] = { "a.ttf" };

	int size = fileSize( ttfname );
	char * pFontFile = (char*) loadFile( ttfname );
	WgVectorGlyphs_p pGlyphs = WgVectorGlyphs::create( pFontFile , size, 0 );

	WgFont_p pFont = WgFont::create();
	pFont->setDefaultVectorGlyphs( pGlyphs );
*/
	// Load bitmap font

	WgFont_p pFont = sdl_wglib::LoadBitmapFont( "../resources/anuvverbubbla_8x8.png", "../resources/anuvverbubbla_8x8.fnt", WgSoftSurfaceFactory::create() );

	// Load and setup cursor

	WgSurface_p pCursorImg = sdl_wglib::loadSurface("../resources/cursors.png", WgSoftSurfaceFactory::create() );

	WgGfxAnim_p pCursorEOL = WgGfxAnim::create();
	pCursorEOL->setSize( WgSize(8,8) );
	pCursorEOL->addFrames(pCursorImg, WgCoord(0,0), WgSize(2,1), 200 );
	pCursorEOL->setPlayMode( WG_FORWARD_LOOPING );

	WgGfxAnim_p pCursorINS = WgGfxAnim::create();
	pCursorINS->setSize( WgSize(8,8) );
	pCursorINS->addFrames( pCursorImg, WgCoord(0,8), WgSize(2,1), 200 );
	pCursorINS->setPlayMode( WG_FORWARD_LOOPING );

	WgCursorPtr pCursor = WgCursor::create();
	pCursor->setAnim(WgCursor::EOL, pCursorEOL);
	pCursor->setAnim(WgCursor::INS, pCursorINS);
	pCursor->setAnim(WgCursor::OVR, pCursorEOL);
	pCursor->setBearing(WgCursor::EOL, WgCoord(0,-8));
	pCursor->setBearing(WgCursor::INS, WgCoord(0,-8));
	pCursor->setBearing(WgCursor::OVR, WgCoord(0,-8));

	WgBase::setDefaultCursor( pCursor );

	// Set default textprop

	WgTextprop prop;

	prop.setFont(pFont);
	prop.setColor( WgColor::white );
	prop.setSize(8);

	WgBase::setDefaultTextprop( prop.register() );


	WgRootPanel_p pRoot = setupGUI( pGfxDevice );

	pRoot->findWidget( WgCoord(10,10), WG_SEARCH_ACTION_TARGET );

   // program main loop

    while (eventLoop( pRoot->EventHandler() ))
    {

		// GET DIRTY RECTS

		int nDirtyRects;
		SDL_Rect	dirtyRects[100];

		if( pRoot->nbDirtyRects() <= 100 )
		{
			nDirtyRects = pRoot->nbDirtyRects();
			for( int i = 0 ; i < nDirtyRects ; i++ )
			{
				const WgRect * pR = pRoot->firstDirtyRect() + i;

				dirtyRects[i].x = pR->x;
				dirtyRects[i].y = pR->y;
				dirtyRects[i].w = pR->w;
				dirtyRects[i].h = pR->h;
			}
		}
		else
		{
			nDirtyRects = 1;

			const WgRect r = pRoot->geo();

			dirtyRects[0].x = r.x;
			dirtyRects[0].y = r.y;
			dirtyRects[0].w = r.w;
			dirtyRects[0].h = r.h;
		}


        // DRAWING STARTS HERE

		SDL_LockSurface( pScreen );
		pRoot->render();
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

	WgBase::exit();

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

int sortWidgetId( const WgWidget * p1, const WgWidget * p2 )
{
	return p1->id() - p2->id();
}


//____ setupGUI() ______________________________________________________________

WgRootPanel_p setupGUI( const WgGfxDevice_p& pDevice )
{
	WgResDB_p pDB = sdl_wglib::LoadStdWidgets( "../resources/blocks.png", WgSoftSurfaceFactory::create() );
	if( !pDB )
		return 0;

	WgRootPanel_p pRoot = WgRootPanel::create( pDevice );

	WgEventHandlerPtr pEventHandler = pRoot->EventHandler();

	WgEventLogger * pEventLogger = new WgEventLogger( std::cout );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_POSITION );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_REPEAT );
	pEventLogger->IgnoreEvent( WG_EVENT_MOUSE_PRESS );
	pEventLogger->IgnoreAllEvents();
	pEventLogger->LogMouseButtonEvents();
	pEventHandler->AddCallback( pEventLogger );


	// Load images

	WgSoftSurfaceFactory_p pFactory = WgSoftSurfaceFactory::create();

	WgSurface_p pBackImg = sdl_wglib::loadSurface("../resources/What-Goes-Up-3.bmp", pFactory );

	WgSurface_p pFlagImg = sdl_wglib::loadSurface("cb2.bmp", pFactory );

	WgSurface_p pSplashImg = sdl_wglib::loadSurface("../resources/splash.png", pFactory );

	WgSurface_p pBigImg = sdl_wglib::loadSurface("../resources/frog.jpg", pFactory );

	// MenuPanel

	WgPopupLayer_p pPopupLayer = WgPopupLayer::create();
	pRoot->setWidget( pPopupLayer );

	// Bottom Flex

	WgFlexPanel_p pBottom = WgFlexPanel::create();
	pPopupLayer->setBaseWidget( pBottom );
	pBottom->setSkin( WgColorSkin::create( WgColor::black ) );

	// Main Flex

	WgFlexPanel_p pFlex = WgFlexPanel::create();
	pBottom->addWidget( pFlex, WG_NORTHWEST, WgCoord(), WG_SOUTHEAST, WgCoord(), WgBorders(10) );

	// Background

//	WgImage * pBackground = new WgImage();
//	pBackground->SetSource( pBackBlock );
//	WgFlexHook * pHook = pFlex->addWidget( pBackground );
//	pHook->SetAnchored( WG_NORTHWEST, WG_SOUTHEAST );

	WgBlockSkin_p pBgSkin = WgBlockSkin::create();
	pBgSkin->setSurface(pBackImg);
	pBgSkin->setBlockGeo( pBackImg->size() );
	pBgSkin->setStateBlock( WG_STATE_NORMAL, WgCoord(0,0) );
	pBgSkin->setTiledCenter(true);
	pFlex->setSkin( pBgSkin );


	//
/*
	{
		WgShaderCapsule * pShader = new WgShaderCapsule();
		pShader->setBlendMode(WG_BLENDMODE_MULTIPLY);
		pShader->setColor( WgColor(0xFF0000FF) );

		WgStackPanel * pStack = new WgStackPanel();
		pShader->setWidget( pStack );

		WgImage * pBg = (WgImage*) pDB->cloneWidget( "plate" );
		pStack->addWidget( pBg );

		WgShaderCapsule * pShader2 = new WgShaderCapsule();
		pShader2->setBlendMode(WG_BLENDMODE_ADD);
		pShader2->setColor( WgColor(0xFFFFFFFF) );
		WgStackHook * pHook = pStack->addWidget( pShader2 );
		pHook->setSizePolicy( WgStackHook::SCALE );
		pHook->setOrigo( WG_CENTER );
		pHook->SetBorders( WgBorders(2) );


		WgImage * pSplash= new WgImage();
		pSplash->SetSource( pSplashBlock );
		pShader2->setWidget( pSplash );

		addResizablePanel( pFlex, pShader, pEventHandler );
	}
*/

/*
	{
		WgStackPanel * pStack = new WgStackPanel();

		WgImage * pBg = (WgImage*) pDB->cloneWidget( "plate" );
		pStack->addWidget( pBg );

		WgListPanel * pVBox = new WgListPanel();
		WgStackHook * pHook = pStack->addWidget( pVBox );
		pHook->setPadding( WgBorders(4) );

		for( int i = 0 ; i < 10 ; i++ )
		{
			WgValueDisplay * pV = new WgValueDisplay();
			pV->setValue( 1000 + i );
			pVBox->addWidget( pV );
		}
		

//		WgValueEditor * pValue = new WgValueEditor();
//		pVBox->addWidget( pValue );

//		WgScrollbar * pScrollbar = (WgScrollbar*) pDB->cloneWidget( "hscrollbar" );
//		pScrollbar->setScrollbarTarget(pValue);
//		pVBox->addWidget( pScrollbar );

		addResizablePanel( pFlex, pStack, pEventHandler );

//		pValue->setValue( 100 );
//		pValue->grabFocus();
	}
*/
/*
	{
		WgKnob * pKnob = new WgKnob();
		addResizablePanel( pFlex, pKnob, pEventHandler );
	}
*/

	// Test PackList

	{
		WgPackList_p pList = WgPackList::create();
		WgSkin_p pPlate = pDB->getSkin("plate");

		pList->setSkin( pPlate );
		pList->setEntrySkin( pDB->getSkin("listentry"));
		pList->setLassoSkin( WgBoxSkin::create( WgColor(0,0,255,64), WgBorders(1), WgColor(0,0,255,128)) );
		pList->setSelectMode( WG_SELECT_MULTI );
//		pList->setOrientation( WG_HORIZONTAL );

		pList->header.setSkin( pDB->getSkin("plate") );
		pList->header.label.set( "LABEL" );
		pList->header.arrow.set( pDB->getSkin("sortarrow"), WG_EAST, WgBorders(0,0,0,2) );
		pList->header.icon.set( pDB->getSkin("smiley"), WG_WEST, WgBorders(0,2,0,0) );

		pList->setSortFunction( sortWidgetId );

		for( int i = 0 ; i < 20 ; i++ )
		{
			int id = rand() % 100;
/*			WgPackPanel_p pEntry = WgPackPanel::create();
			pEntry->setId(id);
*/


			WgTextDisplay_p pLabel = WgTextDisplay::create();
			char str[40];
			sprintf( str, "ENTRY %d", id );
			pLabel->text()->set( str );
			pLabel->text()->setAlignment(WG_CENTER);
			pLabel->text()->setWrap(true);
			pLabel->setMarkOpacity(0);
			pLabel->setId(id);
/*			pEntry->addWidget(pLabel);
			
			WgButton_p pButton = WgButton::cast(pDB->cloneWidget("button"));
			pButton->Label()->set( "PUSH" );
			pEntry->addWidget(pButton);
*/
//			pList->addWidget( pEntry );
			pList->insertWidgetSorted( pLabel );
		}


		WgScrollPanel_p pWindow = WgScrollPanel::cast(pDB->getWidget("view"));
		pWindow->setContent( pList );
//		pWindow->setContentSizePolicy( WgSizePolicy::WG_DEFAULT, WgSizePolicy::WG_BOUND );
		pWindow->setContentSizePolicy( WgSizePolicy::WG_BOUND, WgSizePolicy::WG_DEFAULT );

//		pFlex->addWidget(pWindow, WgRect( 50,50,200,200) );
		pFlex->addWidget(pWindow, WgCoord(50,50) );

		WgPackPanel_p pButtonPanel = WgPackPanel::create();
		pButtonPanel->setOrientation( WG_VERTICAL );
		pFlex->addWidget(pButtonPanel, WgCoord(300,50) );


		WgButton_p pButton = WgButton::cast(pDB->cloneWidget("button"));
		pButton->Label()->set("ADD ENTRY PADDING");
		pButtonPanel->addWidget(pButton );
		pRoot->EventHandler()->AddCallback( WgEventFilter::select(), pButton, cbIncreaseEntryPadding, pList );

		WgButton_p pButton2 = WgButton::cast(pDB->cloneWidget("button"));
		pButton2->Label()->set("ADD ENTRY");
		pButtonPanel->addWidget(pButton2 );
		pRoot->EventHandler()->AddCallback( WgEventFilter::select(), pButton2, cbAddEntryText, pList->firstWidget() );

	}
	
	// Test menus and MenuPanel.
/*
	{
		WgMenu_p pSubMenu1_1 = WgMenu::cast(pDB->cloneWidget( "menu" ));
		pSubMenu1_1->addItem( new WgMenuEntry( 1, WgString("ENTRY 11"), WgString("Help text for entry 11"), WgSkin_p(), 0 ));
		pSubMenu1_1->addItem( new WgMenuEntry( 2, WgString("ENTRY 12"), WgString("Help text for entry 12"), WgSkin_p(), 0 ));
		
		WgMenu_p pSubMenu1 = WgMenu::cast(pDB->cloneWidget( "menu" ));
		pSubMenu1->addItem( new WgMenuEntry( 3, WgString("ENTRY 7"), WgString("Help text for entry 7"), WgSkin_p(), 0 ));
		pSubMenu1->addItem( new WgMenuEntry( 4, WgString("ENTRY 8"), WgString("Help text for entry 8"), WgSkin_p(), 0 ));
		pSubMenu1->addItem( new WgMenuSeparator() );
		pSubMenu1->addItem( new WgMenuSubMenu( 5, WgString("SUBMENU 1_1"), WgString("Help text for submenu 1_1"), WgSkin_p(), 0, pSubMenu1_1.GetRealPtr() ) );

		WgMenu_p pSubMenu2 = WgMenu::cast(pDB->cloneWidget( "menu" ));
		pSubMenu2->addItem( new WgMenuEntry( 6, WgString("ENTRY 9"), WgString("Help text for entry 9"), WgSkin_p(), 0 ));
		pSubMenu2->addItem( new WgMenuSeparator() );
		pSubMenu2->addItem( new WgMenuEntry( 7, WgString("ENTRY 10"), WgString("Help text for entry 10"), WgSkin_p(), 0 ));

		WgMenu_p pMenu = WgMenu::cast(pDB->cloneWidget( "menu" ));
		pMenu->addItem( new WgMenuEntry( 8, WgString("ENTRY 1"), WgString("Help text for entry 1"), WgSkin_p(), 0 ));
		pMenu->addItem( new WgMenuSeparator() );
		pMenu->addItem( new WgMenuEntry( 9, WgString("ENTRY 2"), WgString("Help text for entry 2"), WgSkin_p(), 0 ));
		pMenu->addItem( new WgMenuEntry( 10, WgString("ENTRY 3"), WgString("Help text for entry 3"), WgSkin_p(), 0 ));
		pMenu->addItem( new WgMenuEntry( 11, WgString("ENTRY 4"), WgString("Help text for entry 4"), WgSkin_p(), 0 ));
		pMenu->addItem( new WgMenuEntry( 12, WgString("ENTRY 5"), WgString("Help text for entry 5"), WgSkin_p(), 0 ));
		pMenu->addItem( new WgMenuEntry( 13, WgString("ENTRY 6"), WgString("Help text for entry 6"), WgSkin_p(), 0 ));

		pMenu->addItem( new WgMenuSubMenu( 14, WgString("SUBMENU 1"), WgString("Help text for submenu 1"), WgSkin_p(), 0, pSubMenu1.GetRealPtr() ) );
		pMenu->addItem( new WgMenuSubMenu( 15, WgString("SUBMENU 2"), WgString("Help text for submenu 2"), WgSkin_p(), 0, pSubMenu2.GetRealPtr() ) );

		pFlex->addWidget(pMenu);

//		addResizablePanel( pFlex, pMenu, pEventHandler );
		pMenu->grabFocus();

//		pMenuPanel->OpenMenu( pMenu, WgRect(10,10,100,10), WG_SOUTHWEST );

	}
*/
	// Test menubar and menus
/*
	{
		WgMenubar * pMenubar = WgCast<WgMenubar>(pDB->cloneWidget("menubar"));

		WgMenu * pFileMenu = (WgMenu*) pDB->cloneWidget( "menu" );
		pFileMenu->addItem( new WgMenuEntry( WgString("Open..."), WgString("Open a file"), WgBlocksetPtr(), 0 ));
		pFileMenu->addItem( new WgMenuEntry( WgString("Save"), WgString("Save the file"), WgBlocksetPtr(), 0 ));
		pFileMenu->addItem( new WgMenuSeparator() );
		pFileMenu->addItem( new WgMenuEntry( WgString("Quit"), WgString("Quit this program"), WgBlocksetPtr(), 0 ));
		pMenubar->addMenu( "FILE", pFileMenu );
	
		WgMenu * pHelpMenu = (WgMenu*) pDB->cloneWidget( "menu" );
		pHelpMenu->addItem( new WgMenuEntry( WgString("Help"), WgString("Get some help"), WgBlocksetPtr(), 0 ));
		pHelpMenu->addItem( new WgMenuSeparator() );
		pHelpMenu->addItem( new WgMenuEntry( WgString("About"), WgString("About this program"), WgBlocksetPtr(), 0 ));
		pMenubar->addMenu( "HELP", pHelpMenu );

		addResizablePanel( pFlex, pMenubar, pEventHandler );
	}
*/
/*
	// Test oscilloscope

	{
		WgSurface * pImg = sdl_wglib::loadSurface("../resources/blocks.png", WgSoftSurfaceFactory() );

		WgBlocksetPtr pMarkerBlock = WgBlockset::CreateFromRect( pImg, WgRect(1,120,8,8) );

		WgOscilloscope * pOsc = new WgOscilloscope();

		pOsc->SetBackground( WgColorSkin::create( WgColor(0,0,96) ) );

		float grid[] = {-1.f,-0.5f,0.f,0.5f,1.f};
		pOsc->setHGridLines( 5, grid );
		pOsc->setVGridLines( 5, grid );
		pOsc->setGridColor( WgColor(64,64,64) );

		pOsc->SetMarkerGfx( pMarkerBlock );
		pOsc->addMarker( 30, 0.f );


		float points[256];

		for( int i = 0 ; i < 256 ; i++ )
		{
			points[i] = (float) sin(i/25.f)*0.90f;
		}

		pOsc->setLinePoints(256,points);


		addResizablePanel( pFlex, pOsc, pEventHandler );
	}
*/
	// Test combobox
/*
	{
		WgCombobox * pCombobox = WgCast<WgCombobox>(pDB->cloneWidget("combobox"));

		WgMenu * pComboMenu = (WgMenu*) pDB->cloneWidget( "menu" );
		pComboMenu->addItem( new WgMenuEntry( 1, WgString("Alfa"), WgString("First"), WgBlocksetPtr(), 0 ) );
		pComboMenu->addItem( new WgMenuEntry( 2, WgString("Beta"), WgString("Second"), WgBlocksetPtr(), 0 ) );
		pCombobox->setMenu( pComboMenu );

		addResizablePanel( pFlex, pCombobox, pEventHandler );

	}
*/

	// Test view
/*
	{
		WgScrollPanel * pView = (WgScrollPanel*) pDB->cloneWidget( "view" );

		WgImage * pImage = new WgImage();
		pImage->SetSource( pBigBlock );
		pView->setContent( pImage );
		pView->setOverlayScrollbars(true);
		((WgPanelHook*)pView->HScrollbar()->hook())->setPadding( 10 );
		((WgPanelHook*)pView->VScrollbar()->hook())->setPadding( 10 );

		((WgPanelHook*)pView->content()->hook())->setPadding( 50 );

		addResizablePanel( pFlex, pView, pEventHandler );
	}
*/

	// Test overlapping non-visible
/*
	{
		WgFiller * pCovered = new WgFiller();
		pCovered->setColors( WgColorset::create( WgColor::aliceblue ) );
		pFlex->addWidget( pCovered, WgRect( 10,10,20,20 ) );

		g_pCovered = pCovered;

		WgFiller * pCovering = new WgFiller();
		pCovering->setColors( WgColorset::create( WgColor::wheat ) );
		WgFlexHook * pHook = pFlex->addWidget( pCovering, WgRect( 0,0,50,50 ) );
		pHook->setVisible(false);
	}
*/

	// Modal panel
/*
	g_pModal = new WgModalLayer();
	g_pModal->SetBase( pFlex );

	pRoot->setWidget(g_pModal);


	// Modal button

	WgButton * pModalButton = (WgButton*) pDB->cloneWidget( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonClick(pModalButton, 1), cbCloseModal, pModalButton );

	//

	WgButton * pButton = (WgButton*) pDB->cloneWidget( "button" );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pButton, 1), cbOpenModal, pModalButton );

	WgFlexHook * pHook = pFlex->addWidget( pButton, WgRect(0,0,100,100), WG_NORTHWEST );
*/
	//
/*
	WgImage * pFlag1= new WgImage();
	pFlag1->SetSource( pSplashBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonPress(pFlag1, 1), cbInitDrag, pFlag1 );

	WgFlexHook * pHook = pFlex->addWidget( pFlag1, WgCoord(0,0), WG_CENTER );



	WgImage * pFlag2= new WgImage();
	pFlag2->SetSource( pFlagBlock );
	pEventHandler->AddCallback( WgEventFilter::MouseButtonDrag(pFlag1, 1), cbDragWidget, pFlag1 );

	pHook = pFlex->addWidget( pFlag2, WgCoord(100,100), WG_CENTER );

	//
*/

//	WgPackPanel_p pVBox = WgPackPanel::create();
//	pVBox->setOrientation( WG_VERTICAL );
//	pFlex->addWidget( pVBox, WgCoord(50,50), WG_NORTHWEST );


//	WgImage * pFlag3 = new WgImage();
//	pFlag3->SetSource( pFlagBlock );
//	WgImage * pFlag4= new WgImage();
//	pFlag4->SetSource( pFlagBlock );

//	WgButton_p pButton2 = WgButton::cast(pDB->cloneWidget( "button" ));
//	pButton2->Label()->set( "BUTTON TEXT" );
//	pVBox->addWidget(pButton2);

//	pVBox->addWidget(pFlag3);
//	pVBox->addWidget(pFlag4);


//	WgSizeCapsule * pSizeCapsule = new WgSizeCapsule();
//	pSizeCapsule->setMaxSize( WgSize(150,600) );
//	pSizeCapsule->setWidget( pVBox );
//	WgFlexHook * p = pFlex->addWidget( pVBox, WgCoord(50,50), WG_NORTHWEST );
//	p->setSizePolicy( WG_BOUND, WG_DEFAULT );
//	p->setSize( WgSize(150,10 ) );

//	WgTextDisplay * pText1 = new WgTextDisplay();
//	pText1->setText("TEXTA1");
//	pText1->setEditMode(WG_TEXT_EDITABLE);
//	pVBox->addWidget(pText1);

//	WgTextDisplay_p pText2 = WgTextDisplay::create();
//	pText2->text()->set("TEXTB234ABC sajfas kjfaljsras kjasdfkasd kajfd fkajfa fkdjfa dfasfda asdkfj Hej");
//	pText2->setEditMode(WG_TEXT_EDITABLE);
//	pVBox->addWidget(pText2);

//	pText1->grabFocus();

	// Radiobuttons test

//	pVBox->addWidget( pDB->cloneWidget( "radiobutton" ) );
//	pVBox->addWidget( pDB->cloneWidget( "radiobutton" ) );

//	pFlex->addWidget( pDB->cloneWidget( "radiobutton" ) );
//	pFlex->addWidget( pDB->cloneWidget( "radiobutton" ) );

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

void cbInitDrag( const WgEventPtr& _pEvent, const WgObject_p& pWidget )
{
	WgFlexHook * pHook = static_cast<WgFlexHook*>(WgWidget::cast(pWidget)->hook().GetRealPtr());


	dragStartPos = pHook->floatOfs();
	printf( "DRAG START!\n" );
}

//____ cbDragWidget() __________________________________________________________

void cbDragWidget( const WgEventPtr& _pEvent, const WgObject_p& pObject )
{
	WgWidget_p pWidget = WgWidget::cast(pObject);

	if( _pEvent->type() != WG_EVENT_MOUSE_DRAG || !pWidget->parent() )
		return;

	const WgMouseDragEventPtr pEvent = WgMouseDragEvent::cast(_pEvent);

	WgCoord	dragDistance = pEvent->draggedTotal();

	WgCoord	ofs = dragStartPos + dragDistance;

//	printf( "AccDistance: %d, %d\n", dragDistance.x, dragDistance.y );
	printf( "ofs: %d, %d   start: %d %d   distance: %d, %d\n", ofs.x, ofs.y, dragStartPos.x, dragStartPos.y, dragDistance.x, dragDistance.y );

	WgFlexHook * pHook = static_cast<WgFlexHook*>(pWidget->hook().GetRealPtr());
	pHook->setOfs(dragStartPos+dragDistance);
}

//____ cbOpenModal() __________________________________________________________

void cbOpenModal( const WgEventPtr& _pEvent, const WgObject_p& pWidget )
{
	g_pModal->addModalWidget( WgWidget::cast(pWidget), WgCoord(), WG_SOUTHEAST );
}

//____ cbCloseModal() __________________________________________________________

void cbCloseModal( const WgEventPtr& _pEvent, const WgObject_p& pWidget )
{
	g_pModal->removeWidget( WgWidget::cast(pWidget));
}

//____ cbResizeWidget() _________________________________________________________

void cbResize( const WgEventPtr& _pEvent, const WgObject_p& pWidget )
{
	WgFlexHook * pHook =   static_cast<WgFlexHook*>( WgWidget::cast(pWidget)->hook().GetRealPtr() );
	const WgMouseDragEventPtr pEvent = WgMouseDragEvent::cast(_pEvent);

	WgCoord dragged = pEvent->draggedNow();

	pHook->setSize( pHook->size() + WgSize(dragged.x,dragged.y) );
}

//____ cbIncreaseEntryPadding() _________________________________________________________

void cbIncreaseEntryPadding( const WgEventPtr& _pEvent, const WgObject_p& _pWidget )
{
	WgWidget_p pWidget = WgWidget::cast(_pWidget);

	WgExtendedSkin_p pSkin = WgExtendedSkin::cast(pWidget->skin());
		
	WgRect contentRect = pSkin->contentRect(WgRect(0,0,100,100), WG_STATE_NORMAL );
	WgBorders padding( contentRect.top(), 100 - contentRect.right(), 100 - contentRect.bottom(), contentRect.left() );

	padding.top++;
	padding.left++;
	pSkin->setContentPadding(padding);
	pWidget->refresh();

//	WgList_p pList = WgList::cast(pWidget);
//	pList->setEntrySkin(pSkin);

}

//____ cbAddEntryText() _________________________________________________________

void cbAddEntryText( const WgEventPtr& _pEvent, const WgObject_p& _pWidget )
{
	WgTextDisplay_p pWidget = WgTextDisplay::cast(_pWidget);
	pWidget->text()->append("**** **** **** **** **** **** ****");
}


//____ addResizablePanel() _________________________________________________

void addResizablePanel( const WgFlexPanel_p& pParent, const WgWidget_p& pChild, const WgEventHandlerPtr& pEventHandler )
{
	pEventHandler->AddCallback( WgEventFilter::mouseDrag(WG_BUTTON_MIDDLE), pChild, cbResize, pChild );

	pEventHandler->AddCallback( WgEventFilter::mousePress(WG_BUTTON_RIGHT), pChild, cbInitDrag, pChild );
	pEventHandler->AddCallback( WgEventFilter::mouseDrag(WG_BUTTON_RIGHT), pChild, cbDragWidget, pChild );
}





