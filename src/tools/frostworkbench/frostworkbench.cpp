
#include "frostworkbench.h"

#include <wondergui.h>

#include <algorithm>
#include <cmath>

using namespace wg;

//____ create() _______________________________________________________________

WonderApp_p WonderApp::create()
{
	return new MyApp();
}

//____ init() _________________________________________________________________

bool MyApp::init(wapp::API* pAPI)
{
	if (!setupGUI(pAPI))
	{
		printf("ERROR: Failed to setup GUI!\n");
		return false;
	}

	return true;
}

//____ update() _______________________________________________________________

bool MyApp::update()
{
	if (!m_pWindow)
		return false;

	// Bounce a box around the backdrop, so that the backdrop changes behind the panes.

	auto it = m_pBackdrop->slots.find(m_pBouncer);
	Rect geo = it->geo();
	Size size = m_pBackdrop->size();

	if (geo.x + m_bouncerSpeed.x < 0 || geo.x + geo.w + m_bouncerSpeed.x > size.w)
		m_bouncerSpeed.x = -m_bouncerSpeed.x;

	if (geo.y + m_bouncerSpeed.y < 0 || geo.y + geo.h + m_bouncerSpeed.y > size.h)
		m_bouncerSpeed.y = -m_bouncerSpeed.y;

	it->move(m_bouncerSpeed);

	return true;
}

//____ exit() _________________________________________________________________

void MyApp::exit()
{
}

//____ closeWindow() __________________________________________________________

void MyApp::closeWindow(wapp::Window* pWindow)
{
	if (pWindow == m_pWindow)
		m_pWindow = nullptr;
}

//____ setupGUI() _____________________________________________________________

bool MyApp::setupGUI(wapp::API* pAPI)
{
	m_pWindow = wapp::Window::create(pAPI, {
		.size = { 800, 600 },
		.title = "Frost Workbench - drag the panes with the left mouse button"
	});

	auto pPhoto = pAPI->loadSurface("resources/frog.jpg");
	if (!pPhoto)
		return false;

	// The backdrop: a photo with a box bouncing around on it. First slot is on top.

	m_pBouncer = Filler::create({
		.skin = BoxSkin::create({ .color = Color::Orange, .outlineColor = Color::Black, .outlineThickness = 4 })
	});

	m_pBackdrop = FlexPanel::create();
	m_pBackdrop->slots.pushBack(m_pBouncer, { .pos = { 100,100 }, .size = { 80,80 } });
	m_pBackdrop->slots.pushBack(Filler::create({ .skin = BlockSkin::create(pPhoto) }), { .pin1 = Placement::NorthWest, .pin2 = Placement::SouthEast });

	// Frost with rounded corners and a light haze.

	auto pMask = BlockSkin::create(createRoundedRect(16, 0, HiColor::White), Border(16));

	m_pOverlay = FrostOverlay::create({
		.frost = {
			.color = HiColor(4096, 4096, 4096, 150),
			.downscale = 4,
			.mask = pMask,
			.passes = 3,
			.radius = 24
		}
	});

	m_pOverlay->mainSlot = m_pBackdrop;

	// Panes with a thin highlight along their rounded edge.

	auto pPaneSkin = BlockSkin::create(createRoundedRect(16, 1, HiColor(4096, 4096, 4096, 2048)), Border(16));

	auto pPane1 = Filler::create({ .markPolicy = MarkPolicy::Geometry, .skin = pPaneSkin });
	auto pPane2 = Filler::create({ .markPolicy = MarkPolicy::Geometry, .skin = pPaneSkin });

	m_pOverlay->frostSlots.pushBack(pPane1, { .pos = { 80,80 }, .size = { 320,220 } });
	m_pOverlay->frostSlots.pushBack(pPane2, { .pos = { 360,260 }, .size = { 300,260 } });

	makeDraggable(pPane1);
	makeDraggable(pPane2);

	m_pWindow->mainCapsule()->slot = m_pOverlay;

	return true;
}

//____ makeDraggable() ________________________________________________________

void MyApp::makeDraggable(Widget* pPane)
{
	Base::msgRouter()->addRoute(pPane, MsgType::MouseDrag, [this, pPane](Msg* _pMsg)
		{
			auto pMsg = static_cast<MouseDragMsg*>(_pMsg);

			if (pMsg->button() != MouseButton::Left)
				return;

			auto it = m_pOverlay->frostSlots.find(pPane);
			if (it != m_pOverlay->frostSlots.end())
				it->move(pMsg->draggedNow());
		});
}

//____ createRoundedRect() ____________________________________________________
//
// Creates a white rounded rectangle, anti-aliased, to be stretched as a nine-patch
// with a frame of the radius. A thickness of 0 fills it, otherwise it is an outline.

Surface_p MyApp::createRoundedRect(int radius, int thickness, HiColor color)
{
	int size = radius * 2 + 2;

	auto pSurface = Base::defaultSurfaceFactory()->createSurface(WGBP(Surface,
		_.format = PixelFormat::ARGB_8,
		_.size = { size, size }));

	Color8 col = color;

	auto buffer = pSurface->allocPixelBuffer();

	for (int y = 0; y < size; y++)
	{
		uint8_t* pPixel = buffer.pixels + y * buffer.pitch;

		for (int x = 0; x < size; x++)
		{
			// Distance inwards from the edge, at the center of the pixel.

			float px = x + 0.5f, py = y + 0.5f;
			float cx = std::clamp(px, float(radius), float(size - radius));
			float cy = std::clamp(py, float(radius), float(size - radius));
			float dist = radius - std::hypot(px - cx, py - cy);

			float coverage = std::clamp(dist, 0.f, 1.f);
			if (thickness > 0)
				coverage -= std::clamp(dist - thickness, 0.f, 1.f);

			pPixel[0] = col.b;
			pPixel[1] = col.g;
			pPixel[2] = col.r;
			pPixel[3] = uint8_t(col.a * coverage + 0.5f);
			pPixel += 4;
		}
	}

	pSurface->pullPixels(buffer);
	pSurface->freePixelBuffer(buffer);

	return pSurface;
}
