
#include <cstdlib>
#include <stdio.h>

#ifdef WIN32
#	include <SDL.h>
#	include <SDL_image.h>
#    include <GL/glew.h>
#elif __APPLE__
#	include <SDL2/SDL.h>
#	include <SDL2_image/SDL_image.h>
#else
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_image.h>
#   include <GL/glew.h>
#endif


#include <wondergui.h>


#include <wg_softsurface.h>
#include <wg_softsurfacefactory.h>
#include <wg_softgfxdevice.h>

#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>
#include <wg_glgfxdevice.h>


#include <wg_freetypefont.h>
#include "testwidget.h"

//#define USE_OPEN_GL


using namespace wg;

void 			translateEvents( const InputHandler_p& pInput, const RootPanel_p& pRoot );
MouseButton 	translateMouseButton( Uint8 button );
void 			updateWindowRects( const RootPanel_p& pRoot, SDL_Window * pWindow );
void 			myButtonClickCallback( const Msg_p& pMsg );
void * 			loadFile( const char * pPath );
Blob_p 			loadBlob( const char * pPath );
Surface_p		loadSurface(const std::string& path, PixelFormat pixFormat = PixelFormat::BGRA_8 );

void			convertSDLFormat( PixelDescription * pWGFormat, const SDL_PixelFormat * pSDLFormat );

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


bool shadowLayerTest(RootPanel_p pRoot);
bool stretchBlitTest(RootPanel_p pRoot);
bool scrollIntoViewTest(RootPanel_p pRoot);
bool textClipTest(RootPanel_p pRoot);
bool lineEditorTest(RootPanel_p pRoot);
bool popupOpenerTest(RootPanel_p pRoot);
bool scrollbarTest(RootPanel_p pRoot);
bool modalLayerTest(RootPanel_p pRoot);



//____ main() _________________________________________________________________

int main(int argc, char** argv)
{

	printf("SizeOf Filler: %d\n", (int) sizeof(Filler));
	printf("SizeOf Object: %d\n", (int) sizeof(Object));
	printf("SizeOf Receiver: %d\n", (int) sizeof(Receiver));

	printf("Slot is safe to relocate: %d\n", Slot::safe_to_relocate);
	printf("PackListSlot is safe to relocate: %d\n", PackListSlot::safe_to_relocate);
	printf("LambdaPanelSlot is safe to relocate: %d\n", LambdaPanelSlot::safe_to_relocate);


	// Point test
/*
	Base::init();

	auto pContext0 = Context::create();

	pContext0->setScale(2.25f);

	Base::setActiveContext(pContext0);

	Coord a(1, 2);

	CoordF b = a;
	CoordI c = a;

	float pixX = toPixel(float(a.x));
	float pixY = toPixel(float(a.y));

	CoordI d(QPix::fromPixel(pixX), QPix::fromPixel(pixY));


	CoordI cI1 = Coord(1.f, 2.f);
	 
//	CoordI cI2 = CoordF(1.f, 2.f);									// Should fail!
	CoordI cI3 = static_cast<CoordI>(CoordF(1.f, 2.f));

	CoordI cI4 = Rect(1.f, 2.f, 3.f, 4.f);

//	CoordI cI5 = RectF(1.f, 2.f, 3.f, 4.f);							// Should fail!
	CoordI cI6 = static_cast<CoordI>(RectF(1.f, 2.f, 3.f, 4.f));


	SizeI sI1 = Size(1.f, 2.f);

//	SizeI sI2 = SizeF(1.f, 2.f);										// Should fail!
	SizeI sI3 = static_cast<SizeI>(SizeF(1.f, 2.f));

	SizeI sI4 = Rect(1.f, 2.f, 3.f, 4.f);

//	SizeI cI5 = RectF(1.f, 2.f, 3.f, 4.f);							// Should fail!
	SizeI sI6 = static_cast<SizeI>(RectF(1.f, 2.f, 3.f, 4.f));


	RectI rI1 = Rect(1, 2, 3, 4);

//	RectI rI2 = RectF(1.f, 2.f, 3.f, 4.f);							// Should fail!
	RectI rI3 = static_cast<RectI>(RectF(1.f, 2.f, 3.f, 4.f));

	RectI rI4 = Coord(1.f, 2.f);

//	RectI rI5 = CoordF(1.f, 2.f);									// Should fail!
	RectI rI6 = static_cast<RectI>(CoordF(1.f, 2.f));

	RectI rI7 = Size(1.f, 2.f);

//	RectI rI8 = SizeF(1.f, 2.f);										// Should fail!
	RectI rI9 = static_cast<RectI>(SizeF(1.f, 2.f));


	Coord cP1 = CoordF(1.f, 2.f);	
	Coord cP2 = RectF(1.f, 2.f, 3.f, 4.f);

	Size sP1 = SizeF(1.f, 2.f);	
	Size sP2 = RectF(1.f, 2.f, 3.f, 4.f);

	Rect rP1 = RectF(1.f, 2.f, 3.f, 4.f);
	Rect rP2 = CoordF(1.f, 2.f);
	Rect rP3 = SizeF(1.f, 2.f);
*/

	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 1024, height = 768;


#ifdef USE_OPEN_GL

	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, SDL_WINDOW_OPENGL );

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

	SDL_GLContext context = SDL_GL_CreateContext(pWin);

#ifdef WIN32  
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
#endif

	glDrawBuffer(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();


#else
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, 0);
#endif



	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------

	Base::init();

	Context_p pContext = Context::create();
	pContext->setScale(1.0);

#ifdef USE_OPEN_GL
	pContext->setSurfaceFactory(GlSurfaceFactory::create());
	pContext->setGfxDevice( GlGfxDevice::create(SizeI(width, height)));
