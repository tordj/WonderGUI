
#include <cmath>

#include <wg_freetypefont.h>
#include <wg_basictextlayout.h>

#include "gfxdevicetester.h"

#include "testsuites/testsuite.h"
#include "testsuites/blendtests.h"
#include "testsuites/filltests.h"
#include "testsuites/plottests.h"
#include "testsuites/linetests.h"
#include "testsuites/canvasformattests.h"
#include "testsuites/blittests.h"
#include "testsuites/segmenttests.h"
#include "testsuites/wavetests.h"
#include "testsuites/clutblittests.h"
#include "testsuites/mipmaptests.h"
#include "testsuites/blitconsistencytest.h"
#include "testsuites/piecharttests.h"
#include "testsuites/tintblittests.h"
#include "testsuites/tintsegmenttests.h"
#include "testsuites/blitblendtests.h"
#include "testsuites/a8tests.h"
#include "testsuites/tiletests.h"
#include "testsuites/canvaslayertests.h"
#include "testsuites/rgb565bigendiantests.h"


using namespace wg;
using namespace std;


//____ constructor ____________________________________________________________

GfxDeviceTester::GfxDeviceTester()
{

}

//____ destructor _____________________________________________________________

GfxDeviceTester::~GfxDeviceTester()
{

}

//____ init() _________________________________________________________________

bool GfxDeviceTester::init( AppVisitor * pVisitor )
{
	m_pVisitor = pVisitor;

	// Init textmappers

	auto pMapper = BasicTextLayout::create({ .placement = Placement::Center });
	g_pButtonLabelMapper = pMapper;

	// Init fonts

	Blob_p pFontFile = pVisitor->loadBlob("resources/DroidSans.ttf");
	FreeTypeFont_p pFont = FreeTypeFont::create(pFontFile, 0);

	TextStyle_p pStyle = TextStyle::create({ .color = Color::Black, .font = pFont, .size = 16 });
	Base::setDefaultStyle(pStyle);

	g_pButtonLabelStyle = TextStyle::create({ .color = Color::Black, .font = pFont, .size = 16 });

	//

	setup_cliplist(ClipList::One);
	setup_tests();

	if (!setup_chrome())
		return false;

	update_displaymode();

	return true;
}

//____ update() _______________________________________________________________

bool GfxDeviceTester::update()
{
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

	return true;
}

//____ exit() _________________________________________________________________

void GfxDeviceTester::exit()
{
	teardown_chrome();

	destroy_tests();

	g_pButtonLabelStyle = nullptr;
	g_pViewPanel = nullptr;
	g_pTesteeCanvas = nullptr;
	g_pReferenceCanvas = nullptr;
}

//____ addTestDevice() ___________________________________________________________

void GfxDeviceTester::addTestDevice( Device * pDevice )
{
	pDevice->init(g_canvasSize, PixelFormat::BGRA_8);
	g_testdevices.push_back(pDevice);

	if( g_pReferenceDevice == nullptr )
		g_pReferenceDevice = pDevice;
	else if( g_pTesteeDevice == nullptr )
		g_pTesteeDevice = pDevice;
}

//____ destroy_testdevices() ____________________________________________________

void GfxDeviceTester::destroy_testdevices()
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

//____ update_displaymode() ______________________________________________________

void GfxDeviceTester::update_displaymode()
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
		pPack->setAxis(Axis::X);
		pPack->slots << g_pTesteeCanvas;
		pPack->slots << g_pReferenceCanvas;
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

	if (g_pTesteeCanvas)
		g_pTesteeCanvas->setZoom(g_zoomFactor);

	if (g_pReferenceCanvas)
		g_pReferenceCanvas->setZoom(g_zoomFactor);


/*
	if (g_zoomFactor != 1.f)
	{
		Size size{ (512 * g_zoomFactor), (512 * g_zoomFactor) };

		if (g_pTesteeCanvas)
			g_pTesteeCanvas->canvas.setPresentationScaling(SizePolicy2D::Scale);

		if (g_pReferenceCanvas)
			g_pReferenceCanvas->canvas.setPresentationScaling(SizePolicy2D::Scale);


		auto pZoomer = SizeCapsule::create();
		pZoomer->setSizes(size, size, size);
		pZoomer->slot = viewChild;
		viewChild = pZoomer;

	}
*/

	//

	g_pViewPanel->slot = viewChild;

}

