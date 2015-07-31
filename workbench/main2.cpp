
#include <cstdlib>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <wondergui.h>

#include <wg_softsurface.h>
#include <wg_softgfxdevice.h>

using namespace wg;

void 			translateEvents( const WgInputHandler_p& pInput, const WgRootPanel_p& pRoot );
WgMouseButton 	translateMouseButton( Uint8 button );
void 			updateWindowRects( const WgRootPanel_p& pRoot, SDL_Window * pWindow );
void 			myButtonClickCallback( const WgMsg_p& pMsg );
void * 			loadFile( const char * pPath );

void addResizablePanel( const WgFlexPanel_p& pParent, const WgWidget_p& pChild, const WgMsgRouter_p& pMsgRouter );


bool	bQuit = false;


void freeSDLSurfCallback( void * pSDLSurf )
{
	SDL_FreeSurface( (SDL_Surface*) pSDLSurf );
}

WgInputHandler * pDebug;

//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 320, height = 240;
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, 0);

	SDL_Surface * pWinSurf = SDL_GetWindowSurface( pWin );

	IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG );

	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------

	WgBase::init();

	WgInputHandler_p pInput = WgInputHandler::create();
	pDebug = pInput.rawPtr();
	
	WgPixelType type = WG_PIXEL_UNKNOWN;

	if( pWinSurf->format->BitsPerPixel == 32 )
		type = WG_PIXEL_ARGB_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		type = WG_PIXEL_RGB_8;
		
	WgSoftSurface_p pCanvas = WgSoftSurface::create( WgSize(pWinSurf->w,pWinSurf->h), type, (unsigned char*) pWinSurf->pixels, pWinSurf->pitch, 0 );

	WgSoftGfxDevice_p pGfxDevice = WgSoftGfxDevice::create( pCanvas );

	WgRootPanel_p pRoot = WgRootPanel::create( pGfxDevice );
	WgBase::msgRouter()->setRoot(pRoot);

	// 
	
	WgMsgLogger_p pLogger = WgMsgLogger::create( std::cout );
	pLogger->ignoreAllMsgs();
	pLogger->logMouseMsgs();
	
	WgBase::msgRouter()->broadcastTo( pLogger );


	// Init font
	
	SDL_Surface * pFontSurf = IMG_Load( "../../../resources/anuvverbubbla_8x8.png" );
	WgBlob_p pFontSurfBlob = WgBlob::create( pFontSurf, freeSDLSurfCallback );
	WgSoftSurface_p pFontImg = WgSoftSurface::create( WgSize(pFontSurf->w,pFontSurf->h), WG_PIXEL_ARGB_8, (unsigned char*) pFontSurf->pixels, pFontSurf->pitch, pFontSurfBlob );
		
	char * pFontSpec = (char*) loadFile( "../../../resources/anuvverbubbla_8x8.fnt" );

	WgBitmapGlyphs_p pGlyphs = WgBitmapGlyphs::create( pFontImg, pFontSpec );
	WgFont_p pFont = WgFont::create( pGlyphs, 8 );

	WgTextprop prop;
	prop.setFont(pFont);
	prop.setSize(8);
	WgTextprop_p pProp = prop.reg();

	WgBase::setDefaultTextprop( pProp );

	WgTextStyle_p pStyle = WgTextStyle::create();
	pStyle->setFont(pFont);
	pStyle->setSize(8);
	WgBase::setDefaultStyle(pStyle);

	// Init skins

	SDL_Surface * pSDLSurf = IMG_Load( "../../../resources/simple_button.bmp" );
	WgSoftSurface_p pButtonSurface = WgSoftSurface::create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgBlockSkin_p pSimpleButtonSkin = WgBlockSkin::createClickableFromSurface( pButtonSurface, 0, WgBorder(3) );
	pSimpleButtonSkin->setContentPadding( WgBorder(5) );

	pSDLSurf = IMG_Load( "../../../resources/state_button.bmp" );
	WgSoftSurface_p pStateButtonSurface = WgSoftSurface::create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgBlockSkin_p pStateButtonSkin = WgBlockSkin::createClickSelectableFromSurface( pStateButtonSurface, 0, WgBorder(3) );
	pStateButtonSkin->setContentPadding( WgBorder(5) );

	pSDLSurf = IMG_Load( "../../../resources/grey_pressable_plate.bmp" );
	WgSoftSurface_p pPressablePlateSurface = WgSoftSurface::create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgSkin_p pPressablePlateSkin = WgBlockSkin::createClickableFromSurface( pPressablePlateSurface, 0, WgBorder(3) );
	
	pSDLSurf = IMG_Load( "../../../resources/list_entry.png" );
	WgSoftSurface_p pListEntrySurface = WgSoftSurface::create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgSkin_p pListEntrySkin = WgBlockSkin::createClickableFromSurface( pListEntrySurface, 0, WgBorder(3) );

	//------------------------------------------------------
	// Setup a simple GUI consisting of a filled background and 
	// a button using scaled bitmaps.
	//------------------------------------------------------

	WgFlexPanel_p pFlexPanel = WgFlexPanel::create();
	pFlexPanel->setSkin( WgColorSkin::create(WgColor::burlywood));
	pRoot->setWidget(pFlexPanel);


