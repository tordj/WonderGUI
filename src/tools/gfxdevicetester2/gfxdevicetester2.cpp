#include <algorithm>
#include <cmath>

#include <wondergfx.h>
#include <wg_referencebackend.h>
#include <wg_softsurface.h>

#include "gfxdevicetester2.h"

#include "testsuites/a8tests.h"
#include "testsuites/blendtests.h"
#include "testsuites/blendfixedcolortests.h"
#include "testsuites/blitblendtests.h"
#include "testsuites/blitconsistencytest.h"
#include "testsuites/blittests.h"
#include "testsuites/blurtests.h"
#include "testsuites/canvasformattests.h"
#include "testsuites/canvaslayertests.h"
#include "testsuites/colortests.h"
#include "testsuites/filltests.h"
#include "testsuites/linetests.h"
#include "testsuites/mipmaptests.h"
#include "testsuites/paletteblittests.h"
#include "testsuites/piecharttests.h"
#include "testsuites/segmenttests.h"
#include "testsuites/tiletests.h"
#include "testsuites/tintblittests.h"
#include "testsuites/tinttests.h"
#include "testsuites/wavetests.h"
#include "testsuites/edgemaptests.h"
#include "testsuites/edgemaptoolstests.h"
#include "testsuites/waveformtests.h"
#include "testsuites/samplingtests.h"
#include "testsuites/palettecapacitytests.h"
#include "testsuites/blursourcetests.h"
#include "testsuites/readbacktests.h"
#include "testsuites/edgemaptinttests.h"


using namespace wg;
using namespace std;


//____ create() _______________________________________________________________

WonderApp_p WonderApp::create()
{
	return static_cast<WonderApp*>(new GfxDeviceTester2());
}

//____ init() _________________________________________________________________

bool GfxDeviceTester2::init(wapp::API* pAPI)
{
	m_pAPI = pAPI;

	m_pWindow = wapp::Window::create(pAPI, { .size = {1800,900}, .title = "GfxDevice Tester 2" });

	pAPI->initDefaultWidgetKit();

	setup_backends();
	setup_tests();

	if (!setup_chrome())
		return false;

	select_view(View::Display);

	// Start out showing the first test we can run.

	for (int i = 0; i < (int)m_tests.size(); i++)
	{
		if (m_suites[m_tests[i].suite].bWorking)
		{
			show_in_display(i);
			break;
		}
	}

	return true;
}

//____ update() _______________________________________________________________

bool GfxDeviceTester2::update()
{
	if (m_bAccuracyRunning)
		continue_accuracy_run();

	if (m_bPerformanceRunning)
		continue_performance_run();

	return m_pWindow != nullptr;
}

//____ exit() _________________________________________________________________

void GfxDeviceTester2::exit()
{
	for (auto route : m_routes)
		Base::msgRouter()->deleteRoute(route);
	m_routes.clear();

	destroy_tests();

	for (auto& viewer : { &m_displayViewer, &m_accuracyViewer })
	{
		for (auto& p : viewer->pImages)
			p = nullptr;
	}

	for (auto& backend : m_backends)
		backend = Backend();
}

//____ closeWindow() __________________________________________________________

void GfxDeviceTester2::closeWindow(wapp::Window* pWindow)
{
	m_pWindow = nullptr;
}

//____ setup_backends() _______________________________________________________

void GfxDeviceTester2::setup_backends()
{
	Surface::Blueprint canvasBP = WGBP(Surface,
		_.size = m_canvasSize,
		_.canvas = true);

	// Testee: The native device of the WonderApp framework.

	{
		auto pDevice = Base::defaultGfxDevice();

		string name = string("Native (") + pDevice->typeInfo().className;

		if (pDevice->isInstanceOf(GfxDeviceGen2::TYPEINFO))
		{
			auto pBackend = static_cast<GfxDeviceGen2*>(pDevice.rawPtr())->backend();
			if (pBackend)
				name += string(" + ") + pBackend->typeInfo().className;
		}
		name += ")";

		m_backends[TESTEE] = { name, pDevice, Base::defaultSurfaceFactory()->createSurface(canvasBP) };
	}

	// Reference: GfxDeviceGen2 with ReferenceBackend.

	{
		auto pBackend = ReferenceBackend::create();

		m_backends[REFERENCE] = { "Reference (GfxDeviceGen2 + ReferenceBackend)", GfxDeviceGen2::create(pBackend), SoftSurface::create(canvasBP) };
	}

	// Both canvases are read back to ARGB_8 in the color space of the reference canvas
	// for comparison, so backends with different canvas formats can be compared.

	m_compareColorSpace = m_backends[REFERENCE].pCanvas->colorSpace();
}

//____ setup_tests() __________________________________________________________

void GfxDeviceTester2::setup_tests()
{
	destroy_tests();

	auto add = [this](auto factory) { add_testsuite(factory(), factory()); };

	add([]() { return new BlurTests(); });
	add([]() { return new BlitConsistencyTests(); });
	add([]() { return new FillTests(); });
	add([]() { return new BlendTests(); });
	add([]() { return new LineTests(); });
	add([]() { return new CanvasFormatTests(); });
	add([]() { return new PatchBlitTests(); });
	add([]() { return new SegmentTests(); });
	add([]() { return new WaveTests(); });
	add([]() { return new PaletteBlitTests(); });
	add([]() { return new MipmapTests(); });
	add([]() { return new PieChartTests(); });
	add([]() { return new TintBlitTests(); });
	add([]() { return new TintTests(); });
	add([]() { return new BlitBlendTests(); });
	add([]() { return new A8Tests(); });
	add([]() { return new TileTests(); });
	add([]() { return new CanvasLayerTests(); });
	add([]() { return new BlendFixedColorTests(); });
	add([]() { return new EdgemapTests(); });
	add([]() { return new EdgemapToolsTests(); });
	add([]() { return new WaveformTests(); });
	add([]() { return new ColorTests(); });
	add([]() { return new SamplingTests(); });
	add([]() { return new PaletteCapacityTests(); });
	add([]() { return new BlurSourceTests(); });
	add([]() { return new ReadbackTests(); });
	add([]() { return new EdgemapTintTests(); });
}

//____ add_testsuite() ________________________________________________________

