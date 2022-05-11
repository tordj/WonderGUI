
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
	m_pDB = new KernelDB();

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

	if (!_loadSkins(pVisitor))
		return false;


	auto pSizeBroker = UniformSizeBroker::create();

	auto pBasePanel = PackPanel::create();
	pBasePanel->setAxis(Axis::Y);
	pBasePanel->setSizeBroker(pSizeBroker);

	auto pWindow = ScrollPanel::create();
	pWindow->setSkin(ColorSkin::create(Color8::AntiqueWhite));
	pWindow->scrollbarY.setBackground(BoxSkin::create(WGBP(BoxSkin,
		$.color = Color8::DarkOliveGreen,
		$.outline = 1,
		$.outlineColor = Color8::Black)));
	pWindow->scrollbarY.setBar(m_pPlateSkin);
	pWindow->setAutohideScrollbars(true, false);
	pWindow->setSizeConstraints(SizeConstraint::Equal, SizeConstraint::GreaterOrEqual);
	pWindow->slot = _buildList();
		
	pBasePanel->slots << _buildButtonRow();
	pBasePanel->slots << pWindow;
	pBasePanel->slots.setWeight(0, 1, 0.f);

	pRoot->slot = pBasePanel;

	m_pWindow = pWindow;

	return true;
}


//____ tintModeToggled() ______________________________________________________

void MyApp::tintModeToggled(Msg* _pMsg)
{
	auto pMsg = static_cast<ToggleMsg*>(_pMsg);
	auto pWidget = static_cast<Widget*>(pMsg->sourceRawPtr());

	m_pDB->setTintMode((TintMode)pWidget->id(), pMsg->isSet());
	_refreshSummary();
}

//____ blendModeToggled() ______________________________________________________

void MyApp::blendModeToggled(Msg* _pMsg)
{
	auto pMsg = static_cast<ToggleMsg*>(_pMsg);
	auto pWidget = static_cast<Widget*>(pMsg->sourceRawPtr());

	m_pDB->setBlendMode((BlendMode)pWidget->id(), pMsg->isSet());
	_refreshSummary();
}

//____ sourceFormatToggled() ______________________________________________________

void MyApp::sourceFormatToggled(Msg* _pMsg)
{
	auto pMsg = static_cast<ToggleMsg*>(_pMsg);
	auto pWidget = static_cast<Widget*>(pMsg->sourceRawPtr());

	m_pDB->setSrcFormat((PixelFormat)pWidget->id(), pMsg->isSet());
	_refreshSummary();
}

//____ destFormatToggled() ______________________________________________________

void MyApp::destFormatToggled(Msg* _pMsg)
{
	auto pMsg = static_cast<ToggleMsg*>(_pMsg);
	auto pWidget = static_cast<Widget*>(pMsg->sourceRawPtr());

	m_pDB->setDestFormat((PixelFormat)pWidget->id(), pMsg->isSet());
	_refreshSummary();
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
		$.surface = pPlateSurf,
		$.axis = Axis::X,
		$.frame = 4,
		$.padding = 4
	));

	m_pButtonSkin = BlockSkin::create(WGBP(BlockSkin,
		$.surface = pButtonSurf,
		$.axis = Axis::X,
		$.frame = 4,
		$.padding = { 4,6,4,6 },
		$.states = { State::Normal, State::Hovered, State::Pressed, State::Disabled }
	));

	m_pToggleButtonSkin = BlockSkin::create(WGBP(BlockSkin,
		$.surface = pStateButtonSurf,
		$.axis = Axis::X,
		$.frame = 4,
		$.padding = 4,
		$.states = { State::Normal, State::Hovered, State::Selected, State::SelectedHovered, State::Disabled }
	));

	m_pCheckBoxSkin = BlockSkin::create(WGBP(BlockSkin,
		$.surface = pCheckBoxSurf,
		$.axis = Axis::Y,
		$.frame = 3,
		$.defaultSize = { 12,12 },
		$.states = { State::Normal, State::Selected }
	));

	return true;
}


//____ _refreshSummary() _______________________________________________________

