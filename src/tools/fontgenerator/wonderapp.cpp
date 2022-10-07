
#include "wonderapp.h"

#include <wondergui.h>
#include <wg_freetypefont.h>
#include <string>
#include <fstream>

using namespace wg;
using namespace std;

//____ create() _______________________________________________________________

WonderApp_p WonderApp::create()
{
	return new MyApp();
}


//____ startWindowSize() ______________________________________________________

wg::Size MyApp::startWindowSize()
{
	return { 800,700 };
}

//____ init() _________________________________________________________________

bool MyApp::init(Visitor* pVisitor)
{
	if (!_setupGUI(pVisitor))
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
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
	auto pRoot = pVisitor->rootPanel();

	//

	auto pFontBlob = pVisitor->loadBlob("resources/DroidSans.ttf");
	auto pFont = FreeTypeFont::create(pFontBlob);

	m_pTextStyle = TextStyle::create(WGBP(TextStyle,
									_.font = pFont,
									_.size = 14,
									_.color = Color8::Black,
									_.states = {{State::Disabled, Color8::DarkGrey}} ));

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


	m_pSizeBroker = UniformSizeBroker::create();

	auto pPopupOverlay = PopupOverlay::create();
	
	
	
	auto pBasePanel = PackPanel::create();
	pBasePanel->setAxis(Axis::Y);
	pBasePanel->setSizeBroker(m_pSizeBroker);
	
	auto pSplitPanel = SplitPanel::create();
	pSplitPanel->setAxis(Axis::Y);
	pSplitPanel->setResizeRatio(0.f);
	pSplitPanel->setSplitFactor(0.5f);			//TODO: SplitPanel::setSplitFactor() does not work! Replace with some other function?
	pSplitPanel->setHandleSkin(m_pButtonSkin);
	
	pSplitPanel->slots[0] = createCharsPanel();
	pSplitPanel->slots[1] = createOutputPanel();

	pBasePanel->slots << createInputPanel();
	pBasePanel->slots << pSplitPanel;

	pBasePanel->slots.setWeight(0, 2, {0.f,1.f});

	pPopupOverlay->mainSlot = pBasePanel;
	
	pRoot->slot = pPopupOverlay;

	return true;
}

//____ load() _________________________________________________________________

void MyApp::load()
{

}

//____ save() _________________________________________________________________

void MyApp::save()
{

}

//____ createInputPanel() ________________________________________________________

Widget_p MyApp::createInputPanel()
{
	auto pBase = PackPanel::create();
	pBase->setAxis(Axis::Y);
	pBase->setSizeBroker(m_pSizeBroker);
	pBase->setSkin(m_pPlateSkin);
	
	auto pTopRow = PackPanel::create();
	pTopRow->setAxis(Axis::X);
	pTopRow->setSizeBroker(m_pSizeBroker);
	
	auto pLabel = TextDisplay::create( WGBP(TextDisplay,
											_.display = WGBP(Text,
															 _.style = m_pTextStyle,
															 _.text = "Source: ",
															 _.layout = m_pTextLayoutCentered )
											) );

	auto pPath = TextDisplay::create( WGBP(TextDisplay,
											 _.skin = m_pSectionSkin,
											 _.display = WGBP(Text, _.style = m_pTextStyle, _.layout = m_pTextLayoutCentered, _.text = String("C:\\Test.ttf") )
										   ) );
	
	auto pLoadButton = Button::create( WGBP(Button,
											_.skin = m_pButtonSkin,
											_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = "Select" )
											));
	
	pTopRow->slots << pLabel;
	pTopRow->slots << pPath;
	pTopRow->slots << pLoadButton;
	
	pTopRow->slots.setWeight( 0, 3, {0.f, 1.f, 0.f});
	
	auto pBottomRow = PackPanel::create();
	pBottomRow->setAxis(Axis::X);
