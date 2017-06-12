
#include <cstdlib>
#include <stdio.h>

#ifdef WIN32
#	include <SDL.h>
#	include <SDL_image.h>
#else
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_image.h>
#endif

#include <wondergui.h>

#include <wg_softsurface.h>
#include <wg_softsurfacefactory.h>
#include <wg_softgfxdevice.h>
#include <wg_packlist.h>
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
	printf( "SizeOf Finalizer: %d\n", sizeof(Finalizer) );
	printf( "SizeOf Filler: %d\n", sizeof(Filler) );

	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 640, height = 400;
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, 0);

	SDL_Surface * pWinSurf = SDL_GetWindowSurface( pWin );

	IMG_Init( IMG_INIT_JPG | IMG_INIT_PNG );

	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------

	Base::init();
	Base::initFreeType();
	VectorFont::setSurfaceFactory( SoftSurfaceFactory::create() );

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


	Object_wp wpFin1;
	Finalizer_wp wpFin2;

	auto pButton = Button::create();
	auto pFinalizer1 = Finalizer::create([](Object * pObj) { printf("Finalizing object %p of type %s\n", pObj, pObj->className()); });
	auto pFinalizer2 = Finalizer::create([](Object * pObj) { printf("Finalizing object %p of type %s\n", pObj, pObj->className()); });
	{


		pFinalizer1->attach(pButton);

		pFinalizer2->attach(pFinalizer1);

		wpFin1 = pFinalizer1;
		wpFin2 = pFinalizer2;
	}

	printf("Killing button\n");
	pButton = nullptr;
	pFinalizer1 = nullptr;

	printf("Weak pointer manip\n");
//	wpFin1 = wpFin2;
	wpFin2 = nullptr;
	wpFin1 = nullptr;

	PixelType type = PixelType::Unknown;

	if( pWinSurf->format->BitsPerPixel == 32 )
		type = PixelType::BGRA_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		type = PixelType::BGR_8;
		
	Blob_p pCanvasBlob = Blob::create( pWinSurf->pixels, 0);	
	SoftSurface_p pCanvas = SoftSurface::create( Size(pWinSurf->w,pWinSurf->h), type, pCanvasBlob, pWinSurf->pitch );

	SoftGfxDevice_p pGfxDevice = SoftGfxDevice::create( pCanvas );

	RootPanel_p pRoot = RootPanel::create( pGfxDevice );

//	pRoot->setDebugMode(true);
	
	Base::inputHandler()->setFocusedWindow( pRoot );

	// 
	
	MsgLogger_p pLogger = MsgLogger::create( std::cout );
	pLogger->logAllMsgs();
	pLogger->ignoreMsg( MsgType::Tick );
	
	Base::msgRouter()->broadcastTo( pLogger );

	// Init font

	PixelFormat	format;

	char * pFontSpec = (char*)loadFile("../resources/anuvverbubbla_8x8.fnt");

	SDL_Surface * pFontSurf = IMG_Load( "../resources/anuvverbubbla_8x8.png" );
//	convertSDLFormat( &format, pFontSurf->format );
	SoftSurface_p pFontImg = SoftSurface::create( Size(pFontSurf->w,pFontSurf->h), PixelType::BGRA_8, (unsigned char*) pFontSurf->pixels, pFontSurf->pitch);
	SDL_FreeSurface( pFontSurf );
		
	BitmapFont_p pBmpFont = BitmapFont::create( pFontImg, pFontSpec );


	Blob_p pFontFile = loadBlob("../resources/DroidSans.ttf");
	
	VectorFont_p pFont = VectorFont::create( pFontFile, 1 );


	TextStyle_p pStyle = TextStyle::create();
	pStyle->setFont(pFont);
	pStyle->setSize(16);
	Base::setDefaultStyle(pStyle);

