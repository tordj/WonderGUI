

#include <cstdlib>

#ifdef WIN32
#	include <SDL.h>
#else
#	include <SDL2/SDL.h>
#endif

#include <wg_base.h>
#include <wg_inputhandler.h>
#include <wg_msgrouter.h>

#include <wg_softsurface.h>
#include <wg_softgfxdevice.h>
#include <wg_softsurfacefactory.h>

#include <wg_rootpanel.h>
#include <wg_lambdapanel.h>
#include <wg_splitpanel.h>

#include <wg_popuplayer.h>

#include <wg_filler.h>
#include <wg_textdisplay.h>
#include <wg_fpsdisplay.h>
#include <wg_button.h>
#include <wg_selectbox.h>
#include <wg_texteditor.h>

#include <wg_staticcolorskin.h>
#include <wg_boxskin.h>
#include <wg_blockskin.h>

#include <wg_freetypefont.h>
#include <wg_context.h>


using namespace wg;

void 			translateEvents(RootPanel * pRoot);
MouseButton 	translateMouseButton( uint8_t button );

bool			bQuit = false;	// Set to false by myButtonClickCallback() or translateEvents().

int				rootScale = 64;

//____ drawFills() _____________________________________________________________

void drawFills(GfxDevice_p pGfxDevice, Surface_p pCanvas)
{
	pGfxDevice->beginRender();
	pGfxDevice->beginCanvasUpdate(pCanvas);

	pGfxDevice->fill(HiColor::Black);

	for (int y = 0; y < 17; y++)
	{
		for (int x = 0; x < 17; x++)
		{
			pGfxDevice->fill(RectI((4 + x * 16) * 64 + y * 4, (4 + y * 16) * 64 + x * 4, 10 * 64, 10 * 64), HiColor(4096, 0, 0, 4096));
		}
	}

	pGfxDevice->fill(RectI((18 * 16) * 64, 0, 64 * 10, 64 * 300), HiColor(4096 / 16, 0, 0, 4096));

	pGfxDevice->endCanvasUpdate();
	pGfxDevice->endRender();
}


//____ loadBlob() _____________________________________________________________

Blob_p loadBlob(const char* pPath)
{
	FILE* fp = fopen(pPath, "rb");
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	Blob_p pBlob = Blob::create((int)size);

	size_t nRead = fread(pBlob->data(), 1, size, fp);
	fclose(fp);

	if (nRead < size)
		return 0;

	return pBlob;

}

//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	//------------------------------------------------------
	// Init SDL
	//------------------------------------------------------

	SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 640, height = 480;
	SDL_Window * pWin = SDL_CreateWindow("Hello WonderGUI", posX, posY, width, height, 0);

	SDL_Surface * pWinSurf = SDL_GetWindowSurface( pWin );

	//------------------------------------------------------
	// Init WonderGUI
	//------------------------------------------------------


	Base::init(nullptr);

	Context_p pContext = Context::create();


	SoftGfxDevice_p		pGfxDevice = SoftGfxDevice::create();
	SurfaceFactory_p	pFactory = SoftSurfaceFactory::create();

	pContext->setSurfaceFactory(pFactory);
	pContext->setGfxDevice(pGfxDevice);

	Base::setActiveContext(pContext);

	//

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


	//

	PixelFormat format = PixelFormat::Unknown;

	if( pWinSurf->format->BitsPerPixel == 32 )
		format = PixelFormat::BGRA_8;
	else if( pWinSurf->format->BitsPerPixel == 24 )
		format = PixelFormat::BGR_8;

	Blob_p pCanvasBlob = Blob::create( pWinSurf->pixels, 0);
	SoftSurface_p pCanvas = SoftSurface::create( SizeI(pWinSurf->w,pWinSurf->h), format, pCanvasBlob, pWinSurf->pitch );

	// First we load the 24-bit bmp containing the button graphics.
	// No error handling or such to keep this example short and simple.

	SDL_Surface * pSDLSurf = SDL_LoadBMP( "resources/simple_button.bmp" );
	SoftSurface_p pButtonSurface = SoftSurface::create( SizeI( pSDLSurf->w, pSDLSurf->h ), PixelFormat::BGR_8, (unsigned char*) pSDLSurf->pixels, pSDLSurf->pitch, 0 );
	SDL_FreeSurface(pSDLSurf);

	// Load a font

	Blob_p pFontFile = loadBlob("resources/DroidSans.ttf");

	FreeTypeFont_p pFont = FreeTypeFont::create(pFontFile, 0);

	TextStyle_p pStyle = TextStyle::create();
	pStyle->setFont(pFont);
	pStyle->setSize(16);
	pStyle->setColor(Color::Black);
	Base::setDefaultStyle(pStyle);


	// 

	RootPanel_p pRoot = RootPanel::create(pCanvas, pGfxDevice);

	pRoot->setScale(rootScale);

	auto pPopupLayer = PopupLayer::create();
	pRoot->slot = pPopupLayer;

	Base::inputHandler()->setFocusedWindow(pRoot);

	//

	auto pBaseLambda = LambdaPanel::create();
	pBaseLambda->skin = StaticColorSkin::create(Color8::Beige);
	pPopupLayer->mainSlot = pBaseLambda;

	//

	auto pFiller = Filler::create();
	auto pSkin = BoxSkin::create(3, {	{StateEnum::Normal, Color8::Blue, Color8::Salmon}, 
										{StateEnum::Hovered, Color8::LightBlue, Color8::LightSalmon},
										{StateEnum::Pressed, Color8::DarkBlue, Color8::DarkSalmon} });
	pSkin->setContentPadding(3);

	pFiller->skin = pSkin;

	pBaseLambda->slots.pushBack(pFiller, [](Widget* pWidget, Size parentSize) { return Rect(10,10,100,50); });

	//

	auto pFiller2 = Filler::create();
	auto pSkin2 = BlockSkin::create(pButtonSurface, { StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Disabled }, 3, Axis::X );

	pFiller2->skin = pSkin2;

	pBaseLambda->slots.pushBack(pFiller2, [](Widget* pWidget, Size parentSize) { return Rect(10, 100, 100, 50); });


	//

	auto pLabel = TextDisplay::create();

	pLabel->skin = pSkin;
	pLabel->text.set("Label");

	pBaseLambda->slots.pushBack(pLabel, [](Widget* pWidget, Size parentSize) { return Rect(10, 200, 100, 50); });

