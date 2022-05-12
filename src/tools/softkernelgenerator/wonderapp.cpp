
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
									_.font = pFont,
									_.size = 12,
									_.color = Color8::Black ));

	m_pLabelStyle = TextStyle::create(WGBP(TextStyle,
									_.font = pFont,
									_.size = 14,
									_.color = Color8::Black));


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
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
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

//____ generateSource() _______________________________________________________

void MyApp::generateSource()
{
	ofstream	myStream;
	myStream.open("wg_softkernel.cpp");
	m_pDB->generateSource(myStream);
	myStream.close();
}

//____ clear() _________________________________________________________________

void MyApp::clear()
{
	m_pDB->clear();
	_refreshList();
}

//____ reset() ________________________________________________________________

void MyApp::reset()
{
	m_pDB->reset();
	_refreshList();
}

//____ load() _________________________________________________________________

void MyApp::load()
{

}

//____ save() _________________________________________________________________

void MyApp::save()
{

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
		_.padding = { 4,6,4,6 },
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

	return true;
}


//____ _refreshSummary() _______________________________________________________

void MyApp::_refreshSummary()
{
	_refreshList();
	//	m_pList->slots.erase(2, 1);
	//	m_pList->slots.insert(2, _buildListSummarySection() );
}

//____ _refreshList() _________________________________________________________

void MyApp::_refreshList()
{
	m_pWindow->slot.clear();
	m_pWindow->slot = _buildList();
}



//____ _buildButtonRow() ______________________________________________________