void GfxDeviceTester2::add_testsuite(TestSuite* pTesteeSuite, TestSuite* pReferenceSuite)
{
	SuiteEntry suite;

	suite.name = pTesteeSuite->name;
	suite.pSuites[TESTEE] = pTesteeSuite;
	suite.pSuites[REFERENCE] = pReferenceSuite;
	suite.firstTest = (int)m_tests.size();
	suite.nbTests = (int)pTesteeSuite->tests.size();

	RectSPX canvas = { 0, 0, m_canvasSize.w * 64, m_canvasSize.h * 64 };

	bool bTesteeOk = pTesteeSuite->init(m_backends[TESTEE].pDevice, canvas, m_pAPI);
	bool bReferenceOk = pReferenceSuite->init(m_backends[REFERENCE].pDevice, canvas, m_pAPI);
	suite.bWorking = bTesteeOk && bReferenceOk;

	int suiteIndex = (int)m_suites.size();

	for (int i = 0; i < suite.nbTests; i++)
	{
		TestEntry test;

		test.name = pTesteeSuite->tests[i].name;
		test.suite = suiteIndex;
		test.pTests[TESTEE] = &pTesteeSuite->tests[i];
		test.pTests[REFERENCE] = &pReferenceSuite->tests[i];

		m_tests.push_back(test);
	}

	m_suites.push_back(suite);
}

//____ destroy_tests() ________________________________________________________

void GfxDeviceTester2::destroy_tests()
{
	RectSPX canvas = { 0, 0, m_canvasSize.w * 64, m_canvasSize.h * 64 };

	for (auto& suite : m_suites)
	{
		for (int side = 0; side < 2; side++)
		{
			suite.pSuites[side]->exit(m_backends[side].pDevice, canvas);
			delete suite.pSuites[side];
		}
	}

	m_tests.clear();
	m_suites.clear();
}

//____ setup_chrome() _________________________________________________________

bool GfxDeviceTester2::setup_chrome()
{
	m_pLayout = PackLayout::create({ .wantedSize = PackLayout::WantedSize::Default,
		.expandFactor = PackLayout::Factor::Weight, .shrinkFactor = PackLayout::Factor::Weight });

	m_pRightAligned = BasicTextLayout::create({ .placement = Placement::East });

	auto verdictStyle = [](HiColor color) {
		auto bp = wkit::TextStyles::Strong->blueprint();
		bp.color = color;
		return TextStyle::create(bp);
	};

	m_pOkStyle = verdictStyle(Color::DarkGreen);
	m_pSpecksStyle = verdictStyle(Color::DarkOrange);
	m_pDiffStyle = verdictStyle(Color::Red);

	// Navigation bar with a button for each view.

	auto pNavBar = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .skin = wkit::Skins::Plate, .spacing = 4 });

	auto pViewGroup = ToggleGroup::create();

	const char* viewNames[3] = { "Display", "Accuracy", "Performance" };

	for (int i = 0; i < 3; i++)
	{
		auto pButton = WGCREATE(wkit::ToggleButton, _.label.text = viewNames[i]);
		pViewGroup->add(pButton);

		pNavBar->slots.pushBack(pButton, { .weight = 0 });

		View view = View(i);
		m_routes.push_back(Base::msgRouter()->addRoute(pButton, MsgType::Toggle, [this, view](Msg* pMsg) {
			if (static_cast<ToggleMsg*>(pMsg)->isChecked())
				select_view(view);
		}));

		m_pViewButtons[i] = pButton;
	}

	string backendInfo = "Testee: " + m_backends[TESTEE].name + "      Reference: " + m_backends[REFERENCE].name;

	auto pBackendInfo = TextDisplay::create({ .display = {.layout = m_pRightAligned, .style = wkit::TextStyles::Default, .text = backendInfo.c_str() } });
	pNavBar->slots.pushBack(pBackendInfo, { .weight = 1 });

	// The views, of which one is shown at a time.

	m_pViews[int(View::Display)] = create_display_view();
	m_pViews[int(View::Accuracy)] = create_accuracy_view();
	m_pViews[int(View::Performance)] = create_performance_view();

	m_pViewBase = PaddingCapsule::create();

	auto pRoot = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout, .skin = ColorSkin::create(Color::AntiqueWhite) });
	pRoot->slots.pushBack(pNavBar, { .weight = 0 });
	pRoot->slots.pushBack(m_pViewBase, { .weight = 1 });

	m_pWindow->mainCapsule()->slot = pRoot;

	// Clicks on the cells of the accuracy table select their row.

	m_routes.push_back(Base::msgRouter()->addRoute(MsgType::MouseClick, [this](Msg* pMsg) {
		auto pSource = pMsg->sourceRawPtr();
		if (!pSource || !m_pAccuracyTable || !pSource->isInstanceOf(Widget::TYPEINFO))
			return;

		auto pWidget = static_cast<Widget*>(pSource);
		if (pWidget->parent() == m_pAccuracyTable && pWidget->id() >= 0)
			select_accuracy_row(pWidget->id());
	}));

	return true;
}

//____ create_display_view() __________________________________________________

Widget_p GfxDeviceTester2::create_display_view()
{
	create_viewer(m_displayViewer);

	// Background buttons.

	auto pTopBar = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .skin = wkit::Skins::Plate, .spacing = 4 });

	auto pBgLabel = TextDisplay::create({ .display = {.style = wkit::TextStyles::Strong, .text = "Background: " } });
	pTopBar->slots.pushBack(pBgLabel, { .weight = 0 });

	auto pBgGroup = ToggleGroup::create();

	const char* bgNames[4] = { "Black", "White", "Grey", "Checkered" };

	for (int i = 0; i < 4; i++)
	{
		auto pButton = WGCREATE(wkit::ToggleButton, _.label.text = bgNames[i]);
		pBgGroup->add(pButton);
		if (i == 0)
			pButton->setChecked(true);

		pTopBar->slots.pushBack(pButton, { .weight = 0 });

		Background background = Background(i);
		m_routes.push_back(Base::msgRouter()->addRoute(pButton, MsgType::Toggle, [this, background](Msg* pMsg) {
			if (static_cast<ToggleMsg*>(pMsg)->isChecked() && m_displayViewer.background != background)
			{
				m_displayViewer.background = background;
				refresh_viewer(m_displayViewer);
			}
		}));
	}

	pTopBar->slots.pushBack(Filler::create(), { .weight = 1 });

	// The three boxes.

	auto pBoxes = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .spacing = 16 });

	for (int i = 0; i < 3; i++)
		pBoxes->slots.pushBack(create_image_box(m_displayViewer, i, 1.f), { .weight = 0 });

	auto pBoxesPadding = PaddingCapsule::create({ .child = pBoxes, .padding = 16 });

	auto pScroller = wkit::ScrollCapsuleXY::create();
	pScroller->slot = pBoxesPadding;

	auto pMain = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout });
	pMain->slots.pushBack(pTopBar, { .weight = 0 });
	pMain->slots.pushBack(m_displayViewer.pTitle, { .weight = 0 });
	pMain->slots.pushBack(pScroller, { .weight = 1 });

	// Test tree to the right.

	auto pBase = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout });
	pBase->slots.pushBack(pMain, { .weight = 1 });
	pBase->slots.pushBack(create_test_tree(), { .weight = 0 });

	return pBase;
}

