
#include <cstdlib>
#include <stdio.h>

#ifdef WIN32
#	include <SDL.h>
#	include <SDL_image.h>
#else
#	include <SDL2/SDL.h>
#	include <SDL2_Image/SDL_image.h>
#endif

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>

#include <wondergui.h>

#include <wg_softsurface.h>
#include <wg_softsurfacefactory.h>
#include <wg_softgfxdevice.h>
#include <wg_streamsurface.h>
#include <wg_streamsurfacefactory.h>
#include <wg_streamgfxdevice.h>

#include <wg_gfxstreambuffer.h>
#include <wg_gfxstreamsplitter.h>
#include <wg_gfxstreampump.h>


#include <wg_freetypefont.h>
#include <wg_packlist.h>
#include <wg_popupopener.h>

using namespace wg;
using namespace std;

void 			translateEvents( const InputHandler_p& pInput, const RootPanel_p& pRoot );
MouseButton 	translateMouseButton( Uint8 button );
void 			updateWindowRects( const RootPanel_p& pRoot, SDL_Window * pWindow );
void 			myButtonClickCallback( const Msg_p& pMsg );
void *          loadFile( const char * pPath, int * bytesLoaded );
Blob_p 			loadBlob( const char * pPath );
void			convertSDLFormat( PixelDescription * pWGFormat, const SDL_PixelFormat * pSDLFormat );

bool	bQuit = false;

char * g_pStreamBuffer = NULL;
int    g_streamSize = 0;

char * g_pStreamReader = NULL;

Image_p g_pDisplay;

void freeSDLSurfCallback( void * pSDLSurf )
{
	SDL_FreeSurface( (SDL_Surface*) pSDLSurf );
}


//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{ 

    // Load stream

//    g_pStreamBuffer = (char*) loadFile( argv[1], &g_streamSize );
	g_pStreamBuffer = (char*) loadFile( "output.wax", &g_streamSize );
//	g_pStreamBuffer = (char*) loadFile( "softubehwstream.dat", &g_streamSize );

    if( g_pStreamBuffer == NULL )
        return -1;

    g_pStreamReader = g_pStreamBuffer;


	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 640, height = 480;
	SDL_Window * pWin = SDL_CreateWindow("WonderGUI Stream Analyzer", posX, posY, width, height, SDL_WINDOW_ALLOW_HIGHDPI);

	SDL_Surface * pWinSurf = SDL_GetWindowSurface( pWin );

	IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG );
	 
	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------

	Base::init(nullptr);