/*	WgFiller_p pBackground = WgFiller::create();
	pBackground->setSkin( WgColorSkin::create(WgColor::aqua) );
	pFlexPanel->addWidget(pBackground, WG_NORTHWEST, WgCoord(), WG_SOUTHEAST, WgCoord());
*/

	WgImage_p pImage = WgImage::create();
	pImage->setSkin( pSimpleButtonSkin );
	pFlexPanel->addWidget( pImage, WgRect(0,0,80,33), WG_CENTER );


//	pRoot->msgRouter()->AddCallback( WgMsgFilter::select(), pButton, myButtonClickCallback );


	// Test transparency issue
	
	{
		WgFlexPanel_p pExtraFlex = WgFlexPanel::create();
		pExtraFlex->setSkin( WgColorSkin::create( WgColor(0,0,0,128)));

		WgTextDisplay_p pText = WgTextDisplay::create();
		pText->text.set( "THIS IS SOME TEST TEXT" );
		
		pExtraFlex->addWidget( pText, WgRect( 10,10,100,100) );

		addResizablePanel( pFlexPanel, pExtraFlex, WgBase::msgRouter() );
	}



/*
	WgSizeCapsule_p pCapsule = WgSizeCapsule::create();
	pCapsule->setMaxSize( WgSize(100,1000));
	pFlexPanel->addWidget( pCapsule );

	WgStackPanel_p pStack = WgStackPanel::create();
	pCapsule->setWidget( pStack );


	WgTextDisplay_p pText = WgTextDisplay::create();
	pText->text()->set( "THIS IS THE LONG TEXT THAT SHOULD WRAP AND BE FULLY DISPLAYED." );
	pStack->addWidget(pText);
*/
	
/*
	WgCheckBoxPtr pCheckbox = WgCheckBox::create();
	pCheckbox->Label()->set( "CHECKBOX" );
	pCheckbox->Icon()->set( pCheckboxSkin );
*/

/*
	WgToggleGroup_p pGroup = WgToggleGroup::create();
	

	for( int i = 0 ; i < 4 ; i++ )
	{
		WgToggleButton_p pCheckbox = WgToggleButton::create();
		pCheckbox->label.set( "CHECKBOX" );
		pCheckbox->setSkin( pStateButtonSkin );
		pFlexPanel->addWidget( pCheckbox, WgCoord(10,20*i) );
		pGroup->add( pCheckbox );
	}
*/

/*
	WgTextDisplay_p pText = WgTextDisplay::create();
	pText->text.set( "THIS IS THE TEXT\nLINE TWO.\nAN EXTRA LONG LONG LONG LINE TO TEST THE CLIPPING AND RAND BEHAVIOURS." );
	pFlexPanel->addWidget( pText, WG_NORTHWEST, WG_EAST );
	

	WgStandardPresenter_p pPresenter = WgStandardPresenter::create();
	pPresenter->setAlignment( WG_CENTER );
	pText->text.setPresenter( pPresenter );


	WgValueDisplay_p pValue = WgValueDisplay::create();
	pValue->value.setFormatter( WgTimeFormatter::create("%2H:%2M:%2S"));
	pValue->value.set(3600+60+12);
	pValue->value.setPresenter( pPresenter );
	pFlexPanel->addWidget( pValue, WG_WEST, WG_SOUTHEAST );
*/

	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	while( !bQuit ) 
	{
		translateEvents( pInput, pRoot );

		SDL_LockSurface(pWinSurf);
		pRoot->render();
		SDL_UnlockSurface(pWinSurf);

		updateWindowRects( pRoot, pWin );

		SDL_Delay(20);
    }

	// Exit WonderGUI

	WgBase::exit();

	// Exit SDL

	IMG_Quit();
	SDL_Quit();

    return 0;
}

//____ translateEvents() ___________________________________________________________

void translateEvents( const WgInputHandler_p& pInput, const WgRootPanel_p& pRoot )
{
	// WonderGUI needs Tick-events to keep track of time passed for things such
	// key-repeat, double-click detection, animations etc.  So we create one
	// and put it on the event queue.
	
	static unsigned int oldTicks = 0;
	
	unsigned int ticks = SDL_GetTicks();
	int tickDiff;

	if( oldTicks == 0 )
		tickDiff = 0;
	else
		tickDiff = (int) (ticks - oldTicks);		
	oldTicks = ticks;

	WgBase::msgRouter()->post( WgTickMsg::create(tickDiff) );

	// Process all the SDL events in a loop

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		
		switch( e.type )
		{
			case SDL_QUIT:
				bQuit = true;
				break;
				
			case SDL_MOUSEMOTION:
				pInput->setPointer( pRoot, WgCoord(e.motion.x,e.motion.y) );
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				pInput->setButton( translateMouseButton(e.button.button), true );
				break;

			case SDL_MOUSEBUTTONUP:
				pInput->setButton( translateMouseButton(e.button.button), false );
				break;
				
			default:
				break;
		}
	}
	
	WgBase::msgRouter()->dispatch();	
}

