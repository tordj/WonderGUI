
#include "wonderapp.h"

#include <wondergui.h>
#include <wg_freetypefont.h>
#include <string>
#include <fstream>

#ifdef WIN32
#    include <SDL_image.h>
#elif __APPLE__
#    include <SDL2_image/SDL_image.h>
#else
#    include <SDL2/SDL_image.h>
#endif


using namespace wg;
using namespace std;

//____ create() _______________________________________________________________

WonderApp_p WonderApp::create()
{
	return new MyApp();
}

//____ init() _________________________________________________________________

bool MyApp::init(Visitor* pVisitor)
{
	m_pAppVisitor = pVisitor;

	if (!_setupGUI(pVisitor))
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
	}
		
	auto arguments = pVisitor->programArguments();

	if ( !arguments.empty() )
	{

		if (!loadStream(arguments[0] ))
		{
			printf("ERROR: Could not load stream '%s'!\n", arguments[0].c_str() );
			return false;
		}
	}


	return true;
}

//____ update() _______________________________________________________________

bool MyApp::update()
{
	return true;
}

//____ exit() _________________________________________________________________

void MyApp::exit()
{
}


//____ _setupGUI() ____________________________________________________________

bool MyApp::_setupGUI(Visitor* pVisitor)
{
	m_pWindow = pVisitor->createWindow({ .size = {1600,800}, .title = "Stream Analyzer" });

	auto pRoot = m_pWindow->rootPanel();


	//

	auto pFontBlob = pVisitor->loadBlob("resources/DroidSans.ttf");
	auto pFont = FreeTypeFont::create(pFontBlob);

	m_pTextStyle = TextStyle::create(WGBP(TextStyle,
									_.font = pFont,
									_.size = 14,
									_.color = Color8::Black,
									_.states = {{State::Disabled, Color8::DarkGrey}
	} ));

	m_pLabelStyle = TextStyle::create(WGBP(TextStyle,
									_.font = pFont,
									_.size = 16,
									_.color = Color8::Black));


	Base::setDefaultStyle(m_pTextStyle);

	m_pTextLayoutCentered = BasicTextLayout::create(WGBP(BasicTextLayout,
		_.placement = Placement::Center));


	//

	if (!_loadSkins(pVisitor))
		return false;

	m_pLayout = PackLayout::create({ .wantedSize = PackLayout::WantedSize::Default,
	.expandFactor = PackLayout::Factor::Weight, .shrinkFactor = PackLayout::Factor::Zero });

	auto pPopupOverlay = PopupOverlay::create();
	
	
	
	auto pBasePanel = PackPanel::create();
	pBasePanel->setAxis(Axis::Y);
	pBasePanel->setLayout(m_pLayout);

	pBasePanel->slots << createTopBar();



	auto pSplitPanel = SplitPanel::create();
	pSplitPanel->setAxis(Axis::Y);
	pSplitPanel->setResizeRatio(1.f);
	pSplitPanel->setSplitFactor(0.5f);			//TODO: SplitPanel::setSplitFactor() does not work! Replace with some other function?
	pSplitPanel->setHandleSkin(m_pButtonSkin);
	
	pSplitPanel->slots[0] = createDisplayPanel();
	pSplitPanel->slots[1] = createControlsPanel();

	pBasePanel->slots << pSplitPanel;

	pBasePanel->slots.setWeight(0, 2, {0.f,1.f});

	pPopupOverlay->mainSlot = pBasePanel;
	
	pRoot->slot = pPopupOverlay;

	return true;
}



//____ createTopBar() ________________________________________________________

Widget_p MyApp::createTopBar()
{
	
	auto pBar = PackPanel::create();
	pBar->setAxis(Axis::X);
	pBar->setLayout(m_pLayout);
	pBar->setSkin(m_pPlateSkin);

	auto pLoadButton = Button::create( WGBP(Button,
											_.skin = m_pButtonSkin,
											_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = "Load" )
											));

	auto pSpacer1 = Filler::create( WGBP(Filler, _.defaultSize = { 20,1 } ));

	auto pSpacer2 = Filler::create(WGBP(Filler, _.defaultSize = { 20,1 }));

	
	m_pDisplayToggles = PackPanel::create();



	pBar->slots << pLoadButton;
	pBar->slots << pSpacer1;
	pBar->slots << m_pDisplayToggles;
	pBar->slots << pSpacer2;


	pBar->slots.setWeight( 0, 4, {0.f, 1.f, 0.f, 1.f});
	


	Base::msgRouter()->addRoute( pLoadButton, MsgType::Select, [this](Msg*pMsg){this->selectAndLoadStream();});


	return pBar;
}

