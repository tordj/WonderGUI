
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


//____ startWindowSize() ______________________________________________________

wg::Size MyApp::startWindowSize()
{
	return { 800,700 };
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

//____ selectAndLoadTTF() __________________________________________________________

bool MyApp::selectAndLoadTTF()
{
	static const char * filters[3] = { "*.ttf", "*.ttc", "*.otf" };
	
	const char * pSelectedFile = m_pAppVisitor->openFileDialog("Select Font", nullptr, 3, filters, "Font Files", false);

	if( pSelectedFile )
	{
		bool bLoaded = loadTTF(pSelectedFile);
		return bLoaded;
	}
	
	return false;
}


//____ loadTTF() _________________________________________________________________

bool MyApp::loadTTF( const char * pPath)
{
	auto pBlob = m_pAppVisitor->loadBlob(pPath);
	
	if( pBlob )
	{
		m_pLoadedFontBlob = pBlob;
		m_pTTFPathDisplay->display.setText(pPath);
		return true;
	}
	
	return false;
}

//____ saveBitmapFont() _________________________________________________________________

bool MyApp::saveBitmapFont()
{
//	static const char * filters[3] = { "*.*", "*.ttc", "*.otf" };
	
	const char * pOutputPath = m_pAppVisitor->saveFileDialog("Enter output name without extension", nullptr, 0, nullptr, nullptr);

	if( pOutputPath )
	{
		// Save image

		int saveFormat = m_pImageFormatSelector->selectedEntryId();

		switch (saveFormat)
		{
			case 0:				// 8-bit SURF (uncompressed)
			{
				auto pCopy = m_pBitmapFontSurface->convert(PixelFormat::BGRA_8);
				
				auto pixbuf = pCopy->allocPixelBuffer();
				pCopy->pushPixels(pixbuf);
				
				for( int y = 0 ; y < pixbuf.rect.h ; y++ )
				{
					uint8_t * pLine = pixbuf.pPixels + pixbuf.pitch*y;
					for( int x = 0 ; x < pixbuf.rect.w ; x++ )
					{
						uint8_t alpha = pLine[x*4+3];
						if( alpha < 3 )
							pLine[x*4+3] = 0;
					}
				}
				
				pCopy->pullPixels(pixbuf);
				pCopy->freePixelBuffer(pixbuf);
				
				auto pIndexedSurface = pCopy->convert(PixelFormat::CLUT_8_linear);
				if( pIndexedSurface )
				{
					std::string path = std::string(pOutputPath) + ".surf";

					std::ofstream out(path, std::ios::binary);
					auto pWriter = SurfaceWriter::create({});
					pWriter->writeSurfaceToStream(out, pIndexedSurface);
					out.close();
				}
				else
				{
					int x = 0;
				}
			}


			break;
			case 1:				// 32-bit SURF (uncompressed)
			{
				std::string path = std::string(pOutputPath) + ".surf";

				std::ofstream out(path, std::ios::binary);
				auto pWriter = SurfaceWriter::create({});
				pWriter->writeSurfaceToStream(out, m_pBitmapFontSurface);
				out.close();
			}
			break;
			
			case 2:				// 32-bit PNG (compressed)
			{
				std::string pngPath = std::string(pOutputPath) + ".png";

				// Save PNG

				auto pixbuf = m_pBitmapFontSurface->allocPixelBuffer();
				m_pBitmapFontSurface->pushPixels(pixbuf);

				PixelDescription pixdesc;
				Util::pixelFormatToDescription(pixbuf.format, pixdesc);

				auto pSDLSurf = SDL_CreateRGBSurfaceFrom(pixbuf.pPixels, pixbuf.rect.w, pixbuf.rect.h, pixdesc.bits, pixbuf.pitch, pixdesc.R_mask, pixdesc.G_mask, pixdesc.B_mask, pixdesc.A_mask);

				IMG_SavePNG(pSDLSurf, pngPath.c_str());
				m_pBitmapFontSurface->freePixelBuffer(pixbuf);
			}
			break;
		}

				
		// Save spec
		
		std::string specPath = std::string(pOutputPath) + ".fnt";

		std::ofstream out( specPath );
		out << m_bitmapFontSpec;
		out.close();
		
		return true;
	}
	
	return false;
}

//____ generateBitmapFont() _________________________________________________________

bool MyApp::generateBitmapFont()
{
	if( !m_pLoadedFontBlob )
		return false;
	
	if( m_pCharsEditor->editor.isEmpty() )
		return false;
	
	spx size = m_pSizeSelector->selectedEntryId() * 64;
	FreeTypeFont::RenderMode renderMode = (FreeTypeFont::RenderMode) m_pModeSelector->selectedEntryId();
	bool bUseSRGB = (m_pSRGBSelector->selectedEntryId() == 1);
			
	auto pFont = FreeTypeFont::create( { .blob = m_pLoadedFontBlob, .renderMode = renderMode, .stemDarkening = bUseSRGB });
	if( !pFont )
		return false;

	pFont->setSize(size);

	String charmap = m_pCharsEditor->editor.text();
	
	generateFontSurface(pFont, charmap);
	generateFontSpec(pFont, charmap);
	
	return true;
}

//____ generateFontSpec() _____________________________________________________

bool MyApp::generateFontSpec( FreeTypeFont * pFont, String& charmap )
{
	int size = pFont->size()/64;
	int whitespace = pFont->whitespaceAdvance()/64;
	int monochrome = pFont->isMonochrome();
	int baseline = pFont->maxAscend()/64;
	int linegap = pFont->lineGap()/64;
	
	
	char buffer[1024];
	
	sprintf( buffer, "[INFO BEGIN]\n\nSIZE: %d\nWHITESPACE: %d\nMONOCHROME: %d\nBASELINE: %d\nLINEGAP: %d\n\n[INFO END]\n\n",
			size, whitespace, monochrome, baseline, linegap );
	
	m_bitmapFontSpec = buffer;
	
	m_bitmapFontSpec += "\n[CHARMAP BEGIN]\n\n";
	m_bitmapFontSpec += CharSeq(charmap).getStdString();
	m_bitmapFontSpec += "\n[CHARMAP END]\n\n";
	
	m_bitmapFontSpec += "\n[KERNING BEGIN]\n\n";
	
	for( int i = 0 ; i < charmap.length() ; i++ )
	{
		Glyph glyph1;
		pFont->getGlyphWithoutBitmap(charmap.chars()[i].code(), glyph1);
		
		for( int j = 0 ; j < charmap.length() ; j++ )
		{
			Glyph glyph2;
			pFont->getGlyphWithoutBitmap(charmap.chars()[j].code(), glyph2);
			
			spx kerningSPX = pFont->kerning(glyph1, glyph2);

			if( glyph2.bearingX < 0 )
				kerningSPX += glyph2.bearingX;
			
			int kerning = (kerningSPX - 32) / 64;
			
			if( kerning != 0 )
			{
				sprintf( buffer, "%c%c %d ", charmap.chars()[i].code(), charmap.chars()[j].code(), kerning );
				m_bitmapFontSpec += buffer;
			}
		}
	}
	
	m_bitmapFontSpec += "\n[KERNING END]\n";

	
	return true;
}

//____ generateFontSurface() ___________________________________________________

bool MyApp::generateFontSurface( FreeTypeFont * pFont, String& chars )
{
	bool bUseSRGB = (m_pSRGBSelector->selectedEntryId() == 1);
		
	PixelFormat	outputFormat = bUseSRGB ? PixelFormat::BGRA_8_sRGB : PixelFormat::BGRA_8_linear;

	
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
	pDevice->setBlendMode(BlendMode::Replace);

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
			spx bearingX = glyph.bearingX;
			spx advance = glyph.advance;
			if( bearingX < 0 )
			{
				advance -= bearingX;
				bearingX = 0;
			}
			
			pDevice->setBlitSource(glyph.pSurface);
			pDevice->blit(pos + CoordSPX(bearingX,baselineOfs + glyph.bearingY), glyph.rect);
			pos.x += advance;
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
											 _.display = WGBP(Text, _.style = m_pTextStyle, _.layout = m_pTextLayoutCentered)
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

	m_pTTFPathDisplay = pPath;
	m_pSizeSelector = pSizeSelector;
	m_pModeSelector = pModeSelector;
	m_pSRGBSelector = pSRGBSelector;

	m_pSizeSelector->selectEntryById(10);
	m_pModeSelector->selectEntryById(int(FreeTypeFont::RenderMode::BestShapes));
	m_pSRGBSelector->selectEntryByIndex(0);

	Base::msgRouter()->addRoute( pLoadButton, MsgType::Select, [this](Msg*pMsg){this->selectAndLoadTTF();});

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
	
	auto pLabel = TextDisplay::create( { .display = {.layout = pLeftBottomLayout, .text = String("Preview") } } );
	auto pFiller = Filler::create();
	auto pGenerateButton = Button::create( { .label = { .text = String("Generate")}, .skin = m_pButtonSkin } );
	
	auto pImageFormatSelector = SelectBox::create();
	pImageFormatSelector->setSkin(m_pButtonSkin);
	pImageFormatSelector->setListSkin(m_pPlateSkin);
	pImageFormatSelector->entries.pushBack({ 	{ 0, String("Uncompressed 8-bit indexed SURF") },
												{ 1, String("Uncompressed 32-bit SURF") },
												{ 2, String("Compressed 32-bit PNG")} });
	pImageFormatSelector->selectEntryByIndex(1);
	m_pImageFormatSelector = pImageFormatSelector;
	
	auto pSaveButton = Button::create( { .label = { .text = String("Save")}, .skin = m_pButtonSkin } );

	Base::msgRouter()->addRoute(pGenerateButton, MsgType::Select, [this](Msg*pMsg){this->generateBitmapFont();this->displayBitmapFont();} );
	
	
	pTopPanel->slots << pLabel;
	pTopPanel->slots << pFiller;
	pTopPanel->slots << pGenerateButton;
	pTopPanel->slots << pImageFormatSelector;
	pTopPanel->slots << pSaveButton;

	pTopPanel->slots.setWeight(0, 5, {0.f,1.f,0.f,0.f,0.f});
	pTopPanel->slots.setPadding( 0,5, Border(0,4,4,4) );
	
	
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
	
	
	Base::msgRouter()->addRoute( pSaveButton, MsgType::Select, [this](Msg*pMsg){this->saveBitmapFont();});


	
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
