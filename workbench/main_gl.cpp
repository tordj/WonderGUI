
#include <cstdlib>
#include <stdio.h>

#ifdef WIN32
#	include <SDL.h>
#	include <SDL_image.h>
#	include <GL/glew.h>
#else
#	ifdef __APPLE__
#		include <SDL2/SDL.h>
#		include <SDL2_image/SDL_image.h>
#	else
#		include <SDL2/SDL.h>
#		include <SDL2/SDL_image.h>
#	endif
#endif

#include <wondergui.h>

#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>
#include <wg_glgfxdevice.h>
#include <testwidget.h>


using namespace wg;

void 			translateEvents( const InputHandler_p& pInput, const RootPanel_p& pRoot );
MouseButton 	translateMouseButton( Uint8 button );
void 			updateWindowRects( const RootPanel_p& pRoot, SDL_Window * pWindow );
void 			myButtonClickCallback( const Msg_p& pMsg );
void * 			loadFile( const char * pPath );
Blob_p 			loadBlob( const char * pPath );
void			convertSDLFormat( PixelFormat * pWGFormat, const SDL_PixelFormat * pSDLFormat );

void addResizablePanel( const FlexPanel_p& pParent, const Widget_p& pChild, const MsgRouter_p& pMsgRouter );
Surface_p generateTestSurface();


bool	bQuit = false;


void freeSDLSurfCallback( void * pSDLSurf )
{
	SDL_FreeSurface( (SDL_Surface*) pSDLSurf );
}

InputHandler * pDebug;

//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{ 

	struct Test1
	{
		bool a;
		int		i1;
		bool b;
		int		i2;
		bool c;
		bool d;
	};

	struct Test2
	{
		int		i1;
		int		i2;
		bool a;
		bool b;
		bool c;
		bool d;
	};

	int big = sizeof(Test1);
	int small = sizeof(Test2);

	printf("Big: %d. Small: %d\n", big, small);


	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 1920, height = 1080;
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(  SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(  SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 0 );
    
    SDL_GLContext context = SDL_GL_CreateContext( pWin );

#ifdef WIN32  
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
#endif

	IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG );

	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------

	Base::init();
//	Base::initFreeType();
//	VectorFont::setSurfaceFactory( GlSurfaceFactory::create() );

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
	
	
	pInput->mapCommand( SDLK_ESCAPE, MODKEY_NONE, EditCmd::Escape );

	pInput->mapCommand( SDLK_x, MODKEY_CTRL, EditCmd::Cut );
	pInput->mapCommand( SDLK_c, MODKEY_CTRL, EditCmd::Copy );
	pInput->mapCommand( SDLK_v, MODKEY_CTRL, EditCmd::Paste );

	pInput->mapCommand( SDLK_a, MODKEY_CTRL, EditCmd::SelectAll );
	
	pInput->mapCommand( SDLK_z, MODKEY_CTRL, EditCmd::Undo );
	pInput->mapCommand( SDLK_z, MODKEY_CTRL_SHIFT, EditCmd::Redo );

	

	GlGfxDevice_p pGfxDevice = GlGfxDevice::create( Size(width,height) );

	RootPanel_p pRoot = RootPanel::create( pGfxDevice );
	
	Base::inputHandler()->setFocusedWindow( pRoot );

	// 
	
	MsgLogger_p pLogger = MsgLogger::create( std::cout );
	pLogger->logAllMsgs();
	pLogger->ignoreMsg( MsgType::Tick );
	
	Base::msgRouter()->broadcastTo( pLogger );


	// Init font

	char * pFontSpec = (char*)loadFile("../resources/anuvverbubbla_8x8.fnt");

    assert( glGetError() == 0 );

    
	SDL_Surface * pFontSurf = IMG_Load( "../resources/anuvverbubbla_8x8.png" );
	GlSurface_p pFontImg = GlSurface::create( Size(pFontSurf->w,pFontSurf->h), PixelType::BGRA_8, (uint8_t*) pFontSurf->pixels, pFontSurf->pitch );
	SDL_FreeSurface( pFontSurf );
    assert( glGetError() == 0 );
	BitmapFont_p pBmpFont = BitmapFont::create( pFontImg, pFontSpec );
    assert( glGetError() == 0 );

//	Blob_p pFontFile = loadBlob("../resources/DroidSans.ttf");
	