//____ create_test_tree() _____________________________________________________

Widget_p GfxDeviceTester2::create_test_tree()
{
	auto pList = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout });

	for (auto& suite : m_suites)
	{
		auto pDrawer = WGCREATE(wkit::TreeListDrawer, _.selectable = false);

		string title = suite.bWorking ? suite.name : suite.name + " (init failed)";

		auto pHeader = WGCREATE(wkit::TreeListEntry,
			_.child = TextDisplay::create({ .display = {.style = wkit::TextStyles::Strong, .text = title.c_str() } }),
			_.padding = Border(2, 4, 2, 20),
			_.skin = nullptr);

		auto pContent = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout });

		for (int i = suite.firstTest; i < suite.firstTest + suite.nbTests; i++)
		{
			auto pEntry = WGCREATE(wkit::TreeListEntry,
				_.child = TextDisplay::create({ .display = {.style = wkit::TextStyles::Default, .text = m_tests[i].name.c_str() } }),
				_.id = i,
				_.padding = Border(2, 4, 2, 36),
				_.selectable = suite.bWorking,
				_.disabled = !suite.bWorking);

			pContent->slots.pushBack(pEntry, { .weight = 0 });
			m_tests[i].pTreeEntry = pEntry;
		}

		pDrawer->slots[0] = pHeader;
		pDrawer->slots[1] = pContent;

		pList->slots.pushBack(pDrawer, { .weight = 0 });
		suite.pTreeDrawer = pDrawer;
	}

	auto pSelector = SelectCapsule::create(WGBP(SelectCapsule,
		_.child = pList,
		_.recursive = true,
		_.selectMode = SelectMode::SingleEntry));

	m_pTestTreeSelector = pSelector;

	m_routes.push_back(Base::msgRouter()->addRoute(pSelector, MsgType::Selected, [this](Msg* pMsg) {
		auto p = static_cast<SelectedMsg*>(pMsg);
		if (p->nbSelected() > 0)
			show_test(m_displayViewer, p->selected()[0]->id());
	}));

	auto pScroller = wkit::ScrollCapsuleY::create();
	pScroller->slot = pSelector;

	auto pSidebar = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout, .skin = wkit::Skins::Plate });
	pSidebar->slots.pushBack(TextDisplay::create({ .display = {.style = wkit::TextStyles::Strong, .text = "Tests" } }), { .weight = 0 });
	pSidebar->slots.pushBack(pScroller, { .weight = 1 });

	return pSidebar;
}

//____ create_viewer() ________________________________________________________

void GfxDeviceTester2::create_viewer(Viewer& viewer)
{
	Surface::Blueprint bp = WGBP(Surface,
		_.size = m_canvasSize,
		_.format = PixelFormat::ARGB_8,
		_.colorSpace = m_compareColorSpace,
		_.sampleMethod = SampleMethod::Nearest);

	for (int i = 0; i < 3; i++)
	{
		viewer.pImages[i] = Base::defaultSurfaceFactory()->createSurface(bp);
		viewer.pImages[i]->fill(Color::Black);
	}

	viewer.pTitle = TextDisplay::create({ .display = {.style = wkit::TextStyles::Heading5, .text = "No test selected" }, .skin = ColorSkin::create({ .color = Color::Transparent, .padding = Border(8, 16, 0, 16) }) });

	auto pSlider = Slider::create(WGBP(Slider,
		_.handle = wkit::Skins::ScrollbarHandleX,
		_.skin = wkit::Skins::ScrollbarTrack,
		_.defaultSlideLength = 200,
		_.steps = c_maxDiffFactor - 1,
		_.value = 0.f));

	viewer.pDiffSlider = pSlider;

	viewer.pDiffFactorLabel = TextDisplay::create({ .display = {.style = wkit::TextStyles::Strong, .text = "1x" } });

	Viewer* pViewer = &viewer;
	m_routes.push_back(Base::msgRouter()->addRoute(pSlider, MsgType::ValueUpdate, [this, pViewer](Msg* pMsg) {
		float value = static_cast<ValueUpdateMsg*>(pMsg)->value();
		int factor = 1 + int(std::lround(value * (c_maxDiffFactor - 1)));

		if (factor != pViewer->diffFactor)
		{
			pViewer->diffFactor = factor;
			refresh_diff(*pViewer);
		}
	}));
}

//____ create_image_box() _____________________________________________________
/*
	A label above one of the viewers images. The diff image also gets its slider.
	Images at full size have room for the backend name in their label.
*/

Widget_p GfxDeviceTester2::create_image_box(Viewer& viewer, int image, float zoom)
{
	string label = image == TESTEE ? "Testee" : image == REFERENCE ? "Reference" : "Diff";
	if (image != 2 && zoom >= 1.f)
		label += ": " + m_backends[image].name;

	auto pLabel = TextDisplay::create({ .display = {.style = wkit::TextStyles::Strong, .text = label.c_str() } });

	auto pDisplay = SurfaceDisplay::create(WGBP(SurfaceDisplay,
		_.skin = BoxSkin::create(WGBP(BoxSkin,
			_.color = Color::Black,
			_.outlineThickness = 1,
			_.outlineColor = Color::DarkGrey,
			_.padding = 1)),
		_.surface = viewer.pImages[image],
		_.zoom = zoom,
		_.minUserZoom = zoom,
		_.maxUserZoom = zoom));

	auto pBox = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout, .spacing = 4 });
	pBox->slots.pushBack(pLabel, { .weight = 0 });
	pBox->slots.pushBack(pDisplay, { .weight = 0 });

	if (image == 2)
	{
		auto pSliderRow = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .spacing = 8 });
		pSliderRow->slots.pushBack(TextDisplay::create({ .display = {.style = wkit::TextStyles::Default, .text = "Amplify:" } }), { .weight = 0 });
		pSliderRow->slots.pushBack(viewer.pDiffSlider, { .weight = 1 });
		pSliderRow->slots.pushBack(viewer.pDiffFactorLabel, { .weight = 0 });

		pBox->slots.pushBack(pSliderRow, { .weight = 0 });
	}

	pBox->slots.pushBack(Filler::create(), { .weight = 1 });
	return pBox;
}

