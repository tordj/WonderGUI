
#include "wonderapp.h"

#include <wondergui.h>
#include <wg_freetypefont.h>
#include <wg_softgfxdevice.h>
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
	.expandFactor = PackLayout::Factor::Weight, .shrinkFactor = PackLayout::Factor::Weight });

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
	pSplitPanel->slots[1] = createLowerPanel();

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
	auto pWindow = _standardScrollPanel();

	pWindow->setSkin(ColorSkin::create(Color::DarkSlateBlue));
	pWindow->setSizeConstraints(SizeConstraint::None, SizeConstraint::None);
	pWindow->setPlacement(Placement::Center);

	auto pLineup = PackPanel::create();
	pLineup->setAxis(Axis::X);


	
	pWindow->slot = pLineup;
	
	m_pScreenLineup = pLineup;

	return pWindow;
}

//____ createLowerPanel() _____________________________________________________

Widget_p MyApp::createLowerPanel()
{
	auto pBase = SplitPanel::create();
	pBase->setAxis(Axis::X);
	pBase->setResizeRatio(1.f);
	pBase->setSplitFactor(0.5f);			//TODO: SplitPanel::setSplitFactor() does not work! Replace with some other function?
	pBase->setHandleSkin(m_pButtonSkin);

	pBase->slots[0] = createNavigationPanel();
	pBase->slots[1] = createLogPanel();

	return pBase;
}

//____ createLogPanel() _______________________________________________________

Widget_p MyApp::createLogPanel()
{
	auto pMain = PackPanel::create();
	pMain->setAxis(Axis::Y);
	pMain->setLayout(m_pLayout);
	pMain->setSkin(m_pPlateSkin);

	auto pLogButtonRow = PackPanel::create();
	pLogButtonRow->setAxis(Axis::X);


	auto pFrameLogButton = ToggleButton::create( WGBP(ToggleButton,
											_.skin = m_pToggleButtonSkin,
											_.selected = true,
											_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = "Frame data" )
											));

	auto pFullLogButton = ToggleButton::create( WGBP(ToggleButton,
											_.skin = m_pToggleButtonSkin,
											_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = "File data" )
											));

	auto pResourcesButton = ToggleButton::create( WGBP(ToggleButton,
											_.skin = m_pToggleButtonSkin,
											_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = "Resources" )
											));

	auto pErrorLogButton = ToggleButton::create( WGBP(ToggleButton,
											_.skin = m_pToggleButtonSkin,
											_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = "Errors" )
											));
	
	
	auto pToggleGroup = ToggleGroup::create();
	pToggleGroup->add(pFrameLogButton);
	pToggleGroup->add(pFullLogButton);
	pToggleGroup->add(pResourcesButton);
	pToggleGroup->add(pErrorLogButton);

	Base::msgRouter()->addRoute(pFrameLogButton, MsgType::Toggle, [this](Msg* pMsg)
		{
			auto pToggleMsg = static_cast<ToggleMsg*>(pMsg);
		
			if( pToggleMsg->isSet() )
				this->showFrameLog();
		});
	
	Base::msgRouter()->addRoute(pFullLogButton, MsgType::Toggle, [this](Msg* pMsg)
		{
		auto pToggleMsg = static_cast<ToggleMsg*>(pMsg);
	
		if( pToggleMsg->isSet() )
			this->showFullLog();
		});

	Base::msgRouter()->addRoute(pResourcesButton, MsgType::Toggle, [this](Msg* pMsg)
		{
		auto pToggleMsg = static_cast<ToggleMsg*>(pMsg);
	
		if( pToggleMsg->isSet() )
			this->showResources();
		});

	Base::msgRouter()->addRoute(pErrorLogButton, MsgType::Toggle, [this](Msg* pMsg)
		{
		auto pToggleMsg = static_cast<ToggleMsg*>(pMsg);
	
		if( pToggleMsg->isSet() )
			this->showErrors();
		});

	
	pLogButtonRow->slots << pFrameLogButton;
	pLogButtonRow->slots << pFullLogButton;
	pLogButtonRow->slots << pResourcesButton;
	pLogButtonRow->slots << pErrorLogButton;

	
	TextEditor::Blueprint displayBP;
	displayBP.skin = ColorSkin::create(Color8::LightYellow);
	displayBP.editor.editMode = TextEditMode::Selectable;
	
	// Create frame log hierarchy
	
	auto pFrameLogWindow = _standardScrollPanel();
	
	auto pFrameLogText = TextEditor::create( displayBP );
	pFrameLogWindow->slot = pFrameLogText;

	m_pFrameLogDisplay = pFrameLogText;

	m_pFrameLogContainer = pFrameLogWindow;

	// Create full log hierarchy
	
	auto pFullLogWindow = _standardScrollPanel();

	auto pFullLogText = TextEditor::create( displayBP );
	pFullLogWindow->slot = pFullLogText;

	m_pFullLogDisplay = pFullLogText;

	m_pFullLogContainer = pFullLogWindow;

	// Create error log
	
	auto pErrorsWindow = _standardScrollPanel();

	auto pErrorsText = TextEditor::create( displayBP );
	pErrorsWindow->slot = pErrorsText;

	m_pErrorsDisplay = pErrorsText;

	m_pErrorsContainer = pErrorsWindow;
	
	Base::setErrorHandler( [this](Error& err)
	{
		char temp[1024];
		
		sprintf(temp, "%s: %s - %s\n", toString( err.severity ), err.function, err.message.c_str() );
		
		m_pErrorsDisplay->editor.append(temp);
	});
	
	
	// Create resources view
	
	auto pResourcesWindow = _standardScrollPanel();
	
	auto pResourcesPanel = PackPanel::create();
	pResourcesPanel->setAxis(Axis::Y);
	pResourcesPanel->setSkin(ColorSkin::create(Color::Black));
	pResourcesWindow->slot = pResourcesPanel;
	
	m_pResourcePanel = pResourcesPanel;
	
	m_pResourceContainer = pResourcesWindow;
	
	// Finish it it up
	
	auto pLogCapsule = SizeCapsule::create();
	pLogCapsule->slot = pFrameLogWindow;
	m_pLogCapsule = pLogCapsule;
	
	pMain->slots << pLogButtonRow;
	pMain->slots << pLogCapsule;

	pMain->slots.setWeight(0, 1, 0.f);
	
	return pMain;
}