//	FreeTypeFont::init( SoftSurfaceFactory::create() );

	InputHandler_p pInput = Base::inputHandler();
	
	pInput->mapKey( SDLK_LEFT, Key::Left );
	pInput->mapKey( SDLK_RIGHT, Key::Right );
	pInput->mapKey( SDLK_UP, Key::Up );
	pInput->mapKey( SDLK_DOWN, Key::Down );
	pInput->mapKey( SDLK_BACKSPACE, Key::Backspace );
	pInput->mapKey( SDLK_DELETE, Key::Delete );
	pInput->mapKey( SDLK_END, Key::End );
	pInput->mapKey( SDLK_ESCAPE, Key::Escape );
	pInput->mapKey( SDLK_HOME, Key::Home );
	pInput->mapKey( SDLK_PAGEDOWN, Key::PageDown );
	pInput->mapKey( SDLK_PAGEUP, Key::PageUp );
	pInput->mapKey( SDLK_RETURN, Key::Return );
	pInput->mapKey( SDLK_SPACE, Key::Space );
	pInput->mapKey( SDLK_TAB, Key::Tab );
	pInput->mapKey( SDLK_F1, Key::F1 );
	pInput->mapKey( SDLK_F2, Key::F2 );
	pInput->mapKey( SDLK_F3, Key::F3 );
	pInput->mapKey( SDLK_F4, Key::F4 );
	pInput->mapKey( SDLK_F5, Key::F5 );
	pInput->mapKey( SDLK_F6, Key::F6 );
	pInput->mapKey( SDLK_F7, Key::F7 );
	pInput->mapKey( SDLK_F8, Key::F8 );
	pInput->mapKey( SDLK_F9, Key::F9 );
	pInput->mapKey( SDLK_F10, Key::F10 );
	pInput->mapKey( SDLK_F11, Key::F11 );
	pInput->mapKey( SDLK_F12, Key::F12 );
	
	
	pInput->mapKey( SDLK_LCTRL, Key::Control );
	pInput->mapKey( SDLK_RCTRL, Key::Control );

	pInput->mapKey( SDLK_LSHIFT, Key::Shift );
	pInput->mapKey( SDLK_RSHIFT, Key::Shift );

	pInput->mapKey( SDLK_LALT, Key::Alt );
	pInput->mapKey( SDLK_RALT, Key::Alt );
	
	pInput->mapKey( SDLK_KP_ENTER, Key::Return );
	
	
	pInput->mapCommand( SDLK_ESCAPE, ModKeys::None, EditCmd::Escape );

	pInput->mapCommand( SDLK_x, ModKeys::Ctrl, EditCmd::Cut );
	pInput->mapCommand( SDLK_c, ModKeys::Ctrl, EditCmd::Copy );
	pInput->mapCommand( SDLK_v, ModKeys::Ctrl, EditCmd::Paste );

	pInput->mapCommand( SDLK_a, ModKeys::Ctrl, EditCmd::SelectAll );
	
	pInput->mapCommand( SDLK_z, ModKeys::Ctrl, EditCmd::Undo );
	pInput->mapCommand( SDLK_z, ModKeys::CtrlShift, EditCmd::Redo );


	PixelFormat type = PixelFormat::Undefined;

	if( pWinSurf->format->BitsPerPixel == 32 )
		type = PixelFormat::BGRA_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		type = PixelFormat::BGR_8;
		
	Blob_p pCanvasBlob = Blob::create( pWinSurf->pixels, 0);	
	SoftSurface_p pCanvas = SoftSurface::create( SizeI(pWinSurf->w,pWinSurf->h), type, pCanvasBlob, pWinSurf->pitch );
    
	SoftGfxDevice_p pGfxDevice = SoftGfxDevice::create();

	RootPanel_p pRoot = RootPanel::create( pCanvas, pGfxDevice );
//    pRoot->setGeo( pCanvas->size() );
	
	Base::inputHandler()->setFocusedWindow( pRoot );
	
	// Init font
/*
	PixelFormat	format;

	char * pFontSpec = (char*)loadFile("../resources/anuvverbubbla_8x8.fnt");

	SDL_Surface * pFontSurf = IMG_Load( "../resources/anuvverbubbla_8x8.png" );
//	convertSDLFormat( &format, pFontSurf->format );
	SoftSurface_p pFontImg = SoftSurface::create( SizeI(pFontSurf->w,pFontSurf->h), PixelType::BGRA_8, (unsigned char*) pFontSurf->pixels, pFontSurf->pitch);
	SDL_FreeSurface( pFontSurf );
		
	BitmapFont_p pBmpFont = BitmapFont::create( pFontImg, pFontSpec );


	Blob_p pFontFile = loadBlob("../resources/DroidSans.ttf");
	
	FreeTypeFont_p pFont = FreeTypeFont::create( pFontFile, 1 );


	TextStyle_p pStyle = TextStyle::create();
	pStyle->setFont(pFont);
	pStyle->setSize(16);
	Base::setDefaultStyle(pStyle);
*/

	//------------------------------------------------------
	// Setup the GUI
	//------------------------------------------------------

	PopupOverlay_p pPopupLayer = PopupOverlay::create();
	pRoot->slot = pPopupLayer;



	LambdaPanel_p pBasePanel = LambdaPanel::create();
	pBasePanel->setSkin( ColorSkin::create(Color::Burlywood) );
	pPopupLayer->mainSlot = pBasePanel;

    g_pDisplay = Image::create();
	auto pSkin = BoxSkin::create({ .color = Color::Black, .outline = 2, .outlineColor = Color::HotPink, .padding = 2 });
    g_pDisplay->setSkin(pSkin);
    pBasePanel->slots.pushBack(g_pDisplay, [](Widget * pWidget, Size sz) { return Rect(10,10,240,240); } );


	//------------------------------------------------------
	// Setup streaming
	//------------------------------------------------------