//____ create_progress_bar() __________________________________________________
/*
	A slider the user can't touch makes a progress bar.
*/

Widget_p GfxDeviceTester2::create_progress_bar(Slider_p& pBar, TextDisplay_p& pLabel)
{
	pBar = Slider::create(WGBP(Slider,
		_.markPolicy = MarkPolicy::Ignore,
		_.defaultSlideLength = 240,
		_.skin = FillMeterSkin::create(WGBP(FillMeterSkin,
			_.backColor = Color::White,
			_.color = Color::SteelBlue,
			_.defaultSize = Size(240, 16),
			_.padding = 1))));

	pLabel = TextDisplay::create({ .display = {.style = wkit::TextStyles::Default, .text = "" } });

	auto pPack = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .spacing = 8 });
	pPack->slots.pushBack(pBar, { .weight = 0 });
	pPack->slots.pushBack(pLabel, { .weight = 1 });
	return pPack;
}

//____ create_accuracy_view() _________________________________________________

Widget_p GfxDeviceTester2::create_accuracy_view()
{
	create_viewer(m_accuracyViewer);

	// Table

	m_pAccuracyTable = TablePanel::create({ .columnSpacing = 0, .rowSpacing = 0, .skin = ColorSkin::create({ .color = Color::White, .padding = 4 }) });
	m_pAccuracyTable->setRowSkins(ColorSkin::create(Color::White), ColorSkin::create(Color::PapayaWhip));

	auto pTableScroller = wkit::ScrollCapsuleY::create();
	pTableScroller->slot = m_pAccuracyTable;

	// Preview of selected test: testee and reference at half size, diff below at full size.

	auto pPreview = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout, .spacing = 8 });

	auto pTopRow = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .spacing = 8 });
	pTopRow->slots.pushBack(create_image_box(m_accuracyViewer, TESTEE, 0.5f), { .weight = 0 });
	pTopRow->slots.pushBack(create_image_box(m_accuracyViewer, REFERENCE, 0.5f), { .weight = 0 });

	auto pShowButton = WGCREATE(wkit::Button, _.label.text = "Show in Display view");
	m_routes.push_back(Base::msgRouter()->addRoute(pShowButton, MsgType::Select, [this](Msg* pMsg) {
		if (m_accuracySelected >= 0)
			show_in_display(m_accuracySelected);
	}));

	auto pTitleRow = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout });
	pTitleRow->slots.pushBack(m_accuracyViewer.pTitle, { .weight = 1 });
	pTitleRow->slots.pushBack(pShowButton, { .weight = 0 });

	pPreview->slots.pushBack(pTitleRow, { .weight = 0 });
	pPreview->slots.pushBack(pTopRow, { .weight = 0 });
	pPreview->slots.pushBack(create_image_box(m_accuracyViewer, 2, 1.f), { .weight = 0 });

	auto pPreviewScroller = wkit::ScrollCapsuleY::create();
	pPreviewScroller->slot = PaddingCapsule::create({ .child = pPreview, .padding = 8 });

	auto pMid = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout });
	pMid->slots.pushBack(pTableScroller, { .weight = 1 });
	pMid->slots.pushBack(pPreviewScroller, { .weight = 0 });

	// Bottom bar

	auto pBottom = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .skin = wkit::Skins::Plate, .spacing = 8 });

	m_pAccuracySummary = TextDisplay::create({ .display = {.style = wkit::TextStyles::Default } });

	m_pAccuracyRunButton = WGCREATE(wkit::Button, _.label.text = "RUN ALL");
	m_routes.push_back(Base::msgRouter()->addRoute(m_pAccuracyRunButton, MsgType::Select, [this](Msg* pMsg) {
		if (m_bAccuracyRunning)
		{
			m_bAccuracyRunning = false;
			m_pAccuracyRunButton->label.setText("RUN ALL");
			m_pAccuracyProgressLabel->display.setText("Stopped");
			refresh_accuracy_table();
		}
		else
			start_accuracy_run();
	}));

	pBottom->slots.pushBack(m_pAccuracySummary, { .weight = 1 });
	pBottom->slots.pushBack(create_progress_bar(m_pAccuracyProgress, m_pAccuracyProgressLabel), { .weight = 0 });
	pBottom->slots.pushBack(m_pAccuracyRunButton, { .weight = 0 });

	auto pBase = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout });
	pBase->slots.pushBack(pMid, { .weight = 1 });
	pBase->slots.pushBack(pBottom, { .weight = 0 });

	refresh_accuracy_table();

	return pBase;
}

//____ create_performance_view() ______________________________________________

