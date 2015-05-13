
#include <cstdlib>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <wondergui.h>

#include <wg_softsurface.h>
#include <wg_softgfxdevice.h>

void 			translateEvents( WgEventHandlerPtr pEventHandler );
WgMouseButton 	translateMouseButton( Uint8 button );
void 			updateWindowRects( const WgRootPanelPtr& pRoot, SDL_Window * pWindow );
void 			myButtonClickCallback( const WgEventPtr& pEvent );
void * 			loadFile( const char * pPath );


bool	bQuit = false;


void freeSDLSurfCallback( void * pSDLSurf )
{
	SDL_FreeSurface( (SDL_Surface*) pSDLSurf );
}

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

	WgBase::Init();

	WgPixelType type = WG_PIXEL_UNKNOWN;

	if( pWinSurf->format->BitsPerPixel == 32 )
		type = WG_PIXEL_ARGB_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		type = WG_PIXEL_RGB_8;
		
	WgSoftSurfacePtr pCanvas = WgSoftSurface::Create( WgSize(pWinSurf->w,pWinSurf->h), type, (unsigned char*) pWinSurf->pixels, pWinSurf->pitch, 0 );

	WgSoftGfxDevicePtr pGfxDevice = WgSoftGfxDevice::Create( pCanvas );

	WgRootPanelPtr pRoot = WgRootPanel::Create( pGfxDevice );

	// 
	
	WgEventLogger	eventLogger( std::cout );
	eventLogger.IgnoreAllEvents();
	eventLogger.LogMouseEvents();
	
	pRoot->EventHandler()->AddCallback( &eventLogger );


	// Init font
	
	SDL_Surface * pFontSurf = IMG_Load( "../../../resources/anuvverbubbla_8x8.png" );
	WgBlobPtr pFontSurfBlob = WgBlob::Create( pFontSurf, freeSDLSurfCallback );
	WgSoftSurfacePtr pFontImg = WgSoftSurface::Create( WgSize(pFontSurf->w,pFontSurf->h), WG_PIXEL_ARGB_8, (unsigned char*) pFontSurf->pixels, pFontSurf->pitch, pFontSurfBlob );
		
	char * pFontSpec = (char*) loadFile( "../../../resources/anuvverbubbla_8x8.fnt" );

	WgBitmapGlyphsPtr pGlyphs = WgBitmapGlyphs::Create( pFontImg, pFontSpec );
	WgFontPtr pFont = WgFont::Create( pGlyphs, 8 );

	WgTextprop prop;
	prop.SetFont(pFont);
	prop.SetSize(8);
	WgTextpropPtr pProp = prop.Register();

	WgBase::SetDefaultTextprop( pProp );

	WgTextStylePtr pStyle = WgTextStyle::Create();
	pStyle->SetFont(pFont);
	pStyle->SetSize(8);
	WgBase::SetDefaultStyle(pStyle);

	// Init skins

	SDL_Surface * pSDLSurf = IMG_Load( "../../../resources/simple_button.bmp" );
	WgSoftSurfacePtr pButtonSurface = WgSoftSurface::Create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgBlockSkinPtr pSimpleButtonSkin = WgBlockSkin::CreateClickableFromSurface( pButtonSurface, 0, WgBorder(3) );
	pSimpleButtonSkin->SetContentPadding( WgBorder(5) );

	pSDLSurf = IMG_Load( "../../../resources/state_button.bmp" );
	WgSoftSurfacePtr pStateButtonSurface = WgSoftSurface::Create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgBlockSkinPtr pStateButtonSkin = WgBlockSkin::CreateClickSelectableFromSurface( pStateButtonSurface, 0, WgBorder(3) );
	pStateButtonSkin->SetContentPadding( WgBorder(5) );

	pSDLSurf = IMG_Load( "../../../resources/grey_pressable_plate.bmp" );
	WgSoftSurfacePtr pPressablePlateSurface = WgSoftSurface::Create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgSkinPtr pPressablePlateSkin = WgBlockSkin::CreateClickableFromSurface( pPressablePlateSurface, 0, WgBorder(3) );
	
	pSDLSurf = IMG_Load( "../../../resources/list_entry.png" );
	WgSoftSurfacePtr pListEntrySurface = WgSoftSurface::Create( WgSize( pSDLSurf->w, pSDLSurf->h ), WG_PIXEL_RGB_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	WgSkinPtr pListEntrySkin = WgBlockSkin::CreateClickableFromSurface( pListEntrySurface, 0, WgBorder(3) );

	//------------------------------------------------------
	// Setup a simple GUI consisting of a filled background and 
	// a button using scaled bitmaps.
	//------------------------------------------------------

	WgFlexPanelPtr pFlexPanel = WgFlexPanel::Create();
	pFlexPanel->SetSkin( WgColorSkin::Create(WgColor::burlywood));
	pRoot->SetWidget(pFlexPanel);


/*	WgFillerPtr pBackground = WgFiller::Create();
	pBackground->SetSkin( WgColorSkin::Create(WgColor::aqua) );
	pFlexPanel->AddWidget(pBackground, WG_NORTHWEST, WgCoord(), WG_SOUTHEAST, WgCoord());
*/

	WgImagePtr pImage = WgImage::Create();
	pImage->SetSkin( pSimpleButtonSkin );
	pFlexPanel->AddWidget( pImage, WgRect(0,0,80,33), WG_CENTER );

//	pRoot->EventHandler()->AddCallback( WgEventFilter::Select(), pButton, myButtonClickCallback );


/*
	WgSizeCapsulePtr pCapsule = WgSizeCapsule::Create();
	pCapsule->SetMaxSize( WgSize(100,1000));
	pFlexPanel->AddWidget( pCapsule );

	WgStackPanelPtr pStack = WgStackPanel::Create();
	pCapsule->SetWidget( pStack );


	WgTextDisplayPtr pText = WgTextDisplay::Create();
	pText->Text()->Set( "THIS IS THE LONG TEXT THAT SHOULD WRAP AND BE FULLY DISPLAYED." );
	pStack->AddWidget(pText);
*/
	
/*
	WgCheckBoxPtr pCheckbox = WgCheckBox::Create();
	pCheckbox->Label()->Set( "CHECKBOX" );
	pCheckbox->Icon()->Set( pCheckboxSkin );
*/

/*
	WgToggleGroupPtr pGroup = WgToggleGroup::Create();
	

	for( int i = 0 ; i < 4 ; i++ )
	{
		WgToggleButtonPtr pCheckbox = WgToggleButton::Create();
		pCheckbox->label.Set( "CHECKBOX" );
		pCheckbox->SetSkin( pStateButtonSkin );
		pFlexPanel->AddWidget( pCheckbox, WgCoord(10,20*i) );
		pGroup->Add( pCheckbox );
	}
*/

/*
	WgTextDisplayPtr pText = WgTextDisplay::Create();
	pText->text.Set( "THIS IS THE TEXT\nLINE TWO.\nAN EXTRA LONG LONG LONG LINE TO TEST THE CLIPPING AND RAND BEHAVIOURS." );
	pFlexPanel->AddWidget( pText, WG_NORTHWEST, WG_EAST );
	

	WgStandardPresenterPtr pPresenter = WgStandardPresenter::Create();
	pPresenter->SetAlignment( WG_CENTER );
	pText->text.SetPresenter( pPresenter );


	WgValueDisplayPtr pValue = WgValueDisplay::Create();
	pValue->value.SetFormatter( WgTimeFormatter::Create("%2H:%2M:%2S"));
	pValue->value.Set(3600+60+12);
	pValue->value.SetPresenter( pPresenter );
	pFlexPanel->AddWidget( pValue, WG_WEST, WG_SOUTHEAST );
*/

	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	while( !bQuit ) 
	{
		translateEvents( pRoot->EventHandler() );

		SDL_LockSurface(pWinSurf);
		pRoot->Render();
		SDL_UnlockSurface(pWinSurf);

		updateWindowRects( pRoot, pWin );

		SDL_Delay(20);
    }

	// Exit WonderGUI

	WgBase::Exit();

	// Exit SDL

	IMG_Quit();
	SDL_Quit();

    return 0;
}