/*
    auto pStreamOutputCanvas = SoftSurface::create( {240,240}, PixelFormat::BGRA_8, SurfaceFlag::Canvas );
    pGfxDevice->defineCanvas(CanvasRef::Canvas_1, pStreamOutputCanvas );


	auto pStreamPlug = GfxStreamPlug::create();

	auto pStreamDevice = StreamGfxDevice::create(pStreamPlug->input);
    pStreamDevice->defineCanvas(CanvasRef::Canvas_1, {240,240} );

	auto pSurfaceFactory = StreamSurfaceFactory::create(pStreamPlug->input);

	pStreamPlug->openOutput(0);
	auto pStreamLogger = GfxStreamLogger::create(pStreamPlug->output[0], std::cout);

	pStreamPlug->openOutput(1);
	auto pGfxPlayer = GfxStreamPlayer::create(pStreamPlug->output[1], pGfxDevice, SoftSurfaceFactory::create());
*/

    auto pStreamOutputCanvas = SoftSurface::create( {240,240}, PixelFormat::BGRA_8, SurfaceFlag::Canvas );
    pGfxDevice->defineCanvas(CanvasRef::Canvas_1, pStreamOutputCanvas );

    
    GfxStreamReader_p pStreamReader = GfxStreamReader::create( [](int nBytes, void * pDest)
        {
            int bytesLeft = g_streamSize - (g_pStreamReader - g_pStreamBuffer);
        
        int bytesToRead = std::min( {nBytes,bytesLeft} );
        
            memcpy( pDest, g_pStreamReader, bytesToRead );
            g_pStreamReader+= bytesToRead;
		
//			std::cout << "Dispatcher: asked: " << nBytes << " gave: " << bytesToRead << std::endl;
            return bytesToRead;
        
        } );

	auto pStreamPlayer = GfxStreamPlayer::create(pGfxDevice, SoftSurfaceFactory::create());

    auto pStreamLogger = GfxStreamLogger::create(std::cout);


	auto pStreamSplitter = GfxStreamSplitter::create({ pStreamPlayer->input.ptr(), pStreamLogger->input.ptr() });

	auto pStreamPump = GfxStreamPump::create(pStreamReader->output.ptr(), pStreamSplitter->input.ptr() );


	//------------------------------------------------------
	// Setup graphics
	//------------------------------------------------------