Widget_p GfxDeviceTester2::create_performance_view()
{
	auto pTable = TablePanel::create({ .columnSpacing = 16, .rowSpacing = 4, .skin = ColorSkin::create({ .color = Color::White, .padding = 4 }) });
	pTable->setRowSkins(ColorSkin::create(Color::White), ColorSkin::create(Color::PapayaWhip));

	m_pPerformanceTable = pTable;

	pTable->resize(1 + (int)m_tests.size(), 4);

	const char* headings[4] = { "SUITE", "TEST", "TIME PER RUN", "LOOPS" };
	for (int col = 0; col < 4; col++)
		pTable->slots[0][col] = TextDisplay::create({ .display = {.layout = col < 2 ? nullptr : m_pRightAligned, .style = wkit::TextStyles::Heading6, .text = headings[col] } });

	for (int i = 0; i < (int)m_tests.size(); i++)
	{
		auto& test = m_tests[i];

		test.pPerfTimeCell = TextDisplay::create({ .display = {.layout = m_pRightAligned, .style = wkit::TextStyles::Default } });
		test.pPerfLoopsCell = TextDisplay::create({ .display = {.layout = m_pRightAligned, .style = wkit::TextStyles::Default } });

		pTable->slots[1 + i][0] = TextDisplay::create({ .display = {.style = wkit::TextStyles::Default, .text = m_suites[test.suite].name.c_str() } });
		pTable->slots[1 + i][1] = TextDisplay::create({ .display = {.style = wkit::TextStyles::Default, .text = test.name.c_str() } });
		pTable->slots[1 + i][2] = test.pPerfTimeCell;
		pTable->slots[1 + i][3] = test.pPerfLoopsCell;

		refresh_performance_row(i);
	}

	auto pTableScroller = wkit::ScrollCapsuleY::create();
	pTableScroller->slot = pTable;

	// Bottom bar

	auto pBottom = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout, .skin = wkit::Skins::Plate, .spacing = 8 });

	m_pPerformanceSummary = TextDisplay::create({ .display = {.style = wkit::TextStyles::Default,
		.text = "Times each test on the testee only. Init and exit are not included." } });

	m_pPerformanceRunButton = WGCREATE(wkit::Button, _.label.text = "RUN");
	m_routes.push_back(Base::msgRouter()->addRoute(m_pPerformanceRunButton, MsgType::Select, [this](Msg* pMsg) {
		if (m_bPerformanceRunning)
		{
			m_bPerformanceRunning = false;
			m_pPerformanceRunButton->label.setText("RUN");
			m_pPerformanceProgressLabel->display.setText("Stopped");
		}
		else
			start_performance_run();
	}));

	pBottom->slots.pushBack(m_pPerformanceSummary, { .weight = 1 });
	pBottom->slots.pushBack(create_progress_bar(m_pPerformanceProgress, m_pPerformanceProgressLabel), { .weight = 0 });
	pBottom->slots.pushBack(m_pPerformanceRunButton, { .weight = 0 });

	auto pBase = PackPanel::create({ .axis = Axis::Y, .layout = m_pLayout });
	// Placed directly in pBase, the scroller ends up a bit too tall and pushes the
	// bottom bar partly out of view. Inside a horizontal PackPanel it doesn't.

	auto pMid = PackPanel::create({ .axis = Axis::X, .layout = m_pLayout });
	pMid->slots.pushBack(pTableScroller, { .weight = 1 });
	pBase->slots.pushBack(pMid, { .weight = 1 });
	pBase->slots.pushBack(pBottom, { .weight = 0 });

	return pBase;
}

//____ select_view() __________________________________________________________

void GfxDeviceTester2::select_view(View view)
{
	m_pViewBase->slot = m_pViews[int(view)];

	if (!m_pViewButtons[int(view)]->isChecked())
		m_pViewButtons[int(view)]->setChecked(true);
}

//____ show_in_display() ______________________________________________________
/*
	Switches to the display view, showing the given test selected in the tree.
	Selecting from code doesn't post a message for the selected entry, so the
	test is shown from here.
*/

void GfxDeviceTester2::show_in_display(int test)
{
	auto& suite = m_suites[m_tests[test].suite];

	suite.pTreeDrawer->openImmediately();
	m_pTestTreeSelector->select(m_tests[test].pTreeEntry);

	show_test(m_displayViewer, test);
	select_view(View::Display);
}

//____ render_test() __________________________________________________________
/*
	Renders a test, including its init and exit, on one of the backends.
*/

bool GfxDeviceTester2::render_test(Side side, int test, Background background)
{
	auto& entry = m_tests[test];
	if (!m_suites[entry.suite].bWorking)
		return false;

	auto& backend = m_backends[side];
	GfxDevice* pDevice = backend.pDevice;
	Test* pTest = entry.pTests[side];

	RectSPX canvas = { 0, 0, m_canvasSize.w * 64, m_canvasSize.h * 64 };

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(backend.pCanvas);

	fill_background(pDevice, background);

	if (pTest->init)
		pTest->init(pDevice, canvas);

	pTest->run(pDevice, canvas);

	if (pTest->exit)
		pTest->exit(pDevice, canvas);

	pDevice->endCanvasUpdate();
	pDevice->endRender();
	return true;
}

//____ fill_background() ______________________________________________________

void GfxDeviceTester2::fill_background(GfxDevice* pDevice, Background background)
{
	RectSPX canvas = { 0, 0, m_canvasSize.w * 64, m_canvasSize.h * 64 };

	pDevice->setBlendMode(BlendMode::Replace);

	switch (background)
	{
		case Background::Black:
			pDevice->fill(canvas, Color::Black);
			break;
		case Background::White:
			pDevice->fill(canvas, Color::White);
			break;
		case Background::Grey:
			pDevice->fill(canvas, Color8(128, 128, 128));
			break;
		case Background::Checkered:
		{
			const int squareSize = 16;

			pDevice->fill(canvas, Color8(160, 160, 160));

			for (int y = 0; y < m_canvasSize.h; y += squareSize)
			{
				for (int x = ((y / squareSize) % 2) * squareSize; x < m_canvasSize.w; x += squareSize * 2)
					pDevice->fill(RectSPX(x, y, squareSize, squareSize) * 64, Color8(96, 96, 96));
			}
			break;
		}
	}

	pDevice->setBlendMode(BlendMode::Blend);
}

//____ read_canvas() __________________________________________________________
/*
	Reads back the canvas of a backend as ARGB_8 in the color space we compare in.
*/

bool GfxDeviceTester2::read_canvas(Side side, vector<uint8_t>& pixels)
{
	Surface* pSurface = m_backends[side].pCanvas;
	if (!pSurface || pSurface->pixelSize() != m_canvasSize)
		return false;

	pixels.resize(m_canvasSize.w * m_canvasSize.h * 4);

	auto buffer = pSurface->allocPixelBuffer();
	pSurface->pushPixels(buffer);

	int srcPitchAdd = buffer.pitch - m_canvasSize.w * Util::pixelFormatToDescription(buffer.format).bits / 8;
	int dstPaletteEntries = 0;

	bool bOk = PixelTools::copyPixels(m_canvasSize.w, m_canvasSize.h, buffer.pixels, buffer.format, buffer.colorSpace, buffer.bigEndian, srcPitchAdd,
		buffer.palette, pSurface->paletteSize(),
		pixels.data(), PixelFormat::ARGB_8, m_compareColorSpace, (WG_IS_BIG_ENDIAN == 1), 0, nullptr, dstPaletteEntries, 0);

	pSurface->freePixelBuffer(buffer);
	return bOk;
}

//____ write_image() __________________________________________________________
/*
	Writes ARGB_8 pixels, in the color space we compare in, to one of the images
	we display.
*/