//____ refresh_performance_display() __________________________________________

void GfxDeviceTester::refresh_performance_display()
{
	g_pPerformanceList->slots.clear();

	for (TestEntry t : g_tests)
	{
		if (t.bActive)
		{
			auto pEntry = PackPanel::create();
			pEntry->setAxis(Axis::X);
			pEntry->setSizeBroker(g_pPerformanceEntryBroker);

			auto pLabel = TextDisplay::create();
			pLabel->display.setText(t.name);

			//			auto pValue = NumberDisplay::create();
			//			pValue->value.set(t.devices[TESTEE].render_time*1000);

			auto pValuePacker = PackPanel::create();
			pValuePacker->setAxis(Axis::Y);

			char value[128];

			auto pValueTestee = TextDisplay::create();

#ifdef WIN32			
			sprintf_s(value, 128, " %.1f ms (+ %1.f stalling)", t.devices[TESTEE].render_time * 1000, t.devices[TESTEE].stalling_time * 1000);
#else
			sprintf(value, " %.1f ms (+ %1.f stalling)", t.devices[TESTEE].render_time * 1000, t.devices[TESTEE].stalling_time * 1000);
#endif
			pValueTestee->display.setText(value);
			pValueTestee->display.setLayout(g_pPerformanceValueMapper);

			auto pValueRef = TextDisplay::create();
#ifdef WIN32			
			sprintf_s(value, 128, " %.1f ms (+ %1.f stalling)", t.devices[REFERENCE].render_time * 1000, t.devices[REFERENCE].stalling_time * 1000);
#else
			sprintf(value, " %.1f ms (+ %1.f stalling)", t.devices[REFERENCE].render_time * 1000, t.devices[REFERENCE].stalling_time * 1000);
#endif
			pValueRef->display.setText(value);
			pValueRef->display.setLayout(g_pPerformanceValueMapper);

			pValuePacker->slots << pValueTestee;
			pValuePacker->slots << pValueRef;

			pEntry->slots << pLabel;
			pEntry->slots << pValuePacker;

			//			std::map<int,Widget_p> v;
			//			v[1] = Button::create();

			std::vector<Widget_p> v;
			v.push_back(Button::create());

			//			pEntry->slots.add(1, 2);
			pEntry->slots.pushBack(v.begin(), v.end());

			pEntry->slots.setWeight(0, 2, { 0.f,1.f });
			//			pEntry->slots.setPadding(1, { 0,0,0,10 });

			g_pPerformanceList->slots << pEntry;
		}
	}
}


//____ create_canvas() _______________________________________________________

SurfaceDisplay_p GfxDeviceTester::create_canvas()
{
	auto pCanvas = SurfaceDisplay::create();
	
	auto pSurface = Base::activeContext()->surfaceFactory()->createSurface(g_canvasSize);
	pCanvas->setSurface(pSurface);
//	pCanvas->canvas.setBackColor(Color::Black);
	return pCanvas;
}

//____ display_test_results() _________________________________________________

void GfxDeviceTester::display_test_results()
{
	switch (g_displayMode)
	{
	case DisplayMode::Testee:
		g_pTesteeCanvas->surface()->copy( { 0,0 }, g_pTesteeDevice->canvas() );
//		g_pTesteeCanvas->canvas.present();
		break;

	case DisplayMode::Reference:
		g_pReferenceCanvas->surface()->copy( { 0,0 }, g_pReferenceDevice->canvas() );
//		g_pReferenceCanvas->canvas.present();
		break;

	case DisplayMode::Both:
		g_pTesteeCanvas->surface()->copy( { 0,0 }, g_pTesteeDevice->canvas() );
//		g_pTesteeCanvas->canvas.present();

		g_pReferenceCanvas->surface()->copy( { 0,0 }, g_pReferenceDevice->canvas() );
//		g_pReferenceCanvas->canvas.present();
		break;

	case DisplayMode::Diff:
		break;

	case DisplayMode::Time:
		break;
	}
}

//____ setup_cliplist() _________________________________________________________