/*
	pStreamDevice->beginRender();
    pStreamDevice->beginCanvasUpdate(CanvasRef::Canvas_1);
    
	pStreamDevice->fill( RectI( 0,0,width,height ), Color::Blue);
	pStreamDevice->fill( RectI( 10,10,100,100 ), Color::Red);
    pStreamDevice->setBlitSource(pSplashSurface);
    
	pStreamDevice->blit( { 120,10 } );
    pStreamDevice->endCanvasUpdate();
    pStreamDevice->endRender();

    pStreamDevice->beginRender();
    pStreamDevice->beginCanvasUpdate(CanvasRef::Canvas_1);
    
    pStreamDevice->fill( RectI( 0,0,width,height ), Color::Green);
    pStreamDevice->fill( RectI( 10,10,100,100 ), Color::Yellow);
    
    pStreamDevice->endCanvasUpdate();
    pStreamDevice->endRender();
*/
	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	while( !bQuit )
	{
		translateEvents( pInput, pRoot );

		pStreamPump->pumpUntilFrame();
		bool pumpAgain = true;
//		while( pumpAgain )
			pumpAgain = pStreamPump->pumpFrame();
		
//		pStreamPump->pumpAll();

		g_pDisplay->setImage(nullptr);
        g_pDisplay->setImage(pStreamOutputCanvas);
        
		SDL_LockSurface(pWinSurf);
		pRoot->render();
		SDL_UnlockSurface(pWinSurf);

		bool bUpdateAll = false;
		
		if( bUpdateAll )
		{
			SDL_Rect	r;
			r.x = 0;
			r.y = 0;
			r.w = width;
			r.h = height;
			SDL_UpdateWindowSurfaceRects(pWin, &r, 1);

		}
		else
		{
			CoordSPX ofs = g_pDisplay->spxGlobalPos()/64;
			
			int nRects;
			const RectI* pRects;

			std::tie(nRects, pRects) = pStreamPlayer->dirtyRects(CanvasRef::Canvas_1);

			SDL_Rect	dirt[1000];
			
			for( int i = 0 ; i < nRects ; i++ )
			{
				dirt[i].x = ofs.x + pRects[i].x;
				dirt[i].y = ofs.y + pRects[i].y;
				dirt[i].w = pRects[i].w;
				dirt[i].h = pRects[i].h;
			}
			
			SDL_UpdateWindowSurfaceRects(pWin, dirt, nRects);
		}


		
//		for(int i = 0 ; i < nRects ; i++ )
//			std::cout << "" << pRects[i].x << " " << pRects[i].y << " " << pRects[i].w << " " << pRects[i].h << std::endl;

//		std::cout << "Bytes in buffer:" << pStreamReader->bytesInBuffer() << std::endl;
		
//		updateWindowRects( pRoot, pWin );

//		pStreamLogger->logAll();

		SDL_Delay(10);

		pStreamPlayer->clearDirtyRects();
    }

	// Exit WonderGUI

	Base::exit();

	// Exit SDL

	IMG_Quit();
	SDL_Quit();

    return 0;
}


//____ translateEvents() ___________________________________________________________

void translateEvents( const InputHandler_p& pInput, const RootPanel_p& pRoot )
{
	// WonderGUI needs Tick-events to keep track of time passed for things such
	// key-repeat, double-click detection, animations etc.  So we create one
	// and put it on the event queue.
	
	unsigned int ticks = SDL_GetTicks();
    Base::update( ticks*1000 );

	// Process all the SDL events in a loop

	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		
		switch( e.type )
		{
			case SDL_QUIT:
				bQuit = true;
				break;
				
			case SDL_MOUSEMOTION:
				pInput->setPointer( pRoot, Coord(e.motion.x,e.motion.y) );
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				pInput->setButton( translateMouseButton(e.button.button), true );
				break;

			case SDL_MOUSEBUTTONUP:
				pInput->setButton( translateMouseButton(e.button.button), false );
				break;
				
			case SDL_MOUSEWHEEL:
			{
				bool bInvertScroll = false;
				Coord distance(e.wheel.x, e.wheel.y);
				if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
				{
					bInvertScroll = true;
					distance *= -1;
				}
			
				if (e.wheel.y != 0)
					pInput->setWheelRoll(1, distance.y, bInvertScroll);

				if (e.wheel.x != 0)
					pInput->setWheelRoll(2, distance.x, bInvertScroll);
				break;
			}	
			
			case SDL_KEYDOWN:
			{
				pInput->setKey( e.key.keysym.sym, true );
				break;
			}
			
			case SDL_KEYUP:
			{
				pInput->setKey( e.key.keysym.sym, false );
				break;
			}

			case SDL_TEXTINPUT:
				pInput->putText( e.text.text );
				break;

			
			default:
				break;
		}
	}
	
	Base::msgRouter()->dispatch();	
}

