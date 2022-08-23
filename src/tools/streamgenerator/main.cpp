
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
void * 			loadFile( const char * pPath );
Blob_p 			loadBlob( const char * pPath );
void			convertSDLFormat( PixelDescription * pWGFormat, const SDL_PixelFormat * pSDLFormat );

void addResizablePanel( const FlexPanel_p& pParent, const Widget_p& pChild, const MsgRouter_p& pMsgRouter );

void playDelayFrames(GfxDevice_p pDevice, int nFrames);

void playRectangleDance(GfxDevice_p pDevice, CanvasRef canvas);
void playRectangleDanceDualScreen(GfxDevice_p pDevice, CanvasRef canvasRef1, CanvasRef canvasRef2 );
void playScroll(GfxDevice_p pDevice, RectI canvas );
void playLogoFadeIn(GfxDevice_p pDevice, CanvasRef canvasRef, SurfaceFactory_p pFactory );
void playSurfaceStressTest(GfxDevice_p pDevice, CanvasRef canvasRef, SurfaceFactory_p pFactory );
void playImageStreamingTest(GfxDevice_p pDevice, CanvasRef canvasRef, SurfaceFactory_p pFactory );


CoordSPX positionSprite(SizeSPX dimensions, int tick, int nb, int amount);



bool	bQuit = false;


void freeSDLSurfCallback( void * pSDLSurf )
{
	SDL_FreeSurface( (SDL_Surface*) pSDLSurf );
}

InputHandler * pDebug;

int sortWidgets( const Widget * p1, const Widget * p2 )
{
	return p2->id() - p1->id();
}


//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{ 



	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 640, height = 480;
	SDL_Window * pWin = SDL_CreateWindow("WonderGUI Stream Generator", posX, posY, width, height, SDL_WINDOW_ALLOW_HIGHDPI);

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

//	pRoot->setDebugMode(true);
	
	Base::inputHandler()->setFocusedWindow( pRoot );

	// 
	
	MsgLogger_p pLogger = MsgLogger::create( std::cout );
	pLogger->logAllMsgs();
	
//	Base::msgRouter()->broadcastTo( pLogger );

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
	// Setup a simple GUI consisting of a filled background and 
	// a button using scaled bitmaps.
	//------------------------------------------------------

	PopupOverlay_p pPopupLayer = PopupOverlay::create();
	pRoot->slot = pPopupLayer;



	LambdaPanel_p pBasePanel = LambdaPanel::create();
	pBasePanel->setSkin( ColorSkin::create(Color::Burlywood) );
	pPopupLayer->mainSlot = pBasePanel;

    auto pOutput1 = Image::create();
    pOutput1->setSkin( BoxSkin::create(2, Color8::Black, Color8::HotPink) );
    pBasePanel->slots.pushBack(pOutput1, [](Widget * pWidget, Size sz) { return Rect(10,10,244,244); } );

	auto pOutput2 = Image::create();
	pOutput2->setSkin( BoxSkin::create(2, Color8::Black, Color8::HotPink) );
	pBasePanel->slots.pushBack(pOutput2, [](Widget * pWidget, Size sz) { return Rect(244+20,10,244,244); } );


	//------------------------------------------------------
	// Setup streaming
	//------------------------------------------------------
    
    auto pStreamOutputCanvas1 = SoftSurface::create( {240,240}, PixelFormat::BGRA_8, SurfaceFlag::Canvas );
	auto pStreamOutputCanvas2 = SoftSurface::create( {240,240}, PixelFormat::BGRA_8, SurfaceFlag::Canvas );

	pGfxDevice->defineCanvas(CanvasRef::Canvas_1, pStreamOutputCanvas1);
	pGfxDevice->defineCanvas(CanvasRef::Canvas_2, pStreamOutputCanvas2);

    
//	auto pStreamPlug = GfxStreamPlug::create();