//____ createNavigationPanel() ________________________________________________

Widget_p MyApp::createNavigationPanel()
{
	auto pMain = PackPanel::create();
	pMain->setAxis(Axis::Y);
	pMain->setLayout(m_pLayout);
	pMain->setSkin(m_pPlateSkin);
	
	// Create left section content

	auto pProgressText = TextDisplay::create( WGBP(TextDisplay,
											_.display = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = "1/20" )
											));
	m_pProgressText = pProgressText;
	
	auto pSlider = Slider::create( { .handle = m_pButtonSkin,
									 .skin = ColorSkin::create( Color::Black )
	} );
	
	Base::msgRouter()->addRoute(pSlider, MsgType::ValueUpdate, [this](Msg* pMsg)
	{
		auto pMyMsg = static_cast<ValueUpdateMsg*>(pMsg);
		this->setFrame( int(pMyMsg->fraction() * this->m_frames.size() + 0.5f) );
	});
	
	m_pProgressSlider = pSlider;
	

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

	pMain->slots << pProgressText;
	pMain->slots << pSlider;
	pMain->slots << pPlayButtons;

	Base::msgRouter()->addRoute(pLongLeftButton, MsgType::Select, [this](Msg* pMsg)
		{
			this->setFrame(this->m_currentFrame-5);
		});

	Base::msgRouter()->addRoute(pLeftButton, MsgType::Select, [this](Msg* pMsg)
		{
			this->setFrame(this->m_currentFrame-1);
		});

	Base::msgRouter()->addRoute(pRightButton, MsgType::Select, [this](Msg* pMsg)
		{
			this->setFrame(this->m_currentFrame+1);
		});

	Base::msgRouter()->addRoute(pLongRightButton, MsgType::Select, [this](Msg* pMsg)
		{
			this->setFrame(this->m_currentFrame+5);
		});



	pMain->slots << Filler::create();

	pMain->slots.setWeight(pMain->slots.begin(), pMain->slots.end() - 1, 0);
	pMain->slots.setPadding(pMain->slots.begin(), pMain->slots.end(), 2);

	return pMain;
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

	// Setup streamwrapper and pump

	m_pStreamWrapper = GfxStreamWrapper::create(pStream->begin(), pStream->end());
	
	auto pContext = Base::activeContext();
	
	m_pStreamPlayer	= GfxStreamPlayer::create( pContext->gfxDevice(), pContext->surfaceFactory() );
	
	m_pStreamPump = GfxStreamPump::create( GfxStreamSource_p(m_pStreamWrapper.rawPtr(), m_pStreamWrapper->output), GfxStreamSink_p(m_pStreamPlayer.rawPtr(),m_pStreamPlayer->input) );
	
	//
	
	setupScreens();
	updateGUIAfterReload();
	setFrame(0);
	
	return true;
}

//____ setupScreens() _________________________________________________________