//	VectorFont_p pFont = VectorFont::create( pFontFile, 1 );


	TextStyle_p pStyle = TextStyle::create();
	pStyle->setFont(pBmpFont);
	pStyle->setSize(10);
	Base::setDefaultStyle(pStyle);

	// Init skins

	PixelFormat	format;

	SDL_Surface * pSDLSurf = IMG_Load( "../resources/simple_button.bmp" );
	convertSDLFormat( &format, pSDLSurf->format );
	GlSurface_p pButtonSurface = GlSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pSimpleButtonSkin = BlockSkin::createClickableFromSurface( pButtonSurface, 0, Border(3) );
	pSimpleButtonSkin->setContentPadding( Border(5) );

	pSDLSurf = IMG_Load( "../resources/state_button.bmp" );
	convertSDLFormat( &format, pSDLSurf->format );
	GlSurface_p pStateButtonSurface = GlSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pStateButtonSkin = BlockSkin::createClickSelectableFromSurface( pStateButtonSurface, 0, Border(3) );
	pStateButtonSkin->setContentPadding( Border(5) );
    
	pSDLSurf = IMG_Load( "../resources/grey_pressable_plate.bmp" );
	convertSDLFormat( &format, pSDLSurf->format );
	GlSurface_p pPressablePlateSurface = GlSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pPressablePlateSkin = BlockSkin::createClickableFromSurface( pPressablePlateSurface, 0, Border(3) );
	pPressablePlateSkin->setContentPadding( Border(3) );
    
	pSDLSurf = IMG_Load( "../resources/list_entry.png" );
	convertSDLFormat( &format, pSDLSurf->format );
	GlSurface_p pListEntrySurface = GlSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	Skin_p pListEntrySkin = BlockSkin::createClickableFromSurface( pListEntrySurface, 0, Border(3) );
    
	pSDLSurf = IMG_Load( "../resources/frog.jpg" );
	convertSDLFormat( &format, pSDLSurf->format );
	GlSurface_p pImgSurface = GlSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pImgSkin = BlockSkin::createStaticFromSurface( pImgSurface, Border(3) );

	//------------------------------------------------------
	// Setup a simple GUI consisting of a filled background and 
	// a button using scaled bitmaps.
	//------------------------------------------------------

	FlexPanel_p pFlexPanel = FlexPanel::create();
	pFlexPanel->setSkin( ColorSkin::create(Color::Beige)/*pImgSkin*/  );
	pRoot->child = pFlexPanel;



	Image_p pImage = Image::create();
	pImage->setSkin( pSimpleButtonSkin );
	pFlexPanel->children.addMovable( pImage, Rect(0,0,80,33), Origo::Center );


//	pRoot->msgRouter()->AddCallback( MsgFilter::select(), pButton, myButtonClickCallback );


	// Test transparency issue



	{
		PackPanel_p pVert = PackPanel::create();
		PackPanel_p pHorr = PackPanel::create();
		
		Filler_p pFillerEast = Filler::create();
		Filler_p pFillerSouth = Filler::create();
		
		pFillerEast->setSkin( pPressablePlateSkin );
		pFillerSouth->setSkin( pPressablePlateSkin );
		
		pVert->setOrientation( Orientation::Vertical );
		
		pVert->children << pHorr;
		pVert->children << pFillerSouth;

		TextEditor_p pText = TextEditor::create();
		pText->setSkin( ColorSkin::create( Color::Black ) );
		
		
		TextStyle_p pBig = TextStyle::create();
		pBig->setSize( 16 );
		
		TextStyle_p pRed = TextStyle::create();
		pRed->setColor( Color::Red );
		pRed->setSize( 25 );

		TextStyle_p pAnuv = TextStyle::create();
		pAnuv->setFont( pBmpFont );

		TextStyle_p pLink = TextStyle::create();
		pLink->setColor(Color::Green );
		pLink->setLink( TextLink::create( "www.somewhere.net" ) );

		pText->text.set( "This is a\npiece of TEXT with LINK" );
		pText->text.setCharStyle( pBig, 5, 2 );
		pText->text.setCharStyle( pRed, 10, 3 );
		pText->text.setCharStyle( pAnuv, 19, 4 );
		pText->text.setCharStyle( pLink, 29, 4 );


		pHorr->children << pText;
		pHorr->children << pFillerEast;

		
//		pFlexPanel->addWidget( pVert, Origo::NorthWest, Origo::SouthEast );
	
		pText->grabFocus();
    }


	{
		Image_p pImage = Image::create();
		pImage->setImage( generateTestSurface() );
		pFlexPanel->children.addPinned(pImage, { Origo::NorthWest,20,20 }, { Origo::SouthEast,-20,-20 } );
	}