void GfxDeviceTester2::write_image(Surface* pSurface, const vector<uint8_t>& pixels)
{
	auto buffer = pSurface->allocPixelBuffer();

	int dstPitchAdd = buffer.pitch - m_canvasSize.w * Util::pixelFormatToDescription(buffer.format).bits / 8;
	int dstPaletteEntries = 0;

	PixelTools::copyPixels(m_canvasSize.w, m_canvasSize.h, pixels.data(), PixelFormat::ARGB_8, m_compareColorSpace, (WG_IS_BIG_ENDIAN == 1), 0,
		nullptr, 0,
		buffer.pixels, buffer.format, buffer.colorSpace, buffer.bigEndian, dstPitchAdd, nullptr, dstPaletteEntries, 0);

	pSurface->pullPixels(buffer);
	pSurface->freePixelBuffer(buffer);
}

//____ compare() ______________________________________________________________

void GfxDeviceTester2::compare(const vector<uint8_t>& testee, const vector<uint8_t>& reference, AccuracyResult& result)
{
	const uint8_t* pTestee = testee.data();
	const uint8_t* pRef = reference.data();
	size_t nbValues = reference.size();
	int64_t sum = 0;

	result = AccuracyResult();

	for (size_t i = 0; i < nbValues; i++)
	{
		int diff = std::abs(int(pRef[i]) - int(pTestee[i]));
		sum += diff;

		if (diff > c_diffThreshold)
			result.nbOverThreshold++;

		if (diff > result.maxDiff)
		{
			result.maxDiff = diff;
			int pixel = int(i / 4);
			result.worst = { pixel % m_canvasSize.w, pixel / m_canvasSize.w };
		}
	}

	result.meanDiff = double(sum) / nbValues;
	result.bDone = true;
}

//____ show_test() ____________________________________________________________

void GfxDeviceTester2::show_test(Viewer& viewer, int test)
{
	viewer.test = test;
	refresh_viewer(viewer);
}

//____ refresh_viewer() _______________________________________________________
/*
	Renders the viewers test on both backends and updates its images.
*/

void GfxDeviceTester2::refresh_viewer(Viewer& viewer)
{
	if (viewer.test < 0)
		return;

	auto& test = m_tests[viewer.test];

	string title = m_suites[test.suite].name + " / " + test.name;

	bool bOk = true;
	for (int side = 0; side < 2; side++)
	{
		if (render_test(Side(side), viewer.test, viewer.background) && read_canvas(Side(side), viewer.pixels[side]))
			write_image(viewer.pImages[side], viewer.pixels[side]);
		else
		{
			viewer.pixels[side].clear();
			viewer.pImages[side]->fill(Color::Black);
			bOk = false;
		}
	}

	if (!bOk)
		title += "   (could not be rendered)";

	viewer.pTitle->display.setText(title);

	refresh_diff(viewer);
}

//____ refresh_diff() _________________________________________________________
/*
	The diff image shows the difference of each color channel, multiplied by the
	diff factor. It is opaque, alpha differences are not shown.
*/

void GfxDeviceTester2::refresh_diff(Viewer& viewer)
{
	char label[32];
	snprintf(label, sizeof(label), "%dx", viewer.diffFactor);
	viewer.pDiffFactorLabel->display.setText(label);

	auto& testee = viewer.pixels[TESTEE];
	auto& reference = viewer.pixels[REFERENCE];

	if (testee.empty() || testee.size() != reference.size())
	{
		viewer.pImages[2]->fill(Color::Black);
		return;
	}

	const int alphaOfs = WG_IS_BIG_ENDIAN ? 0 : 3;

	vector<uint8_t> diff(testee.size());

	for (size_t i = 0; i < diff.size(); i += 4)
	{
		for (int c = 0; c < 4; c++)
		{
			if (c == alphaOfs)
				diff[i + c] = 255;
			else
				diff[i + c] = (uint8_t) std::min(255, std::abs(int(testee[i + c]) - int(reference[i + c])) * viewer.diffFactor);
		}
	}

	write_image(viewer.pImages[2], diff);
}

//____ start_accuracy_run() ___________________________________________________

void GfxDeviceTester2::start_accuracy_run()
{
	for (auto& test : m_tests)
		test.accuracy = AccuracyResult();

	m_bAccuracyRunning = true;
	m_accuracyNext = 0;

	m_pAccuracyRunButton->label.setText("STOP");
	m_pAccuracyProgress->setValue(0.f);

	refresh_accuracy_table();
}

//____ continue_accuracy_run() ________________________________________________
/*
	Runs tests until our time budget for this update is spent, so the progress
	bar keeps moving and the window stays responsive.
*/

void GfxDeviceTester2::continue_accuracy_run()
{
	auto start = Clock::now();
	int nbTests = (int)m_tests.size();

	while (m_accuracyNext < nbTests && std::chrono::duration<double>(Clock::now() - start).count() < c_progressBudget)
		run_accuracy_test(m_accuracyNext++);

	char label[256];

	if (m_accuracyNext < nbTests)
	{
		auto& next = m_tests[m_accuracyNext];
		snprintf(label, sizeof(label), "%d / %d   %s / %s", m_accuracyNext, nbTests, m_suites[next.suite].name.c_str(), next.name.c_str());
	}
	else
	{
		snprintf(label, sizeof(label), "Done, %d tests", nbTests);

		m_bAccuracyRunning = false;
		m_pAccuracyRunButton->label.setText("RUN ALL");
		refresh_accuracy_table();
	}

	m_pAccuracyProgress->setValue(nbTests > 0 ? m_accuracyNext / float(nbTests) : 1.f);
	m_pAccuracyProgressLabel->display.setText(label);
}

//____ run_accuracy_test() ____________________________________________________
/*
	Renders a test on both backends, on black, reads back the results and compares them.
*/

void GfxDeviceTester2::run_accuracy_test(int test)
{
	auto& result = m_tests[test].accuracy;

	vector<uint8_t> pixels[2];

	for (int side = 0; side < 2; side++)
	{
		if (!render_test(Side(side), test, Background::Black) || !read_canvas(Side(side), pixels[side]))
		{
			result = AccuracyResult();
			result.bDone = true;
			result.bFailed = true;
			return;
		}
	}

	compare(pixels[TESTEE], pixels[REFERENCE], result);
}

//____ refresh_accuracy_table() _______________________________________________
/*
	Lists the results with the worst first, so what needs looking at is at the top.
	Tests not yet compared are last, in suite order.
*/

