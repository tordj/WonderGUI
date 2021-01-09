
#include <cmath>

#include <fonts/freetype/wg_freetypefont.h>

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

	auto pMapper = StdTextMapper::create();
	pMapper->setPlacement(Placement::Center);
	g_pButtonLabelMapper = pMapper;

	// Init fonts

	Blob_p pFontFile = pVisitor->loadBlob("../resources/DroidSans.ttf");
	FreeTypeFont_p pFont = FreeTypeFont::create(pFontFile, 0);

	TextStyle_p pStyle = TextStyle::create();
	pStyle->setFont(pFont);
	pStyle->setSize(16);
	pStyle->setColor(Color::Black);
	Base::setDefaultStyle(pStyle);

	g_pButtonLabelStyle = TextStyle::create();
	g_pButtonLabelStyle->setFont(pFont);
	g_pButtonLabelStyle->setSize(16);
	g_pButtonLabelStyle->setColor(Color::Black);

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

	if (g_zoomFactor != 1.f)
	{
		SizeI size{ (int)(512 * g_zoomFactor), (int)(512 * g_zoomFactor) };

		if (g_pTesteeCanvas)
			g_pTesteeCanvas->canvas.setPresentationScaling(SizePolicy2D::Scale);

		if (g_pReferenceCanvas)
			g_pReferenceCanvas->canvas.setPresentationScaling(SizePolicy2D::Scale);


		auto pZoomer = SizeCapsule::create();
		pZoomer->setSizes(size, size, size);
		pZoomer->slot = viewChild;
		viewChild = pZoomer;

	}

	//

	g_pViewPanel->viewSlot = viewChild;

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
			pLabel->text.set(t.name);

			//			auto pValue = ValueDisplay::create();
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
			pValueTestee->text.set(value);
			pValueTestee->text.setTextMapper(g_pPerformanceValueMapper);

			auto pValueRef = TextDisplay::create();
#ifdef WIN32			
			sprintf_s(value, 128, " %.1f ms (+ %1.f stalling)", t.devices[REFERENCE].render_time * 1000, t.devices[REFERENCE].stalling_time * 1000);
#else
			sprintf(value, " %.1f ms (+ %1.f stalling)", t.devices[REFERENCE].render_time * 1000, t.devices[REFERENCE].stalling_time * 1000);
#endif
			pValueRef->text.set(value);
			pValueRef->text.setTextMapper(g_pPerformanceValueMapper);

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

Canvas_p GfxDeviceTester::create_canvas()
{
	auto pCanvas = Canvas::create();
	pCanvas->canvas.setSize(g_canvasSize);
	//pCanvas->canvas.setDevice(GlGfxDevice::create({ 0,0,10,10 }));
	pCanvas->canvas.setFactory(Base::activeContext()->surfaceFactory());
	pCanvas->canvas.setBackColor(Color::Black);
	return pCanvas;
}

//____ display_test_results() _________________________________________________