/*
    {
        TestWidget_p pTest = TestWidget::create();
        pFlexPanel->addWidget( pTest, Origo::NorthWest, Origo::SouthEast, Border(20) );
        pTest->start();
    }
*/

/*	
	{
		FlexPanel_p pExtraFlex = FlexPanel::create();
		pExtraFlex->setSkin( ColorSkin::create( Color(0,0,0,128)));

		TextEditor_p pText = TextEditor::create();
		
		TextStyle_p pBig = TextStyle::create();
		pBig->setSize( 16 );
		
		TextStyle_p pRed = TextStyle::create();
		pRed->setColor( Color::Red );
		pRed->setSize( 25 );

		TextStyle_p pAnuv = TextStyle::create();
		pAnuv->setFont( pBmpFont );

		pText->text.set( "This is a\npiece of TEXT" );
		pText->text.setCharStyle( pBig, 5, 2 );
		pText->text.setCharStyle( pRed, 10, 3 );
		pText->text.setCharStyle( pAnuv, 19, 20 );
		
		pExtraFlex->addWidget( pText, Rect( 10,10,100,100) );

		addResizablePanel( pFlexPanel, pExtraFlex, Base::msgRouter() );

		pText->grabFocus();

		translateEvents( pInput, pRoot );

		pText->text.caretTextBegin();
		pText->text.caretLineEnd();
	}
*/
/*
	{
		VolumeMeter_p p = VolumeMeter::create();
		
		p->setSkin( pPressablePlateSkin );
		
		addResizablePanel( pFlexPanel, p, Base::msgRouter() );
	}
*/


/*
	SizeCapsule_p pCapsule = SizeCapsule::create();
	pCapsule->setMaxSize( Size(100,1000));
	pFlexPanel->addWidget( pCapsule );

	StackPanel_p pStack = StackPanel::create();
	pCapsule->setWidget( pStack );


	TextDisplay_p pText = TextDisplay::create();
	pText->text()->set( "THIS IS THE LONG TEXT THAT SHOULD WRAP AND BE FULLY DISPLAYED." );
	pStack->addWidget(pText);
*/
	
/*
	CheckBoxPtr pCheckbox = CheckBox::create();
	pCheckbox->Label()->set( "CHECKBOX" );
	pCheckbox->Icon()->set( pCheckboxSkin );
*/

/*
	ToggleGroup_p pGroup = ToggleGroup::create();
	

	for( int i = 0 ; i < 4 ; i++ )
	{
		ToggleButton_p pCheckbox = ToggleButton::create();
		pCheckbox->label.set( "CHECKBOX" );
		pCheckbox->setSkin( pStateButtonSkin );
		pFlexPanel->addWidget( pCheckbox, Coord(10,20*i) );
		pGroup->add( pCheckbox );
	}
*/

/*
	TextDisplay_p pText = TextDisplay::create();
	pText->text.set( "THIS IS THE TEXT\nLINE TWO.\nAN EXTRA LONG LONG LONG LINE TO TEST THE CLIPPING AND RAND BEHAVIOURS." );
	pFlexPanel->addWidget( pText, WG_NORTHWEST, WG_EAST );
	

	StandardPrinter_p pPrinter = StandardPrinter::create();
	pPrinter->setAlignment( WG_CENTER );
	pText->text.setPrinter( pPrinter );


	ValueDisplay_p pValue = ValueDisplay::create();
	pValue->value.setFormatter( TimeFormatter::create("%2H:%2M:%2S"));
	pValue->value.set(3600+60+12);
	pValue->value.setPrinter( pPrinter );
	pFlexPanel->addWidget( pValue, WG_WEST, WG_SOUTHEAST );
*/

