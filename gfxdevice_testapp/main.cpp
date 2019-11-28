
#ifdef WIN32
#    include <SDL.h>
#    include <SDL_image.h>
#    include <GL/glew.h>
#elif __APPLE__
#    include <SDL2/SDL.h>
#    include <SDL2_image/SDL_image.h>
#else
#    include <SDL2/SDL.h>
#    include <SDL2/SDL_image.h>
#   include <GL/glew.h>
#endif


#include <list>

#include <wondergui.h>
#include <fonts/freetype/wg_freetypefont.h>
#include <gfxdevices/software/wg_softgfxdevice.h>
#include <gfxdevices/software/wg_softsurfacefactory.h>


#include <gfxdevices/stream/wg_streamgfxdevice.h>
#include <gfxdevices/stream/wg_streamsurfacefactory.h>


#include <gfxdevices/opengl/wg_glgfxdevice.h>
#include <gfxdevices/opengl/wg_glsurface.h>
#include <gfxdevices/opengl/wg_glsurfacefactory.h>

#include "wg_fileutil.h"
#include "device.h"

//#include <testunit.h>
#include "testsuites/testsuite.h"
#include "testsuites/blendtests.h"
#include "testsuites/filltests.h"
#include <testsuites/plottests.h>
#include <testsuites/linetests.h>
#include <testsuites/canvasformattests.h>
#include <testsuites/blittests.h>
#include <testsuites/segmenttests.h>
#include <testsuites/wavetests.h>
#include <testsuites/clutblittests.h>
#include <testsuites/mipmaptests.h>
#include <testsuites/blitconsistencytest.h>

using namespace wg;
using namespace std;

enum class DisplayMode
{
	Testee,
	Reference,
	Both,
	Diff,
	Time
};

enum DeviceEnum
{
	TESTEE = 0,
	REFERENCE = 1
};

enum ClipList
{
	One,
	Few,
	Many
};


struct SuiteEntry
{
	bool		bActive = true;
	bool		bWorking;
	int			nbTests;
	TestSuite *	pTesteeSuite;
	TestSuite *	pRefSuite;
};


struct DeviceTest
{
	Test *	pTest = nullptr;
	double	render_time = 0;						// Seconds to render number of rounds
	double	stalling_time = 0;						// Seconds for endRender() call afterwards. OpenGL stalls here. 
};

struct TestEntry
{
	string	name;
	bool	bActive = false;
	bool	bWorking;

	DeviceTest devices[2];							// Testee, followed by Reference
};

const SizeI	g_canvasSize(512, 512);
const SizeI	g_windowSize(1400, 900);



bool		init_system( RectI windowGeo );
void		exit_system();

bool		process_system_events(const RootPanel_p& pRoot);
int64_t		getSystemTicks();
void		update_window_rects( const RectI * pRects, int nRects );

bool		init_wondergui();
void		exit_wondergui();

bool		setup_chrome();
void		teardown_chrome();

void		setup_testdevices();
void		destroy_testdevices();

void		setup_tests();
bool		add_testsuite(TestSuite * pTesteeSuite, TestSuite * pReferenceSuite);
void		regen_testentries();

void		run_tests( Device * pDevice, DeviceEnum device );
void		clock_test(DeviceTest * pDeviceTest, int rounds, Device * pDevice);
void		destroy_tests();

void		update_displaymode();
void		display_test_results();

void		setup_cliplist(ClipList list);


MouseButton translateSDLMouseButton(Uint8 button);
Canvas_p create_canvas();
void refresh_performance_display();
void refresh_performance_measurements();


SDL_Window *		g_pSDLWindow = nullptr;

Surface_p			g_pWindowSurface = nullptr;				// Set by init_system()
GfxDevice_p			g_pBaseGfxDevice = nullptr;
SurfaceFactory_p	g_pBaseSurfaceFactory = nullptr;
RootPanel_p			g_pRoot = nullptr;
ScrollPanel_p		g_pViewPanel = nullptr;

Device *			g_pTesteeDevice = nullptr;
Device *			g_pReferenceDevice = nullptr;

Canvas_p			g_pTesteeCanvas = nullptr;
Canvas_p			g_pReferenceCanvas = nullptr;

Widget_p			g_pPerformanceDisplay = nullptr;
PackList_p			g_pPerformanceList = nullptr;
TextMapper_p		g_pPerformanceValueMapper = nullptr;
SizeBroker_p		g_pPerformanceEntryBroker = nullptr;

vector<Device*>		g_testdevices;
vector<SuiteEntry>	g_testsuites;
vector<TestEntry>	g_tests;

vector<RectI>		g_clipList;

TextStyle_p			g_pButtonLabelStyle = nullptr;
TextMapper_p		g_pButtonLabelMapper = nullptr;

DisplayMode			g_displayMode = DisplayMode::Testee;
float				g_zoomFactor = 1.f;

bool				g_bRedrawTestee = true;
bool				g_bRedrawReference = true;
bool				g_bRefreshPerformance = false;
//____ main() _________________________________________________________________