Widget_p	MyApp::_buildButtonRow()
{
	auto pButtonRow = PackPanel::create();
	pButtonRow->setAxis(Axis::X);
	pButtonRow->setSkin(m_pPlateSkin);

	auto pClearButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label.text = "Clear"));

	auto pResetButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label.text = "Reset"));

	auto pLoadButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label.text = "Load"));

	auto pSaveButton = Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label.text = "Save"));

	Base::msgRouter()->addRoute( pClearButton, MsgType::Select, [this](Msg*) {this->clear(); });
	Base::msgRouter()->addRoute( pResetButton, MsgType::Select, [this](Msg*) {this->reset(); });
	Base::msgRouter()->addRoute( pLoadButton, MsgType::Select, [this](Msg*) {this->load(); });
	Base::msgRouter()->addRoute( pSaveButton, MsgType::Select, [this](Msg*) {this->save(); });


	pButtonRow->slots << pClearButton;
	pButtonRow->slots << pResetButton;
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
		_.color = HiColor::White,
		_.outline = 1,
		_.outlineColor = Color8::Black,
		_.padding = { 4,2,4,2 }
	)));

	// Togglebuttons for TintModes

	auto pColumn = PackPanel::create();
	pColumn->setAxis(Axis::Y);
	pColumn->slots << TextDisplay::create(WGBP(TextDisplay,
												_.display.text = "Tint Modes",
												_.display.style = m_pLabelStyle));

	TintMode tintModes[] = { TintMode::None, TintMode::Flat, TintMode::GradientX, TintMode::GradientY, TintMode::GradientXY };

	for (auto tintMode : tintModes)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
												_.id = (int)tintMode;
												_.icon.skin = m_pCheckBoxSkin,
												_.icon.padding = { 0,4,0,0 },
												_.label.text = toString(tintMode),
												_.selected = m_pDB->tintMode(tintMode)));

		if (tintMode == TintMode::None)
			pWidget->setEnabled(false);

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
												_.display.text = "Blend Modes",
												_.display.style = m_pLabelStyle ));

	BlendMode blendModes[] = { BlendMode::Replace, BlendMode::Blend, BlendMode::Add, BlendMode::Subtract, BlendMode::Multiply, BlendMode::Max, BlendMode::Min, BlendMode::Invert, BlendMode::Morph };

	for (auto blendMode : blendModes)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
												_.id = (int)blendMode;
												_.icon.skin = m_pCheckBoxSkin,
												_.icon.padding = { 0,4,0,0 },
												_.label.text = toString(blendMode),
												_.selected = m_pDB->blendMode(blendMode)));

		if (blendMode == BlendMode::Replace || blendMode == BlendMode::Blend)
			pWidget->setEnabled(false);

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
		_.display.text = "Source Format",
		_.display.style = m_pLabelStyle));

	PixelFormat sourceFormats[] = { PixelFormat::BGRA_8_linear, PixelFormat::BGRA_8_sRGB, PixelFormat::BGRX_8_linear, PixelFormat::BGRX_8_sRGB,
								PixelFormat::BGR_8_linear, PixelFormat::BGR_8_sRGB, PixelFormat::BGRA_4_linear, PixelFormat::BGR_565_linear,
								PixelFormat::CLUT_8_linear, PixelFormat::CLUT_8_sRGB, PixelFormat::A_8, PixelFormat::RGB_565_bigendian };

	for (auto format : sourceFormats)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
			_.id = (int)format;
			_.icon.skin = m_pCheckBoxSkin,
			_.icon.padding = { 0,4,0,0 },
			_.label.text = toString(format),
			_.selected = m_pDB->srcFormat(format) ));

		auto pObj = this;
		Base::msgRouter()->addRoute(pWidget, MsgType::Toggle, [pObj](Msg* pMsg) {pObj->sourceFormatToggled(pMsg); });
		pColumn->slots << pWidget;
	}

	pTopSection->slots << pColumn;

	// Togglebuttons for Dest Format

	pColumn = PackPanel::create();
	pColumn->setAxis(Axis::Y);
	pColumn->slots << TextDisplay::create(WGBP(TextDisplay,
		_.display.text = "Dest Format",
		_.display.style = m_pLabelStyle));

	PixelFormat destFormats[] = { PixelFormat::BGRA_8_linear, PixelFormat::BGRA_8_sRGB, PixelFormat::BGRX_8_linear, PixelFormat::BGRX_8_sRGB,
								PixelFormat::BGR_8_linear, PixelFormat::BGR_8_sRGB, PixelFormat::BGRA_4_linear, PixelFormat::BGR_565_linear,
								PixelFormat::A_8, PixelFormat::RGB_565_bigendian };

	for (auto format : destFormats)
	{
		auto pWidget = ToggleButton::create(WGBP(ToggleButton,
			_.id = (int)format;
			_.icon.skin = m_pCheckBoxSkin,
			_.icon.padding = { 0,4,0,0 },
			_.label.text = toString(format),
			_.selected = m_pDB->destFormat(format)));

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
		_.color = HiColor::White,
		_.outline = 1,
		_.outlineColor = Color8::Black,
		_.padding = { 4,2,4,2 }
	)));

	return pSection;
}

//____ _buildListSummarySection() _____________________________________________

wg::Widget_p MyApp::_buildListSummarySection()
{
	auto pSection = PackPanel::create();
	pSection->setAxis(Axis::Y);
	pSection->setSkin(BoxSkin::create(WGBP(BoxSkin,
		_.color = HiColor::White,
		_.outline = 1,
		_.outlineColor = Color8::Black,
		_.padding = { 4,2,4,2 }
	)));

	auto kernelCount = m_pDB->countKernels();

	pSection->slots << TextDisplay::create(WGBP(TextDisplay,
		_.display.text = "Kernels to be generated:",
		_.display.style = m_pLabelStyle));

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
		_.display.text = amount.c_str(),
		_.display.style = m_pLabelStyle));




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
									_.display.text = entry.first.c_str() ));

		columns[column][1]->slots << NumberDisplay::create(WGBP(NumberDisplay,
			_.display.value = entry.second ));

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
		_.color = HiColor::White,
		_.outline = 1,
		_.outlineColor = Color8::Black,
		_.padding = { 4,2,4,2 }
	)));

	pSection->slots << Filler::create();

	pSection->slots << Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label.text = "Generate Source Code"
	));

	pSection->slots << Filler::create();

	pSection->slots[1].setWeight(0);

	pSection->setSizeBroker(UniformSizeBroker::create());

	Base::msgRouter()->addRoute(MsgType::Select, [this](Msg* pMsg) {this->generateSource(); });

	return pSection;
}