/*
	StdTextMapper_p pMapper = StdTextMapper::create();
	pMapper->setSelectionBackColor(Color(0,0,255,128), BlendMode::Blend);
	Base::setDefaultTextMapper(pMapper);
*/

	StdTextMapper::cast(Base::defaultTextMapper())->setSelectionBack(Color(255,255,255,255), BlendMode::Invert);

	// Init skins


	SDL_Surface * pSDLSurf = IMG_Load( "../resources/simple_button.bmp" );
	convertSDLFormat( &format, pSDLSurf->format );
	SoftSurface_p pButtonSurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pSimpleButtonSkin = BlockSkin::createClickableFromSurface( pButtonSurface, 0, Border(3) );
	pSimpleButtonSkin->setContentPadding( Border(5) );

	pSDLSurf = IMG_Load( "../resources/state_button.bmp" );
	convertSDLFormat( &format, pSDLSurf->format );
	SoftSurface_p pStateButtonSurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pStateButtonSkin = BlockSkin::createClickSelectableFromSurface( pStateButtonSurface, 0, Border(3) );
	pStateButtonSkin->setContentPadding( Border(5) );

	pSDLSurf = IMG_Load( "../resources/grey_pressable_plate.bmp" );
	convertSDLFormat( &format, pSDLSurf->format );
	SoftSurface_p pPressablePlateSurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pPressablePlateSkin = BlockSkin::createClickableFromSurface( pPressablePlateSurface, 0, Border(3) );
	pPressablePlateSkin->setContentPadding( Border(3) );
	
	pSDLSurf = IMG_Load( "../resources/list_entry.png" );
	convertSDLFormat( &format, pSDLSurf->format );
	SoftSurface_p pListEntrySurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGRA_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	Skin_p pListEntrySkin = BlockSkin::createClickSelectableFromSurface( pListEntrySurface, 0, Border(2) );

	pSDLSurf = IMG_Load( "../resources/frog.jpg" );
	convertSDLFormat( &format, pSDLSurf->format );
	SoftSurface_p pImgSurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	BlockSkin_p pImgSkin = BlockSkin::createStaticFromSurface( pImgSurface, Border(3) );

	pSDLSurf = IMG_Load( "../resources/up_down_arrow.png" );
	convertSDLFormat( &format, pSDLSurf->format );
	SoftSurface_p pUpDownArrowSurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGRA_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	Skin_p pUpDownArrowSkin = BlockSkin::createSelectableFromSurface( pUpDownArrowSurface, 0, Border(0) );

	pSDLSurf = IMG_Load( "../resources/simple_icon.png" );
	convertSDLFormat( &format, pSDLSurf->format );
	SoftSurface_p pSimpleIconSurface = SoftSurface::create( Size( pSDLSurf->w, pSDLSurf->h ), PixelType::BGRA_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, &format );
	SDL_FreeSurface( pSDLSurf );
	Skin_p pSimpleIconSkin = BlockSkin::createStaticFromSurface( pSimpleIconSurface, Border(0) );

	//------------------------------------------------------
	// Setup a simple GUI consisting of a filled background and 
	// a button using scaled bitmaps.
	//------------------------------------------------------

	FlexPanel_p pFlexPanel = FlexPanel::create();
	pFlexPanel->setSkin( pImgSkin /*ColorSkin::create(Color::Burlywood)*/ );
	pRoot->child = pFlexPanel;


/*	Filler_p pBackground = Filler::create();
	pBackground->setSkin( ColorSkin::create(Color::aqua) );
	pFlexPanel->addWidget(pBackground, WG_NORTHWEST, Coord(), WG_SOUTHEAST, Coord());
*/

	Button_p pImage0 = Button::create();
	pImage0->setSkin(pSimpleButtonSkin);
	pFlexPanel->children.addMovable(pImage0, Rect(0, 0, 80 * 2, 33 * 2), Origo::East, Origo::East);

	Base::msgRouter()->addRoute(MsgType::Select, [&](const Msg_p& pMsg) { bQuit = true; });


//	Image_p pImage = Image::create();
//	pImage->setSkin( pSimpleButtonSkin );
//	pFlexPanel->children.addMovable( pImage, Rect(0,0,80*2,33*2), Origo::Center, Origo::Center );