int main(int argc, char *argv[] )
{
	if (!init_system({ 20,20, g_windowSize }))
		return -1;
	
	if (!init_wondergui() )
		return -1;

	setup_cliplist(ClipList::One);
	setup_testdevices();
	setup_tests();

	if (!setup_chrome())
		return -1;

	update_displaymode();

	bool bContinue = true;

	int64_t prevTicks = getSystemTicks();

	while (bContinue)
	{
		// Handle system events

		bContinue = process_system_events(g_pRoot);
		Base::msgRouter()->dispatch();

		// Run tests

		if (g_bRefreshPerformance)
		{
			refresh_performance_measurements();
			refresh_performance_display();
			g_bRefreshPerformance = false;
		}

		if (g_displayMode != DisplayMode::Time)
		{
			if (g_bRedrawTestee)
			{
				run_tests(g_pTesteeDevice, TESTEE);
				g_bRedrawTestee = false;
			}

			if (g_bRedrawReference)
			{
				run_tests(g_pReferenceDevice, REFERENCE);
				g_bRedrawReference = false;
			}

			display_test_results();
		}

		// Render

		g_pRoot->render();
		SDL_UpdateWindowSurface(g_pSDLWindow);

		//

		SDL_Delay(10);

		// Update time

		int64_t ticks = getSystemTicks();
		Base::msgRouter()->post(TickMsg::create(ticks, (int)(ticks-prevTicks)));
		prevTicks = ticks;
	}

	teardown_chrome();

	destroy_tests();
	destroy_testdevices();

	exit_wondergui();
	exit_system();
	return 0;
}

//____ update_displaymode() ______________________________________________________

void update_displaymode()
{
	Widget_p viewChild;

	// Setup view panel


	switch (g_displayMode)
	{
		case DisplayMode::Testee:
			g_pTesteeCanvas = create_canvas();
			g_pReferenceCanvas = nullptr;
			viewChild = g_pTesteeCanvas;
			break;

		case DisplayMode::Reference:
			g_pTesteeCanvas = nullptr;
			g_pReferenceCanvas = create_canvas();
			viewChild = g_pReferenceCanvas;
			break;

		case DisplayMode::Both:
		{
			g_pTesteeCanvas = create_canvas();
			g_pReferenceCanvas = create_canvas();

			auto pPack = PackPanel::create();
			pPack->setOrientation(Orientation::Horizontal);
			pPack->children << g_pTesteeCanvas;
			pPack->children << g_pReferenceCanvas;
			viewChild = pPack;

			break;
		}
		case DisplayMode::Diff:
			break;
		case DisplayMode::Time:
			refresh_performance_display();
			viewChild = g_pPerformanceDisplay;
			break;
	}

	// Set zoom factor

	if (g_zoomFactor != 1.f)
	{
		SizeI size{ (int) (512 * g_zoomFactor), (int) (512 * g_zoomFactor) };

		if (g_pTesteeCanvas)
			g_pTesteeCanvas->canvas.setPresentationScaling(SizePolicy2D::Scale);

		if (g_pReferenceCanvas)
			g_pReferenceCanvas->canvas.setPresentationScaling(SizePolicy2D::Scale);


		auto pZoomer = SizeCapsule::create();
		pZoomer->setSizes(size, size, size);
		pZoomer->child = viewChild;
		viewChild = pZoomer;

	}

	//

	g_pViewPanel->view = viewChild;

}
 
//____ refresh_performance_display() __________________________________________

void refresh_performance_display()
{
	g_pPerformanceList->children.clear();

	for (TestEntry t : g_tests)
	{
		if (t.bActive)
		{
			auto pEntry = PackPanel::create();
			pEntry->setOrientation(Orientation::Horizontal);
			pEntry->setSizeBroker(g_pPerformanceEntryBroker);

			auto pLabel = TextDisplay::create();
			pLabel->text.set(t.name);

//			auto pValue = ValueDisplay::create();
//			pValue->value.set(t.devices[TESTEE].render_time*1000);

			auto pValuePacker = PackPanel::create();
			pValuePacker->setOrientation(Orientation::Vertical);

			char value[128];

			auto pValueTestee = TextDisplay::create();
//            sprintf(value, " %.1f ms (+ %1.f stalling)", t.devices[TESTEE].render_time * 1000, t.devices[TESTEE].stalling_time * 1000);
			sprintf_s(value, 128, " %.1f ms (+ %1.f stalling)", t.devices[TESTEE].render_time * 1000, t.devices[TESTEE].stalling_time * 1000);
			pValueTestee->text.set(value);
			pValueTestee->text.setTextMapper(g_pPerformanceValueMapper);

			auto pValueRef = TextDisplay::create();
//            sprintf(value, " %.1f ms (+ %1.f stalling)", t.devices[REFERENCE].render_time * 1000, t.devices[REFERENCE].stalling_time * 1000);
			sprintf_s(value, 128, " %.1f ms (+ %1.f stalling)", t.devices[REFERENCE].render_time * 1000, t.devices[REFERENCE].stalling_time * 1000);
			pValueRef->text.set(value);
			pValueRef->text.setTextMapper(g_pPerformanceValueMapper);

			pValuePacker->children << pValueTestee;
			pValuePacker->children << pValueRef;

			pEntry->children << pLabel;
			pEntry->children << pValuePacker;

//			std::map<int,Widget_p> v;
//			v[1] = Button::create();

			std::vector<Widget_p> v;
			v.push_back(Button::create());

			Widget_p * pBegin = nullptr;
			Widget_p * pEnd = nullptr;

//			pEntry->children.add(1, 2);
			pEntry->children.add(v.begin(), v.end());

			pEntry->children.setWeight(0, 2, { 0.f,1.f });
//			pEntry->children.setPadding(1, { 0,0,0,10 });

			g_pPerformanceList->children << pEntry;
		}
	}
}