void GfxDeviceTester::setup_cliplist(ClipList list)
{
	g_clipList.clear();

	SizeSPX rectSize;
	SizeSPX canvasSize = g_canvasSize * 64;

	switch (list)
	{
	case ClipList::One:
		rectSize = canvasSize;
		break;
	case ClipList::Few:
		rectSize = canvasSize / 8;
		break;
	case ClipList::Many:
		rectSize = canvasSize / 32;
		break;
	}

	CoordSPX rectOfs;

	while (rectOfs.y + rectSize.h <= canvasSize.h)
	{
		while (rectOfs.x + rectSize.w <= canvasSize.w)
		{
			g_clipList.push_back({ rectOfs, rectSize });
			rectOfs.x += rectSize.w * 2;
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

void GfxDeviceTester::run_tests(Device* pDevice, DeviceEnum device)
{
	if (!pDevice)
		return;

	auto pGfxDevice = pDevice->beginRender();
	//	pDevice->setClip(g_canvasSize);

	pGfxDevice->fill(g_canvasSize*64, Color::Black);
	pGfxDevice->setClipList((int)g_clipList.size(), &g_clipList[0]);

	for (auto& test : g_tests)
	{
		if (test.bActive)
		{
			if (test.devices[device].pTest->init != nullptr)
				test.devices[device].pTest->init(pGfxDevice, g_canvasSize*64);

			test.devices[device].pTest->run(pGfxDevice, g_canvasSize*64);

			if (test.devices[device].pTest->exit != nullptr)
				test.devices[device].pTest->exit(pGfxDevice, g_canvasSize*64);
		}
	}

	pGfxDevice->resetClipList();
	pDevice->endRender();
}


//____ refresh_performance_measurements() ___________________________________________________

void GfxDeviceTester::refresh_performance_measurements()
{
	for (auto& test : g_tests)
	{
		if (test.bActive)
		{
			clock_test(&test.devices[TESTEE], 1000, g_pTesteeDevice);
			clock_test(&test.devices[REFERENCE], 1000, g_pReferenceDevice);
		}
	}
}


//____ clock_test() ___________________________________________________________

void GfxDeviceTester::clock_test(DeviceTest* pDeviceTest, int rounds, Device* pDevice)
{
	uint64_t first = m_pVisitor->time();

	uint64_t start;
	uint64_t end;
	uint64_t stallBegin;
	uint64_t stallEnd;

	pDevice->beginRender();
	pDevice->gfxDevice()->setClipList((int)g_clipList.size(), &g_clipList[0]);

	if (pDeviceTest->pTest->init != nullptr)
		pDeviceTest->pTest->init(pDevice->gfxDevice(), g_canvasSize);

	do { start = m_pVisitor->time(); } while (start == first);

	for (int i = 0; i < rounds; i++)
		pDeviceTest->pTest->run(pDevice->gfxDevice(), g_canvasSize);

	end = m_pVisitor->time();

	if (pDeviceTest->pTest->exit != nullptr)
		pDeviceTest->pTest->exit(pDevice->gfxDevice(), g_canvasSize);

	stallBegin = m_pVisitor->time();

	pDevice->endRender();

	stallEnd = m_pVisitor->time();


	pDeviceTest->render_time = (end - start)/1000000.0;
	pDeviceTest->stalling_time = (stallEnd - stallBegin)/1000000.0;
}

//____ setup_tests() ____________________________________________________

void GfxDeviceTester::setup_tests()
{
	destroy_tests();

	add_testsuite(new BlitConsistencyTests(), new BlitConsistencyTests());
	add_testsuite(new FillTests(), new FillTests());
	add_testsuite(new BlendTests(), new BlendTests());
	add_testsuite(new PlotTests(), new PlotTests());
	add_testsuite(new LineTests(), new LineTests());
	add_testsuite(new CanvasFormatTests(), new CanvasFormatTests());
	add_testsuite(new PatchBlitTests(), new PatchBlitTests());
	add_testsuite(new SegmentTests(), new SegmentTests());
	add_testsuite(new WaveTests(), new WaveTests());
	add_testsuite(new ClutBlitTests(), new ClutBlitTests());
	add_testsuite(new MipmapTests(), new MipmapTests());
	add_testsuite(new PieChartTests(), new PieChartTests());
	add_testsuite(new TintBlitTests(), new TintBlitTests());
	add_testsuite(new TintSegmentTests(), new TintSegmentTests());
	add_testsuite(new BlitBlendTests(), new BlitBlendTests());
	add_testsuite(new A8Tests(), new A8Tests());
	add_testsuite(new TileTests(), new TileTests());
	add_testsuite(new CanvasLayerTests(), new CanvasLayerTests());
//	add_testsuite(new RGB565BigEndianTests(), new RGB565BigEndianTests());	// Can only be handled by software rendering for now.

	regen_testentries();
}

//____ add_testsuite() _____________________________________________________________

bool GfxDeviceTester::add_testsuite(TestSuite* pTesteeSuite, TestSuite* pReferenceSuite)
{
	SuiteEntry se;

	se.pTesteeSuite = pTesteeSuite;
	se.pRefSuite = pReferenceSuite;

	bool bTesteeWorking = pTesteeSuite->init(g_pTesteeDevice->gfxDevice(), g_canvasSize*64, m_pVisitor);
	bool bRefWorking = pReferenceSuite->init(g_pReferenceDevice->gfxDevice(), g_canvasSize*64, m_pVisitor);

	bool bWorking = (bTesteeWorking && bRefWorking);

	se.bWorking = bWorking;
	se.bActive = bWorking;
	se.nbTests = (int)pTesteeSuite->tests.size();

	g_testsuites.push_back(se);

	return bWorking;
}

//____ regen_testentries() ____________________________________________________

void GfxDeviceTester::regen_testentries()
{
	g_tests.clear();

	for (auto& suite : g_testsuites)
	{
		if (suite.bActive)
		{
			for (int i = 0; i < suite.nbTests; i++)
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

void GfxDeviceTester::destroy_tests()
{
	for (auto& suite : g_testsuites)
	{
		delete suite.pTesteeSuite;
		delete suite.pRefSuite;
	}

	g_tests.clear();
	g_testsuites.clear();
}

//____ setup_chrome() _________________________________________________________

bool GfxDeviceTester::setup_chrome()
{
	// Load resources

	auto pPlateSurface = m_pVisitor->loadSurface("resources/grey_plate.bmp");
	assert(pPlateSurface);
	BlockSkin_p pPlateSkin = BlockSkin::create({ .frame = 3, .padding = 5, .surface = pPlateSurface } );

	auto pPressablePlateSurface = m_pVisitor->loadSurface("resources/grey_pressable_plate.bmp");
	assert(pPressablePlateSurface);
	BlockSkin_p pPressablePlateSkin = BlockSkin::create({ .axis = Axis::X, .frame = Border(3), .padding = Border(3), .states = { State::Hovered, State::Pressed, State::Disabled }, .surface = pPressablePlateSurface });

	auto pButtonSurface = m_pVisitor->loadSurface("resources/simple_button.bmp");
	assert(pButtonSurface);
	BlockSkin_p pSimpleButtonSkin = BlockSkin::create({ .axis = Axis::X, .frame = Border(3), .padding = Border(5), .states = { State::Hovered, {}, State::Pressed, {}, State::Disabled, {} }, .surface = pButtonSurface });



	//    BoxStateSkin_p pPlateSkin = BoxSkin::create( Border(2), Color::Red, Color::Blue );
	//    BoxStateSkin_p pPressablePlateSkin = pPlateSkin;
	//    BoxStateSkin_p pSimpleButtonSkin = pPlateSkin;

		// Setup base layers

	auto pLayerStack = StackPanel::create();
	pLayerStack->setSkin( StaticColorSkin::create(Color::AntiqueWhite) );
	m_pVisitor->rootPanel()->slot = pLayerStack;

	// Divid screen into sidebar and canvaspanel with top section

	auto pMidSection = PackPanel::create();
	pMidSection->setAxis(Axis::X);
	pMidSection->setSizeBroker(UniformSizeBroker::create());
	auto it = pLayerStack->slots << pMidSection;
	(*it).setSizePolicy(SizePolicy2D::Stretch);

	auto pSidebar = PackPanel::create();
	pSidebar->setAxis(Axis::Y);
	pSidebar->setSkin( pPlateSkin );
	pSidebar->setSizeBroker(UniformSizeBroker::create());

	auto pCanvasPanel = PackPanel::create();
	pCanvasPanel->setAxis(Axis::Y);
	pCanvasPanel->setSizeBroker(UniformSizeBroker::create());

	auto pViewNav = PackPanel::create();
	pViewNav->setAxis(Axis::X);
	pViewNav->setSizeBroker(UniformSizeBroker::create());
	pViewNav->setSkin( pPlateSkin );

	auto pViewPanel = ScrollPanel::create();
	pViewPanel->setSkin( StaticColorSkin::create(Color8::SlateGrey) );
	g_pViewPanel = pViewPanel;

	pMidSection->slots << pCanvasPanel;
	pMidSection->slots << pSidebar;

	pMidSection->slots[0].setWeight(1.f);
	pMidSection->slots[1].setWeight(0.f);


	pCanvasPanel->slots << pViewNav;
	pCanvasPanel->slots << pViewPanel;

	pCanvasPanel->slots[0].setWeight(0.f);
	pCanvasPanel->slots[1].setWeight(1.f);


	// Setup view navigator

	auto pClipSection = PackPanel::create();
	auto pLeftFiller = Filler::create();
	auto pDispModeSection = PackPanel::create();
	auto pRightFiller = Filler::create();
	auto pDispZoomSection = PackPanel::create();

	pViewNav->slots << pClipSection;
	pViewNav->slots << pLeftFiller;
	pViewNav->slots << pDispModeSection;
	pViewNav->slots << pRightFiller;
	pViewNav->slots << pDispZoomSection;

	pViewNav->slots[0].setWeight(0.f);
	pViewNav->slots[1].setWeight(1.f);
	pViewNav->slots[2].setWeight(0.f);
	pViewNav->slots[3].setWeight(1.f);
	pViewNav->slots[4].setWeight(0.f);

	// Setup clip mode section

	pClipSection->setAxis(Axis::X);
	pClipSection->setSizeBroker(UniformSizeBroker::create());

	auto pClipLabel = TextDisplay::create();
	pClipLabel->display.setText("ClipRects: ");
	pClipLabel->display.setStyle(g_pButtonLabelStyle);


	auto pNoClipButton = Button::create();
	pNoClipButton->setSkin( pSimpleButtonSkin );
	pNoClipButton->label.setText("One");
	pNoClipButton->label.setStyle(g_pButtonLabelStyle);
	pNoClipButton->label.setLayout(g_pButtonLabelMapper);

	auto pFewButton = Button::create();
	pFewButton->setSkin( pSimpleButtonSkin );
	pFewButton->label.setText("Few");
	pFewButton->label.setStyle(g_pButtonLabelStyle);
	pFewButton->label.setLayout(g_pButtonLabelMapper);

	auto pManyButton = Button::create();
	pManyButton->setSkin( pSimpleButtonSkin );
	pManyButton->label.setText("Many");
	pManyButton->label.setStyle(g_pButtonLabelStyle);
	pManyButton->label.setLayout(g_pButtonLabelMapper);

	pClipSection->slots << pClipLabel;
	pClipSection->slots << pNoClipButton;
	pClipSection->slots << pFewButton;
	pClipSection->slots << pManyButton;

	Base::msgRouter()->addRoute(pNoClipButton, MsgType::Select, [this](Msg* pMsg) { setup_cliplist(ClipList::One); });
	Base::msgRouter()->addRoute(pFewButton, MsgType::Select, [this](Msg* pMsg) { setup_cliplist(ClipList::Few); });
	Base::msgRouter()->addRoute(pManyButton, MsgType::Select, [this](Msg* pMsg) { setup_cliplist(ClipList::Many); });

	// Setup display mode section

	pDispModeSection->setAxis(Axis::X);
	pDispModeSection->setSizeBroker(UniformSizeBroker::create());


	auto pTesteeButton = Button::create();
	pTesteeButton->setSkin( pSimpleButtonSkin );
	pTesteeButton->label.setText("Testee");
	pTesteeButton->label.setStyle(g_pButtonLabelStyle);
	pTesteeButton->label.setLayout(g_pButtonLabelMapper);

	auto pRefButton = Button::create();
	pRefButton->setSkin( pSimpleButtonSkin );
	pRefButton->label.setText("Reference");
	pRefButton->label.setStyle(g_pButtonLabelStyle);
	pRefButton->label.setLayout(g_pButtonLabelMapper);

	auto pBothButton = Button::create();
	pBothButton->setSkin( pSimpleButtonSkin );
//	pBothButton->text = { "Both", g_pButtonLabelStyle, g_pButtonLabelMapper }
	pBothButton->label.setText("Both");
	pBothButton->label.setStyle(g_pButtonLabelStyle);
	pBothButton->label.setLayout(g_pButtonLabelMapper);

	auto pDiffButton = Button::create();
	pDiffButton->setSkin( pSimpleButtonSkin );
	pDiffButton->label.setText("Diff");
	pDiffButton->label.setStyle(g_pButtonLabelStyle);
	pDiffButton->label.setLayout(g_pButtonLabelMapper);

	auto pTimeButton = Button::create();
	pTimeButton->setSkin( pSimpleButtonSkin );
	pTimeButton->label.setText("Time");
	pTimeButton->label.setStyle(g_pButtonLabelStyle);
	pTimeButton->label.setLayout(g_pButtonLabelMapper);

	pDispModeSection->slots << pTesteeButton;
	pDispModeSection->slots << pRefButton;
	pDispModeSection->slots << pBothButton;
	pDispModeSection->slots << pDiffButton;
	pDispModeSection->slots << pTimeButton;

	Base::msgRouter()->addRoute(pTesteeButton, MsgType::Select, [this](Msg* pMsg) {
		g_displayMode = DisplayMode::Testee;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pRefButton, MsgType::Select, [this](Msg* pMsg) {
		g_displayMode = DisplayMode::Reference;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pBothButton, MsgType::Select, [this](Msg* pMsg) {
		g_displayMode = DisplayMode::Both;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pDiffButton, MsgType::Select, [this](Msg* pMsg) {
		g_displayMode = DisplayMode::Diff;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pTimeButton, MsgType::Select, [this](Msg* pMsg) {
		g_displayMode = DisplayMode::Time;
		update_displaymode();
	});

	// Setup display zoom section

	pDispModeSection->setAxis(Axis::X);
	pDispModeSection->setSizeBroker(UniformSizeBroker::create());

	auto pX1Button = Button::create();
	pX1Button->setSkin( pSimpleButtonSkin );
	pX1Button->label.setText(" X1 ");
	pX1Button->label.setStyle(g_pButtonLabelStyle);
	pX1Button->label.setLayout(g_pButtonLabelMapper);

	auto pX2Button = Button::create();
	pX2Button->setSkin( pSimpleButtonSkin );
	pX2Button->label.setText(" X2 ");
	pX2Button->label.setStyle(g_pButtonLabelStyle);
	pX2Button->label.setLayout(g_pButtonLabelMapper);

	auto pX4Button = Button::create();
	pX4Button->setSkin( pSimpleButtonSkin );
	pX4Button->label.setText(" X4 ");
	pX4Button->label.setStyle(g_pButtonLabelStyle);
	pX4Button->label.setLayout(g_pButtonLabelMapper);

	auto pX8Button = Button::create();
	pX8Button->setSkin( pSimpleButtonSkin );
	pX8Button->label.setText(" X8 ");
	pX8Button->label.setStyle(g_pButtonLabelStyle);
	pX8Button->label.setLayout(g_pButtonLabelMapper);

	pDispZoomSection->slots << pX1Button;
	pDispZoomSection->slots << pX2Button;
	pDispZoomSection->slots << pX4Button;
	pDispZoomSection->slots << pX8Button;

	Base::msgRouter()->addRoute(pX1Button, MsgType::Select, [this](Msg* pMsg) {
		g_zoomFactor = 1.f;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pX2Button, MsgType::Select, [this](Msg* pMsg) {
		g_zoomFactor = 2.f;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pX4Button, MsgType::Select, [this](Msg* pMsg) {
		g_zoomFactor = 4.f;
		update_displaymode();
	});

	Base::msgRouter()->addRoute(pX8Button, MsgType::Select, [this](Msg* pMsg) {
		g_zoomFactor = 8.f;
		update_displaymode();
	});

	// Setup sidebar

	auto pTestList = PackList::create();
	pTestList->setSelectMode(SelectMode::FlipOnSelect);

	Base::msgRouter()->addRoute(pTestList, MsgType::ItemsSelect, [&](Msg* _pMsg) {

		auto pMsg = static_cast<ItemsSelectMsg*>(_pMsg);
		auto p = pMsg->items();
		for (int x = 0; x < pMsg->nbItems(); x++)
			g_tests[p[x].id].bActive = true;

		g_bRedrawTestee = true;
		g_bRedrawReference = true;
	});

	Base::msgRouter()->addRoute(pTestList, MsgType::ItemsUnselect, [&](Msg* _pMsg) {

		auto pMsg = static_cast<ItemsUnselectMsg*>(_pMsg);
		auto p = pMsg->items();
		for (int x = 0; x < pMsg->nbItems(); x++)
			g_tests[p[x].id].bActive = false;

		g_bRedrawTestee = true;
		g_bRedrawReference = true;
	});


	auto pSkin = BoxSkin::create(1, Color::White, Color::Black, 8);

	auto pEntrySkin = BoxSkin::create({ .color = Color::White, .outline = 1, .outlineColor = Color::Black, .padding = 8,
			.states = {
				{State::Hovered, {.color = Color::AntiqueWhite } },
				{State::SelectedHovered, {.color = Color::AntiqueWhite } },
				{State::Selected, {.color = Color::Aquamarine }}
			}
	});

	pTestList->setSkin( pSkin );
	pTestList->setEntrySkin(pEntrySkin);

	int id = 0;
	for (TestEntry& test : g_tests)
	{
		auto pEntry = TextDisplay::create();
		pEntry->setId(id++);
		pEntry->display.setText(test.name);
		pTestList->slots.pushBack(pEntry);
	}

	auto pTestScrollPanel = ScrollPanel::create();
	pTestScrollPanel->setSizeConstraints( SizeConstraint::Equal, SizeConstraint::None );
	pTestScrollPanel->slot = pTestList;

	pTestScrollPanel->scrollbarY.setBackground(StaticColorSkin::create(Color::Green));
	pTestScrollPanel->scrollbarY.setBar(pSimpleButtonSkin);

	pSidebar->slots << pTestScrollPanel;


	// Setup performance display

	{
		auto pMapper = BasicTextLayout::create({ .placement = Placement::East });
		g_pPerformanceValueMapper = pMapper;

		auto pBroker = UniformSizeBroker::create();
		g_pPerformanceEntryBroker = pBroker;


		auto pBase = PackPanel::create();
		pBase->setAxis(Axis::Y);

		auto pList = PackList::create();
		pList->setSkin( StaticColorSkin::create(Color::White) );

		auto pOddEntrySkin = BoxSkin::create(0, Color::White, Color::White);
		auto pEvenEntrySkin = BoxSkin::create(0, Color::PaleGreen, Color::PaleGreen);

		pList->setEntrySkin(pOddEntrySkin, pEvenEntrySkin);


		// Create the bottom section

		auto pBottom = PackPanel::create();
		pBottom->setSkin( pPlateSkin );
		pBottom->setAxis(Axis::X);
		pBottom->setSizeBroker(UniformSizeBroker::create());

		auto pRefresh = Button::create();
		pRefresh->setSkin( pSimpleButtonSkin );
		pRefresh->label.setText("REFRESH");
		pRefresh->label.setStyle(g_pButtonLabelStyle);
		pRefresh->label.setLayout(g_pButtonLabelMapper);

		Base::msgRouter()->addRoute(pRefresh, MsgType::Select, [this](Msg* pMsg) {
			g_bRefreshPerformance = true;
		});


		auto pFiller = Filler::create();

		pBottom->slots << pFiller;
		pBottom->slots << pRefresh;

		//

		pBase->slots << pList;
		pBase->slots << pBottom;

		g_pPerformanceDisplay = pBase;
		g_pPerformanceList = pList;
	}

	return true;
}

//____ teardown_chrome() ______________________________________________________

void GfxDeviceTester::teardown_chrome()
{
}