//____ translateMouseButton() __________________________________________________
//
// Translate SDL mouse button enums to WonderGUI equivalents.
//
WgMouseButton translateMouseButton( Uint8 button )
{
	switch( button )
	{
		default:
		case SDL_BUTTON_LEFT:
			return WG_BUTTON_LEFT;
		case SDL_BUTTON_MIDDLE:
			return WG_BUTTON_MIDDLE;
		case SDL_BUTTON_RIGHT:
			return WG_BUTTON_RIGHT;
		case SDL_BUTTON_X1:
			return WG_BUTTON_X1;
		case SDL_BUTTON_X2:
			return WG_BUTTON_X2;
	}
}

//____ updateWindowRects() _____________________________________________________
//
// Updates the rectangles of the SDL Window that WonderGUI has modified.
//
void updateWindowRects( const WgRootPanel_p& pRoot, SDL_Window * pWindow )
{	
	int nRects = pRoot->nbUpdatedRects();
	if( nRects == 0 )
		return;
	
	const WgRect * pUpdatedRects = pRoot->firstUpdatedRect();
	SDL_Rect * pSDLRects = (SDL_Rect*) WgBase::memStackAlloc( sizeof(SDL_Rect) * nRects );

	for( int i = 0 ; i < nRects ; i++ )
	{
		pSDLRects[i].x = pUpdatedRects[i].x;
		pSDLRects[i].y = pUpdatedRects[i].y;
		pSDLRects[i].w = pUpdatedRects[i].w;
		pSDLRects[i].h = pUpdatedRects[i].h;
	}

	SDL_UpdateWindowSurfaceRects( pWindow, pSDLRects, nRects );

	WgBase::memStackRelease( sizeof(SDL_Rect) * nRects );
}

//____ myButtonClickCallback() _________________________________________________

void myButtonClickCallback( const WgMsg_p& pMsg )
{
	bQuit = true;
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

void cbInitDrag( const WgMsg_p& _pMsg, const WgObject_p& pObject )
{
	WgWidget_p pWidget = WgWidget::cast(pObject);

	WgFlexHook_p pHook = WgFlexHook::cast(pWidget->hook());


	dragStartPos = pHook->floatOfs();
	printf( "DRAG START!\n" );
}

//____ cbDragWidget() __________________________________________________________

void cbDragWidget( const WgMsg_p& _pMsg, const WgObject_p& pObject )
{
	WgWidget_p pWidget = WgWidget::cast(pObject);
	
	if( _pMsg->type() != WG_MSG_MOUSE_DRAG || !pWidget->parent() )
		return;

	const WgMouseDragMsg_p pMsg = WgMouseDragMsg::cast(_pMsg);



	WgCoord	dragDistance = pMsg->draggedTotal();

	WgCoord	ofs = dragStartPos + dragDistance;

//	printf( "AccDistance: %d, %d\n", dragDistance.x, dragDistance.y );
	printf( "ofs: %d, %d   start: %d %d   distance: %d, %d\n", ofs.x, ofs.y, dragStartPos.x, dragStartPos.y, dragDistance.x, dragDistance.y );

	WgFlexHook_p pHook = WgFlexHook::cast(pWidget->hook());
	pHook->setOfs(dragStartPos+dragDistance);
}


//____ cbResizeWidget() _________________________________________________________
/*
void cbResize( const WgMsg_p _pMsg, WgObject_p _pFlexHook )
{
	WgFlexHook * pHook = static_cast<WgFlexHook*>(_pFlexHook);
	const WgMsg::MouseButtonDrag* pMsg = static_cast<const WgMsg::MouseButtonDrag*>(_pMsg);

	WgCoord dragged = pMsg->draggedNow();

	pHook->setSize( pHook->size() + WgSize(dragged.x,dragged.y) );
}
*/



//____ addResizablePanel() _________________________________________________

void addResizablePanel( const WgFlexPanel_p& pParent, const WgWidget_p& pChild, const WgMsgRouter_p& pMsgRouter )
{
	WgHook * pHook = pParent->addWidget( pChild );
//	pMsgRouter->AddCallback( WgMsgFilter::MouseButtonDrag(pChild, 3), cbResize, pHook );

	pMsgRouter->addRoute( WgMsgFilter::mousePress(WG_BUTTON_LEFT), pChild, WgMsgFunc::create(cbInitDrag, pChild) );
	pMsgRouter->addRoute( WgMsgFilter::mouseDrag(WG_BUTTON_LEFT), pChild, WgMsgFunc::create(cbDragWidget, pChild) );
}
