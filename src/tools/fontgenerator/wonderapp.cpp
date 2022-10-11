
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
	
	loadTTF("resources/DroidSans.ttf");
		
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

	m_pLayout = PackLayout::create({ .wantedSize = PackLayout::WantedSize::Default,
	.startSize = PackLayout::StartSize::Default, .expandFactor = PackLayout::Factor::Weight, .shrinkFactor = PackLayout::Factor::Zero });

	auto pPopupOverlay = PopupOverlay::create();
	
	
	
	auto pBasePanel = PackPanel::create();
	pBasePanel->setAxis(Axis::Y);
	pBasePanel->setLayout(m_pLayout);
	
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

//____ loadTTF() _________________________________________________________________

bool MyApp::loadTTF( const char * pPath)
{
	std::ifstream file( pPath, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	auto pBlob = Blob::create(size);
	
	if (file.read((char*)pBlob->data(), size))
	{
		m_pLoadedFontBlob = pBlob;
		return true;
	}
	
	return false;
}

//____ saveBitmapFont() _________________________________________________________________

bool MyApp::saveBitmapFont()
{

}

//____ generateBitmapFont() ___________________________________________________

bool MyApp::generateBitmapFont()
{
	if( !m_pLoadedFontBlob )
		return false;
	
	String chars = m_pCharsEditor->editor.text();

	spx size = m_pSizeSelector->selectedEntryId() * 64;
	FreeTypeFont::RenderMode renderMode = (FreeTypeFont::RenderMode) m_pModeSelector->selectedEntryId();
	bool bUseSRGB = (m_pSRGBSelector->selectedEntryId() == 1);
		
	PixelFormat	outputFormat = bUseSRGB ? PixelFormat::BGRA_8_sRGB : PixelFormat::BGRA_8_linear;
	
	auto pFont = FreeTypeFont::create( { .blob = m_pLoadedFontBlob, .renderMode = renderMode, .stemDarkening = bUseSRGB });
	if( !pFont )
		return false;

	
	pFont->setSize(size);
	
	// Calculate size of surface needed.
		
	spx maxWidth = 512*64;
	spx ofsX = 0;
	int nRows = 1;

	for( int i = 0 ; i < chars.length() ; i++ )
	{
		Glyph glyph;
		pFont->getGlyphWithBitmap(chars.chars()[i].code(), glyph);
		
		if( glyph.pSurface )
		{
			ofsX += glyph.rect.w+64;
		
			if( ofsX > maxWidth )
			{
				nRows++;
				ofsX = glyph.rect.w+64;
			}
		}
	}
	
	SizeI surfaceSize;
	
	int rowPixelHeight = (pFont->maxAscend() + pFont->maxDescend()) / 64;
	surfaceSize.h = 1 + (rowPixelHeight+1) * nRows;
	
	if( nRows == 1 )
		surfaceSize.w = ofsX / 64;
	else
		surfaceSize.w = maxWidth / 64;
	
	// Generate and fill in surface
	
	auto pSurface = Base::activeContext()->surfaceFactory()->createSurface( { .format = outputFormat, .size = surfaceSize });
	auto pDevice = Base::activeContext()->gfxDevice();

	HiColor lineColor(1024,0,0,4096);
	
	pDevice->beginRender();
	
	pDevice->beginCanvasUpdate(pSurface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(Color::Transparent);
	pDevice->setBlendMode(BlendMode::Blend);

	CoordSPX pos;
	int rowHeight = (pFont->maxAscend() + pFont->maxDescend()) / 64;
	spx baselineOfs = pFont->maxAscend();
	
	pDevice->setTintColor(Color::White);
	pDevice->drawLine(pos, Direction::Right, surfaceSize.w*64, lineColor);
	pos.y += 64;
	
	for( int i = 0 ; i < chars.length() ; i++ )
	{
		Glyph glyph;
		pFont->getGlyphWithBitmap(chars.chars()[i].code(), glyph);
		
		if( pos.x + glyph.rect.w+64 > surfaceSize.w*64  )
		{
			pos.x = 0;
			pos.y += rowHeight*64;
			pDevice->drawLine(pos, Direction::Right, surfaceSize.w*64, lineColor);
			pos.y += 64;
		}

		if( glyph.pSurface )
		{
			pDevice->setBlitSource(glyph.pSurface);
			pDevice->blit(pos + CoordSPX(0,baselineOfs + glyph.bearingY), glyph.rect);
			pos.x += glyph.rect.w;
			pDevice->drawLine(pos, Direction::Down, rowHeight*64, lineColor );
			pos.x += 64;
		}		
	}

	pos.x = 0;
	pos.y += rowHeight*64;
	pDevice->drawLine(pos, Direction::Right, surfaceSize.w*64, lineColor);

	
	pDevice->endCanvasUpdate();
	pDevice->endRender();
	
	m_pBitmapFontSurface = pSurface;
	
	m_pFontDisplaySurface = Base::activeContext()->surfaceFactory()->createSurface( { .format = PixelFormat::BGRA_8, .size = surfaceSize });

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(m_pFontDisplaySurface);
	pDevice->fill( HiColor::White );
	pDevice->setBlitSource(pSurface);
	pDevice->setTintColor(HiColor::Black);
	pDevice->blit({0,0});
	pDevice->endCanvasUpdate();
	pDevice->endRender();
	
	
	return true;
}

//____ displayBitmapFont() ___________________________________________________

void MyApp::displayBitmapFont()
{
	m_pBitmapDisplay->setImage(m_pFontDisplaySurface);
}

//____ createInputPanel() ________________________________________________________

Widget_p MyApp::createInputPanel()
{
	auto pBase = PackPanel::create();
	pBase->setAxis(Axis::Y);
	pBase->setLayout(m_pLayout);
	pBase->setSkin(m_pPlateSkin);
	
	auto pTopRow = PackPanel::create();
	pTopRow->setAxis(Axis::X);
	pTopRow->setLayout(m_pLayout);
	
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
	
	char tmp[8];
	for( int i = 6 ; i <= 32 ; i++ )
	{
		sprintf( tmp, "%i", i);
		pSizeSelector->entries.pushBack(SelectBoxEntry(i, String(tmp)));
	}
	
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
	
	auto pSRGBLabel = TextDisplay::create( WGBP(TextDisplay,
												_.display = WGBP(Text,
																 _.style = m_pTextStyle,
																 _.text = "Output format:",
																 _.layout = m_pTextLayoutCentered )
												));

	auto pSRGBSelector = SelectBox::create();
	pSRGBSelector->setSkin(m_pButtonSkin);
	pSRGBSelector->setListSkin(m_pPlateSkin);
	pSRGBSelector->entries.pushBack({ 	{ 0, String("Linear RGB") },
											{ 1, String("sRGB") } });

	pBottomRow->slots << pSizeLabel;
	pBottomRow->slots << pSizeSelector;
	pBottomRow->slots << pModeLabel;
	pBottomRow->slots << pModeSelector;
	pBottomRow->slots << pSRGBLabel;
	pBottomRow->slots << pSRGBSelector;

	pBottomRow->slots.setPadding( 0, 6, Border(0,0,6,6) );
	
	
	pBase->slots << pTopRow;
	pBase->slots << pBottomRow;

	pBase->slots[1].setPadding({4,0,0,0});

	m_pSizeSelector = pSizeSelector;
	m_pModeSelector = pModeSelector;
	m_pSRGBSelector = pSRGBSelector;

	m_pSizeSelector->selectEntryByIndex(0);
	m_pModeSelector->selectEntryByIndex(0);
	m_pSRGBSelector->selectEntryByIndex(0);

	
	
	return pBase;
}

//____ createCharsPanel() _____________________________________________________

Widget_p MyApp::createCharsPanel()
{
	auto pBase = PackPanel::create();
	pBase->setAxis(Axis::Y);
	pBase->setLayout(m_pLayout);
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

	m_pCharsEditor = TextEditor::create( WGBP(TextEditor,
											_.skin = m_pSectionSkin
											));
	
	pWindow->slot = m_pCharsEditor;
	
	
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
	pBase->setLayout(m_pLayout);
	pBase->setSkin(m_pPlateSkin);

	
	auto pTopPanel = PackPanel::create();
	pTopPanel->setAxis(Axis::X);
	pTopPanel->setLayout(m_pLayout);
	
	auto pLeftBottomLayout = BasicTextLayout::create( { .placement = Placement::SouthWest } );
	
	auto pLabel = TextDisplay::create( { .display = { .text = String("Preview"), .layout = pLeftBottomLayout } } );
	auto pFiller = Filler::create();
	auto pGenerateButton = Button::create( { .label = { .text = String("Generate")}, .skin = m_pButtonSkin } );
	auto pSaveButton = Button::create( { .label = { .text = String("Save")}, .skin = m_pButtonSkin } );

	Base::msgRouter()->addRoute(pGenerateButton, MsgType::Select, [this](Msg*pMsg){this->generateBitmapFont();this->displayBitmapFont();} );
	
	
	pTopPanel->slots << pLabel;
	pTopPanel->slots << pFiller;
	pTopPanel->slots << pGenerateButton;
	pTopPanel->slots << pSaveButton;

	pTopPanel->slots.setWeight(0, 4, {0.f,1.f,0.f,0.f});
	pTopPanel->slots.setPadding( 0,4, Border(0,4,4,4) );
	
	
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
	pWindow->setSizeConstraints(SizeConstraint::None, SizeConstraint::None);
	
	pBase->slots << pTopPanel;
	pBase->slots << pWindow;

	pBase->slots.setWeight(0, 2, {0.f, 1.f} );
	
	m_pBitmapDisplay = Image::create();
	pWindow->slot = m_pBitmapDisplay;
	
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