/*
	FpsDisplay_p pFps = FpsDisplay::create();
	pFps->setSkin( pPressablePlateSkin );
	pFlexPanel->addWidget( pFps, Coord(0,0), Origo::SouthWest );
*/	

	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

    int tick = 0;

	/*
		I need the following crap on my Windows dev machine to
		make something that resembles single buffering work :(

		Apparently single buffering in SDL2/Windows doesn't work on my dev machine, 
		so I get double buffering although I specify single buffering when initializing.
		(don't know if it is SDL's, Windows' or crappy device drivers fault, 
		it works fine in SDL2/Linux on same hardware)

		So I render to the front buffer instead, but apparently nothing will be displayed
		until the rendering pipeline has been filled and flushed twice... so we just clear the
		color buffer and flush() to make sure that everything is displayed on first render call.
	*/

	glDrawBuffer(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glFlush();

	int		topWave[2001];
	int		bottomWave[2001];

	WaveLine	topLine, bottomLine;

	topLine.color = { 255,0,0,128 };
	topLine.thickness = 10.f;
	topLine.pWave = topWave;
	topLine.length = 2001;

	bottomLine.color = Color::White;
	bottomLine.thickness = 5.2f;
	bottomLine.pWave = bottomWave;
	bottomLine.length = 2001;




	auto pMyCanvas = GlSurface::create({ 400,400 }, PixelType::BGR_8);
//	pMyCanvas->setScaleMode(ScaleMode::Interpolate);
	pMyCanvas->setScaleMode(ScaleMode::Nearest);


	while( !bQuit ) 
	{
		for (int i = 0; i < 2001; i++)
		{
			topWave[i] = (int)(((sin((i + tick) / 10.0) * 60) * 256)+ ((sin((i + tick/3) / 20.0) * 80) * 256));
			bottomWave[i] = (int)(((400 - i/4 + sin((i - tick/2) / 20.0) * 6) * 256) + ((sin(i*2 / 20.0) * 34) * 256) + ((sin(tick / 18.0 + i/200.0) * 100) * 256));
		}


		translateEvents( pInput, pRoot );

//		pGfxDevice->setCanvas(pMyCanvas);

//		pRoot->render();

//		pGfxDevice->setCanvas({ 400,400 });

		pGfxDevice->beginRender();

		pImgSkin->render(pGfxDevice, { 0,0,1920,1080 }, StateEnum::Normal, { 0,0,1920,1080 });

		pGfxDevice->clipDrawHorrWave({ 10,0,1900,1080 }, { 0,500 }, 1900, topLine, bottomLine, { 0,0,255,128 }, Color::Yellow);

//		pGfxDevice->stretchBlitSubPixelWithInvert(pMyCanvas, 0,0,400,400, 0,0, 200, 200 );
//		pGfxDevice->blit(pMyCanvas, { 0,0,400,400 }, { 0,0 });
//		pGfxDevice->stretchBlit(pMyCanvas, { 0,0,400,400 }, { 0,0,200,200 });
		pGfxDevice->endRender();


//		updateWindowRects( pRoot, pWin );

 //       glClearColor(0.1f, 0.3f, 0.3f, 1.0f );
 //       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//		glFlush();
/*
         pGfxDevice->beginRender();

//        pGfxDevice->setTintColor(Color::Red );
        
//        pGfxDevice->fill( Rect(1,1,320-2,400-2), Color::Aquamarine);

        
//       pGfxDevice->wg::GfxDevice::blit( pButtonSurface, 50, 50 );
//        pGfxDevice->wg::GfxDevice::stretchBlit(pButtonSurface, Rect(50,50,160,40));

        

        Color plotColors[400];
        Coord plotCoords[400];
        
        for( int i = 0 ; i < 400 ; i++ )
        {
            plotColors[i] = Color::Red;
            plotCoords[i] = Coord(i,i);
        }
        
        
//       pGfxDevice->clipPlotPixels( Rect(0,0,width,height), 100, plotCoords, plotColors);

        
//        for(int x = 0 ; x < 200 ; x++ )
//        {
//            pGfxDevice->clipDrawVertLine(Rect(0,0,width,height), Coord(x*2,x), 40, Color::Aquamarine);
//        }

        
//        pGfxDevice->clipDrawHorrLine(Rect(10,10,width-20,height-20), Coord(0,20), 800, Color::Aquamarine);
//        pGfxDevice->clipDrawVertLine(Rect(10,10,width-20,height-20), Coord(20,0), 800, Color::Aquamarine);

        
        
//        pGfxDevice->drawLine( Coord(10,10), Coord(200,40), Color::Red, 3.f );
        
//        pGfxDevice->fill(Rect(21, 21, 100, 100 ), Color::AntiqueWhite );
//        pGfxDevice->fillSubPixel(RectF(21.8f, 21.5f, 0.4f, 0.4f ), Color::AntiqueWhite );
        
        pGfxDevice->endRender();
*/        
		SDL_Delay(20);
        tick++;
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
	
	static unsigned int oldTicks = 0;
	
	unsigned int ticks = SDL_GetTicks();
	int tickDiff;

	if( oldTicks == 0 )
		tickDiff = 0;
	else
		tickDiff = (int) (ticks - oldTicks);		
	oldTicks = ticks;

	Base::msgRouter()->post( TickMsg::create(ticks, tickDiff) );

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
				Coord distance( e.wheel.x, e.wheel.y );
//				if( e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED )
//					distance *= -1;
			
				pInput->setWheelRoll( 1, distance );
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
	
	const Rect * pUpdatedRects = pRoot->firstUpdatedRect();
	SDL_Rect * pSDLRects = (SDL_Rect*) Base::memStackAlloc( sizeof(SDL_Rect) * nRects );

	for( int i = 0 ; i < nRects ; i++ )
	{
		pSDLRects[i].x = pUpdatedRects[i].x;
		pSDLRects[i].y = pUpdatedRects[i].y;
		pSDLRects[i].w = pUpdatedRects[i].w;
		pSDLRects[i].h = pUpdatedRects[i].h;
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
		
	int nRead = fread( pBlob->content(), 1, size, fp );
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


Coord dragStartPos;

//____ cbInitDrag() ___________________________________________________________

void cbInitDrag( const Msg_p& _pMsg, const Object_p& pObject )
{
	Widget_p pWidget = Widget::cast(pObject);

	dragStartPos = pWidget->pos();
	printf( "DRAG START!\n" );
}

//____ cbDragWidget() __________________________________________________________

void cbDragWidget( const Msg_p& _pMsg, const Object_p& pObject )
{
	Widget_p pWidget = Widget::cast(pObject);
	
	if( _pMsg->type() != MsgType::MouseDrag || !pWidget->parent() )
		return;

	const MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);



	Coord	dragDistance = pMsg->draggedTotal();

	Coord	ofs = dragStartPos + dragDistance;

//	printf( "AccDistance: %d, %d\n", dragDistance.x, dragDistance.y );
	printf( "ofs: %d, %d   start: %d %d   distance: %d, %d\n", ofs.x, ofs.y, dragStartPos.x, dragStartPos.y, dragDistance.x, dragDistance.y );

	FlexPanel_p pFlex = FlexPanel::cast(pWidget->parent());
	int index = pFlex->children.index(pWidget);

	pFlex->children.move(index, dragStartPos + dragDistance);
}

//____ cbMoveResize() _________________________________________________________

void cbMoveResize( const Msg * _pMsg )
{
	static Coord posAtPress[MouseButton_Max];
	
//	Widget_p _pWidget = _pMsg->source();
	Object_p _pWidget = _pMsg->source();

	auto	pWidget = Widget::cast(_pWidget);
	auto	pParent = FlexPanel::cast(pWidget->parent());

	int index = pParent->children.index(pWidget);

	switch( _pMsg->type() )
	{
		case MsgType::MousePress:
		{
			auto pMsg = static_cast<const MousePressMsg*>(_pMsg);
			posAtPress[(int)pMsg->button()] = pWidget->pos();
			 
		}
		break;
		case MsgType::MouseDrag:
		{
			auto pMsg = static_cast<const MouseDragMsg*>(_pMsg);
			if( pMsg->button() == MouseButton::Right )
			{
				pParent->children.setSize( index, pWidget->size() + pMsg->draggedNow().toSize() );
			}
			else if( pMsg->button() == MouseButton::Middle )
			{
				pParent->children.setOfs( index, posAtPress[(int)MouseButton::Middle] + pMsg->draggedTotal() );
			}
		}
		break;
		case MsgType::MouseRelease:
		break;
		
		default:
		break;
	}
}


//____ addResizablePanel() _________________________________________________

void addResizablePanel( const FlexPanel_p& pParent, const Widget_p& pChild, const MsgRouter_p& pMsgRouter )
{
	auto it = pParent->children.add(pChild);

//	pHook->setSizePolicy(SizePolicy::Bound, SizePolicy::Bound);

	pMsgRouter->addRoute( pChild, cbMoveResize );
}

//____ convertSDLFormat() ______________________________________________________

void convertSDLFormat( PixelFormat * pWGFormat, const SDL_PixelFormat * pSDLFormat )
{
	pWGFormat->type = PixelType::Custom;
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

//____ generateTestSurface () _____________________________________________________

Surface_p generateTestSurface()
{
	uint8_t tex[3*3*4] = {	255,0,0, 0,255,0, 0,0,255, 0,0,
								255,128,128, 128,255,128, 128,128,255,0,0,
								255,128,0, 128,255,0, 0,128,255,0,0 };

	Surface_p p = GlSurface::create( Size(3,3), PixelType::BGR_8, tex, 11 );
	p->setScaleMode(ScaleMode::Interpolate);
	return p;
}

