
#include <wondergui.h>
#include <wonderplugin.h>
#include <wg_freetypefont.h>

#include <string>
#include <fstream>

using namespace wg;
using namespace std;


#ifdef WIN32
#define	DLLEXPORTPREFIX __declspec(dllexport)
#else
#define	DLLEXPORTPREFIX __attribute__ ((visibility("default")))
#endif

extern "C" {
	DLLEXPORTPREFIX int		init( wg_plugin_interface * pInterface, void * pRealHostBridge,
								  wg_obj hPluginCapsule, wg_obj hGfxDevice, wg_obj hSurfaceFactory );
	DLLEXPORTPREFIX int		update(void);
	DLLEXPORTPREFIX void	exitX(void);

	DLLEXPORTPREFIX void	mapInputKey(uint32_t native_keycode, wg_key translated_keycode);
	DLLEXPORTPREFIX void	mapInputCommand(uint32_t native_keycode, wg_modkeys modkeys, wg_editCommand command);

}


Blob_p loadBlob(const std::string& path);

TextStyle_p		g_pTextStyle;

PluginRoot_p 	g_pPluginRoot;

//____ init() _________________________________________________________________

DLLEXPORTPREFIX int init( wg_plugin_interface * pInterface, void * pRealHostBridge, wg_obj hPluginCapsule, wg_obj hGfxDevice, wg_obj hSurfaceFactory )
{
	PluginBase::init(pInterface, pRealHostBridge);
	
	auto pSurfaceFactory 	= PluginSurfaceFactory::create(hSurfaceFactory);
	auto pGfxDevice 		= PluginGfxDevice::create(hGfxDevice,pSurfaceFactory);
	
	Base::setDefaultSurfaceFactory(pSurfaceFactory);
	Base::setDefaultGfxDevice(pGfxDevice);
	
	
	auto pPluginRoot = PluginRoot::create(hPluginCapsule);

	auto pFontBlob = loadBlob("resources/DroidSans.ttf");
	auto pFont = FreeTypeFont::create(pFontBlob);

	g_pTextStyle = TextStyle::create(WGBP(TextStyle,
									_.font = pFont,
									_.size = 14,
									_.color = Color8::Black,
									_.states = {{State::Disabled, Color8::DarkGrey}
	} ));

	Base::setDefaultStyle(g_pTextStyle);

	
	
	
	
	auto pMainPanel = PackPanel::create();
	pMainPanel->setAxis(Axis::Y);
	pMainPanel->setSkin(ColorSkin::create({ .color = Color::DarkGray, .padding = 4 }));

	auto pLayout = PackLayout::create({
		.expandFactor = PackLayout::Factor::Weight
		});

	pMainPanel->setLayout(pLayout);

	auto pSkin1 = ColorSkin::create({ .states = { {State::Normal, Color::Red}, {State::Hovered, Color::Pink}, {State::Pressed, Color::DarkRed} }  });
	auto pSkin2 = ColorSkin::create({ .states = { {State::Normal, Color::Green}, {State::Hovered, Color::LightGreen}, {State::Pressed, Color::DarkGreen} }  });
	auto pSkin3 = ColorSkin::create({ .states = { {State::Normal, Color::Blue}, {State::Hovered, Color::LightBlue}, {State::Pressed, Color::DarkBlue} }  });

	
	auto pFiller1 = TextEditor::create({ .skin = pSkin1 });
	auto pFiller2 = TextEditor::create({ .skin = pSkin2 });
	auto pFiller3 = Filler::create({ .skin = pSkin3 });

	pMainPanel->slots << pFiller1;
	pMainPanel->slots << pFiller2;
	pMainPanel->slots << pFiller3;

	pPluginRoot->slot = pMainPanel;

	g_pPluginRoot = pPluginRoot;
	
	
	
	return 1;
}

//____ update() _______________________________________________________________

DLLEXPORTPREFIX int update(void)
{
	return 1;
}

//____ exit() _________________________________________________________________

DLLEXPORTPREFIX void exitX(void)
{
	g_pTextStyle = nullptr;
	g_pPluginRoot = nullptr;
	PluginBase::exit();
}

//____ mapInputKey() _______________________________________________________

DLLEXPORTPREFIX void mapInputKey(uint32_t native_keycode, wg_key translated_keycode)
{
	Base::inputHandler()->mapKey(native_keycode, (Key) translated_keycode);
}

//____ mapInputCommand() ___________________________________________________

DLLEXPORTPREFIX void mapInputCommand(uint32_t native_keycode, wg_modkeys modkeys, wg_editCommand command)
{
	Base::inputHandler()->mapCommand(native_keycode, (ModKeys) modkeys, (EditCmd) command);
}


//____ loadBlob() _________________________________________________________

Blob_p loadBlob(const std::string& path)
{
	FILE* fp;

#ifdef WIN32
	errno_t err = fopen_s(&fp, path.c_str(), "rb");
#else
	fp = fopen(path.c_str(), "rb");
#endif
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	int size = (int)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	Blob_p pBlob = Blob::create(size);

	int nRead = (int)fread(pBlob->data(), 1, size, fp);
	fclose(fp);

	if (nRead < size)
		return 0;

	return pBlob;
}

