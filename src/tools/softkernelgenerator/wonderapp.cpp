
#include "wonderapp.h"

#include <wondergui.h>
#include <wg_freetypefont.h>
#include <string>

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
	return { 640,480 };
}

//____ init() _________________________________________________________________

bool MyApp::init(Visitor* pVisitor)
{
	if (!_setupGUI(pVisitor))
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
	}

	m_pDB = new KernelDB();
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
	delete m_pDB;
}


//____ _setupGUI() ____________________________________________________________

bool MyApp::_setupGUI(Visitor* pVisitor)
{
	auto pRoot = pVisitor->rootPanel();

	//

	auto pFontBlob = pVisitor->loadBlob("resources/DroidSans.ttf");
	auto pFont = FreeTypeFont::create(pFontBlob);

	auto pStyle = TextStyle::create(WGBP(TextStyle,
									$.font = pFont,
									$.size = 12));

	Base::setDefaultStyle(pStyle);

	//

	string path = "resources/greyskin/";

	auto pPlateSurf = pVisitor->loadSurface(path + "plate.bmp");
	auto pButtonSurf = pVisitor->loadSurface(path + "button.bmp");
	auto pStateButtonSurf = pVisitor->loadSurface(path + "state_button.bmp");

	if (!pPlateSurf || !pButtonSurf || !pStateButtonSurf)
		return false;

	m_pPlateSkin = BlockSkin::create(WGBP(BlockSkin,
		$.surface = pPlateSurf,
		$.axis = Axis::X,
		$.frame = 4,
		$.padding = 4
	));

	m_pButtonSkin = BlockSkin::create(WGBP(BlockSkin,
		$.surface = pButtonSurf,
		$.axis = Axis::X,
		$.frame = 4,
		$.padding = 4,
		$.states = { State::Normal, State::Hovered, State::Pressed, State::Disabled }
	));

	m_pToggleButtonSkin = BlockSkin::create(WGBP(BlockSkin,
		$.surface = pStateButtonSurf,
		$.axis = Axis::X,
		$.frame = 4,
		$.padding = 4,
		$.states = { State::Normal, State::Hovered, State::Selected, State::SelectedHovered, State::Disabled }
	));


	auto pSizeBroker = UniformSizeBroker::create();

	auto pBasePanel = PackPanel::create();
	pBasePanel->setAxis(Axis::Y);
	pBasePanel->setSizeBroker(pSizeBroker);

	auto pButtonRow = PackPanel::create();
	pButtonRow->setAxis(Axis::X);
	pButtonRow->setSkin(m_pPlateSkin);

	auto pWindow = ScrollPanel::create();
	pWindow->setSkin(ColorSkin::create(Color8::AntiqueWhite));
	pWindow->scrollbarY.setBackground(BoxSkin::create(WGBP(BoxSkin,
		$.color = Color8::DarkOliveGreen,
		$.outline = 1,
		$.outlineColor = Color8::Black)));
	pWindow->scrollbarY.setBar(m_pPlateSkin);
	pWindow->setAutohideScrollbars(true, false);
	pWindow->slot = _buildList();
	
	
	pBasePanel->slots << pButtonRow;
	pBasePanel->slots << pWindow;
	pBasePanel->slots.setWeight(0, 1, 0.f);


	pRoot->slot = pBasePanel;
	return true;
}

//____ _buildList() ___________________________________________________________

Widget_p	MyApp::_buildList()
{
	auto pList = PackPanel::create();
	pList->setAxis(Axis::Y);

	auto pTopSection = PackPanel::create();
	pTopSection->setAxis(Axis::X);
	pTopSection->setSkin(m_pPlateSkin);

	auto pModeColumn = PackPanel::create();
	pModeColumn->setAxis(Axis::Y);

	auto pTintModeNone = ToggleButton::create();
	pTintModeNone->icon.setSkin(m_pToggleButtonSkin);
	pTintModeNone->icon.setPadding({ 0,4,0,0 });
	pTintModeNone->label.setText("None");

	auto pTintModeFlat = ToggleButton::create();
	pTintModeFlat->icon.setSkin(m_pToggleButtonSkin);
	pTintModeFlat->icon.setPadding({ 0,4,0,0 });
	pTintModeFlat->label.setText("Flat");

	pModeColumn->slots << pTintModeNone;
	pModeColumn->slots << pTintModeFlat;

	
	pTopSection->slots << pModeColumn;


	pList->slots << pTopSection;
	return pList;
}