void MyApp::_refreshSummary()
{
	m_pWindow->slot.clear();
	
	m_pWindow->slot = _buildList();

//	m_pList->slots.erase(2, 1);
//	m_pList->slots.insert(2, _buildListSummarySection() );
}


//____ _buildButtonRow() ______________________________________________________

Widget_p	MyApp::_buildButtonRow()
{
	auto pButtonRow = PackPanel::create();
	pButtonRow->setAxis(Axis::X);
	pButtonRow->setSkin(m_pPlateSkin);

	auto pClearButton = Button::create(WGBP(Button,
		$.skin = m_pButtonSkin,
		$.label.text = "Clear"));

	auto pLoadButton = Button::create(WGBP(Button,
		$.skin = m_pButtonSkin,
		$.label.text = "Load"));

	auto pSaveButton = Button::create(WGBP(Button,
		$.skin = m_pButtonSkin,
		$.label.text = "Save"));


	pButtonRow->slots << pClearButton;
	pButtonRow->slots << pLoadButton;
	pButtonRow->slots << pSaveButton;
	pButtonRow->slots.setPadding(pButtonRow->slots.begin(), pButtonRow->slots.end(), Border( 0, 2, 0, 2 ) );

	return pButtonRow;
}


//____ _buildList() ___________________________________________________________

Widget_p MyApp::_buildList()
{
	auto pList = PackPanel::create();
	pList->setAxis(Axis::Y);

	pList->slots << _buildGlobalSettingsSection();
	pList->slots << _buildOptimizedBlitsSection();
	pList->slots << _buildListSummarySection();
	pList->slots << _buildExportSection();

	m_pList = pList;

	return pList;
}

//____ _buildGlobalSettingsSection() __________________________________________

wg::Widget_p MyApp::_buildGlobalSettingsSection()
{
	auto pTopSection = PackPanel::create();
	pTopSection->setAxis(Axis::X);
	pTopSection->setSkin(BoxSkin::create(WGBP(BoxSkin,
		$.color = HiColor::White,
		$.outline = 1,
		$.outlineColor = Color8::Black,
		$.padding = { 4,2,4,2 }
	)));

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
												$.icon.skin = m_pCheckBoxSkin,
												$.icon.padding = { 0,4,0,0 },
												$.label.text = toString(tintMode),
												$.selected = m_pDB->tintMode(tintMode)));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->tintModeToggled(pMsg); });
		pColumn->slots << pWidget;
	}

	pColumn->slots.setPadding(0, pColumn->slots.size(), Border(1, 0, 1, 0));
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
												$.icon.skin = m_pCheckBoxSkin,
												$.icon.padding = { 0,4,0,0 },
												$.label.text = toString(blendMode),
												$.selected = m_pDB->blendMode(blendMode)));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->blendModeToggled(pMsg); });
		pColumn->slots << pWidget;
	}

	pTopSection->slots << pColumn;
	pTopSection->setSizeBroker(UniformSizeBroker::create());


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
			$.icon.skin = m_pCheckBoxSkin,
			$.icon.padding = { 0,4,0,0 },
			$.label.text = toString(format),
			$.selected = m_pDB->srcFormat(format) ));

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
			$.icon.skin = m_pCheckBoxSkin,
			$.icon.padding = { 0,4,0,0 },
			$.label.text = toString(format),
			$.selected = m_pDB->destFormat(format)));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->destFormatToggled(pMsg); });
		pColumn->slots << pWidget;
	}

	pTopSection->slots << pColumn;

	pTopSection->slots.setPadding(0, pTopSection->slots.size(), Border( 0, 5, 0, 5 ) );

	return pTopSection;
}

//____ _buildOptimizedBlitsSection() __________________________________________

wg::Widget_p MyApp::_buildOptimizedBlitsSection()
{
	auto pSection = PackPanel::create();
	pSection->setAxis(Axis::Y);
	pSection->setSkin(BoxSkin::create(WGBP(BoxSkin,
		$.color = HiColor::White,
		$.outline = 1,
		$.outlineColor = Color8::Black,
		$.padding = { 4,2,4,2 }
	)));

	return pSection;
}

//____ _buildListSummarySection() _____________________________________________