void GfxDeviceTester2::refresh_accuracy_table()
{
	const int nbColumns = 7;

	m_accuracyRowOrder.clear();
	for (int i = 0; i < (int)m_tests.size(); i++)
		m_accuracyRowOrder.push_back(i);

	std::stable_sort(m_accuracyRowOrder.begin(), m_accuracyRowOrder.end(), [this](int a, int b) {
		const AccuracyResult& ra = m_tests[a].accuracy;
		const AccuracyResult& rb = m_tests[b].accuracy;

		if (ra.bDone != rb.bDone)
			return ra.bDone;
		if (!ra.bDone)
			return false;
		if (ra.bFailed != rb.bFailed)
			return ra.bFailed;
		if (ra.nbOverThreshold != rb.nbOverThreshold)
			return ra.nbOverThreshold > rb.nbOverThreshold;
		return ra.maxDiff > rb.maxDiff;
	});

	auto pTable = m_pAccuracyTable;
	pTable->resize(1 + (int)m_tests.size(), nbColumns);

	Skin_p pCellSkin = ColorSkin::create({ .color = Color::Transparent, .padding = Border(2, 8) });

	const char* headings[nbColumns] = { "SUITE", "TEST", "MAX", "MEAN", "OVER", "WORST AT", "RESULT" };
	for (int col = 0; col < nbColumns; col++)
		pTable->slots[0][col] = TextDisplay::create({ .display = {.layout = col < 2 ? nullptr : m_pRightAligned, .style = wkit::TextStyles::Heading6, .text = headings[col] },
			.id = -1, .skin = pCellSkin });

	int nbDone = 0, nbOk = 0, nbSpecks = 0, nbDiff = 0, nbFailed = 0;

	for (int row = 0; row < (int)m_accuracyRowOrder.size(); row++)
	{
		int testIndex = m_accuracyRowOrder[row];
		const TestEntry& test = m_tests[testIndex];
		const AccuracyResult& r = test.accuracy;

		string values[nbColumns];
		TextStyle_p pVerdictStyle = wkit::TextStyles::Default;

		values[0] = m_suites[test.suite].name;
		values[1] = test.name;

		if (r.bDone)
		{
			nbDone++;

			if (r.bFailed)
			{
				values[6] = "FAILED";
				pVerdictStyle = m_pDiffStyle;
				nbFailed++;
			}
			else
			{
				char buffer[64];

				snprintf(buffer, sizeof(buffer), "%d", r.maxDiff);
				values[2] = buffer;
				snprintf(buffer, sizeof(buffer), "%.2f", r.meanDiff);
				values[3] = buffer;
				snprintf(buffer, sizeof(buffer), "%d", r.nbOverThreshold);
				values[4] = buffer;
				if (r.maxDiff > 0)
				{
					snprintf(buffer, sizeof(buffer), "%d, %d", r.worst.x, r.worst.y);
					values[5] = buffer;
				}

				if (r.nbOverThreshold == 0)
				{
					values[6] = "OK";
					pVerdictStyle = m_pOkStyle;
					nbOk++;
				}
				else if (r.nbOverThreshold <= c_maxSpecks)
				{
					values[6] = "SPECKS";
					pVerdictStyle = m_pSpecksStyle;
					nbSpecks++;
				}
				else
				{
					values[6] = "DIFF";
					pVerdictStyle = m_pDiffStyle;
					nbDiff++;
				}
			}
		}
		else
			values[6] = "-";

		for (int col = 0; col < nbColumns; col++)
		{
			pTable->slots[1 + row][col] = TextDisplay::create({
				.display = {.layout = col < 2 ? nullptr : m_pRightAligned, .style = col == 6 ? pVerdictStyle : wkit::TextStyles::Default, .text = values[col].c_str() },
				.id = testIndex,
				.markPolicy = MarkPolicy::Geometry,
				.skin = pCellSkin });
		}
	}

	// Restore selection, the row might have moved.

	int selected = m_accuracySelected;
	m_accuracySelected = -1;
	if (selected >= 0)
		select_accuracy_row(selected);

	char summary[256];
	if (nbDone == 0)
		snprintf(summary, sizeof(summary), "Press RUN ALL to render every test on both backends, on black, and compare.");
	else
		snprintf(summary, sizeof(summary), "%d tests compared: %d OK, %d with specks, %d differ, %d failed. Click a row to view the test.", nbDone, nbOk, nbSpecks, nbDiff, nbFailed);

	m_pAccuracySummary->display.setText(summary);
}

//____ select_accuracy_row() __________________________________________________

void GfxDeviceTester2::select_accuracy_row(int test)
{
	static Skin_p s_pNormalSkin = ColorSkin::create({ .color = Color::Transparent, .padding = Border(2, 8) });
	static Skin_p s_pSelectedSkin = ColorSkin::create({ .color = Color::Aquamarine, .padding = Border(2, 8) });

	auto setRowSkin = [this](int test, Skin* pSkin) {
		auto it = std::find(m_accuracyRowOrder.begin(), m_accuracyRowOrder.end(), test);
		if (it == m_accuracyRowOrder.end())
			return;

		int row = 1 + int(it - m_accuracyRowOrder.begin());
		for (int col = 0; col < m_pAccuracyTable->columns.size(); col++)
		{
			Widget* pCell = m_pAccuracyTable->slots[row][col]._widget();
			if (pCell)
				pCell->setSkin(pSkin);
		}
	};

	if (m_accuracySelected >= 0)
		setRowSkin(m_accuracySelected, s_pNormalSkin);

	m_accuracySelected = test;
	setRowSkin(test, s_pSelectedSkin);

	if (m_accuracyViewer.test != test)
		show_test(m_accuracyViewer, test);
}

//____ start_performance_run() ________________________________________________

void GfxDeviceTester2::start_performance_run()
{
	for (int i = 0; i < (int)m_tests.size(); i++)
	{
		m_tests[i].performance = PerformanceResult();
		refresh_performance_row(i);
	}

	m_bPerformanceRunning = true;
	m_performanceNext = 0;

	m_pPerformanceRunButton->label.setText("STOP");
	m_pPerformanceProgress->setValue(0.f);
	m_pPerformanceSummary->display.setText("Times each test on the testee only. Init and exit are not included.");
}

//____ continue_performance_run() _____________________________________________
/*
	Times one test per update, so the progress bar keeps moving and the window
	stays responsive.
*/

