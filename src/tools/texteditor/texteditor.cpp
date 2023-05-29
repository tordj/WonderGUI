
#include "texteditor.h"

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

//____ init() _________________________________________________________________

bool MyApp::init(Visitor* pVisitor)
{
	m_pAppVisitor = pVisitor;

	if (!setupGUI())
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
	}
	
	// Add any text-file from the argument list

	auto arguments = pVisitor->programArguments();

	if( arguments.empty() )
	{
		createEditorWindow("Untitled 1");
		createEditorWindow("Untitled 2");

	}
	for ( auto& arg : arguments )
	{
		openFile(arg);
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


//____ setupGUI() ____________________________________________________________

bool MyApp::setupGUI()
{

	auto pFontBlob = m_pAppVisitor->loadBlob("resources/DroidSans.ttf");
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

	if (!_loadSkins(m_pAppVisitor))
		return false;

	return true;
}

//____ setupWindowGUI() _______________________________________________________

bool MyApp::setupWindowGUI(EditorWindow& instance)
{
	// Create our TextEditor widget

	auto pTextEditor = TextEditor::create( WGBP(TextEditor,
												_.skin = m_pSectionSkin ));

	// Create and setup a scrollpanel to wrap the text editor.
	
	auto pScrollPanel = ScrollPanel::create();

	pScrollPanel->scrollbarX.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pScrollPanel->scrollbarX.setBar(m_pPlateSkin);

	pScrollPanel->scrollbarY.setBackground(BoxSkin::create(WGBP(BoxSkin,
		_.color = Color8::DarkOliveGreen,
		_.outline = 1,
		_.outlineColor = Color8::Black)));
	pScrollPanel->scrollbarY.setBar(m_pPlateSkin);

	pScrollPanel->setAutohideScrollbars(true, true);
	pScrollPanel->setSizeConstraints(SizeConstraint::GreaterOrEqual, SizeConstraint::GreaterOrEqual);

	
	pScrollPanel->slot = pTextEditor;
	

	instance.pRootPanel->slot = pScrollPanel;
	instance.pEditor = pTextEditor;
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

	auto pSpacer = Filler::create( WGBP(Filler, _.defaultSize = { 20,1 } ));

	
	pBar->slots << pLoadButton;
	pBar->slots << pSpacer;

	pBar->slots.setWeight( 0, 2, {0.f, 1.f});
	


	Base::msgRouter()->addRoute( pLoadButton, MsgType::Select, [this](Msg*pMsg){this->selectAndLoadImage();});

	return pBar;
}

//____ createInfoPanel() _____________________________________________________

Widget_p MyApp::createInfoPanel()
{
	auto pBase = PackPanel::create();
	pBase->setAxis(Axis::Y);
	pBase->setLayout(m_pLayout);
	pBase->setSkin(m_pPlateSkin);

	
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

//____ selectAndLoadImage() ___________________________________________________

void MyApp::selectAndLoadImage()
{
/*
 auto selectedFiles = m_pAppVisitor->openMultiFileDialog("Select Images", "", { "*.surf", "*.qoi" }, "Image files");
	
	if( selectedFiles.empty()  )
		return;

	m_imagePaths = selectedFiles;
	loadImage(0);
*/
}

//____ createEditorWindow() ______________________________________________________

bool MyApp::createEditorWindow( const std::string& windowTitle )
{
	EditorWindow instance;
	
	auto pWindow = m_pAppVisitor->createWindow({ .size = {800,600}, .title = windowTitle });

	instance.pWindow = pWindow;
	instance.pRootPanel = pWindow->rootPanel();
	
	m_editorWindows.push_back(instance);
	
	if( m_editorWindows.size() == 1 )
		setupGUI();
	
	setupWindowGUI(m_editorWindows.back());
	
	auto pWindowRaw = pWindow.rawPtr();
	auto pThis = this;
	pWindow->setCloseRequestHandler([pThis,pWindowRaw](void) {
		
		pThis->m_editorWindows.erase(std::remove_if(pThis->m_editorWindows.begin(), pThis->m_editorWindows.end(), [pWindowRaw](const EditorWindow& instance ) {return instance.pWindow.rawPtr() == pWindowRaw;}  ), pThis->m_editorWindows.end());

		return true;
	});
	
}



//____ openFile() ____________________________________________________________

bool MyApp::openFile(const std::string& path)
{
	createEditorWindow(path);

	
	FILE* fp;

#ifdef WIN32
	errno_t err = fopen_s(&fp, path.c_str(), "rb");
#else
	fp = fopen(path.c_str(), "rb");
#endif
	if (!fp)
		return false;

	fseek(fp, 0, SEEK_END);
	int size = (int)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	auto pText = new char[size+1];
	int nRead = (int)fread(pText, 1, size, fp);
	fclose(fp);

	if (nRead < size)
		return 0;
	
	pText[size] = 0;
	
	m_editorWindows.back().pEditor->editor.setText(pText);
	
	return true;
}
