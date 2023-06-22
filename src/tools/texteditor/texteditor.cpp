
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

//____ createEditorWindow() ______________________________________________________

bool MyApp::createEditorWindow( const std::string& windowTitle )
{
	
	auto pWindow = m_pAppVisitor->createWindow({ .size = {800,600}, .title = windowTitle });

	if (m_editorWindows.empty())
		setupGUI();


	auto pEditorWindow = EditorWindow::create(pWindow, this);

	m_editorWindows.push_back(pEditorWindow);
	
		
	auto pWindowRaw = pEditorWindow.rawPtr();
	auto pThis = this;
	pWindow->setCloseRequestHandler([pThis,pWindowRaw](void) {
		
		pThis->m_editorWindows.erase(std::remove_if(pThis->m_editorWindows.begin(),
		pThis->m_editorWindows.end(),
		[pWindowRaw](const EditorWindow_p& pWindow ) {return pWindow.rawPtr() == pWindowRaw;}  ),
		pThis->m_editorWindows.end());

		return true;
	});
	
	return true;
}

//____ createButton() _________________________________________________________

Button_p MyApp::createButton(const char* label)
{
	return Button::create(WGBP(Button,
		_.skin = m_pButtonSkin,
		_.label = WGBP(Text, _.layout = m_pTextLayoutCentered, _.style = m_pTextStyle, _.text = label)
	));
}

//____ createScrollPanel() ____________________________________________________

ScrollPanel_p MyApp::createScrollPanel()
{
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

	return pScrollPanel;
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
	
	m_editorWindows.back()->setContent(pText);
	
	return true;
}