//	pLabel->setScale(128);

	//

	auto pFpsDisplay = FpsDisplay::create();

	pBaseLambda->slots.pushBack(pFpsDisplay, [](Widget* pWidget, Size parentSize) { return Rect(200, 10, 100, 100); });

	//

	auto pButton = Button::create();
	pButton->skin = pSkin2;
	pButton->text.set("BUTTON");

	pBaseLambda->slots.pushBack(pButton, [](Widget* pWidget, Size parentSize) { return Rect(10, 300, 100, 50); });

	//

	auto pSelectBox = SelectBox::create();
	
	pSelectBox->skin = pSkin2;
	pSelectBox->setListSkin(pSkin);

	pSelectBox->entries.pushBack({ SelectBoxEntry(1, String("One")), SelectBoxEntry(2,String("Two")) });

	pBaseLambda->slots.pushBack(pSelectBox, [](Widget* pWidget, Size parentSize) { return Rect(10, 400, 100, 50); });

	//

	auto pSplitPanel = SplitPanel::create();


	auto pTextEditor = TextEditor::create();
	pTextEditor->text.set("EDITABLE");
	pTextEditor->skin = pSkin;

	auto pTextEditor2= TextEditor::create();
	pTextEditor2->text.set("EDIT2");
	pTextEditor2->skin = pSkin;

	pSplitPanel->slots[0] = pTextEditor;

	pSplitPanel->slots[1] = pTextEditor2;

	pSplitPanel->handleSkin = pSkin2;

	pBaseLambda->slots.pushBack(pSplitPanel, [](Widget* pWidget, Size parentSize) { return Rect(200, 200, 100, 200); });

	//






	//------------------------------------------------------
	// Program Main Loop
	//------------------------------------------------------

	int64_t	timestamp = 0;

	while( !bQuit )
	{
		// Loop through SDL events, translate them to WonderGUI events
		// and process them.

		translateEvents(pRoot);


		Base::update(timestamp);

		// Let WonderGUI render any updated/dirty regions of the screen.

		SDL_LockSurface(pWinSurf);

		pRoot->render();


//		drawFills(pGfxDevice, pCanvas);

		SDL_UnlockSurface(pWinSurf);
		SDL_UpdateWindowSurface( pWin );
		SDL_Delay(20);
		timestamp += 20000;
	}

	// Cleanup. We should null all our smartpointers so that all objects are
	// deleted before calling Base::exit().

	pCanvas = nullptr;
	pCanvasBlob = nullptr;
	pRoot = nullptr;
	pBaseLambda = nullptr;

	pFiller = nullptr;
	pFiller2 = nullptr;
	pLabel = nullptr;
	pFpsDisplay = nullptr;
	pButton = nullptr;


	// Exit WonderGUI

	Base::exit();

	// Exit SDL

	SDL_Quit();

	return 0;
}

//____ translateEvents() ___________________________________________________________

void translateEvents(RootPanel * pRoot)
{
	// Process all the SDL events in a loop.
	// In this example we only use mouse input, but typically you
	// would also need to translate keyboard events.

	SDL_Event e;
	while(SDL_PollEvent(&e)) {

		switch( e.type )
		{
			case SDL_QUIT:
				bQuit = true;
				break;

			case SDL_MOUSEMOTION:
				Base::inputHandler()->setPointer( pRoot, Coord(e.motion.x, e.motion.y)*64/rootScale);
				break;

			case SDL_MOUSEBUTTONDOWN:
				Base::inputHandler()->setButton( translateMouseButton(e.button.button), true );
				break;

			case SDL_MOUSEBUTTONUP:
				Base::inputHandler()->setButton( translateMouseButton(e.button.button), false );
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
				Base::inputHandler()->setWheelRoll(1, distance, bInvertScroll);
				break;
			}

			case SDL_KEYDOWN:
			{
				Base::inputHandler()->setKey(e.key.keysym.sym, true);
				break;
			}

			case SDL_KEYUP:
			{
				Base::inputHandler()->setKey(e.key.keysym.sym, false);
				break;
			}

			case SDL_TEXTINPUT:
				Base::inputHandler()->putText(e.text.text);
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
MouseButton translateMouseButton( uint8_t button )
{
	switch( button )
	{
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
		default:
			return MouseButton::None;
	}
}