void GfxDeviceTester::display_test_results()
{
	switch (g_displayMode)
	{
	case DisplayMode::Testee:
		g_pTesteeCanvas->canvas.surface()->copyFrom(g_pTesteeDevice->canvas(), { 0,0 });
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

void GfxDeviceTester::setup_cliplist(ClipList list)
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

	pGfxDevice->fill(g_canvasSize, Color::Black);
	pGfxDevice->setClipList((int)g_clipList.size(), &g_clipList[0]);

	for (auto& test : g_tests)
	{
		if (test.bActive)
		{
			if (test.devices[device].pTest->init != nullptr)
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

	regen_testentries();
}

//____ add_testsuite() _____________________________________________________________

bool GfxDeviceTester::add_testsuite(TestSuite* pTesteeSuite, TestSuite* pReferenceSuite)
{
	SuiteEntry se;

	se.pTesteeSuite = pTesteeSuite;
	se.pRefSuite = pReferenceSuite;

	bool bTesteeWorking = pTesteeSuite->init(g_pTesteeDevice->gfxDevice(), g_canvasSize, m_pVisitor);
	bool bRefWorking = pReferenceSuite->init(g_pReferenceDevice->gfxDevice(), g_canvasSize, m_pVisitor);

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

	auto pPlateSurface = m_pVisitor->loadSurface("../resources/grey_plate.bmp");
	assert(pPlateSurface);
	BlockSkin_p pPlateSkin = BlockSkin::create(pPlateSurface, BorderI(3));
	pPlateSkin->setContentPadding(BorderI(5));

	auto pPressablePlateSurface = m_pVisitor->loadSurface("../resources/grey_pressable_plate.bmp");
	assert(pPressablePlateSurface);
	BlockSkin_p pPressablePlateSkin = BlockSkin::create(pPressablePlateSurface, { StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Disabled }, BorderI(3), Axis::X);
	pPressablePlateSkin->setContentPadding(BorderI(3));

	auto pButtonSurface = m_pVisitor->loadSurface("../resources/simple_button.bmp");
	assert(pButtonSurface);
	BlockSkin_p pSimpleButtonSkin = BlockSkin::create(pButtonSurface, { StateEnum::Normal, StateEnum::Hovered, StateEnum::Pressed, StateEnum::Disabled }, BorderI(3), Axis::X);
	pSimpleButtonSkin->setContentPadding(BorderI(5));



	//    BoxStateSkin_p pPlateSkin = BoxSkin::create( Border(2), Color::Red, Color::Blue );
	//    BoxStateSkin_p pPressablePlateSkin = pPlateSkin;
	//    BoxStateSkin_p pSimpleButtonSkin = pPlateSkin;

		// Setup base layers

	auto pLayerStack = StackPanel::create();
	pLayerStack->skin = StaticColorSkin::create(Color::AntiqueWhite);
	m_pVisitor->rootPanel()->slot = pLayerStack;

	// Divid screen into sidebar and canvaspanel with top section

	auto pMidSection = PackPanel::create();
	pMidSection->setAxis(Axis::X);
	pMidSection->setSizeBroker(UniformSizeBroker::create());
	auto it = pLayerStack->slots << pMidSection;
	(*it).setSizePolicy(SizePolicy2D::Stretch);

	auto pSidebar = PackPanel::create();
	pSidebar->setAxis(Axis::Y);
	pSidebar->skin = pPlateSkin;
	pSidebar->setSizeBroker(UniformSizeBroker::create());

	auto pCanvasPanel = PackPanel::create();
	pCanvasPanel->setAxis(Axis::Y);
	pCanvasPanel->setSizeBroker(UniformSizeBroker::create());

	auto pViewNav = PackPanel::create();
	pViewNav->setAxis(Axis::X);
	pViewNav->setSizeBroker(UniformSizeBroker::create());
	pViewNav->skin = pPlateSkin;

	auto pViewPanel = ScrollPanel::create();
	pViewPanel->skin = StaticColorSkin::create(Color8::SlateGrey);
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
	pClipLabel->text.set("ClipRects: ");
	pClipLabel->text.setStyle(g_pButtonLabelStyle);


	auto pNoClipButton = Button::create();
	pNoClipButton->skin = pSimpleButtonSkin;
	pNoClipButton->text.set("One");
	pNoClipButton->text.setStyle(g_pButtonLabelStyle);
	pNoClipButton->text.setTextMapper(g_pButtonLabelMapper);

	auto pFewButton = Button::create();
	pFewButton->skin = pSimpleButtonSkin;
	pFewButton->text.set("Few");
	pFewButton->text.setStyle(g_pButtonLabelStyle);
	pFewButton->text.setTextMapper(g_pButtonLabelMapper);

	auto pManyButton = Button::create();
	pManyButton->skin = pSimpleButtonSkin;
	pManyButton->text.set("Many");
	pManyButton->text.setStyle(g_pButtonLabelStyle);
	pManyButton->text.setTextMapper(g_pButtonLabelMapper);

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
	pTesteeButton->skin = pSimpleButtonSkin;
	pTesteeButton->text.set("Testee");
	pTesteeButton->text.setStyle(g_pButtonLabelStyle);
	pTesteeButton->text.setTextMapper(g_pButtonLabelMapper);

	auto pRefButton = Button::create();
	pRefButton->skin = pSimpleButtonSkin;
	pRefButton->text.set("Reference");
	pRefButton->text.setStyle(g_pButtonLabelStyle);
	pRefButton->text.setTextMapper(g_pButtonLabelMapper);

	auto pBothButton = Button::create();
	pBothButton->skin = pSimpleButtonSkin;
//	pBothButton->text = { "Both", g_pButtonLabelStyle, g_pButtonLabelMapper }
	pBothButton->text.set("Both");
	pBothButton->text.setStyle(g_pButtonLabelStyle);
	pBothButton->text.setTextMapper(g_pButtonLabelMapper);

	auto pDiffButton = Button::create();
	pDiffButton->skin = pSimpleButtonSkin;
	pDiffButton->text.set("Diff");
	pDiffButton->text.setStyle(g_pButtonLabelStyle);
	pDiffButton->text.setTextMapper(g_pButtonLabelMapper);

	auto pTimeButton = Button::create();
	pTimeButton->skin = pSimpleButtonSkin;
	pTimeButton->text.set("Time");
	pTimeButton->text.setStyle(g_pButtonLabelStyle);
	pTimeButton->text.setTextMapper(g_pButtonLabelMapper);

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
	pX1Button->skin = pSimpleButtonSkin;
	pX1Button->text.set(" X1 ");
	pX1Button->text.setStyle(g_pButtonLabelStyle);
	pX1Button->text.setTextMapper(g_pButtonLabelMapper);

	auto pX2Button = Button::create();
	pX2Button->skin = pSimpleButtonSkin;
	pX2Button->text.set(" X2 ");
	pX2Button->text.setStyle(g_pButtonLabelStyle);
	pX2Button->text.setTextMapper(g_pButtonLabelMapper);

	auto pX4Button = Button::create();
	pX4Button->skin = pSimpleButtonSkin;
	pX4Button->text.set(" X4 ");
	pX4Button->text.setStyle(g_pButtonLabelStyle);
	pX4Button->text.setTextMapper(g_pButtonLabelMapper);

	auto pX8Button = Button::create();
	pX8Button->skin = pSimpleButtonSkin;
	pX8Button->text.set(" X8 ");
	pX8Button->text.setStyle(g_pButtonLabelStyle);
	pX8Button->text.setTextMapper(g_pButtonLabelMapper);

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




	auto pSkin = BoxSkin::create(1, Color::White, Color::Black);
	pSkin->setContentPadding(8);

	auto pEntrySkin = BoxSkin::create(1, Color::White, Color::Black);
	pEntrySkin->setContentPadding(8);
	pEntrySkin->setColors(StateEnum::Hovered, Color::AntiqueWhite, Color::Black);
	pEntrySkin->setColors(StateEnum::SelectedHovered, Color::Aquamarine, Color::Black);
	pEntrySkin->setColors(StateEnum::Selected, Color::Aquamarine, Color::Black);

	pTestList->skin = pSkin;
	pTestList->setEntrySkin(pEntrySkin);

	int id = 0;
	for (TestEntry& test : g_tests)
	{
		auto pEntry = TextDisplay::create();
		pEntry->setId(id++);
		pEntry->text.set(test.name);
		pTestList->slots.pushBack(pEntry);
	}

	auto pTestScrollPanel = ScrollPanel::create();
	pTestScrollPanel->viewSlot.setWidthPolicy(SizePolicy::Confined);
	pTestScrollPanel->viewSlot = pTestList;


	auto pTestScrollbar = Scrollbar::create();

	pTestScrollbar->setBackgroundSkin(StaticColorSkin::create(Color::Green));
	pTestScrollbar->setHandleSkin(pSimpleButtonSkin);


	pTestScrollPanel->vscrollbar = pTestScrollbar;

	pSidebar->slots << pTestScrollPanel;


	// Setup performance display

	{
		auto pMapper = StdTextMapper::create();
		pMapper->setPlacement(Placement::East);
		g_pPerformanceValueMapper = pMapper;

		auto pBroker = UniformSizeBroker::create();
		g_pPerformanceEntryBroker = pBroker;


		auto pBase = PackPanel::create();
		pBase->setAxis(Axis::Y);

		auto pList = PackList::create();
		pList->skin = StaticColorSkin::create(Color::White);

		auto pOddEntrySkin = BoxSkin::create(0, Color::White, Color::White);
		pOddEntrySkin->setContentPadding(BorderI(0));

		auto pEvenEntrySkin = BoxSkin::create(0, Color::PaleGreen, Color::PaleGreen);
		pEvenEntrySkin->setContentPadding(BorderI(0));

		pList->setEntrySkin(pOddEntrySkin, pEvenEntrySkin);


		// Create the bottom section

		auto pBottom = PackPanel::create();
		pBottom->skin = pPlateSkin;
		pBottom->setAxis(Axis::X);
		pBottom->setSizeBroker(UniformSizeBroker::create());

		auto pRefresh = Button::create();
		pRefresh->skin = pSimpleButtonSkin;
		pRefresh->text.set("REFRESH");
		pRefresh->text.setStyle(g_pButtonLabelStyle);
		pRefresh->text.setTextMapper(g_pButtonLabelMapper);

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