//	pRoot->msgRouter()->AddCallback( MsgFilter::select(), pButton, myButtonClickCallback );


	// Test transparency issue


/*
	{
		PackPanel_p pVert = PackPanel::create();
		PackPanel_p pHorr = PackPanel::create();
		
		Filler_p pFillerEast = Filler::create();
		pFillerEast->setSkin( pPressablePlateSkin );

		Filler_p pFillerSouth = Filler::create();
		pFillerSouth->setSkin( pPressablePlateSkin );
		
		pVert->setOrientation( Orientation::Vertical );
		
		PackPanel_p pHorr2 = PackPanel::create();

		LineEditor_p pEditLine = LineEditor::create();
		pEditLine->setSkin(ColorSkin::create(Color::Black));
		pEditLine->text.setMaxChars(8);
		pHorr2->children.add( pEditLine );

		Filler_p pFillerEast2 = Filler::create();
		pFillerEast2->setSkin( pPressablePlateSkin );
		pHorr2->children.add( pFillerEast2 );

		Filler_p pFillerMid = Filler::create();
		pFillerMid->setSkin(pPressablePlateSkin);



		pVert->children.add(pHorr2);
		pVert->children.add(pFillerMid);
		pVert->children.add( pHorr );
		pVert->children.add( pFillerSouth );

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


		pHorr->children.add( pText );
		pHorr->children.add( pFillerEast );

		
		pFlexPanel->children.addPinned( pVert, Origo::NorthWest, Origo::SouthEast );
	
		pEditLine->grabFocus();
		for (auto it = pVert->children.begin(); it != pVert->children.end(); it++ )
			printf("%s\n", it->className());


		for (auto& it : pVert->children)
		{
//			Rect r = it.geo;
			printf("%s\n", it.className() );
		}

	}
	*/

	{
		PackPanel_p pPanel = PackPanel::create();
		pFlexPanel->children.addPinned(pPanel, Origo::NorthWest, Origo::SouthEast);

		Widget_p widgets[5];

		char temp[100];

		for (int i = 0; i < 5; i++)
		{
			sprintf(temp, "%d", i);

			TextDisplay_p p = TextDisplay::create();
			p->setSkin(pPressablePlateSkin);
			p->text.set(temp);
			widgets[i] = p;
		}
		

//		auto it = pPanel->children.begin();
//		for (int i = 0; i < 5; i++)

		auto it = pPanel->children << widgets[0];

		pPanel->children.insert( 1, &widgets[1], 4 );

//			it = pPanel->children.insert(it, widgets[i]);

		pPanel->children.remove( 1,2 );

	}



	ScrollPanel_p pScrollPanel;
	{
		Scrollbar_p pScrollbar = Scrollbar::create();
		pScrollbar->setBackgroundSkin( ColorSkin::create( Color::Green ));
		pScrollbar->setHandleSkin( ColorSkin::create( Color::Red ));
		pScrollPanel = ScrollPanel::create();
		pScrollPanel->vscrollbar = pScrollbar;

		pFlexPanel->children.addPinned(pScrollPanel, { 0,0.75 }, Origo::SouthEast);
	}




	{
		PackList_p pList = PackList::create();

		BoxSkin_p pHeaderSkin = BoxSkin::create( Color::Aquamarine, Border(2), Color::DarkRed );
		pHeaderSkin->setContentPadding( 8 );

		pList->header.label.set("Label");
		pList->header.setSkin( pHeaderSkin );
		pList->header.arrow.set( pUpDownArrowSkin, Origo::East );
		pList->header.icon.set( pSimpleIconSkin );
		
		pList->setSortFunction( sortWidgets );
		pList->setSelectMode( SelectMode::MultiEntries );

		BoxSkin_p pListSkin = BoxSkin::create( Color::Chocolate, Border(2), Color::Yellow );
		pListSkin->setContentPadding( 8 );
		pList->setSkin( pListSkin );

		pList->setEntrySkin( pListEntrySkin );
		pList->setLassoSkin( ColorSkin::create( Color(0,0,0,128)));


		BoxSkin_p pEditBoxSkin = BoxSkin::create(Color::Black, 1, Color::White );
		pEditBoxSkin->setContentPadding(2);

		for( int i = 0 ; i < 20 ; i++ )
		{
			auto p = PackPanel::create();
			p->setId(i);


			auto p1 = TextDisplay::create();

			char label[15];
			sprintf(label, "Entry %d", i );

			p1->text.set( label );

			p->children.add(p1);

			auto p2 = TextEditor::create();
			p2->setSkin( pEditBoxSkin );
			p2->text.set( "Edit me!");
			p->children.add(p2);

			pList->children.add(p);
		}


		pScrollPanel->view = pList;

//		pScrollPanel->setStepFunc([](Direction dir, int steps) { return 10*steps; });
//		pScrollPanel->setJumpFunc([](Direction dir, int steps) { return 100*steps; });
		pScrollPanel->setWheelRollFunc([](Direction dir, int steps) { return 1*steps; });

//		pFlexPanel->addWidget( pList, FlexOrigo(0,0.75), Origo::SouthEast);

	}


 /*  {
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
				if( e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED )
					distance *= -1;
			
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
/*
	Widget_p pWidget = Widget::cast(pObject);

	FlexHook_p pHook = FlexHook::cast(pWidget->hook());


	dragStartPos = pHook->floatOfs();
	printf( "DRAG START!\n" );
*/
}

