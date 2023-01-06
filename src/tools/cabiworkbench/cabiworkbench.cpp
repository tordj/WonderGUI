
#include "cabiworkbench.h"

#include <wondergui.h>
#include <wg_cabi.h>
#include <wg_cabi_root_outcalls.h>
#include <wg_freetypefont.h>

#include <string>
#include <fstream>

#ifdef WIN32
#	include <Windows.h>
#	include <libloaderapi.h>
#else
#	ifdef __APPLE__
#		include <dlfcn.h>
#	else
#	endif
#endif


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

	if (!_setupGUI(pVisitor))
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
	}

	
	void * pLib = _loadLibrary("cabiclient");
	if( pLib == nullptr )
		return false;
	
	
	 
	m_pInitClient = (initClientFunc) _loadSymbol(pLib, "init" );
	m_pUpdateClient = (updateClientFunc) _loadSymbol(pLib, "update" );
	m_pExitClient = (exitClientFunc) _loadSymbol(pLib, "exitX" );


	wg_c_callCollection	c_calls;
	wg_populateCallCollection(&c_calls);
	wg_cabi_root_outcalls rootOutCalls = makeCABI_root_outcalls(m_pCABICapsule);

	m_pInitClient(&c_calls, &rootOutCalls);
	
	return true;
}

//____ update() _______________________________________________________________

bool MyApp::update()
{
	m_pUpdateClient();
	return true;
} 

//____ exit() _________________________________________________________________

void MyApp::exit()
{
	m_pExitClient();
}


//____ _setupGUI() ____________________________________________________________

bool MyApp::_setupGUI(Visitor* pVisitor)
{
	m_pWindow = pVisitor->createWindow({ .size = {800,600}, .title = "CABI Workbench" });

	auto pRoot = m_pWindow->rootPanel();

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
	.expandFactor = PackLayout::Factor::Weight, .shrinkFactor = PackLayout::Factor::Zero });

	auto pPopupOverlay = PopupOverlay::create();
	
	
	
	auto pBasePanel = PackPanel::create();
	pBasePanel->setAxis(Axis::Y);
	pBasePanel->setLayout(m_pLayout);
	
	auto pSplitPanel = SplitPanel::create();
	pSplitPanel->setAxis(Axis::X);
	pSplitPanel->setResizeRatio(1.f);
	pSplitPanel->setSplit(0.5f);			//TODO: SplitPanel::setSplitFactor() does not work! Replace with some other function?
	pSplitPanel->setHandleSkin(m_pButtonSkin);
	
	auto pCABICapsule = CABICapsule::create();



	pSplitPanel->slots[0] = pCABICapsule;
	pSplitPanel->slots[1] = Filler::create({ .skin = m_pButtonSkin });

	pBasePanel->slots << pSplitPanel;

	pBasePanel->slots.setWeight(0, 2, {0.f,1.f});

	pPopupOverlay->mainSlot = pBasePanel;
	
	pRoot->slot = pPopupOverlay;

	m_pCABICapsule = pCABICapsule;

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

//____ _loadLibrary() _________________________________________________________

void * MyApp::_loadLibrary(const char* pPath)
{
#ifdef __APPLE__

//		std::string path = "lib" + pPath + ".dylib";

		return dlopen(pPath, RTLD_LAZY | RTLD_LOCAL);
#endif

#ifdef WIN32

		return (void*) LoadLibraryA(pPath);
#endif
		return nullptr;
}

//____ _loadSymbol() __________________________________________________________

void* MyApp::_loadSymbol(void* pLibrary, const char* pSymbol)
{
#ifdef __APPLE__
	return dlsym(pLib, pSymbol);
#endif

#ifdef WIN32

	void * pSym = GetProcAddress((HMODULE)pLibrary, pSymbol);

	if (pSym == nullptr)
	{
		DWORD err = GetLastError();

		int x = 0;
	}

	return pSym;
#endif
	return nullptr;
}

