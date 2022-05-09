
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

	m_pTextStyle = TextStyle::create(WGBP(TextStyle,
									$.font = pFont,
									$.size = 12,
									$.color = Color8::Black ));

	m_pLabelStyle = TextStyle::create(WGBP(TextStyle,
									$.font = pFont,
									$.size = 14,
									$.color = Color8::Black));


	Base::setDefaultStyle(m_pTextStyle);

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


//____ tintModeToggled() ______________________________________________________

void MyApp::tintModeToggled(Msg* pMsg)
{

}

//____ blendModeToggled() ______________________________________________________

void MyApp::blendModeToggled(Msg* pMsg)
{

}

//____ sourceFormatToggled() ______________________________________________________

void MyApp::sourceFormatToggled(Msg* pMsg)
{

}

//____ destFormatToggled() ______________________________________________________

void MyApp::destFormatToggled(Msg* pMsg)
{

}


//____ _buildList() ___________________________________________________________

Widget_p	MyApp::_buildList()
{
	auto pList = PackPanel::create();
	pList->setAxis(Axis::Y);

	auto pTopSection = PackPanel::create();
	pTopSection->setAxis(Axis::X);
	pTopSection->setSkin(m_pPlateSkin);

	// Togglebuttons for TintModes

	auto pColumn = PackPanel::create();
	pColumn->setAxis(Axis::Y);
	pColumn->slots << TextDisplay::create(WGBP(TextDisplay,
												$.display.text = "Tint Modes",
												$.display.style = m_pLabelStyle));

	TintMode tintModes[] = { TintMode::Flat, TintMode::GradientX, TintMode::GradientY, TintMode::GradientXY };

	for (auto tintMode : tintModes)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
												$.id = (int)tintMode;
												$.icon.skin = m_pToggleButtonSkin,
												$.icon.padding = { 0,4,0,0 },
												$.label.text = toString(tintMode) ));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->tintModeToggled(pMsg); });
		pColumn->slots << pWidget;
	}
	
	pTopSection->slots << pColumn;

	// Togglebuttons for BlendModes

	pColumn = PackPanel::create();
	pColumn->setAxis(Axis::Y);
	pColumn->slots << TextDisplay::create(WGBP(TextDisplay,
												$.display.text = "Blend Modes",
												$.display.style = m_pLabelStyle ));

	BlendMode blendModes[] = { BlendMode::Add, BlendMode::Subtract, BlendMode::Multiply, BlendMode::Max, BlendMode::Min, BlendMode::Invert, BlendMode::Morph };

	for (auto blendMode : blendModes)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
												$.id = (int)blendMode;
												$.icon.skin = m_pToggleButtonSkin,
												$.icon.padding = { 0,4,0,0 },
												$.label.text = toString(blendMode)));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->blendModeToggled(pMsg); });
		pColumn->slots << pWidget;
	}

	pTopSection->slots << pColumn;


	// Togglebuttons for Source Format

	pColumn = PackPanel::create();
	pColumn->setAxis(Axis::Y);
	pColumn->slots << TextDisplay::create(WGBP(TextDisplay,
		$.display.text = "Source Format",
		$.display.style = m_pLabelStyle));

	PixelFormat sourceFormats[] = { PixelFormat::BGRA_8_linear, PixelFormat::BGRA_8_sRGB, PixelFormat::BGRX_8_linear, PixelFormat::BGRX_8_sRGB,
								PixelFormat::BGR_8_linear, PixelFormat::BGR_8_sRGB, PixelFormat::BGRA_4_linear, PixelFormat::BGR_565_linear,
								PixelFormat::CLUT_8_linear, PixelFormat::CLUT_8_sRGB, PixelFormat::A_8, PixelFormat::RGB_565_bigendian };

	for (auto format : sourceFormats)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
			$.id = (int)format;
		$.icon.skin = m_pToggleButtonSkin,
			$.icon.padding = { 0,4,0,0 },
			$.label.text = toString(format)));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->sourceFormatToggled(pMsg); });
		pColumn->slots << pWidget;
	}

	pTopSection->slots << pColumn;

	// Togglebuttons for Dest Format

	pColumn = PackPanel::create();
	pColumn->setAxis(Axis::Y);
	pColumn->slots << TextDisplay::create(WGBP(TextDisplay,
		$.display.text = "Dest Format",
		$.display.style = m_pLabelStyle));

	PixelFormat destFormats[] = { PixelFormat::BGRA_8_linear, PixelFormat::BGRA_8_sRGB, PixelFormat::BGRX_8_linear, PixelFormat::BGRX_8_sRGB,
								PixelFormat::BGR_8_linear, PixelFormat::BGR_8_sRGB, PixelFormat::BGRA_4_linear, PixelFormat::BGR_565_linear,
								PixelFormat::A_8, PixelFormat::RGB_565_bigendian };

	for (auto format : destFormats)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
			$.id = (int)format;
			$.icon.skin = m_pToggleButtonSkin,
			$.icon.padding = { 0,4,0,0 },
			$.label.text = toString(format)));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->destFormatToggled(pMsg); });
		pColumn->slots << pWidget;
	}

	pTopSection->slots << pColumn;

	pTopSection->slots.setPadding(0, pTopSection->slots.size(), Border( 0, 5, 0, 5 ) );

	pList->slots << pTopSection;
	return pList;
}

