
#include "streamplayer.h"

#include <wondergui.h>
#include <wg_freetypefont.h>
#include <wg_softgfxdevice.h>
#include <extensions/gfxstream/c-api/wondergfxstream_c.h>
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

	std::string	streampath;

	auto arguments = pVisitor->programArguments();

	if( arguments.size() >= 1 )
		streampath = arguments[0];
	else
	{
		std::vector<std::string>	filetypes;

		streampath = pVisitor->openFileDialog("Load gfxstream", "", filetypes, "");
	}

	if( !_loadStream(streampath) )
	{
		printf("ERROR: Could not load stream '%s'\n", streampath.c_str() );
		return false;
	};

	if (!_setupGUI(pVisitor))
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
	}


	_setupStreamPlaying();




	return true;
}

//____ update() _______________________________________________________________

bool MyApp::update()
{
	wg_pumpUntilFrame(m_pump);
	wg_pumpFrame(m_pump);


	return true;
}

//____ exit() _________________________________________________________________

void MyApp::exit()
{
	if( m_pump )
		wg_release(m_pump);
	if( m_player )
		wg_release(m_player);
	if( m_wrapper )
		wg_release(m_wrapper);
}

//____ _loadStream() __________________________________________________________

bool MyApp::_loadStream(std::string path)
{
	m_pStreamBlob = m_pAppVisitor->loadBlob(path);

	if( !m_pStreamBlob )
		return false;

	return true;
}

//____ _setupStreamPlaying() __________________________________________________

bool MyApp::_setupStreamPlaying()
{
	m_pStreamCanvas = SoftSurface::create( WGBP(Surface,
							  _.size = {800,480},
							  _.canvas = true ));

	m_pDisplay->setSurface(m_pStreamCanvas);

	wg_static_cast<SoftGfxDevice_p>(Base::defaultGfxDevice())->defineCanvas( CanvasRef::Canvas_1, m_pStreamCanvas );

	// We use the C-interface here just to try it! B-)

	wg_obj	gfxDevice = static_cast<Object*>(Base::defaultGfxDevice());
	wg_obj	surfaceFactory = static_cast<Object*>(Base::defaultSurfaceFactory());
	wg_obj	edgemapFactory = static_cast<Object*>(Base::defaultEdgemapFactory());

	


	m_wrapper = wg_createStreamWrapper(m_pStreamBlob->begin(), m_pStreamBlob->end());
	m_player = wg_createStreamPlayer(gfxDevice, surfaceFactory, edgemapFactory );

	wg_component tap = wg_getStreamWrapperOutput(m_wrapper);
	wg_component sink =wg_getStreamPlayerInput(m_player);


	m_pump = wg_createStreamPumpWithInputOutput(tap, sink);



	return true;
}



//____ _setupGUI() ____________________________________________________________

bool MyApp::_setupGUI(Visitor* pVisitor)
{
	m_pWindow = pVisitor->createWindow({ .size = {800,480}, .title = "WonderGUI Stream Player" });

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

	Base::setDefaultStyle(m_pTextStyle);

	m_pTextLayoutCentered = BasicTextLayout::create(WGBP(BasicTextLayout,
		_.placement = Placement::Center));


	//

	auto pBgSkin = ColorSkin::create( Color::PapayaWhip );

	m_pDisplay = SurfaceDisplay::create( WGBP(SurfaceDisplay,
											  _.skin = pBgSkin));


	pRoot->slot = m_pDisplay;



	return true;
}