//____ create_canvas() _______________________________________________________

Canvas_p create_canvas()
{
	auto pCanvas = Canvas::create();
	pCanvas->canvas.setSize(g_canvasSize);
	//pCanvas->canvas.setDevice(GlGfxDevice::create({ 0,0,10,10 }));
	pCanvas->canvas.setSurfaceFactory( GlSurfaceFactory::create() );
	pCanvas->canvas.setBackColor(Color::Black);
	return pCanvas;
}

//____ display_test_results() _________________________________________________

void display_test_results()
{
	switch (g_displayMode)
	{
		case DisplayMode::Testee:
			g_pTesteeCanvas->canvas.surface()->copyFrom(g_pTesteeDevice->canvas(), { 0,0 } );
			g_pTesteeCanvas->canvas.present();
			break;

		case DisplayMode::Reference:
			g_pReferenceCanvas->canvas.surface()->copyFrom(g_pReferenceDevice->canvas(), { 0,0 });
			g_pReferenceCanvas->canvas.present();
			break;

		case DisplayMode::Both:
			g_pTesteeCanvas->canvas.surface()->copyFrom(g_pTesteeDevice->canvas(), { 0,0 });
			g_pTesteeCanvas->canvas.present();

			g_pReferenceCanvas->canvas.surface()->copyFrom(g_pReferenceDevice->canvas(), { 0,0 });
			g_pReferenceCanvas->canvas.present();
			break;

		case DisplayMode::Diff:
			break;

		case DisplayMode::Time:
			break;
	}
}

//____ setup_cliplist() _________________________________________________________

void setup_cliplist(ClipList list)
{
	g_clipList.clear();

	SizeI rectSize;

	switch (list)
	{
	case ClipList::One:
		rectSize = g_canvasSize;
		break;
	case ClipList::Few:
		rectSize = g_canvasSize / 8;
		break;
	case ClipList::Many:
		rectSize = g_canvasSize / 32;
		break;
	}

	CoordI rectOfs;

	while (rectOfs.y + rectSize.h <= g_canvasSize.h)
	{
		while (rectOfs.x + rectSize.w <= g_canvasSize.w)
		{
			g_clipList.push_back({ rectOfs, rectSize });
			rectOfs.x += rectSize.w*2;
		}

		if ((rectOfs.x / rectSize.w) % 2 == 1)
			rectOfs.x = 0;
		else
			rectOfs.x = rectSize.w;

		rectOfs.y += rectSize.h;
	}

	g_bRedrawTestee = true;
	g_bRedrawReference = true;
}


//____ run_tests() ____________________________________________________________

void run_tests(Device * pDevice, DeviceEnum device)
{
	if (!pDevice)
		return;

	auto pGfxDevice = pDevice->beginRender();
//	pDevice->setClip(g_canvasSize);

	pGfxDevice->fill(g_canvasSize, Color::Black);
	pGfxDevice->setClipList(g_clipList.size(), &g_clipList[0] );

	for (auto& test : g_tests)
	{
		if (test.bActive)
		{
			if(test.devices[device].pTest->init != nullptr )
				test.devices[device].pTest->init(pGfxDevice, g_canvasSize);

			test.devices[device].pTest->run(pGfxDevice, g_canvasSize);

			if (test.devices[device].pTest->exit != nullptr)
				test.devices[device].pTest->exit(pGfxDevice, g_canvasSize);
		}
	}

	pGfxDevice->clearClipList();
	pDevice->endRender();
}


//____ refresh_performance_measurements() ___________________________________________________

void refresh_performance_measurements()
{
	for (auto& test : g_tests)
	{
		if (test.bActive)
		{
			clock_test(&test.devices[TESTEE], 1000, g_pTesteeDevice );
			clock_test(&test.devices[REFERENCE], 1000, g_pReferenceDevice );
		}
	}
}


//____ clock_test() ___________________________________________________________

void clock_test(DeviceTest * pDeviceTest, int rounds, Device * pDevice )
{
	Uint64 first = SDL_GetPerformanceCounter();
	Uint64 start;
	Uint64 end;
	Uint64 stallBegin;
	Uint64 stallEnd;

	pDevice->beginRender();
	pDevice->gfxDevice()->setClipList(g_clipList.size(), &g_clipList[0]);

	if(pDeviceTest->pTest->init != nullptr)
		pDeviceTest->pTest->init(pDevice->gfxDevice(), g_canvasSize);

	do { start = SDL_GetPerformanceCounter(); } while (start == first);

	for( int i = 0 ; i < rounds ; i++ )
		pDeviceTest->pTest->run(pDevice->gfxDevice(), g_canvasSize);

	end = SDL_GetPerformanceCounter();

	if (pDeviceTest->pTest->exit != nullptr)
		pDeviceTest->pTest->exit(pDevice->gfxDevice(), g_canvasSize);

	stallBegin = SDL_GetPerformanceCounter();

	pDevice->endRender();

	stallEnd = SDL_GetPerformanceCounter();

	Uint64 freq = SDL_GetPerformanceFrequency();

	pDeviceTest->render_time = (end - start) / (double)freq;
	pDeviceTest->stalling_time = (stallEnd - stallBegin) / (double)freq;
}