void GfxDeviceTester2::continue_performance_run()
{
	int nbTests = (int)m_tests.size();

	if (m_performanceNext < nbTests)
	{
		run_performance_test(m_performanceNext);
		refresh_performance_row(m_performanceNext);
		m_performanceNext++;
	}

	char label[256];

	if (m_performanceNext < nbTests)
	{
		auto& next = m_tests[m_performanceNext];
		snprintf(label, sizeof(label), "%d / %d   %s / %s", m_performanceNext, nbTests, m_suites[next.suite].name.c_str(), next.name.c_str());
	}
	else
	{
		snprintf(label, sizeof(label), "Done, %d tests", nbTests);

		m_bPerformanceRunning = false;
		m_pPerformanceRunButton->label.setText("RUN");

		double total = 0;
		int nbFailed = 0;
		for (auto& test : m_tests)
		{
			if (test.performance.bFailed)
				nbFailed++;
			else
				total += test.performance.secondsPerRun;
		}

		string summary = "Sum of one run of each test: " + format_time(total) + ".";
		if (nbFailed > 0)
			summary += " " + std::to_string(nbFailed) + " tests could not be run.";
		summary += " Init and exit are not included.";

		m_pPerformanceSummary->display.setText(summary);
	}

	m_pPerformanceProgress->setValue(nbTests > 0 ? m_performanceNext / float(nbTests) : 1.f);
	m_pPerformanceProgressLabel->display.setText(label);
}

//____ run_performance_test() _________________________________________________
/*
	Times a test on the testee.

	Each measurement times a number of calls to run() including the endRender()
	and a read back of a pixel, which makes sure the backend has finished its work,
	also when it is done asynchronously, as by a GPU. The same without any calls
	to run() is measured as a baseline and subtracted. That way neither init(),
	exit() nor the overhead of a render pass are included.

	The number of calls to run() is increased until a measurement takes long enough
	to be accurate. Then the fastest of a few measurements is kept, as it is the one
	least disturbed by anything else going on.
*/

void GfxDeviceTester2::run_performance_test(int test)
{
	auto& result = m_tests[test].performance;
	result = PerformanceResult();
	result.bDone = true;

	if (!m_suites[m_tests[test].suite].bWorking)
	{
		result.bFailed = true;
		return;
	}

	Test* pTest = m_tests[test].pTests[TESTEE];

	// Warm up, so any first time setup (caches, shaders, buffers) isn't measured.
	// Since that makes it slower than later runs, it gives us a low first guess
	// of the number of loops needed.

	double warmup = time_test(pTest, 1);
	double baseline = std::min(time_test(pTest, 0), time_test(pTest, 0));

	int loops = 1;
	if (warmup - baseline > 0)
		loops = (int) std::clamp(std::floor(c_perfTargetTime / (warmup - baseline)), 1.0, double(c_perfMaxLoops));

	// Increase number of loops until a measurement takes long enough.

	double fastest;

	while (true)
	{
		fastest = time_test(pTest, loops);

		double time = fastest - baseline;

		if (time >= c_perfTargetTime || loops >= c_perfMaxLoops)
			break;

		// Aim a bit over the target, but grow at least 2x and at most 100x each step.

		double factor = time > 0 ? c_perfTargetTime * 1.25 / time : 100.0;
		factor = std::clamp(factor, 2.0, 100.0);

		loops = (int) std::min(double(c_perfMaxLoops), std::ceil(loops * factor));
		baseline = std::min(baseline, time_test(pTest, 0));
	}

	// Measure. Tests that are slow even without looping get fewer rounds,
	// noise is small in comparison and we don't want to wait forever.

	int rounds = fastest > c_perfTargetTime * 10 ? c_perfSlowRounds : c_perfRounds;

	for (int round = 1; round < rounds; round++)
	{
		baseline = std::min(baseline, time_test(pTest, 0));
		fastest = std::min(fastest, time_test(pTest, loops));
	}

	result.loops = loops;
	result.secondsPerRun = std::max(0.0, fastest - baseline) / loops;
}

//____ time_test() ____________________________________________________________
/*
	Seconds from just before the first call to run() until the testee has finished
	rendering. See run_performance_test().
*/

double GfxDeviceTester2::time_test(Test* pTest, int loops)
{
	auto& backend = m_backends[TESTEE];
	GfxDevice* pDevice = backend.pDevice;

	RectSPX canvas = { 0, 0, m_canvasSize.w * 64, m_canvasSize.h * 64 };

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(backend.pCanvas);

	fill_background(pDevice, Background::Black);

	if (pTest->init)
		pTest->init(pDevice, canvas);

	auto start = Clock::now();

	for (int i = 0; i < loops; i++)
		pTest->run(pDevice, canvas);

	if (pTest->exit)
		pTest->exit(pDevice, canvas);

	pDevice->endCanvasUpdate();
	pDevice->endRender();

	sync_testee();

	auto end = Clock::now();

	return std::chrono::duration<double>(end - start).count();
}

//____ sync_testee() __________________________________________________________
/*
	Reads back a pixel from the testee canvas, which forces it to finish rendering.
*/

void GfxDeviceTester2::sync_testee()
{
	Surface* pCanvas = m_backends[TESTEE].pCanvas;

	auto buffer = pCanvas->allocPixelBuffer(RectI(0, 0, 1, 1));
	pCanvas->pushPixels(buffer);
	pCanvas->freePixelBuffer(buffer);
}

//____ refresh_performance_row() ______________________________________________

void GfxDeviceTester2::refresh_performance_row(int test)
{
	auto& entry = m_tests[test];
	auto& r = entry.performance;

	if (!r.bDone)
	{
		entry.pPerfTimeCell->display.setText("-");
		entry.pPerfTimeCell->display.setStyle(wkit::TextStyles::Default);
		entry.pPerfLoopsCell->display.setText("");
	}
	else if (r.bFailed)
	{
		entry.pPerfTimeCell->display.setText("FAILED");
		entry.pPerfTimeCell->display.setStyle(m_pDiffStyle);
		entry.pPerfLoopsCell->display.setText("");
	}
	else
	{
		entry.pPerfTimeCell->display.setText(format_time(r.secondsPerRun));
		entry.pPerfTimeCell->display.setStyle(wkit::TextStyles::Default);
		entry.pPerfLoopsCell->display.setText(std::to_string(r.loops));
	}
}

//____ format_time() __________________________________________________________

string GfxDeviceTester2::format_time(double seconds)
{
	char buffer[64];

	if (seconds < 0.000001)
		snprintf(buffer, sizeof(buffer), "%.1f ns", seconds * 1000000000.0);
	else if (seconds < 0.001)
		snprintf(buffer, sizeof(buffer), "%.2f us", seconds * 1000000.0);
	else if (seconds < 1.0)
		snprintf(buffer, sizeof(buffer), "%.3f ms", seconds * 1000.0);
	else
		snprintf(buffer, sizeof(buffer), "%.3f s", seconds);

	return buffer;
}