//____ createDisplayPanel() _____________________________________________________

Widget_p MyApp::createDisplayPanel()
{
	auto pWindow = ScrollPanel::create();

	pWindow->scrollbarX.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pWindow->scrollbarX.setBar(m_pPlateSkin);

	pWindow->scrollbarY.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pWindow->scrollbarY.setBar(m_pPlateSkin);

	pWindow->setSkin(ColorSkin::create(Color::DarkSlateBlue));

	pWindow->setAutohideScrollbars(true, true);
	pWindow->setSizeConstraints(SizeConstraint::None, SizeConstraint::None);
	pWindow->setPlacement(Placement::Center);

	auto pLineup = PackPanel::create();
	pLineup->setAxis(Axis::X);


	
	pWindow->slot = pLineup;
	
	m_pScreenLineup = pLineup;

	return pWindow;
}

//____ createControlsPanel() _____________________________________________________

Widget_p MyApp::createControlsPanel()
{
	auto pBase = SplitPanel::create();
	pBase->setAxis(Axis::X);
	pBase->setResizeRatio(1.f);
	pBase->setSplitFactor(0.5f);			//TODO: SplitPanel::setSplitFactor() does not work! Replace with some other function?
	pBase->setHandleSkin(m_pButtonSkin);

	auto pLeftSection = PackPanel::create();
	pLeftSection->setAxis(Axis::Y);
	pLeftSection->setLayout(m_pLayout);
	pLeftSection->setSkin(m_pPlateSkin);


	auto pLogSection = PackPanel::create();
	pLogSection->setAxis(Axis::Y);
	pLogSection->setLayout(m_pLayout);
	pLogSection->setSkin(m_pPlateSkin);

	pBase->slots[0] = pLeftSection;
	pBase->slots[1] = pLogSection;

	// Create log section content

	auto pLogWindow = ScrollPanel::create();

	pLogWindow->scrollbarX.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pLogWindow->scrollbarX.setBar(m_pPlateSkin);

	pLogWindow->scrollbarY.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pLogWindow->scrollbarY.setBar(m_pPlateSkin);

	pLogWindow->setAutohideScrollbars(true, true);
	pLogWindow->setSizeConstraints(SizeConstraint::GreaterOrEqual, SizeConstraint::GreaterOrEqual);

	auto pLogText = TextDisplay::create({ .skin = ColorSkin::create(Color8::LightYellow)});


	pLogWindow->slot = pLogText;

	pLogSection->slots << pLogWindow;

	// Create left section content



	auto pPlayButtons = PackPanel::create();
	pPlayButtons->setAxis(Axis::X);
	pPlayButtons->setLayout(m_pLayout);

	auto pLongLeftButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = " << ")
	));


	auto pLeftButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = " < ")
	));

	auto pPauseButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = " || ")
	));


	auto pRightButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = " > ")
	));

	auto pLongRightButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = " >> ")
	));

	pPlayButtons->slots << Filler::create();
	pPlayButtons->slots << pLongLeftButton;
	pPlayButtons->slots << pLeftButton;
	pPlayButtons->slots << pPauseButton;
	pPlayButtons->slots << pRightButton;
	pPlayButtons->slots << pLongRightButton;
	pPlayButtons->slots << Filler::create();

	pPlayButtons->slots.setWeight(1, 5, 0.f);

	pLeftSection->slots << pPlayButtons;


	Base::msgRouter()->addRoute(pLeftButton, MsgType::Select, [this](Msg* pMsg)
		{
		});

	Base::msgRouter()->addRoute(pRightButton, MsgType::Select, [this](Msg* pMsg)
		{
		});




	pLeftSection->slots << Filler::create();

	pLeftSection->slots.setWeight(pLeftSection->slots.begin(), pLeftSection->slots.end() - 1, 0);

	return pBase;
}


//____ _loadSkins() ___________________________________________________________