#else
	{
		SDL_Surface * pScreen = SDL_GetWindowSurface(pWin);

		PixelFormat type = PixelFormat::Unknown;

		if (pScreen->format->BitsPerPixel == 32)
			type = PixelFormat::BGRA_8;
		else if (pScreen->format->BitsPerPixel == 24)
			type = PixelFormat::BGR_8;

		Blob_p pBlob = Blob::create(pScreen->pixels, nullptr);

		SoftSurface_p		pCanvas = SoftSurface::create( SizeI(width, height), type, pBlob, pScreen->pitch);
		GfxDevice_p			pDevice = SoftGfxDevice::create(pCanvas);
		SurfaceFactory_p	pFactory = SoftSurfaceFactory::create();

		pContext->setSurfaceFactory(pFactory);
		pContext->setGfxDevice(pDevice);
	}
#endif
	Base::setActiveContext(pContext);


//	FreeTypeFont::init(SoftSurfaceFactory::create());

	InputHandler_p pInput = Base::inputHandler();

	pInput->mapKey(SDLK_LEFT, Key::Left);
	pInput->mapKey(SDLK_RIGHT, Key::Right);
	pInput->mapKey(SDLK_UP, Key::Up);
	pInput->mapKey(SDLK_DOWN, Key::Down);
	pInput->mapKey(SDLK_BACKSPACE, Key::Backspace);
	pInput->mapKey(SDLK_DELETE, Key::Delete);
	pInput->mapKey(SDLK_END, Key::End);
	pInput->mapKey(SDLK_ESCAPE, Key::Escape);
	pInput->mapKey(SDLK_HOME, Key::Home);
	pInput->mapKey(SDLK_PAGEDOWN, Key::PageDown);
	pInput->mapKey(SDLK_PAGEUP, Key::PageUp);
	pInput->mapKey(SDLK_RETURN, Key::Return);
	pInput->mapKey(SDLK_SPACE, Key::Space);
	pInput->mapKey(SDLK_TAB, Key::Tab);
	pInput->mapKey(SDLK_F1, Key::F1);
	pInput->mapKey(SDLK_F2, Key::F2);
	pInput->mapKey(SDLK_F3, Key::F3);
	pInput->mapKey(SDLK_F4, Key::F4);
	pInput->mapKey(SDLK_F5, Key::F5);
	pInput->mapKey(SDLK_F6, Key::F6);
	pInput->mapKey(SDLK_F7, Key::F7);
	pInput->mapKey(SDLK_F8, Key::F8);
	pInput->mapKey(SDLK_F9, Key::F9);
	pInput->mapKey(SDLK_F10, Key::F10);
	pInput->mapKey(SDLK_F11, Key::F11);
	pInput->mapKey(SDLK_F12, Key::F12);


	pInput->mapKey(SDLK_LCTRL, Key::Control);
	pInput->mapKey(SDLK_RCTRL, Key::Control);

	pInput->mapKey(SDLK_LSHIFT, Key::Shift);
	pInput->mapKey(SDLK_RSHIFT, Key::Shift);

	pInput->mapKey(SDLK_LALT, Key::Alt);
	pInput->mapKey(SDLK_RALT, Key::Alt);

	pInput->mapKey(SDLK_KP_ENTER, Key::Return);


	pInput->mapCommand(SDLK_ESCAPE, MODKEY_NONE, EditCmd::Escape);

	pInput->mapCommand(SDLK_x, MODKEY_CTRL, EditCmd::Cut);
	pInput->mapCommand(SDLK_c, MODKEY_CTRL, EditCmd::Copy);
	pInput->mapCommand(SDLK_v, MODKEY_CTRL, EditCmd::Paste);

	pInput->mapCommand(SDLK_a, MODKEY_CTRL, EditCmd::SelectAll);

	pInput->mapCommand(SDLK_z, MODKEY_CTRL, EditCmd::Undo);
	pInput->mapCommand(SDLK_z, MODKEY_CTRL_SHIFT, EditCmd::Redo);

/*
	PixelFormat format = PixelFormat::Unknown;

		if( pWinSurf->format->BitsPerPixel == 32 )
			format = PixelFormat::BGRA_8;
		else if( pWinSurf->format->BitsPerPixel == 24 )
			format = PixelFormat::BGR_8;

		Blob_p pCanvasBlob = Blob::create( pWinSurf->pixels, 0);
		SoftSurface_p pCanvas = SoftSurface::create( SizeI(pWinSurf->w,pWinSurf->h), format, pCanvasBlob, pWinSurf->pitch );

		SoftGfxDevice_p pGfxDevice = SoftGfxDevice::create( pCanvas );
*/
	GfxDevice_p pGfxDevice				= Base::activeContext()->gfxDevice();
	SurfaceFactory_p pSurfaceFactory	= Base::activeContext()->surfaceFactory();

	RootPanel_p pRoot = RootPanel::create(pGfxDevice);

//		pRoot->setDebugMode(true);

	Base::inputHandler()->setFocusedWindow(pRoot);

	//

	MsgLogger_p pLogger = MsgLogger::create(std::cout);
	pLogger->logAllMsgs();
	pLogger->ignoreMsg(MsgType::Tick);
	pLogger->ignoreMsg( MsgType::MouseMove);
	pLogger->ignoreMsg(MsgType::MouseDrag);
	pLogger->ignoreMsg(MsgType::MouseRepeat);

	Base::msgRouter()->broadcastTo(pLogger);

	// Init font

	PixelDescription	pixelDesc;

	char * pFontSpec = (char*)loadFile("../resources/anuvverbubbla_8x8.fnt");

	SDL_Surface * pFontSurf = IMG_Load("../resources/anuvverbubbla_8x8.png");
	//	convertSDLFormat( &pixelDesc, pFontSurf->format );
	Surface_p pFontImg = pSurfaceFactory->createSurface(SizeI(pFontSurf->w, pFontSurf->h), PixelFormat::BGRA_8, (unsigned char*)pFontSurf->pixels, pFontSurf->pitch);
	SDL_FreeSurface(pFontSurf);

	BitmapFont_p pBmpFont = BitmapFont::create(pFontImg, pFontSpec);

	TextStyle_p pStyle = TextStyle::create();
	pStyle->setFont(pBmpFont);
	pStyle->setSize(8);
	Base::setDefaultStyle(pStyle);