//____ setup_testdevices() ____________________________________________________

void setup_testdevices()
{
	auto pSoftwareDevice = new SoftwareDevice();
	pSoftwareDevice->init(g_canvasSize, PixelFormat::BGRA_8);
	g_testdevices.push_back(pSoftwareDevice);

//    auto pSoftwareDevice2 = new SoftwareDevice();
//    pSoftwareDevice2->init(g_canvasSize, PixelFormat::BGRA_8);
//    g_testdevices.push_back(pSoftwareDevice2);

	auto pOpenGLDevice = new OpenGLDevice();
	pOpenGLDevice->init(g_canvasSize, PixelFormat::BGRA_8);
	g_testdevices.push_back(pOpenGLDevice);

//	auto pStreamToSoftwareDevice = new StreamToSoftwareDevice();
//	pStreamToSoftwareDevice->init(g_canvasSize, PixelFormat::BGRA_8);
//	g_testdevices.push_back( pStreamToSoftwareDevice);



//	auto pSoftCanvas = SoftSurface::create(g_canvasSize);
//	auto pSoftDevice = SoftGfxDevice::create(pSoftCanvas);

//	auto pOpenGLCanvas = SoftSurface::create(g_canvasSize);
//	auto pOpenGLDevice = SoftGfxDevice::create(pSoftCanvas);


	//	auto pOpenGLCanvas = GlSurface::create(g_canvasSize);
	//	auto pOpenGL = GlGfxDevice::create(pOpenGLCanvas);

	//	auto pSoft2 = SoftGfxDevice::create();
	//	auto pOpenGL = GlGfxDevice::create( SizeI(256,256) );
	//	auto pOpenGL2 = GlGfxDevice::create(SizeI(256, 256));


	g_pReferenceDevice = g_testdevices[0];
	g_pTesteeDevice = g_testdevices[1];

}

//____ destroy_testdevices() ____________________________________________________

void destroy_testdevices()
{
	for (auto pDevice : g_testdevices)
	{
		pDevice->exit();
		delete pDevice;
	}
	g_testdevices.clear();

	g_pTesteeDevice = nullptr;
	g_pReferenceDevice = nullptr;
}

//____ setup_tests() ____________________________________________________

void setup_tests()
{
	destroy_tests();

	add_testsuite(new BlitConsistencyTests(), new BlitConsistencyTests());
	add_testsuite(new FillTests(), new FillTests());
	add_testsuite(new BlendTests(), new BlendTests());
	add_testsuite( new PlotTests(), new PlotTests());
	add_testsuite(new LineTests(), new LineTests());
	add_testsuite( new CanvasFormatTests(), new CanvasFormatTests());
	add_testsuite(new PatchBlitTests(), new PatchBlitTests());
	add_testsuite(new SegmentTests(), new SegmentTests());
	add_testsuite(new WaveTests(), new WaveTests());
	add_testsuite(new ClutBlitTests(), new ClutBlitTests());
	add_testsuite(new MipmapTests(), new MipmapTests());


	regen_testentries();
}

//____ add_testsuite() _____________________________________________________________

bool add_testsuite(TestSuite * pTesteeSuite, TestSuite * pReferenceSuite)
{
	SuiteEntry se;

	se.pTesteeSuite = pTesteeSuite;
	se.pRefSuite = pReferenceSuite;

	bool bTesteeWorking = pTesteeSuite->init(g_pTesteeDevice->gfxDevice(), g_canvasSize);
	bool bRefWorking = pReferenceSuite->init(g_pReferenceDevice->gfxDevice(), g_canvasSize);

	bool bWorking = (bTesteeWorking && bRefWorking);

	se.bWorking = bWorking;
	se.bActive = bWorking;
	se.nbTests = pTesteeSuite->tests.size();

	g_testsuites.push_back(se);

	return bWorking;
}

//____ regen_testentries() ____________________________________________________

void regen_testentries()
{
	g_tests.clear();

	for (auto& suite : g_testsuites)
	{
		if (suite.bActive)
		{
			for (int i = 0; i < suite.nbTests; i++ )
			{
				TestEntry t;

				t.bWorking = suite.bWorking;
				t.bActive = false;
				t.name = suite.pTesteeSuite->tests[i].name;

				t.devices[TESTEE].pTest = &suite.pTesteeSuite->tests[i];
				t.devices[REFERENCE].pTest = &suite.pRefSuite->tests[i];

				g_tests.push_back(t);
			}
		}
	}
}


//____ destroy_tests() ____________________________________________________

void destroy_tests()
{
	for (auto& suite : g_testsuites)
	{
		delete suite.pTesteeSuite;
		delete suite.pRefSuite;
	}

	g_tests.clear();
	g_testsuites.clear();
}

//____ update_window_rects() __________________________________________________

void update_window_rects(const RectI * pRects, int nRects)
{
	if (nRects == 0)
		return;

	std::vector<SDL_Rect>	rects;

	for (int i = 0; i < nRects; i++)
	{
		SDL_Rect r = { pRects[i].x, pRects[i].y, pRects[i].w, pRects[i].h };
		rects.push_back(r);
	}

	SDL_UpdateWindowSurfaceRects(g_pSDLWindow, &rects.front(), nRects);
}


//____ setup_chrome() _________________________________________________________