//____ translateEvents() ___________________________________________________________

void translateEvents( WgEventHandlerPtr pEventHandler )
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

	pEventHandler->QueueEvent( WgTickEvent::Create(tickDiff) );

	// Process all the SDL events in a loop

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		
		switch( e.type )
		{
			case SDL_QUIT:
				bQuit = true;
				break;
				
			case SDL_MOUSEMOTION:
				pEventHandler->QueueEvent( WgMouseMoveEvent::Create( WgCoord(e.motion.x,e.motion.y)) );
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				pEventHandler->QueueEvent( WgMousePressEvent::Create( translateMouseButton(e.button.button)));
				break;

			case SDL_MOUSEBUTTONUP:
				pEventHandler->QueueEvent( WgMouseReleaseEvent::Create( translateMouseButton(e.button.button)));
				break;
				
			default:
				break;
		}
	}
	
	pEventHandler->ProcessEvents();	
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
void updateWindowRects( const WgRootPanelPtr& pRoot, SDL_Window * pWindow )
{	
	int nRects = pRoot->NbUpdatedRects();
	if( nRects == 0 )
		return;
	
	const WgRect * pUpdatedRects = pRoot->FirstUpdatedRect();
	SDL_Rect * pSDLRects = (SDL_Rect*) WgBase::MemStackAlloc( sizeof(SDL_Rect) * nRects );

	for( int i = 0 ; i < nRects ; i++ )
	{
		pSDLRects[i].x = pUpdatedRects[i].x;
		pSDLRects[i].y = pUpdatedRects[i].y;
		pSDLRects[i].w = pUpdatedRects[i].w;
		pSDLRects[i].h = pUpdatedRects[i].h;
	}

	SDL_UpdateWindowSurfaceRects( pWindow, pSDLRects, nRects );

	WgBase::MemStackRelease( sizeof(SDL_Rect) * nRects );
}

//____ myButtonClickCallback() _________________________________________________

void myButtonClickCallback( const WgEventPtr& pEvent )
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