//	pBottomRow->setSizeBroker(m_pSizeBroker);

	auto pSizeLabel = TextDisplay::create( WGBP(TextDisplay,
												_.display = WGBP(Text,
																 _.style = m_pTextStyle,
																 _.text = "Size:",
																 _.layout = m_pTextLayoutCentered )
												));
	
	auto pSizeSelector = SelectBox::create();
	pSizeSelector->setSkin(m_pButtonSkin);
	pSizeSelector->setListSkin(m_pPlateSkin);
	pSizeSelector->entries.pushBack({ {6, String("6") }, {7, String("7") }, {8, String("8")}, {9, String("9")}, {10, String("10")} });

	

	auto pModeLabel = TextDisplay::create( WGBP(TextDisplay,
												_.display = WGBP(Text,
																 _.style = m_pTextStyle,
																 _.text = "Render mode:",
																 _.layout = m_pTextLayoutCentered )
												));

	auto pModeSelector = SelectBox::create();
	pModeSelector->setSkin(m_pButtonSkin);
	pModeSelector->setListSkin(m_pPlateSkin);
	pModeSelector->entries.pushBack({ 	{ int(FreeTypeFont::RenderMode::Monochrome), String("Monochrome") },
										{ int(FreeTypeFont::RenderMode::CrispEdges), String("CrispEdges") },
										{ int(FreeTypeFont::RenderMode::BestShapes), String("BestShapes")} });

	auto pOutputLabel = TextDisplay::create( WGBP(TextDisplay,
												_.display = WGBP(Text,
																 _.style = m_pTextStyle,
																 _.text = "Output format:",
																 _.layout = m_pTextLayoutCentered )
												));

	auto pOutputSelector = SelectBox::create();
	pOutputSelector->setSkin(m_pButtonSkin);
	pOutputSelector->setListSkin(m_pPlateSkin);
	pOutputSelector->entries.pushBack({ 	{ 0, String("Linear RGB") },
											{ 1, String("sRGB") } });

	
	pBottomRow->slots << pSizeLabel;
	pBottomRow->slots << pSizeSelector;
	pBottomRow->slots << pModeLabel;
	pBottomRow->slots << pModeSelector;
	pBottomRow->slots << pOutputLabel;
	pBottomRow->slots << pOutputSelector;

	pBottomRow->slots.setPadding( 0, 6, Border(0,0,6,6) );
	
	
	pBase->slots << pTopRow;
	pBase->slots << pBottomRow;

	pBase->slots[1].setPadding({4,0,0,0});
	
	return pBase;
}

//____ createCharsPanel() _____________________________________________________

Widget_p MyApp::createCharsPanel()
{
	auto pBase = PackPanel::create();
	pBase->setAxis(Axis::Y);
	pBase->setSizeBroker(m_pSizeBroker);
	pBase->setSkin(m_pPlateSkin);
	
	auto pLabel = TextDisplay::create( { .display = { .text = String("Characters to include") } } );
	
	
	
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

	pWindow->setAutohideScrollbars(true, true);
	pWindow->setSizeConstraints(SizeConstraint::GreaterOrEqual, SizeConstraint::GreaterOrEqual);

	auto pEditor = TextEditor::create( WGBP(TextEditor,
											_.skin = m_pSectionSkin
											));
	
	pWindow->slot = pEditor;
	
	
	pBase->slots << pLabel;
	pBase->slots << pWindow;

	pBase->slots.setWeight(0, 2, {0.f, 1.f} );

	return pBase;
}

//____ createOutputPanel() _____________________________________________________

Widget_p MyApp::createOutputPanel()
{
	auto pBase = PackPanel::create();
	pBase->setAxis(Axis::Y);
	pBase->setSizeBroker(m_pSizeBroker);
	pBase->setSkin(m_pPlateSkin);

	auto pLabel = TextDisplay::create( { .display = { .text = String("Preview") } } );

	
	auto pWindow = ScrollPanel::create();
	pWindow->setSkin(m_pSectionSkin);
	
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
	
	pWindow->setAutohideScrollbars(true, false);
	pWindow->setSizeConstraints(SizeConstraint::Equal, SizeConstraint::GreaterOrEqual);
	
	pBase->slots << pLabel;
	pBase->slots << pWindow;

	pBase->slots.setWeight(0, 2, {0.f, 1.f} );
	
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