//	auto pGfxPlayer = GfxStreamPlayer::create(pGfxDevice, SoftSurfaceFactory::create());

	// Writer

	char* pBigBuffer = new char[10000000];
	char* pWrite = pBigBuffer;

	auto pStreamWriter = GfxStreamWriter::create([&pWrite](int nBytes, const void* pData) 
		{
			memcpy(pWrite, pData, nBytes);
			pWrite += nBytes;
		});

	// Buffer

	auto pStreamBuffer = GfxStreamBuffer::create();
	
	// Encoder with splitter, feeding writer and buffer.

//	auto pEncoder = GfxStreamEncoder::create(pGfxPlayer->stream.ptr());
//	auto pEncoder = GfxStreamEncoder::create(pStreamLogger->stream.ptr());

	auto pFirstSplitter = GfxStreamSplitter::create( { pStreamBuffer->input.ptr(), pStreamWriter->input.ptr() });

	auto pEncoder = GfxStreamEncoder::create(pFirstSplitter->input.ptr());

	// Logger

	auto pStreamLogger = GfxStreamLogger::create(std::cout);

	// Player

	auto pStreamPlayer = GfxStreamPlayer::create(pGfxDevice, SoftSurfaceFactory::create());


	// Streampump taking from buffer and feeding logger and/or player

	auto pAfterBufferSplitter = GfxStreamSplitter::create({ pStreamLogger->input.ptr(), pStreamPlayer->input.ptr() });

	auto pStreamPump = GfxStreamPump::create(pStreamBuffer->output.ptr(), pAfterBufferSplitter->input.ptr());

	// StreamGfxDevice and StreamSurfaceFactory feeding encoder

	auto pStreamDevice = StreamGfxDevice::create(pEncoder);
    pStreamDevice->defineCanvas(CanvasRef::Canvas_1, {240,240} );
	pStreamDevice->defineCanvas(CanvasRef::Canvas_2, {240,240} );

	auto pSurfaceFactory = StreamSurfaceFactory::create(pEncoder);


//	pStreamPlug->openOutput(0);
//	auto pStreamLogger = GfxStreamLogger::create(pStreamPlug->output[0], std::cout);

//	pStreamPlug->openOutput(1);
//	auto pGfxPlayer = GfxStreamPlayer::create(pStreamPlug->output[1], pGfxDevice, SoftSurfaceFactory::create());
    
    //------------------------------------------------------
    // Setup stream saving
    //------------------------------------------------------

/*
    char * pBigBuffer = new char[10000000];

    char * pWrite = pBigBuffer;
    char * pRead = pBigBuffer;

    pStreamPlug->openOutput(2);
    
    auto pSaveInStream = &pStreamPlug->output[2];
*/
   
	//------------------------------------------------------
	// Record stream
	//------------------------------------------------------

//    playRectangleDance( pStreamDevice, CanvasRef::Canvas_1 );
	  playRectangleDanceDualScreen( pStreamDevice, CanvasRef::Canvas_1, CanvasRef::Canvas_2 );
//    playLogoFadeIn( pStreamDevice, CanvasRef::Canvas_1, pSurfaceFactory );
//    playSurfaceStressTest( pStreamDevice, CanvasRef::Canvas_1, pSurfaceFactory );

//	playImageStreamingTest( pStreamDevice, CanvasRef::Canvas_1, pSurfaceFactory );

	//------------------------------------------------------
	// Save stream to file
	//------------------------------------------------------

	ofstream save("output.wax", ios::out | ios::binary | ios::trunc);

    save.write( (char*) pBigBuffer, pWrite - pBigBuffer);
    
	save.close();


	//------------------------------------------------------
	// Setup graphics
	//------------------------------------------------------

