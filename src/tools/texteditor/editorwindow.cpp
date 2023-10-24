
#include "editorwindow.h"
#include "texteditor.h"

#include <wondergui.h>
#include <wg_freetypefont.h>
#include <string>
#include <fstream>


using namespace wg;
using namespace std;


//____ constructor ____________________________________________________________

EditorWindow::EditorWindow(Window_p pWindow, MyApp* pApp)
{
	m_pWindow = pWindow;
	m_pRootPanel = m_pWindow->rootPanel();

	m_pApp = pApp;

	_setupGUI();

}

//____ destructor _____________________________________________________________

EditorWindow::~EditorWindow()
{

}

//____ setContent() _____________________________________________________

void EditorWindow::setContent(const char* pContent)
{
	m_pEditor->editor.setText(pContent);
}


//____ _setupGUI() _______________________________________________________

bool EditorWindow::_setupGUI()
{
	//

	auto pMainContainer = PackPanel::create();
	pMainContainer->setAxis(Axis::Y);

	// Create and setup the top-bar

	auto pTopBar = _createTopBar();


	// Create our TextEditor widget

	auto pTextEditor = TextEditor::create( { .skin = m_pApp->m_pSectionSkin });


	// Create and setup a scrollpanel to wrap the text editor.

	auto pScrollPanel = m_pApp->createScrollPanel();

	pScrollPanel->slot = pTextEditor;


	// 

	pMainContainer->slots << pTopBar;
	pMainContainer->slots << pScrollPanel;
	pMainContainer->setSlotWeight(0, 2, { 0.f, 1.f });


	pMainContainer->setLayout(PackLayout::create( {
		.expandFactor = PackLayout::Factor::Weight,
		.shrinkFactor = PackLayout::Factor::Weight
	} ));


	m_pRootPanel->slot = pMainContainer;
	m_pEditor = pTextEditor;
	return true;
}



//____ _createTopBar() ________________________________________________________

Widget_p EditorWindow::_createTopBar()
{

	auto pBar = PackPanel::create();
	pBar->setAxis(Axis::X);
	pBar->setLayout(m_pApp->m_pLayout);
	pBar->setSkin(m_pApp->m_pPlateSkin);

	auto pLoadButton = m_pApp->createButton("Load");
	auto pSaveButton = m_pApp->createButton("Save");
	auto pNewButton = m_pApp->createButton("New");

	auto pSpacer = Filler::create(WGBP(Filler, _.defaultSize = { 20,1 }));
	
	pBar->slots.pushBack( { {pLoadButton, { .weight = 0 }},
							{pSaveButton, { .weight = 0 }},
							{pSpacer, { .weight = 1 }} } );

	Base::msgRouter()->addRoute(pLoadButton, MsgType::Select, [this](Msg* pMsg) {this->_selectAndLoadFile(); });
	Base::msgRouter()->addRoute(pLoadButton, MsgType::Select, [this](Msg* pMsg) {this->_saveFile(); });

	return pBar;
}


//____ _selectAndLoadFile() ___________________________________________________

void EditorWindow::_selectAndLoadFile()
{
	/*
	 auto selectedFiles = m_pAppVisitor->openMultiFileDialog("Select Images", "", { "*.surf", "*.qoi" }, "Image files");

		if( selectedFiles.empty()  )
			return;

		m_imagePaths = selectedFiles;
		loadImage(0);
	*/
}

//____ _saveFile() ___________________________________________________

bool EditorWindow::_saveFile()
{
	return false;
}