wg::Widget_p MyApp::_buildListSummarySection()
{
	auto pSection = PackPanel::create();
	pSection->setAxis(Axis::Y);
	pSection->setSkin(BoxSkin::create(WGBP(BoxSkin,
		$.color = HiColor::White,
		$.outline = 1,
		$.outlineColor = Color8::Black,
		$.padding = { 4,2,4,2 }
	)));

	auto kernelCount = m_pDB->countKernels();

	pSection->slots << TextDisplay::create(WGBP(TextDisplay,
		$.display.text = "Kernels to be generated:",
		$.display.style = m_pLabelStyle));

	pSection->slots << _buildLabeledList(4, { {"Plot: ", kernelCount.plot},
												{"Fill: ", kernelCount.fill},
												{"Line: ", kernelCount.line},
												{"ClipLine: ", kernelCount.clipLine},
												{"PlotList: ", kernelCount.plotList},
												{"Segments: ", kernelCount.segment},
												{"Pass 1 blits: ", kernelCount.pass1blits},
												{"Pass 1 blits (fast8): ", kernelCount.pass1blits_fast8},
												{"Pass 2 blits: ", kernelCount.pass2blits},
												{"Pass 2 blits (fast8): ", kernelCount.pass2blits_fast8},
												{"Custom straight blits: ", kernelCount.optimizedStraightBlits},
												{"Custom transform blits: ", kernelCount.optimizedTransformBlits}
										});


	int total = kernelCount.plot + kernelCount.fill + kernelCount.line + kernelCount.clipLine +
		kernelCount.plotList + kernelCount.segment + kernelCount.pass1blits + kernelCount.pass1blits_fast8 +
		kernelCount.pass2blits + kernelCount.pass2blits_fast8 + kernelCount.optimizedStraightBlits +
		kernelCount.optimizedTransformBlits;



	std::string amount = "Total amount: " + std::to_string(total);

	pSection->slots << TextDisplay::create(WGBP(TextDisplay,
		$.display.text = amount.c_str(),
		$.display.style = m_pLabelStyle));




	return pSection;

}

//____ _buildLabeledList() ____________________________________________________

wg::Widget_p MyApp::_buildLabeledList(int nColumns, std::initializer_list < std::pair<string, int>> list)
{
	auto pSection = PackPanel::create();
	pSection->setAxis(Axis::X);
	pSection->setSizeBroker(UniformSizeBroker::create());

	PackPanel_p columns[16][2];

	for (int i = 0; i < nColumns; i++)
	{
		auto pMain = PackPanel::create();
		pMain->setAxis(Axis::X);

		auto pLabels = PackPanel::create();
		pLabels->setAxis(Axis::Y);

		auto pValues = PackPanel::create();
		pValues->setAxis(Axis::Y);
		
		pMain->slots << pLabels;
		pMain->slots << pValues;
				
		pSection->slots << pMain;

		columns[i][0] = pLabels;
		columns[i][1] = pValues;

		pSection->slots << pMain;
	}

	int column = 0;
	for (auto& entry : list)
	{
		columns[column][0]->slots << TextDisplay::create(WGBP(TextDisplay,
									$.display.text = entry.first.c_str() ));

		columns[column][1]->slots << NumberDisplay::create(WGBP(NumberDisplay,
			$.display.value = entry.second ));

		column = (column + 1) % (nColumns);
	}



	return pSection;

}


//____ _buildExportSection() __________________________________________________

wg::Widget_p MyApp::_buildExportSection()
{
	auto pSection = PackPanel::create();
	pSection->setAxis(Axis::X);
	pSection->setSkin(BoxSkin::create(WGBP(BoxSkin,
		$.color = HiColor::White,
		$.outline = 1,
		$.outlineColor = Color8::Black,
		$.padding = { 4,2,4,2 }
	)));

	pSection->slots << Filler::create();

	pSection->slots << Button::create(WGBP(Button,
		$.skin = m_pButtonSkin,
		$.label.text = "Export"
	));

	pSection->slots << Filler::create();

	pSection->slots[1].setWeight(0);

	pSection->setSizeBroker(UniformSizeBroker::create());

	return pSection;
}