bool MyApp::_loadSkins(Visitor * pVisitor)
{
	string path = "resources/greyskin/";

	auto pPlateSurf = pVisitor->loadSurface(path + "plate.bmp");
	auto pButtonSurf = pVisitor->loadSurface(path + "button.bmp");
	auto pStateButtonSurf = pVisitor->loadSurface(path + "state_button.bmp");
	auto pCheckBoxSurf = pVisitor->loadSurface(path + "checkbox.png");

	if (!pPlateSurf || !pButtonSurf || !pStateButtonSurf)
		return false;

	m_pPlateSkin = BlockSkin::create(WGBP(BlockSkin,
		_.surface = pPlateSurf,
		_.axis = Axis::X,
		_.frame = 4,
		_.padding = 4
	));

	m_pButtonSkin = BlockSkin::create(WGBP(BlockSkin,
		_.surface = pButtonSurf,
		_.axis = Axis::X,
		_.frame = 4,
		_.padding = { 4,10,4,10 },
		_.states = { State::Normal, State::Hovered, State::Pressed, State::Disabled }
	));

	m_pToggleButtonSkin = BlockSkin::create(WGBP(BlockSkin,
		_.surface = pStateButtonSurf,
		_.axis = Axis::X,
		_.frame = 4,
		_.padding = 4,
		_.states = { State::Normal, State::Hovered, State::Selected, State::SelectedHovered, State::Disabled }
	));

	m_pCheckBoxSkin = BlockSkin::create(WGBP(BlockSkin,
		_.surface = pCheckBoxSurf,
		_.axis = Axis::Y,
		_.frame = 3,
//		_.defaultSize = { 12,12 },
		_.states = { State::Normal, State::Selected }
	));

	m_pSectionSkin = BoxSkin::create(WGBP(BoxSkin,
		_.color = HiColor::White,
		_.outline = 1,
		_.outlineColor = Color8::Black,
		_.padding = { 2,2,2,2 }
	));


	return true;
}

//____ selectAndLoadStream() ___________________________________________________

void MyApp::selectAndLoadStream()
{
	auto selectedFile = m_pAppVisitor->openFileDialog("Select GfxStream", "", { "*.wax", "*.dat" }, "Stream files");
	
	if( selectedFile.empty()  )
		return;

	loadStream(selectedFile);
}

//____ loadStream() ____________________________________________________________

bool MyApp::loadStream(std::string path)
{

	auto pStream = m_pAppVisitor->loadBlob(path);

	m_pStreamBlob = pStream;

	// Parse stream, find all frames

	m_frames.clear();

	for (auto pChunk = GfxStream::iterator(pStream->begin()); pChunk != GfxStream::iterator(pStream->end()); pChunk++ )
	{
		if (pChunk->type() == GfxChunkId::BeginRender)
			m_frames.push_back(pChunk);
	}

	//

	setupScreens();
	updateGUIAfterReload();

	return true;
}

//____ setupScreens() _________________________________________________________

void MyApp::setupScreens()
{
	m_screens.clear();

	SurfaceFactory_p	pFactory = Base::activeContext()->surfaceFactory();

	for (int i = 0; i < 11; i++)
	{
		auto pSurf = pFactory->createSurface({ .format = PixelFormat::RGB_565_bigendian, .identity = int(CanvasRef::Default) + i, .size = {240,240}});
		pSurf->fill(HiColor::Black);

		m_screens.push_back(pSurf);
	}
}

//____ updateGUIAfterReload() ________________________________________________

void MyApp::updateGUIAfterReload()
{

	m_pScreenLineup->slots.clear();
	m_pDisplayToggles->slots.clear();


	int toggleNb = 0;

	for (auto pScreen : m_screens)
	{
		// Fill in screen lineup

		auto pPanel = TwoSlotPanel::create();
		pPanel->setAxis(Axis::Y);
		pPanel->setSkin(m_pPlateSkin);

		auto pLabel = TextDisplay::create( WGBP(TextDisplay,
											_.display.text = toString((CanvasRef)pScreen->identity()),
											_.display.layout = m_pTextLayoutCentered
										));

		auto pDisplay = SurfaceDisplay::create({ .surface = pScreen });

		pPanel->slots[0] = pLabel;
		pPanel->slots[1] = pDisplay;

		m_pScreenLineup->slots << pPanel;

		// Fill in toggles

		char label[6];
		sprintf(label, " %d ", toggleNb);

		auto pToggle = ToggleButton::create( WGBP(ToggleButton,
												_.skin = m_pToggleButtonSkin,
												_.label.text = label,
												_.selected = true
											) );

		auto pScreenLineup = m_pScreenLineup;
		Base::msgRouter()->addRoute(pToggle, MsgType::Toggle, [toggleNb, pScreenLineup](Msg* pMsg)
			{
				auto pMessage = static_cast<ToggleMsg*>(pMsg);
				pScreenLineup->slots[toggleNb].setVisible(pMessage->isSet());
			});

		m_pDisplayToggles->slots << pToggle;

		toggleNb++;
	}




	m_pScreenLineup->slots.setPadding(m_pScreenLineup->slots.begin(), m_pScreenLineup->slots.end(), 6);
}