//____ translateMouseButton() __________________________________________________
//
// Translate SDL mouse button enums to WonderGUI equivalents.
//
MouseButton translateMouseButton( Uint8 button )
{
	switch( button )
	{
		default:
		case SDL_BUTTON_LEFT:
			return MouseButton::Left;
		case SDL_BUTTON_MIDDLE:
			return MouseButton::Middle;
		case SDL_BUTTON_RIGHT:
			return MouseButton::Right;
		case SDL_BUTTON_X1:
			return MouseButton::X1;
		case SDL_BUTTON_X2:
			return MouseButton::X2;
	}
}

//____ updateWindowRects() _____________________________________________________
//
// Updates the rectangles of the SDL Window that WonderGUI has modified.
//
void updateWindowRects( const RootPanel_p& pRoot, SDL_Window * pWindow )
{	
	int nRects = pRoot->nbUpdatedRects();
	if( nRects == 0 )
		return;
	
	const RectSPX * pUpdatedRects = pRoot->firstUpdatedRect();
	SDL_Rect * pSDLRects = (SDL_Rect*) Base::memStackAlloc( sizeof(SDL_Rect) * nRects );

	for( int i = 0 ; i < nRects ; i++ )
	{
		pSDLRects[i].x = pUpdatedRects[i].x / 64;
		pSDLRects[i].y = pUpdatedRects[i].y / 64;
		pSDLRects[i].w = pUpdatedRects[i].w / 64;
		pSDLRects[i].h = pUpdatedRects[i].h / 64;
	}

	SDL_UpdateWindowSurfaceRects( pWindow, pSDLRects, nRects );

	Base::memStackRelease( sizeof(SDL_Rect) * nRects );
}

//____ myButtonClickCallback() _________________________________________________

void myButtonClickCallback( const Msg_p& pMsg )
{
	bQuit = true;
}


//____ loadBlob() _____________________________________________________________

Blob_p loadBlob( const char * pPath )
{
	FILE * fp = fopen( pPath, "rb" );
	if( !fp )
		return 0;

	fseek( fp, 0, SEEK_END );
	int size = ftell(fp);
	fseek( fp, 0, SEEK_SET );

	Blob_p pBlob = Blob::create( size );
		
	int nRead = fread( pBlob->data(), 1, size, fp );
	fclose( fp );

	if( nRead < size )
		return 0;

	return pBlob;

}


//____ loadFile() _____________________________________________________________

void * loadFile( const char * pPath, int * bytesLoaded )
{
    FILE* fp = fopen(pPath, "rb");
    if( !fp )
        return 0;

    fseek( fp, 0, SEEK_END );
    size_t size = ftell(fp);
    fseek( fp, 0, SEEK_SET );

    char * pMem = (char*) malloc( size+1 );
    pMem[size] = 0;
    size_t nRead = fread( pMem, 1, size, fp );
    fclose( fp );

    if( nRead < size )
    {
        free( pMem );
        * bytesLoaded = 0;
        return 0;
    }

    * bytesLoaded = nRead;
    return pMem;

}
//____ convertSDLFormat() ______________________________________________________

void convertSDLFormat( PixelDescription * pWGFormat, const SDL_PixelFormat * pSDLFormat )
{
	pWGFormat->format = PixelFormat::Custom;
	pWGFormat->bits = pSDLFormat->BitsPerPixel;

	pWGFormat->R_mask = pSDLFormat->Rmask;
	pWGFormat->G_mask = pSDLFormat->Gmask;
	pWGFormat->B_mask = pSDLFormat->Bmask;
	pWGFormat->A_mask = pSDLFormat->Amask;

	pWGFormat->R_shift = pSDLFormat->Rshift;
	pWGFormat->G_shift = pSDLFormat->Gshift;
	pWGFormat->B_shift = pSDLFormat->Bshift;
	pWGFormat->A_shift = pSDLFormat->Ashift;

	pWGFormat->R_bits = 8 - pSDLFormat->Rloss;
	pWGFormat->G_bits = 8 - pSDLFormat->Gloss;
	pWGFormat->B_bits = 8 - pSDLFormat->Bloss;
	pWGFormat->A_bits = 8 - pSDLFormat->Aloss;
}