//	auto pSoftSurfaceFactory = SoftSurfaceFactory::create();


	PixelDescription	format;

	auto pSDLSurf = IMG_Load("resources/splash.png");
	convertSDLFormat(&format, pSDLSurf->format);
	Surface_p pSplashSurface = pSurfaceFactory->createSurface({ .format = PixelFormat::BGRA_8, .size = SizeI(pSDLSurf->w, pSDLSurf->h) }, (uint8_t *)pSDLSurf->pixels, pSDLSurf->pitch, &format);
	SDL_FreeSurface(pSDLSurf);



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

	pStreamPlayer->setStoreDirtyRects(true);
	
	while( !bQuit )
	{
		translateEvents( pInput, pRoot );
        
        
        pStreamPump->pumpFrame();

        pOutput1->setImage(nullptr);
        pOutput1->setImage(pStreamOutputCanvas1);

		pOutput2->setImage(nullptr);
		pOutput2->setImage(pStreamOutputCanvas2);

		
		SDL_LockSurface(pWinSurf);
		pRoot->render();
		SDL_UnlockSurface(pWinSurf);

		int				nRects1,nRects2;
		const RectI	* pRects1, *pRects2;
		
		std::tie(nRects1,pRects1) = pStreamPlayer->dirtyRects(wg::CanvasRef::Canvas_1);
		std::tie(nRects2,pRects2) = pStreamPlayer->dirtyRects(wg::CanvasRef::Canvas_2);

		
		
		
		SDL_Rect	r;
		r.x = 0;
		r.y = 0;
		r.w = width;
		r.h = height;
		SDL_UpdateWindowSurfaceRects(pWin, &r, 1);


//		updateWindowRects( pRoot, pWin );

//		pStreamLogger->logAll();

		SDL_Delay(10);
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

//____ playImageStreamingTest() _______________________________________________

void playImageStreamingTest(GfxDevice_p pDevice, CanvasRef canvasRef, SurfaceFactory_p pFactory )
{
	SDL_Surface * pLogoImg = IMG_Load( "IDR_TOPLOGO_NARROW_SUMMIT.png" );
//    convertSDLFormat( &format, pFontSurf->format );

	SizeI logoSize = SizeI(pLogoImg->w,pLogoImg->h);

	SoftSurface_p pOrgSurf = SoftSurface::create( logoSize, PixelFormat::BGRA_8, (unsigned char*) pLogoImg->pixels, pLogoImg->pitch);
	
	SDL_FreeSurface( pLogoImg );

	Surface_p pLogoSurf = pFactory->createSurface( logoSize, PixelFormat::BGRA_8 );

	pLogoSurf->copy( CoordI(), pOrgSurf );
	
	SizeI canvasSize = pDevice->canvas(canvasRef).size;

	
	pDevice->beginRender();
	pDevice->beginCanvasUpdate(canvasRef);
	
	pDevice->setBlitSource(pLogoSurf);
	pDevice->blit( {0,0} );
	
	pDevice->endCanvasUpdate();
	
	pDevice->endRender();
}


//____ playSurfaceStressTest() _________________________________________________

void playSurfaceStressTest(GfxDevice_p pDevice, CanvasRef canvasRef, SurfaceFactory_p pFactory )
{
    SDL_Surface * pLogoImg = IMG_Load( "resources/logo-200.png" );
//    convertSDLFormat( &format, pFontSurf->format );

    SizeI logoSize = SizeI(pLogoImg->w,pLogoImg->h);

    SoftSurface_p pOrgSurf = SoftSurface::create( logoSize, PixelFormat::BGRA_8, (unsigned char*) pLogoImg->pixels, pLogoImg->pitch);
    
    SDL_FreeSurface( pLogoImg );

    Surface_p pLogoSurf = pFactory->createSurface( logoSize, PixelFormat::A_8 );

    pLogoSurf->copy( CoordI(), pOrgSurf );
    
    SizeI canvasSize = pDevice->canvas(canvasRef).size;

    int ticker = 0;
    int length = 600;
    
    Surface_p pOldCanvas;
    
    SizeI backCanvasSize( 256,256 );
    
    while (ticker < length)
    {
        auto pCanvas = pFactory->createSurface( backCanvasSize, PixelFormat::RGB_565_bigendian );
        
        
        
        pDevice->beginRender();
        pDevice->beginCanvasUpdate(pCanvas);
        
        pDevice->fill(Color::Black);

        if( pOldCanvas )
        {
            pDevice->setBlitSource(pOldCanvas);
            pDevice->setTintColor(HiColor(4096,3800,3500,3930));
//            pDevice->blit( CoordI( (canvasSize.w - logoSize.w)/2, (canvasSize.h - logoSize.h)/2 ));
            pDevice->rotScaleBlit( backCanvasSize*64, 3, 1.02f );
        }

        
        pDevice->setBlitSource(pLogoSurf);
        pDevice->setTintColor(HiColor(4096,4096,4096,4096));
//        pDevice->rotScaleBlit( canvasSize, -ticker, 1.f );
//        pDevice->blit( CoordI( (canvasSize.w - logoSize.w)/2, (canvasSize.h - logoSize.h)/2 ));

        float val = sin(ticker*0.1f);
        
        SizeI wantedSize( logoSize.w + (logoSize.w/2 * val), logoSize.h );
        
        pDevice->stretchBlit( RectI( (canvasSize.w - wantedSize.w)/2, (canvasSize.h - wantedSize.h)/2, wantedSize )*64 );
        
        pDevice->endCanvasUpdate();
        pOldCanvas = pCanvas;

        pDevice->beginCanvasUpdate(canvasRef);
        
        
        pDevice->setBlitSource(pCanvas);
        pDevice->blit( {0,0}, RectI((backCanvasSize.w-canvasSize.w)/2,(backCanvasSize.h-canvasSize.h)/2, canvasSize)*64 );
        
        pDevice->endCanvasUpdate();
        
        pDevice->endRender();
        ticker++;
    }
}

//____ playLogoFadeIn() _________________________________________________

void playLogoFadeIn(GfxDevice_p pDevice, CanvasRef canvasRef, SurfaceFactory_p pFactory )
{
    SDL_Surface * pLogoImg = IMG_Load( "resources/logo-200.png" );
//	SDL_Surface * pLogoImg = IMG_Load( "softube_logo_transparent_small.png" );

	//    convertSDLFormat( &format, pFontSurf->format );

    SizeI logoSize = SizeI(pLogoImg->w,pLogoImg->h);

    SoftSurface_p pOrgSurf = SoftSurface::create( logoSize, PixelFormat::BGRA_8, (unsigned char*) pLogoImg->pixels, pLogoImg->pitch);
    
    SDL_FreeSurface( pLogoImg );

    Surface_p pLogoSurf = pFactory->createSurface( logoSize, PixelFormat::A_8 );

    pLogoSurf->copy( CoordI(), pOrgSurf );
    
    SizeI canvasSize = pDevice->canvas(canvasRef).size;

    int ticker = 0;
    int length = 30;
    while (ticker < length)
    {
        pDevice->beginRender();
        pDevice->beginCanvasUpdate(canvasRef);
        
        pDevice->fill(Color::Black);
        
        pDevice->setBlitSource(pLogoSurf);
        pDevice->setTintColor(HiColor(4096,4096,4096,ticker*4096/length));
        pDevice->blit( CoordSPX( (canvasSize.w - logoSize.w)/2*64, (canvasSize.h - logoSize.h)/2*64 ));
        
        pDevice->endCanvasUpdate();
        pDevice->endRender();
        ticker++;
    }
}

//____ playRectangleDance() _________________________________________________

void playRectangleDance(GfxDevice_p pDevice, CanvasRef canvasRef )
{
	int ticker = 0;
	SizeSPX spriteSize(30*64, 30*64);
    SizeSPX canvasSize = pDevice->canvas(canvasRef).size;
	SizeSPX moveDim(canvasSize.w - spriteSize.w, canvasSize.h - spriteSize.h);

	while (ticker < 600)
	{
		pDevice->beginRender();
        pDevice->beginCanvasUpdate(canvasRef);
        
		pDevice->fill(Color::Black);
		pDevice->fill({ positionSprite(moveDim, ticker, 0, 3),spriteSize }, Color::Red);
		pDevice->fill({ positionSprite(moveDim, ticker, 1, 3),spriteSize }, Color::Green);
		pDevice->fill({ positionSprite(moveDim, ticker, 2, 3),spriteSize }, Color::Blue);

        pDevice->endCanvasUpdate();
		pDevice->endRender();
		ticker++;
	}
}

//____ playDRectangleDanceDualScreen() _________________________________________________

void playRectangleDanceDualScreen(GfxDevice_p pDevice, CanvasRef canvasRef1, CanvasRef canvasRef2 )
{
	int ticker = 0;
	SizeSPX spriteSize1(40*64, 40*64);
	SizeSPX canvasSize1 = pDevice->canvas(canvasRef1).size;
	SizeSPX moveDim1(canvasSize1.w - spriteSize1.w, canvasSize1.h - spriteSize1.h);

	SizeSPX spriteSize2(20*64, 20*64);
	SizeSPX canvasSize2 = pDevice->canvas(canvasRef2).size;
	SizeSPX moveDim2(canvasSize2.w - spriteSize2.w, canvasSize2.h - spriteSize2.h);

	while (ticker < 600)
	{
		pDevice->beginRender();

		pDevice->beginCanvasUpdate(canvasRef1);
		
		pDevice->fill(Color::Black);
		pDevice->fill({ positionSprite(moveDim1, ticker, 0, 3),spriteSize1 }, Color::Red);
		pDevice->fill({ positionSprite(moveDim1, ticker, 1, 3),spriteSize1 }, Color::Green);
		pDevice->fill({ positionSprite(moveDim1, ticker, 2, 3),spriteSize1 }, Color::Blue);

		pDevice->endCanvasUpdate();

		pDevice->beginCanvasUpdate(canvasRef2);
		
		pDevice->fill(Color::Black);
		pDevice->fill({ positionSprite(moveDim2, ticker, 8, 9),spriteSize2 }, Color(255,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 7, 9),spriteSize2 }, Color(224,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 6, 9),spriteSize2 }, Color(192,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 5, 9),spriteSize2 }, Color(160,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 4, 9),spriteSize2 }, Color(128,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 3, 9),spriteSize2 }, Color(96,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 2, 9),spriteSize2 }, Color(64,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 1, 9),spriteSize2 }, Color(32,0,0) );
		pDevice->fill({ positionSprite(moveDim2, ticker, 0, 9),spriteSize2 }, Color(0,0,0) );

		pDevice->endCanvasUpdate();


		pDevice->endRender();
		ticker++;
	}
}