//	Blob_p pFontFile = loadBlob("../resources/DroidSans.ttf");

//	FreeTypeFont_p pFont = FreeTypeFont::create(pFontFile, 1);

//	TextStyle_p pStyle = TextStyle::create();
//	pStyle->setFont(pFont);
//	pStyle->setSize(16);
//	Base::setDefaultStyle(pStyle);

	/*
		StdTextMapper_p pMapper = StdTextMapper::create();
		pMapper->setSelectionBackColor(Color(0,0,255,128), BlendMode::Blend);
		Base::setDefaultTextMapper(pMapper);
	*/

	StdTextMapper::cast(Base::defaultTextMapper())->setSelectionBack(Color(255, 255, 255, 255), BlendMode::Invert);

	// Init skins

	SDL_Surface * pSDLSurf = IMG_Load("../resources/simple_button.bmp");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pButtonSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGR_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	BlockSkin_p pSimpleButtonSkin = BlockSkin::createClickableFromSurface(pButtonSurface, 0, BorderI(3));
	pSimpleButtonSkin->setContentPadding(BorderI(5));

	pSDLSurf = IMG_Load("../resources/simple_icon.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pBackgroundSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGR_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	BlockSkin_p pBackgroundSkin = BlockSkin::createStaticFromSurface(pBackgroundSurface);


	pSDLSurf = IMG_Load("../resources/splash.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pSplashSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGR_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	BlockSkin_p pSplashSkin = BlockSkin::createStaticFromSurface(pSplashSurface);


	pSDLSurf = IMG_Load("../resources/state_button.bmp");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pStateButtonSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGR_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	BlockSkin_p pStateButtonSkin = BlockSkin::createClickSelectableFromSurface(pStateButtonSurface, 0, BorderI(3));
	pStateButtonSkin->setContentPadding(BorderI(5));

	pSDLSurf = IMG_Load("../resources/grey_pressable_plate.bmp");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pPressablePlateSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGR_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	BlockSkin_p pPressablePlateSkin = BlockSkin::createClickableFromSurface(pPressablePlateSurface, 0, BorderI(3));
	pPressablePlateSkin->setContentPadding(BorderI(3));

	pSDLSurf = IMG_Load("../resources/list_entry.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pListEntrySurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGRA_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	Skin_p pListEntrySkin = BlockSkin::createClickSelectableFromSurface(pListEntrySurface, 0, BorderI(2));

	pSDLSurf = IMG_Load("../resources/splash.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pImgSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGR_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	BlockSkin_p pImgSkin = BlockSkin::createStaticFromSurface(pImgSurface);
	pImgSurface->setScaleMode(ScaleMode::Interpolate);


	pSDLSurf = IMG_Load("../resources/up_down_arrow.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pUpDownArrowSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGRA_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	Skin_p pUpDownArrowSkin = BlockSkin::createSelectableFromSurface(pUpDownArrowSurface, 0, BorderI(0));

	pSDLSurf = IMG_Load("../resources/simple_icon.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pSimpleIconSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGRA_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	Skin_p pSimpleIconSkin = BlockSkin::createStaticFromSurface(pSimpleIconSurface, BorderI(0));

	//------------------------------------------------------
	// Setup a simple GUI consisting of a filled background and
	// a button using scaled bitmaps.
	//------------------------------------------------------

	DragNDropLayer_p pDnDLayer = DragNDropLayer::create();
	pRoot->child = pDnDLayer;

	PopupLayer_p pPopupLayer = PopupLayer::create();
	pDnDLayer->base = pPopupLayer;

	LambdaPanel_p pBasePanel = LambdaPanel::create();
	pBasePanel->setSkin(StaticColorSkin::create(Color::Burlywood));
	pPopupLayer->base = pBasePanel;

	/*	Filler_p pBackground = Filler::create();
		pBackground->setSkin( ColorSkin::create(Color::aqua) );
		pFlexPanel->addWidget(pBackground, WG_NORTHWEST, CoordI(), WG_SOUTHEAST, CoordI());
	*/


	//	auto pTestSkin = BoxSkin::create({ {StateEnum::Normal, Color::Beige}, {StateEnum::Pressed, Color::Red} }, BorderI(5), { {StateEnum::Hovered, Color::Green} });
	//	pTestSkin->setBlendMode(BlendMode::Add);

	auto pTestSkin = MultiBlockSkin::create({ 10,10 }, BorderI(4));

	int layer1 = pTestSkin->addLayer(pPressablePlateSurface, { StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Disabled }, Orientation::Horizontal);
	pTestSkin->setLayerBlendMode(layer1, BlendMode::Blend);

	//	int layer2 = pTestSkin->addLayer(pBackgroundSurface, { 0,0 });
	//	pTestSkin->setLayerTint(layer2, { {StateEnum::Normal, Color::Transparent}, {StateEnum::Hovered, {255,255,255,64} } });

	Button_p pImage0 = Button::create();
	pImage0->setSkin(pTestSkin);
	pImage0->setPointerStyle(PointerStyle::Crosshair);

	pBasePanel->children.add(pImage0, [](Widget * pWidget, SizeI size) {return RectI(size.w - 80 * 2, (size.h - 33 * 2) / 2, 80 * 2, 33 * 2); });

	Base::msgRouter()->addRoute(pImage0, MsgType::Select, [&](const Msg_p& pMsg) { bQuit = true; });


	//	Image_p pImage = Image::create();
	//	pImage->setSkin( pSimpleButtonSkin );
	//	pFlexPanel->children.addMovable( pImage, RectI(0,0,80*2,33*2), Origo::Center, Origo::Center );


	//	pRoot->msgRouter()->AddCallback( MsgFilter::select(), pButton, myButtonClickCallback );

	
//	shadowLayerTest(pRoot);
//	stretchBlitTest(pRoot);
//	scrollIntoViewTest(pRoot);
//	textClipTest(pRoot);
//	lineEditorTest(pRoot);
//	popupOpenerTest(pRoot);
//	scrollbarTest(pRoot);
	modalLayerTest(pRoot);

	
	// Test IChild and IChildIterator baseclasses
/*
	{
		auto pContainer = PackPanel::create();

		std::vector<Widget_p>	children;

		for (int i = 0; i < 10; i++)
		{
			auto p = Filler::create();
			p->setId(i);
			children.push_back(p);
		}

		pContainer->children.add(children.begin(), children.end());

		IChildren_p pChildren = pContainer->children.ptr();


		for (auto& child : *pChildren)
			printf("%d\n", child.id());


	}
*/
/*
	// Test drag n drop support

	{
		static Widget_p s_pPicked = nullptr;
		static Coord	s_pickOfs;

		auto pDropTargetSkin = MultiBlockSkin::create({ 10,10 }, BorderI(4));

		int layer1 = pDropTargetSkin->addLayer(pPressablePlateSurface, { StateEnum::Normal, StateEnum::Targeted, StateEnum::Pressed, StateEnum::Disabled }, Orientation::Horizontal);
		pDropTargetSkin->setLayerBlendMode(layer1, BlendMode::Blend);


		auto pPickable1 = Filler::create();
		pPickable1->setSkin(pTestSkin);
//		pPickable1->text.set( "Drag Me 1" );
		pPickable1->setPickable(true, 1);
		pBasePanel->children.add(pPickable1, [](Widget * pWidget, Size size) {return Rect( 0,0,100,50 ); });

		auto pPickable2 = Filler::create();
		pPickable2->setSkin(pTestSkin);
//		pPickable2->text.set("Drag Me 2");
		pPickable2->setPickable(true, 2);
		pBasePanel->children.add(pPickable2, [](Widget * pWidget, Size size) {return Rect(size.w-100, 0, 100, 50); });

		auto pTrash = Filler::create();
		pTrash->setSkin(pTestSkin);
//		pTrash->text.set("Trash Can");
		pTrash->setDropTarget(true);
		pBasePanel->children.add(pTrash, [](Widget * pWidget, Size size) {return Rect(50, 200, 100, 50); });

		pBasePanel->setDropTarget(true);

		Base::msgRouter()->addRoute(MsgType::DropPick, [](Msg * _pMsg) {

			auto pMsg = static_cast<DropPickMsg*>(_pMsg);
			s_pPicked = pMsg->pickedFrom();
			s_pickOfs = pMsg->pickOfs();
			pMsg->setDragWidget(s_pPicked, Coord() - s_pickOfs );

			pMsg->setPayload(Payload::create());
		});

		Base::msgRouter()->addRoute(pBasePanel, MsgType::DropProbe, [](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);
			pMsg->accept();
		});

		Base::msgRouter()->addRoute(pBasePanel, MsgType::DropDeliver, [pBasePanel](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);

			CoordI pos = pBasePanel->toLocal(pMsg->pointerPos()) - s_pickOfs;

			pBasePanel->children.add(s_pPicked, [pos](Widget * pWidget, Size size) {return Rect(pos, 100, 50); });
			pMsg->accept();
			
		});



	}
*/
	
	
/*
	{
		auto pDropTargetSkin = MultiBlockSkin::create({ 10,10 }, BorderI(4));

		int layer1 = pDropTargetSkin->addLayer(pPressablePlateSurface, { StateEnum::Normal, StateEnum::Targeted, StateEnum::Pressed, StateEnum::Disabled }, Orientation::Horizontal);
		pDropTargetSkin->setLayerBlendMode(layer1, BlendMode::Blend);



		TextDisplay_p pPickable1 = TextDisplay::create();
		pPickable1->setSkin(pTestSkin);
		pPickable1->text.set( "Drag Me 1" );
		pPickable1->setPickable(true, 1);
		pBasePanel->children.add(pPickable1, [](Widget * pWidget, SizeI size) {return RectI( 0,0,100,50 ); });

		TextDisplay_p pPickable2 = TextDisplay::create();
		pPickable2->setSkin(pTestSkin);
		pPickable2->text.set("Drag Me 2");
		pPickable2->setPickable(true, 2);
		pBasePanel->children.add(pPickable2, [](Widget * pWidget, SizeI size) {return RectI(size.w-100, 0, 100, 50); });

		TextDisplay_p pDropTarget1 = TextDisplay::create();
		pDropTarget1->setSkin(pTestSkin);
		pDropTarget1->text.set("Drop 1 here");
		pDropTarget1->setDropTarget(true);
		pBasePanel->children.add(pDropTarget1, [](Widget * pWidget, SizeI size) {return RectI(50, 200, 100, 50); });

		TextDisplay_p pDropTargetAny = TextDisplay::create();
		pDropTargetAny->setSkin(pDropTargetSkin);
		pDropTargetAny->text.set("Drop any here");
		pDropTargetAny->setDropTarget(true);
		pBasePanel->children.add(pDropTargetAny, [](Widget * pWidget, SizeI size) {return RectI(size.w/2 - 25, 200, 100, 50); });

		TextDisplay_p pDropTarget2 = TextDisplay::create();
		pDropTarget2->setSkin(pDropTargetSkin);
		pDropTarget2->text.set("Drop 2 here");
		pDropTarget2->setDropTarget(true);
		pBasePanel->children.add(pDropTarget2, [](Widget * pWidget, SizeI size) {return RectI(size.w-150, 200, 100, 50); });



		Base::msgRouter()->addRoute(pDropTarget1, MsgType::DropProbe, [](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);
			if( pMsg->pickCategory() == 1 )
			{
				pMsg->accept();
			}
		});

		Base::msgRouter()->addRoute(pDropTarget1, MsgType::DropDeliver, [](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);
			if( pMsg->pickCategory() == 1 )
			{
				pMsg->accept();
			}
		});


		Base::msgRouter()->addRoute(pDropTarget2, MsgType::DropProbe, [](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);
			if( pMsg->pickCategory() == 2 )
			{
				pMsg->accept();
			}
		});

		Base::msgRouter()->addRoute(pDropTarget2, MsgType::DropDeliver, [](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);
			if( pMsg->pickCategory() == 2 )
			{
				pMsg->accept();
			}
		});


		Base::msgRouter()->addRoute(pDropTargetAny, MsgType::DropProbe, [](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);
			pMsg->accept();
		});

		Base::msgRouter()->addRoute(pDropTargetAny, MsgType::DropDeliver, [](Msg * _pMsg) {
			auto pMsg = static_cast<DropProbeMsg*>(_pMsg);
			pMsg->accept();
		});

	}
*/
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
//			RectI r = it.geo;
			printf("%s\n", it.className() );
		}

	}
	*/
/*
	{
		auto pOpener = PopupOpener::create();

		pOpener->setSkin(pPressablePlateSkin);
		pOpener->label.set("OPEN");
		pOpener->setPointerStyle(PointerStyle::Crosshair);

		pBasePanel->children.add(pOpener, [](Widget*pWidget, SizeI parentSize) {return RectI(30, 30, 100, 100); });


		auto pMenu = PackPanel::create();
		pMenu->setOrientation(Orientation::Vertical);
//		pMenu->setSelectable(false);

		auto pSkin = BoxSkin::create(Color::Red, BorderI(0), Color::Red );
		pSkin->setContentPadding(12);
		pMenu->setSkin(pSkin);

		auto pEntry1 = TextDisplay::create();
		pEntry1->text.set("Entry One");
		pEntry1->setSkin(pPressablePlateSkin);
		pMenu->children << pEntry1;

		auto pEntry2 = TextDisplay::create();
		pEntry2->text.set("Entry Two");
		pEntry2->setSkin(pPressablePlateSkin);
		pMenu->children << pEntry2;

		auto pEntry3 = TextDisplay::create();
		pEntry3->text.set("Entry Three");
		pEntry3->setSkin(pPressablePlateSkin);
		pMenu->children << pEntry3;

		auto pSubMenuOpener = PopupOpener::create();
		pSubMenuOpener->label.set("Sub Menu");
		pSubMenuOpener->setSkin(pPressablePlateSkin);
		pSubMenuOpener->setOpenOnHover(true);
		pMenu->children << pSubMenuOpener;

		auto pSubMenu = PackPanel::create();
		pSubMenu->setOrientation(Orientation::Vertical);
		pSubMenu->setSkin(pSkin);

		auto pSubEntry1 = TextDisplay::create();
		pSubEntry1->text.set("Subentry One");
		pSubEntry1->setSkin(pPressablePlateSkin);
		pSubMenu->children << pSubEntry1;

		auto pSubEntry2 = TextDisplay::create();
		pSubEntry2->text.set("Subentry Two");
		pSubEntry2->setSkin(pPressablePlateSkin);
		pSubMenu->children << pSubEntry2;

		auto pSubEntry3 = TextDisplay::create();
		pSubEntry3->text.set("Subentry Three");
		pSubEntry3->setSkin(pPressablePlateSkin);
		pSubMenu->children << pSubEntry3;

		pSubMenuOpener->setPopup(pSubMenu);

		auto pButtons = PackPanel::create();

		auto pBtn1 = Button::create();
		pBtn1->setSkin(pPressablePlateSkin);
		pBtn1->label.set("Button 1");
		pButtons->children << pBtn1;

		auto pBtn2 = Button::create();
		pBtn2->setSkin(pPressablePlateSkin);
		pBtn2->label.set("Button 2");
		pButtons->children << pBtn2;

		pMenu->children << pButtons;

		pOpener->setPopup( pMenu );

	}
*/
/*
	{
		auto pSplit = SplitPanel::create();
		pSplit->setSkin(ColorSkin::create(Color::Beige));
		pSplit->setSplitFactor(0.5f);
		pSplit->setHandleSkin(pPressablePlateSkin);
//		pSplit->setHandleThickness(10);
//		pSplit->setOrientation(Orientation::Horizontal);


		auto pMapper = StdTextMapper::create();
		pMapper->setLineWrap(true);
		pMapper->setAlignment(Origo::SouthEast);

		TextEditor_p pText1 = TextEditor::create();
		pText1->setSkin(ColorSkin::create(Color::SandyBrown));
		pText1->text.set("Once upon a time in a land.");
		pText1->text.setTextMapper(pMapper);

		TextEditor_p pText2 = TextEditor::create();
		pText2->setSkin(ColorSkin::create(Color::Black));
		pText2->text.set("Once upon a time in a land far, far away...");
		pText2->text.setTextMapper(pMapper);

		auto pCapsule = SizeCapsule::create();
		pCapsule->setMinSize({ 0, 10 } );
		pCapsule->setMaxSize({ INT_MAX,pText1->matchingHeight(100) });
		pCapsule->child = pText1;

		pSplit->first = pCapsule;
		pSplit->second = pText2;

		pSplit->setScaleBehavior(SplitPanel::ScaleBehavior::ScaleBoth);
//		pSplit->setBrokerFunction([](Widget*pFirst, Widget*pSecond, int length, float splitFactor, int handle) { return handle + pFirst->size().h; });

		auto pOuterSplit = SplitPanel::create();
		pOuterSplit->setHandleSkin(pPressablePlateSkin);
		pOuterSplit->first = pSplit;


		pBasePanel->children.add(pOuterSplit, [](Widget*pWidget, SizeI parentSize) {return RectI(0, 0, 100, parentSize.h); });
	}
*/

/*
	{
		auto pPanel = LambdaPanel::create();
		pBasePanel->children.add(pPanel, [](Widget*p, SizeI sz) { return RectI(0, 0, sz.w, sz.h); });

		Color cols[5] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {128,128,128}, {255,255,255} };

		for (int i = 0; i < 5; i++)
		{
			auto pChild = Filler::create();
			pChild->setSkin(ColorSkin::create(cols[i]));
			pPanel->children.add(pChild, [&](Widget*p, SizeI sz) { return RectI(i * 10, i * 10, 100, 50);  });
		}

		pPanel->children.moveToFront(4);
	}
*/


/*
	{
		PackPanel_p pPanel = PackPanel::create();
		pBasePanel->children.add(pPanel, [](Widget*p, SizeI sz) { return RectI(0, 0, sz.w, sz.h); } );

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

		pPanel->children.insert( 0, &widgets[1], 4 );

//			it = pPanel->children.insert(it, widgets[i]);

		it = pPanel->children.begin();
		auto it2 = it;
		it2++;
		pPanel->children.hide( it, ++it2 );

	}
*/

/*
	ScrollPanel_p pScrollPanel;
	{
		Scrollbar_p pScrollbar = Scrollbar::create();
		pScrollbar->setBackgroundSkin( ColorSkin::create( Color::Green ));
		pScrollbar->setHandleSkin( ColorSkin::create( Color::Red ));
		pScrollPanel = ScrollPanel::create();
		pScrollPanel->vscrollbar = pScrollbar;

		pBasePanel->children.add(pScrollPanel, [](Widget *p, SizeI sz) { return RectI(0, 0.75*sz.h, sz.w, sz.h * 0.25); } );
	}




	{
		PackList_p pList = PackList::create();

		BoxSkin_p pHeaderSkin = BoxSkin::create( Color::Aquamarine, BorderI(2), Color::DarkRed );
		pHeaderSkin->setContentPadding( 8 );

		pList->header.label.set("Label");
		pList->header.setSkin( pHeaderSkin );
		pList->header.arrow.set( pUpDownArrowSkin, Origo::East );
		pList->header.icon.set( pSimpleIconSkin );

		pList->setSortFunction( sortWidgets );
		pList->setSelectMode( SelectMode::MultiEntries );

		BoxSkin_p pListSkin = BoxSkin::create( Color::Chocolate, BorderI(2), Color::Yellow );
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


		auto pChildren = pList->children.ptr();

		for (int i = 0; i < pChildren->size(); i++)
		{
			bool bSelected = pChildren->isSelected(i);
		}

		for (auto it = pList->children.begin(); it != pList->children.end(); it++)
		{
			bool bSelected = pList->children.isSelected(it);
		}

		for (Widget& child : pList->children)
		{
			bool bSelected = pList->children.isSelected(pList->children.index(&child));
			if (bSelected)
				printf("Selected!\n");
			else
				printf("Not selected!\n");
		}


//		auto selected = pList->children.subset([](it) { return pList->children.isSelected(it); });

	}
*/



 /*  {
		TestWidget_p pTest = TestWidget::create();
		pFlexPanel->addWidget( pTest, Origo::NorthWest, Origo::SouthEast, BorderI(20) );
		pTest->start();
	}
*/

/*
	{
		FlexPanel_p pExtraFlex = FlexPanel::create();

		TextEditor_p pText = TextEditor::create();

		auto pMapper = StdTextMapper::create();
		pMapper->setLineWrap(true);
		pMapper->setAlignment(Origo::North);

		pText->text.setTextMapper(pMapper);


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
		pText->setSkin(ColorSkin::create(Color(0, 0, 0, 128)));

		pExtraFlex->children.addMovable( pText, RectI( 10,10,100,100) );

		pBasePanel->children.add(pExtraFlex, [](Widget*pWidget, SizeI parentSize) { return RectI(parentSize); });

//		addResizablePanel( pFlexPanel, pExtraFlex, Base::msgRouter() );

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
	pCapsule->setMaxSize( SizeI(100,1000));
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
		pFlexPanel->addWidget( pCheckbox, CoordI(10,20*i) );
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
	pFlexPanel->addWidget( pFps, CoordI(0,0), Origo::SouthWest );
*/

	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	pSDLSurf = IMG_Load("../resources/flipping.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pFlippingSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGR_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	pFlippingSurface->setScaleMode(ScaleMode::Interpolate);

	pSDLSurf = IMG_Load("../resources/clockface_2500.png");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pClockSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGRA_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	pClockSurface->setScaleMode(ScaleMode::Interpolate);

	pSDLSurf = IMG_Load("../resources/frog.jpg");
	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pFrogSurface = pSurfaceFactory->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), PixelFormat::BGRA_8, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);
	pFrogSurface->setScaleMode(ScaleMode::Interpolate);

	float rot = 0.1f;
	float scale = 0.00101f;
	float scaleInc = 0.00001f;

	while( !bQuit )
	{

		translateEvents( pInput, pRoot );

		pRoot->render();
//		SDL_UpdateWindowSurface(pWin);
		updateWindowRects(pRoot, pWin);

/*
		rot += 0.1f;
		scale += scaleInc;
		if (scale > 0.01f)
		{
			scaleInc = -scaleInc;
			scale = 0.01f;
		}

		if (scale < 0.f)
		{
			scaleInc = -scaleInc;
			scale = 0.f;
		}


		auto pDevice = Base::activeContext()->gfxDevice();

		pDevice->beginRender();
		pDevice->fill(Color::Blue);
		pDevice->setBlitSource(pFrogSurface);
//		pDevice->rotScaleBlit({ 0,0,width,height }, RectF(pFrogSurface->size()).center(), 0.f, 1.f);

		pDevice->rotScaleBlit({ 10,10,width - 20,height - 20 }, CoordF(1024/2,768/2), rot, scale );


//		pDevice->blit({ 0,0 });

		pDevice->endRender();

		SDL_UpdateWindowSurface(pWin);
*/


		SDL_Delay(16);
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
				pInput->setPointer(pRoot, { QPix::fromPixel(e.motion.x), QPix::fromPixel(e.motion.y) });
				break;

			case SDL_MOUSEBUTTONDOWN:
				pInput->setButton( translateMouseButton(e.button.button), true );
				break;

			case SDL_MOUSEBUTTONUP:
				pInput->setButton( translateMouseButton(e.button.button), false );
				break;

			case SDL_MOUSEWHEEL:
			{
				CoordI distance( e.wheel.x, e.wheel.y );
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
		pSDLRects[i].x = pUpdatedRects[i].x.pixel;
		pSDLRects[i].y = pUpdatedRects[i].y.pixel;
		pSDLRects[i].w = pUpdatedRects[i].w.pixel;
		pSDLRects[i].h = pUpdatedRects[i].h.pixel;
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
	size_t size = ftell(fp);
	fseek( fp, 0, SEEK_SET );

	Blob_p pBlob = Blob::create( (int) size );

	size_t nRead = fread( pBlob->data(), 1, size, fp );
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
	size_t size = ftell(fp);
	fseek( fp, 0, SEEK_SET );

	char * pMem = (char*) malloc( size+1 );
	pMem[size] = 0;
	size_t nRead = fread( pMem, 1, size, fp );
	fclose( fp );

	if( nRead < size )
	{
		free( pMem );
		return 0;
	}

	return pMem;

}


CoordI dragStartPos;

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



	CoordI	dragDistance = pMsg->draggedTotal();

	CoordI	ofs = dragStartPos + dragDistance;

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
	static CoordI posAtPress[MouseButton_Max];

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

void convertSDLFormat( PixelDescription * pWGFormat, const SDL_PixelFormat * pSDLFormat )
{
	pWGFormat->format = PixelFormat::Custom;
	pWGFormat->bits = pSDLFormat->BitsPerPixel;
	pWGFormat->bIndexed = false;

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


//____ loadSurface() ___________________________________________________________

Surface_p loadSurface(const std::string& path, PixelFormat pixFormat)
{
	PixelDescription	pixelDesc;

	auto pSDLSurf = IMG_Load(path.c_str());
	if (pSDLSurf == nullptr)
	{
		printf("ERROR: Could not load '%s'\n", path.c_str());
		exit(-1);
	}

	convertSDLFormat(&pixelDesc, pSDLSurf->format);
	Surface_p pImgSurface = Base::activeContext()->surfaceFactory()->createSurface(SizeI(pSDLSurf->w, pSDLSurf->h), pixFormat, (unsigned char*)pSDLSurf->pixels, pSDLSurf->pitch, &pixelDesc);
	SDL_FreeSurface(pSDLSurf);

	return pImgSurface;
}


//____ shadowLayerTest() ______________________________________________________

bool shadowLayerTest( RootPanel_p pRoot )
{
/*
	Surface_p pImgSurface = loadSurface("../resources/shadow.png", PixelFormat::A8);
	
	BlockSkin_p pShadowSkin = BlockSkin::createStaticFromSurface(pImgSurface);
	pShadowSkin->setFrame({ 0,128,128,0 });
	pShadowSkin->setContentPadding({ 0,128,128,0 });
	pImgSurface->setScaleMode(ScaleMode::Nearest);


	auto pShadowLayer = ShadowLayer::create();
	auto pFrontLayer = FlexPanel::create();
	auto pBaseLayer = FlexPanel::create();

	pRoot->child = pShadowLayer;

	pShadowLayer->base = pBaseLayer;
	pShadowLayer->front = pFrontLayer;


	auto pFiller1 = Filler::create();
	auto pFiller2 = Filler::create();
	auto pFiller3 = Filler::create();

	pFiller1->setSkin(ColorSkin::create(Color::DarkKhaki));
	pFrontLayer->children.addMovable(pFiller1, { 0,0,50,50 });

	pFiller2->setSkin(ColorSkin::create(Color::DarkSlateBlue));
	pFrontLayer->children.addMovable(pFiller2, { 50,50,50,50 });

	pFiller3->setSkin(ColorSkin::create(Color::DarkSeaGreen));
	pFrontLayer->children.addMovable(pFiller3, { 100,100,50,50 });

	auto pBackground = Filler::create();
	pBackground->setSkin(ColorSkin::create(Color::LightSalmon));
	pBaseLayer->children.addPinned(pBackground, Origo::NorthWest, Origo::SouthEast);

	pShadowLayer->shadows.add(pFiller1, pShadowSkin);
	pShadowLayer->shadows.add(pFiller2, pShadowSkin);
	pShadowLayer->shadows.add(pFiller3, pShadowSkin);

	pFiller1->setId(7);

	Base::msgRouter()->addRoute(pFiller1, MsgType::MouseDrag, [pFrontLayer](Msg* _pMsg)
	{
		MouseDragMsg* pMsg = static_cast<MouseDragMsg*>(_pMsg);
		pFrontLayer->children.move(0, pMsg->draggedNow());
	});

	Base::msgRouter()->addRoute(pFiller2, MsgType::MouseDrag, [pFrontLayer](Msg* _pMsg)
	{
		MouseDragMsg* pMsg = static_cast<MouseDragMsg*>(_pMsg);
		pFrontLayer->children.move(1, pMsg->draggedNow());
	});

	Base::msgRouter()->addRoute(pFiller3, MsgType::MouseDrag, [pFrontLayer](Msg* _pMsg)
	{
		MouseDragMsg* pMsg = static_cast<MouseDragMsg*>(_pMsg);
		pFrontLayer->children.move(2, pMsg->draggedNow());
	});
*/
	return true;
}

//____ stretchBlitTest() ______________________________________________________

bool stretchBlitTest(RootPanel_p pRoot)
{
	Surface_p pImgSurface = loadSurface("../resources/white_frame_256x256.png", PixelFormat::BGR_8 );
	pImgSurface->setScaleMode(ScaleMode::Interpolate);

	auto pBack = FlexPanel::create();
	pBack->setSkin(StaticColorSkin::create(Color::Blue));
	pRoot->child = pBack;

	auto pImage = Image::create();
	pImage->setImage(pImgSurface, { 0,0,256,256 });
//	pImage->setImage(pImgSurface, { 1,1,254,254 });
	pBack->children.addMovable(pImage, { 10,10,256,256 } );

	Base::msgRouter()->addRoute(pImage, MsgType::MouseDrag, [pBack,pImage](Msg* pMsg) { 
	
		Coord distance = static_cast<MouseDragMsg*>(pMsg)->draggedNow();

		pBack->children.setSize(0, pImage->size()+Size(distance.x, distance.y) );
	
	});

	return true;
}

//____ scrollIntoViewTest() ______________________________________________________

bool scrollIntoViewTest(RootPanel_p pRoot)
{
	auto pEditor = TextEditor::create();

	pEditor->text.set("TEXT AREA.");
	pEditor->setSkin(ColorSkin::create(Color::YellowGreen));

	auto pScrollPanel = ScrollPanel::create();
	pScrollPanel->view = pEditor;
	pScrollPanel->setSkin(ColorSkin::create(Color::Pink));


	auto pFlex = FlexPanel::create();
	pFlex->children.addMovable(pScrollPanel, { 10,10,200,50 });

	pRoot->child = pFlex;

	return true;
}

//____ textClipTest() ______________________________________________________

bool textClipTest(RootPanel_p pRoot)
{
	auto pSkin = ColorSkin::create(Color::YellowGreen);
	pSkin->setContentPadding(10);

	auto pEditor = TextEditor::create();

	pEditor->text.set("TEXTAREA LINE 1.\nTEXTAREA LINE 2\nTEXTAREA LINE 3.");
	pEditor->setSkin( pSkin );

	auto pFlex = FlexPanel::create();
	pFlex->children.addMovable(pEditor, { 10,10,50,30 });

	pRoot->child = pFlex;

	return true;
}

//____ lineEditorTest() ______________________________________________________

bool lineEditorTest(RootPanel_p pRoot)
{
	auto pSkin = ColorSkin::create(Color::YellowGreen);
	pSkin->setContentPadding(10);

	auto pEditor = LineEditor::create();

	pEditor->text.set("TEXT.");
	pEditor->setSkin(pSkin);

	auto pFlex = FlexPanel::create();
	pFlex->children.addMovable(pEditor, { 10,100,50,30 });

	pRoot->child = pFlex;

	return true;
}

//____ popupOpenerTest() ______________________________________________________

bool popupOpenerTest(RootPanel_p pRoot)
{

	auto pButtonSkin = BoxSkin::create(1, Color::LightGrey, Color::Black);
	pButtonSkin->setContentPadding(2);


	auto pPopupLayer = PopupLayer::create();
	pRoot->child = pPopupLayer;
	pPopupLayer->setSkin(ColorSkin::create(Color::PaleGoldenrod));

	auto pFlex = FlexPanel::create();
	pPopupLayer->base = pFlex;

	auto  pOpener = PopupOpener::create();
	pOpener->setSkin(pButtonSkin);
	pOpener->label.set("OPEN POPUP");
	pFlex->children.addMovable(pOpener, { 50,50,50,30 });

	auto pOpened = Filler::create();
	pOpened->setSkin(pButtonSkin);
	pOpened->setPreferredSize({ 50,100 });
	pOpener->setPopup(pOpened);

	return true;
}

//____ scrollbarTest() ______________________________________________________

bool scrollbarTest(RootPanel_p pRoot)
{
	auto pButtonSkin = BoxSkin::create(1, Color::LightGrey, Color::Black);
	pButtonSkin->setContentPadding(5);


	auto pEditor = TextEditor::create();

	pEditor->text.set("TEXT AREA.");
	pEditor->setSkin(ColorSkin::create(Color::YellowGreen));

	auto pScrollPanel = ScrollPanel::create();
	pScrollPanel->view = pEditor;
	pScrollPanel->setSkin(ColorSkin::create(Color::Pink));

	auto pVScrollbar = Scrollbar::create();
	pVScrollbar->setFwdButtonSkin(pButtonSkin);
	pVScrollbar->setBwdButtonSkin(pButtonSkin);
	pVScrollbar->setBackgroundSkin(ColorSkin::create(Color::DarkGreen));
	pVScrollbar->setHandleSkin(pButtonSkin);

	auto pHScrollbar = Scrollbar::create();
	pHScrollbar->setFwdButtonSkin(pButtonSkin);
	pHScrollbar->setBwdButtonSkin(pButtonSkin);
	pHScrollbar->setBackgroundSkin(ColorSkin::create(Color::DarkGreen));
	pHScrollbar->setHandleSkin(pButtonSkin);


	pScrollPanel->vscrollbar = pVScrollbar;
	pScrollPanel->hscrollbar = pHScrollbar;



	auto pFlex = FlexPanel::create();
	pFlex->children.addMovable(pScrollPanel, { 10,10,200,50 });

	pRoot->child = pFlex;

	return true;
}

//____ modalLayerTest() ______________________________________________________

bool modalLayerTest(RootPanel_p pRoot)
{
	lineEditorTest(pRoot);
	Widget_p p = pRoot->child;

	scrollbarTest(pRoot);

	auto pLayer = ModalLayer::create();

	pLayer->base = pRoot->child;
//	Widget_p pTemp = pRoot->child;
//	pLayer->base = pTemp;

	pRoot->child = pLayer;


	pLayer->modals << p;

	return true;
}