//____ cbDragWidget() __________________________________________________________

void cbDragWidget( const Msg_p& _pMsg, const Object_p& pObject )
{
/*
	Widget_p pWidget = Widget::cast(pObject);
	
	if( _pMsg->type() != MsgType::MouseDrag || !pWidget->parent() )
		return;

	const MouseDragMsg_p pMsg = MouseDragMsg::cast(_pMsg);



	Coord	dragDistance = pMsg->draggedTotal();

	Coord	ofs = dragStartPos + dragDistance;

//	printf( "AccDistance: %d, %d\n", dragDistance.x, dragDistance.y );
	printf( "ofs: %d, %d   start: %d %d   distance: %d, %d\n", ofs.x, ofs.y, dragStartPos.x, dragStartPos.y, dragDistance.x, dragDistance.y );

	FlexHook_p pHook = FlexHook::cast(pWidget->hook());
	pHook->setOfs(dragStartPos+dragDistance);
*/
}

//____ cbMoveResize() _________________________________________________________

void cbMoveResize( const Msg_p& _pMsg, const Object_p& _pWidget )
{
/*
	static Coord posAtPress[MouseButton_Max];
	
	auto	pWidget = Widget::cast(_pWidget);
	FlexHook_p 	pHook = FlexHook::cast(pWidget->hook());

	switch( _pMsg->type() )
	{
		case MsgType::MousePress:
		{
			auto pMsg = MousePressMsg::cast(_pMsg);
			posAtPress[(int)pMsg->button()] = pWidget->pos();
			
		}
		break;
		case MsgType::MouseDrag:
		{
			auto pMsg = MouseDragMsg::cast(_pMsg);
			if( pMsg->button() == MouseButton::Right )
			{
				pHook->setSize( pHook->size() + pMsg->draggedNow().toSize() );
			}
			else if( pMsg->button() == MouseButton::Middle )
			{
				pHook->setOfs( posAtPress[(int)MouseButton::Middle] + pMsg->draggedTotal() );
			}
		}
		break;
		case MsgType::MouseRelease:
		break;
		
		default:
		break;
	}
*/
}


//____ addResizablePanel() _________________________________________________

void addResizablePanel( const FlexPanel_p& pParent, const Widget_p& pChild, const MsgRouter_p& pMsgRouter )
{
	pParent->children.add( pChild );
//	pMsgRouter->addRoute( pChild, MsgFunc::create(cbMoveResize, pChild) );
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