//____ playScroll() _________________________________________________

void playScroll(GfxDevice_p pDevice, RectI canvas )
{
    int ticker = 0;
    SizeI spriteSize(30, 30);
    SizeI moveDim(canvas.w - spriteSize.w, canvas.h - spriteSize.h);

    
    while (ticker < 600)
    {
        CoordI pos = canvas.pos();
        pos.x += ticker;
        pos.y += (canvas.h - spriteSize.h) / 2;
        
        pDevice->beginRender();
        pDevice->beginCanvasUpdate(CanvasRef::Canvas_1);
        
        pDevice->fill( canvas, Color::Black);
        pDevice->fill( RectI( pos, spriteSize ), Color::Red);

        pDevice->endCanvasUpdate();
        pDevice->endRender();
        ticker++;
    }
}


CoordSPX positionSprite(SizeSPX dimensions, int tick, int nb, int amount)
{
	const float PI = 3.14159265f;

	CoordSPX	radius = { dimensions.w / 2, dimensions.h / 2 };

	if (tick < 90)
		radius *= sin(tick*PI / 180);

	if (tick > 600 - 90)
		radius *= 1.f - sin((tick - (600 - 90))*PI / 180);


	CoordSPX c;
	c.x = (int)(cos((tick + nb*360.f / amount)*PI / 180)*radius.x + dimensions.w / 2);
	c.y = (int)(sin((tick + nb*360.f / amount)*PI / 180)*radius.y + dimensions.h / 2);
	return c;
}

//____ play general stuff ________________________________________________

void playDelayFrames(GfxDevice_p pDevice, int nFrames)
{
	for (int i = 0; i < nFrames; i++)
	{
		pDevice->beginRender();
		pDevice->endRender();
	}
}