void MyApp::setupScreens()
{
	m_screens.clear();

	SurfaceFactory_p	pFactory = Base::activeContext()->surfaceFactory();

	// Ugly typecast! Will only work with SoftGfxDevice!
	SoftGfxDevice_p		pGfxDevice = wg_dynamic_cast<SoftGfxDevice_p>(Base::activeContext()->gfxDevice());
	
	for (int i = 0; i < 11; i++)
	{
		auto pSurf = pFactory->createSurface({ .format = PixelFormat::RGB_565_bigendian, .identity = int(CanvasRef::Default) + i, .size = {240,240}});
		pSurf->fill(HiColor::Black);

		m_screens.push_back(pSurf);
		
		pGfxDevice->defineCanvas(CanvasRef(int(CanvasRef::Default) + i), wg_dynamic_cast<SoftSurface_p>(pSurf));
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

	//
	
	m_pProgressSlider->setSteps( m_frames.size() );

	
	_logFullStream();
	_updateFrameCounterAndSlider();
}

//____ setFrame() _____________________________________________________________

void MyApp::setFrame( int frame )
{
	if( m_frames.size() == 0 )
		return;
	
	if( frame < 0 )
		frame = 0;
	if( frame >= m_frames.size() )
		frame = m_frames.size() -1;
	
	if( frame == m_currentFrame )
		return;
	
	// Update the diplays and log
	
/*	if( frame > m_currentFrame )
	{
		_playFrames( m_currentFrame+1, frame+1 );
	}
	else
*/	{
		_resetStream();
		_playFrames( 0, frame+1 );
	}

	// Update the log
	
	_logFrames( frame, frame+1);

	//

	m_currentFrame = frame;
	
	// Update slider and frame counter
	
	_updateFrameCounterAndSlider();
		
}

//____ showFrameLog() _________________________________________________________

void MyApp::showFrameLog()
{
	m_pLogCapsule->slot = m_pFrameLogContainer;
}

//____ showFullLog() __________________________________________________________

void MyApp::showFullLog()
{
	m_pLogCapsule->slot = m_pFullLogContainer;
}

//____ showResources() ________________________________________________________

void MyApp::showResources()
{
	m_pLogCapsule->slot = m_pResourceContainer;
}

//____ showErrors() ___________________________________________________________

void MyApp::showErrors()
{
	m_pLogCapsule->slot = m_pErrorsContainer;
}



//____ _resetStream() _________________________________________________________

void MyApp::_resetStream()
{
	m_pStreamWrapper->restart();
	m_pStreamPlayer->reset();
}

//____ _playFrames() __________________________________________________________

void MyApp::_playFrames( int begin, int end )
{
	uint8_t * pBegin = begin == 0 ? (uint8_t*) m_pStreamBlob->begin() : (uint8_t*) m_frames[begin];
	uint8_t * pEnd = end == m_frames.size() ? (uint8_t*) m_pStreamBlob->end() : (uint8_t*) m_frames[end];

	auto pWrapper = GfxStreamWrapper::create( pBegin, pEnd );
	
	m_pStreamPump->setInput({pWrapper, pWrapper->output});
	m_pStreamPump->pumpAll();
}

//____ _logFrames() ___________________________________________________________

void MyApp::_logFrames( int begin, int end )
{
	uint8_t * pBegin = begin == 0 ? (uint8_t*) m_pStreamBlob->begin() : (uint8_t*) m_frames[begin];
	uint8_t * pEnd = end == m_frames.size() ? (uint8_t*) m_pStreamBlob->end() : (uint8_t*) m_frames[end];

	auto pWrapper = GfxStreamWrapper::create( pBegin, pEnd );

	std::ostringstream	logStream;
	auto pLogger = GfxStreamLogger::create( logStream );
	pLogger->setDisplayOffset(false);
	
	auto pPump = GfxStreamPump::create( {pWrapper, pWrapper->output}, {pLogger, pLogger->input} );
	pPump->pumpAll();

	m_pFrameLogDisplay->editor.setText( logStream.str() );
}

//____ _updateFrameCounterAndSlider() _________________________________________

void MyApp::_updateFrameCounterAndSlider()
{
	char temp[20];
	
	sprintf( temp, "%d / %d", m_currentFrame+1, (int) m_frames.size() );
	
	
	m_pProgressText->display.setText(temp);
	
	m_pProgressSlider->setValue( m_currentFrame / float(m_frames.size()) );
}


//____ _logFullStream() _______________________________________________________

void MyApp::_logFullStream()
{
	auto pWrapper = GfxStreamWrapper::create( m_pStreamBlob->begin(), m_pStreamBlob->end()  );

	std::ostringstream	logStream;
	auto pLogger = GfxStreamLogger::create( logStream );
	pLogger->setDisplayOffset(true);
	
	auto pPump = GfxStreamPump::create( {pWrapper, pWrapper->output}, {pLogger, pLogger->input} );
	pPump->pumpAll();

	m_pFullLogDisplay->editor.setText( logStream.str() );

}

//____ _standardScrollPanel() _________________________________________________

ScrollPanel_p MyApp::_standardScrollPanel()
{
	auto pWidget = ScrollPanel::create();

	pWidget->scrollbarX.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pWidget->scrollbarX.setBar(m_pPlateSkin);

	pWidget->scrollbarY.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pWidget->scrollbarY.setBar(m_pPlateSkin);

	pWidget->setAutohideScrollbars(true, true);
	pWidget->setSizeConstraints(SizeConstraint::GreaterOrEqual, SizeConstraint::GreaterOrEqual);

	return pWidget;
}