bool setup_chrome()
{
	// Load resources

	auto pPlateSurface = FileUtil::loadSurface("../resources/grey_plate.bmp", g_pBaseSurfaceFactory );
	BlockSkin_p pPlateSkin = BlockSkin::createStaticFromSurface(pPlateSurface, BorderI(3));
	pPlateSkin->setContentPadding(BorderI(5));

	auto pPressablePlateSurface = FileUtil::loadSurface("../resources/grey_pressable_plate.bmp", g_pBaseSurfaceFactory );
	BlockSkin_p pPressablePlateSkin = BlockSkin::createClickableFromSurface(pPressablePlateSurface, 0, BorderI(3));
	pPressablePlateSkin->setContentPadding(BorderI(3));

	auto pButtonSurface = FileUtil::loadSurface("../resources/simple_button.bmp", g_pBaseSurfaceFactory );
	BlockSkin_p pSimpleButtonSkin = BlockSkin::createClickableFromSurface(pButtonSurface, 0, BorderI(3));
	pSimpleButtonSkin->setContentPadding(BorderI(5));


    
//    BoxSkin_p pPlateSkin = BoxSkin::create( Border(2), Color::Red, Color::Blue );
//    BoxSkin_p pPressablePlateSkin = pPlateSkin;
//    BoxSkin_p pSimpleButtonSkin = pPlateSkin;
    
	// Setup base layers

	auto pLayerStack = StackPanel::create();
	pLayerStack->setSkin(StaticColorSkin::create(Color::AntiqueWhite));
	g_pRoot->child = pLayerStack;

	// Divid screen into sidebar and canvaspanel with top section

	auto pMidSection = PackPanel::create();
	pMidSection->setOrientation(Orientation::Horizontal);
	pMidSection->setSizeBroker(UniformSizeBroker::create());
	auto it = pLayerStack->children << pMidSection;
	pLayerStack->children.setSizePolicy(it, SizePolicy2D::Stretch);

	auto pSidebar = PackPanel::create();
	pSidebar->setOrientation(Orientation::Vertical);
	pSidebar->setSkin(pPlateSkin);
	pSidebar->setSizeBroker(UniformSizeBroker::create());

	auto pCanvasPanel = PackPanel::create();
	pCanvasPanel->setOrientation(Orientation::Vertical);
	pCanvasPanel->setSizeBroker(UniformSizeBroker::create());

	auto pViewNav = PackPanel::create();
	pViewNav->setOrientation(Orientation::Horizontal);
	pViewNav->setSizeBroker(UniformSizeBroker::create());
	pViewNav->setSkin( pPlateSkin );

	auto pViewPanel = ScrollPanel::create();
	pViewPanel->setSkin( StaticColorSkin::create(Color::SlateGrey) );
	g_pViewPanel = pViewPanel;

	pMidSection->children << pCanvasPanel;
	pMidSection->children << pSidebar;

	pMidSection->children.setWeight(0, 1.f);
	pMidSection->children.setWeight(1, 0.f);


	pCanvasPanel->children << pViewNav;
	pCanvasPanel->children << pViewPanel;

	pCanvasPanel->children.setWeight(0, 0.f);
	pCanvasPanel->children.setWeight(1, 1.f);


	// Setup view navigator

	auto pClipSection = PackPanel::create();
	auto pLeftFiller = Filler::create();
	auto pDispModeSection = PackPanel::create();
	auto pRightFiller = Filler::create();
	auto pDispZoomSection = PackPanel::create();

	pViewNav->children << pClipSection;
	pViewNav->children << pLeftFiller;
	pViewNav->children << pDispModeSection;
	pViewNav->children << pRightFiller;
	pViewNav->children << pDispZoomSection;

	pViewNav->children.setWeight(0, 0.f);
	pViewNav->children.setWeight(1, 1.f);
	pViewNav->children.setWeight(2, 0.f);
	pViewNav->children.setWeight(3, 1.f);
	pViewNav->children.setWeight(4, 0.f);

	// Setup clip mode section

	pClipSection->setOrientation(Orientation::Horizontal);
	pClipSection->setSizeBroker(UniformSizeBroker::create());

	auto pClipLabel = TextDisplay::create();
	pClipLabel->text.set("ClipRects: ");
	pClipLabel->text.setStyle(g_pButtonLabelStyle);


	auto pNoClipButton = Button::create();
	pNoClipButton->setSkin(pSimpleButtonSkin);
	pNoClipButton->label.set("One");
	pNoClipButton->label.setStyle(g_pButtonLabelStyle);
	pNoClipButton->label.setTextMapper(g_pButtonLabelMapper);

	auto pFewButton = Button::create();
	pFewButton->setSkin(pSimpleButtonSkin);
	pFewButton->label.set("Few");
	pFewButton->label.setStyle(g_pButtonLabelStyle);
	pFewButton->label.setTextMapper(g_pButtonLabelMapper);

	auto pManyButton = Button::create();
	pManyButton->setSkin(pSimpleButtonSkin);
	pManyButton->label.set("Many");
	pManyButton->label.setStyle(g_pButtonLabelStyle);
	pManyButton->label.setTextMapper(g_pButtonLabelMapper);

	pClipSection->children << pClipLabel;
	pClipSection->children << pNoClipButton;
	pClipSection->children << pFewButton;
	pClipSection->children << pManyButton;
	
	Base::msgRouter()->addRoute(pNoClipButton, MsgType::Select, [](Msg* pMsg) { setup_cliplist(ClipList::One); });
	Base::msgRouter()->addRoute(pFewButton, MsgType::Select, [](Msg* pMsg) { setup_cliplist(ClipList::Few); });
	Base::msgRouter()->addRoute(pManyButton, MsgType::Select, [](Msg* pMsg) { setup_cliplist(ClipList::Many); });

	// Setup display mode section

	pDispModeSection->setOrientation(Orientation::Horizontal);
	pDispModeSection->setSizeBroker(UniformSizeBroker::create());


	auto pTesteeButton = Button::create();
	pTesteeButton->setSkin(pSimpleButtonSkin);
	pTesteeButton->label.set("Testee");
	pTesteeButton->label.setStyle(g_pButtonLabelStyle);
	pTesteeButton->label.setTextMapper(g_pButtonLabelMapper);

	auto pRefButton = Button::create();
	pRefButton->setSkin(pSimpleButtonSkin);
	pRefButton->label.set("Reference");
	pRefButton->label.setStyle(g_pButtonLabelStyle);
	pRefButton->label.setTextMapper(g_pButtonLabelMapper);

	auto pBothButton = Button::create();
	pBothButton->setSkin(pSimpleButtonSkin);
	pBothButton->label.set("Both");
	pBothButton->label.setStyle(g_pButtonLabelStyle);
	pBothButton->label.setTextMapper(g_pButtonLabelMapper);

	auto pDiffButton = Button::create();
	pDiffButton->setSkin(pSimpleButtonSkin);
	pDiffButton->label.set("Diff");
	pDiffButton->label.setStyle(g_pButtonLabelStyle);
	pDiffButton->label.setTextMapper(g_pButtonLabelMapper);

	auto pTimeButton = Button::create();
	pTimeButton->setSkin(pSimpleButtonSkin);
	pTimeButton->label.set("Time");
	pTimeButton->label.setStyle(g_pButtonLabelStyle);
	pTimeButton->label.setTextMapper(g_pButtonLabelMapper);

	pDispModeSection->children	<< pTesteeButton;
	pDispModeSection->children	<< pRefButton;
	pDispModeSection->children	<< pBothButton;
	pDispModeSection->children	<< pDiffButton;
	pDispModeSection->children  << pTimeButton;

	Base::msgRouter()->addRoute(pTesteeButton, MsgType::Select, [](Msg* pMsg) {
		g_displayMode = DisplayMode::Testee;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pRefButton, MsgType::Select, [](Msg* pMsg) {
		g_displayMode = DisplayMode::Reference;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pBothButton, MsgType::Select, [](Msg* pMsg) {
		g_displayMode = DisplayMode::Both;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pDiffButton, MsgType::Select, [](Msg* pMsg) {
		g_displayMode = DisplayMode::Diff;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pTimeButton, MsgType::Select, [](Msg* pMsg) {
		g_displayMode = DisplayMode::Time;
		update_displaymode();
	});

	// Setup display zoom section

	pDispModeSection->setOrientation(Orientation::Horizontal);
	pDispModeSection->setSizeBroker(UniformSizeBroker::create());

	auto pX1Button = Button::create();
	pX1Button->setSkin(pSimpleButtonSkin);
	pX1Button->label.set(" X1 ");
	pX1Button->label.setStyle(g_pButtonLabelStyle);
	pX1Button->label.setTextMapper(g_pButtonLabelMapper);

	auto pX2Button = Button::create();
	pX2Button->setSkin(pSimpleButtonSkin);
	pX2Button->label.set(" X2 ");
	pX2Button->label.setStyle(g_pButtonLabelStyle);
	pX2Button->label.setTextMapper(g_pButtonLabelMapper);

	auto pX4Button = Button::create();
	pX4Button->setSkin(pSimpleButtonSkin);
	pX4Button->label.set(" X4 ");
	pX4Button->label.setStyle(g_pButtonLabelStyle);
	pX4Button->label.setTextMapper(g_pButtonLabelMapper);

	auto pX8Button = Button::create();
	pX8Button->setSkin(pSimpleButtonSkin);
	pX8Button->label.set(" X8 ");
	pX8Button->label.setStyle(g_pButtonLabelStyle);
	pX8Button->label.setTextMapper(g_pButtonLabelMapper);

	pDispZoomSection->children << pX1Button;
	pDispZoomSection->children << pX2Button;
	pDispZoomSection->children << pX4Button;
	pDispZoomSection->children << pX8Button;

	Base::msgRouter()->addRoute(pX1Button, MsgType::Select, [](Msg* pMsg) {
		g_zoomFactor = 1.f;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pX2Button, MsgType::Select, [](Msg* pMsg) {
		g_zoomFactor = 2.f;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pX4Button, MsgType::Select, [](Msg* pMsg) {
		g_zoomFactor = 4.f;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pX8Button, MsgType::Select, [](Msg* pMsg) {
		g_zoomFactor = 8.f;
		update_displaymode();
	});

	// Setup sidebar

	auto pTestList = PackList::create();
	pTestList->setSelectMode(SelectMode::FlipOnSelect);

	Base::msgRouter()->addRoute(pTestList, MsgType::ItemsSelect, [&](Msg* _pMsg) {

		auto pMsg = ItemsSelectMsg::cast(_pMsg);
		auto p = pMsg->items();
		for (int x = 0; x < pMsg->nbItems(); x++)
			g_tests[p[x].id].bActive = true;

		g_bRedrawTestee = true;
		g_bRedrawReference = true;
	});

	Base::msgRouter()->addRoute(pTestList, MsgType::ItemsUnselect, [&](Msg* _pMsg) {

		auto pMsg = ItemsUnselectMsg::cast(_pMsg);
		auto p = pMsg->items();
		for (int x = 0; x < pMsg->nbItems(); x++)
			g_tests[p[x].id].bActive = false;

		g_bRedrawTestee = true;
		g_bRedrawReference = true;
	});




	auto pSkin = BoxSkin::create(1, Color::White, Color::Black);
	pSkin->setContentPadding(8);

	auto pEntrySkin = BoxSkin::create(1, Color::White, Color::Black);
	pEntrySkin->setContentPadding(8);
	pEntrySkin->setColors(StateEnum::Hovered, Color::AntiqueWhite, Color::Black);
	pEntrySkin->setColors(StateEnum::SelectedHovered, Color::Aquamarine, Color::Black);
	pEntrySkin->setColors(StateEnum::Selected, Color::Aquamarine, Color::Black);

	pTestList->setSkin(pSkin);
	pTestList->setEntrySkin(pEntrySkin);

	int id = 0;
	for (TestEntry&  test : g_tests)
	{
		auto pEntry = TextDisplay::create();
		pEntry->setId(id++);
		pEntry->text.set(test.name);
		pTestList->children.add(pEntry);
	}

	auto pTestScrollPanel = ScrollPanel::create();
	pTestScrollPanel->view.setWidthPolicy(SizePolicy::Confined);
	pTestScrollPanel->view = pTestList;


	auto pTestScrollbar = Scrollbar::create();

	pTestScrollbar->setBackgroundSkin(StaticColorSkin::create(Color::Green));
	pTestScrollbar->setHandleSkin(pSimpleButtonSkin);


	pTestScrollPanel->vscrollbar = pTestScrollbar;

	pSidebar->children << pTestScrollPanel;


	// Setup performance display

	{
		auto pMapper = StdTextMapper::create();
		pMapper->setAlignment(Origo::East);
		g_pPerformanceValueMapper = pMapper;

		auto pBroker = UniformSizeBroker::create();
		g_pPerformanceEntryBroker = pBroker;


		auto pBase = PackPanel::create();
		pBase->setOrientation(Orientation::Vertical);

		auto pList = PackList::create();
		pList->setSkin(StaticColorSkin::create(Color::White));

		auto pOddEntrySkin = BoxSkin::create(0, Color::White, Color::White);
		pOddEntrySkin->setContentPadding(BorderI(0));

		auto pEvenEntrySkin = BoxSkin::create(0, Color::PaleGreen, Color::PaleGreen);
		pEvenEntrySkin->setContentPadding(BorderI(0));

		pList->setEntrySkin(pOddEntrySkin, pEvenEntrySkin);


		// Create the bottom section

		auto pBottom = PackPanel::create();
		pBottom->setSkin(pPlateSkin);
		pBottom->setOrientation(Orientation::Horizontal);
		pBottom->setSizeBroker(UniformSizeBroker::create());

		auto pRefresh = Button::create();
		pRefresh->setSkin(pSimpleButtonSkin);
		pRefresh->label.set("REFRESH");
		pRefresh->label.setStyle(g_pButtonLabelStyle);
		pRefresh->label.setTextMapper(g_pButtonLabelMapper);

		Base::msgRouter()->addRoute(pRefresh, MsgType::Select, [](Msg * pMsg) {
			g_bRefreshPerformance = true;
		});


		auto pFiller = Filler::create();

		auto it = pBottom->children << pFiller;
		pBottom->children << pRefresh;

		//

		pBase->children << pList;
		pBase->children << pBottom;

		g_pPerformanceDisplay = pBase;
		g_pPerformanceList = pList;
	}

	return true;
}

//____ teardown_chrome() ______________________________________________________

void teardown_chrome()
{
}


//____ init_wondergui() _______________________________________________________

bool init_wondergui()
{
	Base::init();

    g_pBaseGfxDevice = GlGfxDevice::create(g_windowSize, 0);
    g_pBaseSurfaceFactory = GlSurfaceFactory::create();

    // g_pBaseSurfaceFactory = SoftSurfaceFactory::create();
    
    // g_pBaseGfxDevice = SoftGfxDevice::create( SoftSurface::cast(g_pWindowSurface));

    
	Context_p pContext = Context::create();
    pContext->setScale(1.0);
    pContext->setSurfaceFactory(g_pBaseSurfaceFactory);
	Base::setActiveContext(pContext);


	g_pRoot = RootPanel::create(g_pBaseGfxDevice);

//	g_pRoot->setDebugMode(true);

	FreeTypeFont::init(g_pBaseSurfaceFactory);

	// Init textmappers

	auto pMapper = StdTextMapper::create();
	pMapper->setAlignment(Origo::Center);
	g_pButtonLabelMapper = pMapper;

	// Init fonts

	Blob_p pFontFile = FileUtil::loadBlob("../resources/DroidSans.ttf");
	FreeTypeFont_p pFont = FreeTypeFont::create(pFontFile, 1);

	TextStyle_p pStyle = TextStyle::create();
	pStyle->setFont(pFont);
	pStyle->setSize(16);
	pStyle->setColor(Color::Black);
	Base::setDefaultStyle(pStyle);

	g_pButtonLabelStyle = TextStyle::create();
	g_pButtonLabelStyle->setFont(pFont);
	g_pButtonLabelStyle->setSize(16);
	g_pButtonLabelStyle->setColor(Color::Black);
	Base::setDefaultStyle(pStyle);


	return true;
}

//____ exit_wondergui() _______________________________________________________

void exit_wondergui()
{
	g_pButtonLabelStyle = nullptr;
	g_pRoot = nullptr;
	g_pBaseGfxDevice = nullptr;
	g_pBaseSurfaceFactory = nullptr;

	g_pViewPanel = nullptr;
	g_pTesteeCanvas = nullptr;
	g_pReferenceCanvas = nullptr;

	FreeTypeFont::exit();
	Base::exit();
}


//____ init_system() _______________________________________________________

bool init_system( RectI windowGeo )
{
	// initialize SDL video
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return false;
	}

	// make sure SDL cleans up before exit
	atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    
	SDL_Window * pWin = SDL_CreateWindow("GfxDevice TestApp", windowGeo.x, windowGeo.y, windowGeo.w, windowGeo.h, SDL_WINDOW_OPENGL /*| SDL_WINDOW_ALLOW_HIGHDPI*/);
	if( pWin == nullptr )
	{
		printf("Unable to create SDL window: %s\n", SDL_GetError());
		return false;
	}
    
	SDL_GLContext context = SDL_GL_CreateContext(pWin);

//	SDL_GL_SetSwapInterval(1);

	//	SDL_Renderer * pRenderer = SDL_CreateRenderer(pWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

#ifdef WIN32  
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
#endif

	glDrawBuffer(GL_FRONT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();


//    GlGfxDevice_p pGfxDevice = GlGfxDevice::create(windowGeo.size());
//    GlGfxDevice::create( {100,100} );

/*
	SDL_Surface * pWinSurf = SDL_GetWindowSurface(pWin);
	if (pWinSurf == nullptr)
	{
		printf("Unable to get window SDL Surface: %s\n", SDL_GetError());
		return false;
	}

	PixelFormat format = PixelFormat::Unknown;

	switch (pWinSurf->format->BitsPerPixel)
	{
		case 32:
			format = PixelFormat::BGRA_8;
			break;
		case 24:
			format = PixelFormat::BGR_8;
			break;
		default:
		{
			printf("Unsupported pixelformat of SDL Surface!\n");
			return false;
		}
	}

    GlGfxDevice_p pGfxDevice = GlGfxDevice::create(windowGeo.size());

    
	g_pSDLWindow = pWin;

	Blob_p pCanvasBlob = Blob::create(pWinSurf->pixels, 0);
	g_pWindowSurface = SoftSurface::create(SizeI(pWinSurf->w, pWinSurf->h), format, pCanvasBlob, pWinSurf->pitch);
*/
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	return true;
}

//____ exit_system() __________________________________________________________

void exit_system()
{
	IMG_Quit();
}

//____ getSystemTicks() ________________________________________________________

int64_t getSystemTicks()
{
	return SDL_GetPerformanceCounter() * 1000 / SDL_GetPerformanceFrequency();
}


//____ process_system_events() ________________________________________________

bool process_system_events(const RootPanel_p& pRoot)
{
	// Process all the SDL events in a loop

	InputHandler_p pInput = Base::inputHandler();

	SDL_Event e;
	while (SDL_PollEvent(&e)) {

		switch (e.type)
		{
		case SDL_QUIT:
			return false;

		case SDL_MOUSEMOTION:
			pInput->setPointer(pRoot, Coord(QPix::fromPixel(e.motion.x), QPix::fromPixel(e.motion.y)));
			break;

		case SDL_MOUSEBUTTONDOWN:
			pInput->setButton(translateSDLMouseButton(e.button.button), true);
			break;

		case SDL_MOUSEBUTTONUP:
			pInput->setButton(translateSDLMouseButton(e.button.button), false);
			break;

		case SDL_MOUSEWHEEL:
		{
			Coord distance(e.wheel.x, e.wheel.y);
			if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
				distance *= -1;

			pInput->setWheelRoll(1, distance);
			break;
		}

		case SDL_KEYDOWN:
		{
			pInput->setKey(e.key.keysym.sym, true);
			break;
		}

		case SDL_KEYUP:
		{
			pInput->setKey(e.key.keysym.sym, false);
			break;
		}

		case SDL_TEXTINPUT:
			pInput->putText(e.text.text);
			break;


		default:
			break;
		}
	}

	return true;
}

//____ translateSDLMouseButton() __________________________________________________
//
// Translate SDL mouse button enums to WonderGUI equivalents.
//
MouseButton translateSDLMouseButton(Uint8 button)
{
	switch (button)
	{
	default:
	case SDL_BUTTON_LEFT:
		return MouseButton::Left;
	case SDL_BUTTON_MIDDLE:
		return MouseButton::Middle;
	case SDL_BUTTON_RIGHT:
		return MouseButton::Right;
	case SDL_BUTTON_X1:
		return MouseButton::X1;
	case SDL_BUTTON_X2:
		return MouseButton::X2;
	}
}



