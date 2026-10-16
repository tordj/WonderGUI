#include "frost.h"

#include <wondergui.h>
#include <wg_gfxdevice_gen2.h>
#include <wg_backendlogger.h>
#include <wg_softbackend.h>
#include <wg_softsurface.h>
#include <wg_softkernels_default.h>

#include <cstring>
#include <sstream>
#include <vector>

using namespace wg;

namespace
{
	const SizeI		c_canvasSize = { 160, 120 };
	const int		c_square = 8;				// Size of the squares of the checkerboard backdrop.

	//____ Scene ______________________________________________________________
	//
	// A FrostOverlay with a checkerboard backdrop and one pane, rendered by the software
	// backend through a BackendLogger so we can count the blurs.
	//
	// The backdrop is a FlexPanel with the checkerboard in the background and a small
	// marker on top that we can change to dirty parts of the backdrop.

	class Scene
	{
	public:
		Scene()
		{
			Base::setErrorHandler([this](Error& error) { errors++; });

			canvas = SoftSurface::create(WGBP(Surface, _.size = c_canvasSize, _.format = PixelFormat::ARGB_8, _.canvas = true));

			auto pBackend = SoftBackend::create();
			addDefaultSoftKernels(pBackend);
			device = GfxDeviceGen2::create(BackendLogger::create(&log, pBackend));

			auto pChecker = LambdaSkin::create(WGBP(LambdaSkin,
				_.opaque = true,
				_.renderFunc = [this](GfxDevice* pDevice, const RectSPX& canvas, int scale, State state, float value, float value2)
				{
					backdropRenders++;
					pDevice->fill(canvas, HiColor::Black);
					for (int y = 0; y < canvas.h / 64; y += c_square)
						for (int x = 0; x < canvas.w / 64; x += c_square)
							if (_isWhite(x, y))
								pDevice->fill(RectSPX(canvas.x + x * 64, canvas.y + y * 64, c_square * 64, c_square * 64), HiColor::White);
				}));

			background = Filler::create(WGBP(Filler, _.skin = pChecker));
			marker = Filler::create();

			// First slot is on top.

			flex = FlexPanel::create();
			flex->slots.pushBack(marker, WGBP(FlexPanelSlot, _.pos = { 0,0 }, _.size = { 8,8 }));
			flex->slots.pushBack(background, WGBP(FlexPanelSlot, _.pin1 = Placement::NorthWest, _.pin2 = Placement::SouthEast));

			overlay = FrostOverlay::create();
			overlay->mainSlot = flex;

			pane = Filler::create();
			overlay->frostSlots.pushBack(pane, WGBP(FrostOverlay::Slot, _.pos = { 40,30 }, _.size = { 80,60 }));

			root = RootPanel::create(canvas, device);
			root->slot = overlay;
		}

		~Scene()
		{
			Base::setErrorHandler(nullptr);
		}

		// Renders what is dirty and reads back the canvas.

		void render()
		{
			log.str("");
			backdropRenders = 0;
			root->render();

			pixels.resize(c_canvasSize.w * c_canvasSize.h * 4);

			auto buffer = canvas->allocPixelBuffer();
			canvas->pushPixels(buffer);
			for (int y = 0; y < c_canvasSize.h; y++)
				std::memcpy(pixels.data() + y * c_canvasSize.w * 4, buffer.pixels + y * buffer.pitch, c_canvasSize.w * 4);
			canvas->freePixelBuffer(buffer);
		}


		// Number of blur commands in the last render.

		int blurs() const
		{
			std::istringstream lines(log.str());
			std::string line;
			int n = 0;
			while (std::getline(lines, line))
				if (line.rfind("    Blur: ", 0) == 0)
					n++;
			return n;
		}

		// Channel of a pixel in the last render. Channels are in BGRA order.

		int at(int x, int y, int channel = 1) const { return pixels[(y * c_canvasSize.w + x) * 4 + channel]; }

		// Pixel is exactly the checkerboard.

		bool isSharp(int x, int y) const
		{
			int expected = _isWhite(x, y) ? 255 : 0;
			return at(x, y, 0) == expected && at(x, y, 1) == expected && at(x, y, 2) == expected;
		}

		bool isSharp(const RectI& area) const
		{
			for (int y = area.y; y < area.y + area.h; y++)
				for (int x = area.x; x < area.x + area.w; x++)
					if (!isSharp(x, y))
						return false;
			return true;
		}

		// Difference between brightest and darkest green within area. The checkerboard
		// has 255, frost should have hardly any.

		int contrast(const RectI& area) const
		{
			int lo = 255, hi = 0;
			for (int y = area.y; y < area.y + area.h; y++)
				for (int x = area.x; x < area.x + area.w; x++)
				{
					lo = std::min(lo, at(x, y));
					hi = std::max(hi, at(x, y));
				}
			return hi - lo;
		}

		SoftSurface_p		canvas;
		GfxDeviceGen2_p		device;
		std::ostringstream	log;

		RootPanel_p			root;
		FrostOverlay_p		overlay;
		FlexPanel_p			flex;
		Filler_p			background;
		Filler_p			marker;
		Filler_p			pane;

		int					backdropRenders = 0;
		int					errors = 0;
		std::vector<uint8_t> pixels;

	private:
		static bool _isWhite(int x, int y) { return ((x / c_square) + (y / c_square)) % 2 == 1; }
	};
}

//____ constructor ____________________________________________________________

FrostTest::FrostTest()
{
	ADD_TEST(frostBlursBackdrop);
	ADD_TEST(frostIsCached);
	ADD_TEST(frostMask);
	ADD_TEST(paneOutsideBackdrop);
	ADD_TEST(slotChanges);
}

FrostTest::~FrostTest()
{
}

//____ frostBlursBackdrop() ___________________________________________________

bool FrostTest::frostBlursBackdrop(std::ostream& output)
{
	Scene s;
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.backdropRenders == 1);
	TEST_ASSERT(s.blurs() > 0);

	// Outside the pane the backdrop is copied as it is.

	TEST_ASSERT(s.isSharp({ 0,0,160,30 }));
	TEST_ASSERT(s.isSharp({ 0,90,160,30 }));
	TEST_ASSERT(s.isSharp({ 0,30,40,60 }));
	TEST_ASSERT(s.isSharp({ 120,30,40,60 }));

	// Within the pane the checkerboard is blurred into grey.

	RectI inside = { 44,34,72,52 };
	TEST_ASSERT(s.contrast(inside) < 48);
	TEST_ASSERT(s.at(80, 60) > 64 && s.at(80, 60) < 224);

	// A haze on top of the blur.

	s.overlay->frost.setColor(HiColor(4096, 4096, 4096, 2048));
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.backdropRenders == 0);
	TEST_ASSERT(s.blurs() > 0);
	TEST_ASSERT(s.contrast(inside) < 32);
	TEST_ASSERT(s.at(80, 60) > 160);
	TEST_ASSERT(s.isSharp({ 0,0,160,30 }));

	return true;
}

//____ frostIsCached() ________________________________________________________

bool FrostTest::frostIsCached(std::ostream& output)
{
	Scene s;
	s.render();

	// Content in the pane changes, frost stays.

	s.pane->setSkin(ColorSkin::create(HiColor(4096, 0, 0, 1024)));
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.backdropRenders == 0);
	TEST_ASSERT(s.blurs() == 0);
	TEST_ASSERT(s.at(80, 60, 2) > s.at(80, 60, 1));			// Tinted red, on top of frost.
	TEST_ASSERT(s.contrast({ 44,34,72,52 }) < 48);

	// Pane moves. The backdrop is cached, the frost is recalculated.

	s.overlay->frostSlots[0].setPos({ 60,40 });
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.backdropRenders == 0);
	TEST_ASSERT(s.blurs() > 0);
	TEST_ASSERT(s.isSharp({ 40,30,20,60 }));				// Where the pane was.
	TEST_ASSERT(s.isSharp({ 40,30,80,10 }));
	TEST_ASSERT(s.contrast({ 64,44,72,52 }) < 48);

	// Backdrop changes far from the pane, frost stays. The marker is opaque, so
	// the checkerboard below it isn't rendered.

	s.marker->setSkin(ColorSkin::create(HiColor(0, 0, 4096)));
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.blurs() == 0);
	TEST_ASSERT(s.at(4, 4, 0) == 255 && s.at(4, 4, 1) == 0);	// Marker is blue.

	// Backdrop changes behind the pane, frost is recalculated.

	s.flex->slots[0].move({ 90,60 });
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.backdropRenders == 1);						// Where the marker was.
	TEST_ASSERT(s.blurs() > 0);
	TEST_ASSERT(s.isSharp({ 0,0,8,8 }));					// Marker has left.
	TEST_ASSERT(s.at(94, 64, 0) > s.at(94, 64, 1));			// Blue shines through the frost.

	// Nothing changed, nothing rendered.

	s.render();
	TEST_ASSERT(s.backdropRenders == 0);
	TEST_ASSERT(s.blurs() == 0);

	return true;
}

//____ frostMask() ____________________________________________________________

bool FrostTest::frostMask(std::ostream& output)
{
	Scene s;

	// A mask with a hole in the top left corner and a red band at the bottom.

	auto pMask = LambdaSkin::create(WGBP(LambdaSkin,
		_.renderFunc = [](GfxDevice* pDevice, const RectSPX& canvas, int scale, State state, float value, float value2)
		{
			spx corner = 10 * 64;
			pDevice->fill(RectSPX(canvas.x + corner, canvas.y, canvas.w - corner, corner), HiColor::White);
			pDevice->fill(RectSPX(canvas.x, canvas.y + corner, canvas.w, canvas.h - corner * 2), HiColor::White);
			pDevice->fill(RectSPX(canvas.x, canvas.y + canvas.h - corner, canvas.w, corner), HiColor(4096, 0, 0));
		}));

	s.overlay->frost.setMask(pMask);
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.blurs() > 0);

	TEST_ASSERT(s.isSharp({ 40,30,10,10 }));				// Hole in the mask.
	TEST_ASSERT(s.contrast({ 50,34,66,40 }) < 48);			// Frost.

	TEST_ASSERT(s.at(80, 85, 2) > 64);						// Red band.
	TEST_ASSERT(s.at(80, 85, 1) == 0);
	TEST_ASSERT(s.at(80, 85, 0) == 0);

	TEST_ASSERT(s.isSharp({ 0,0,160,30 }));					// Outside the pane.
	TEST_ASSERT(s.isSharp({ 0,90,160,30 }));

	return true;
}

//____ paneOutsideBackdrop() __________________________________________________

bool FrostTest::paneOutsideBackdrop(std::ostream& output)
{
	Scene s;

	s.overlay->frostSlots[0].setPos({ 120,90 });
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.contrast({ 124,94,36,26 }) < 64);
	TEST_ASSERT(s.isSharp({ 0,0,120,120 }));

	s.overlay->frostSlots[0].setPos({ -40,-30 });
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.contrast({ 0,0,36,26 }) < 64);
	TEST_ASSERT(s.isSharp({ 40,30,120,90 }));

	// Pane entirely outside.

	s.overlay->frostSlots[0].setPos({ 200,200 });
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.isSharp({ 0,0,160,120 }));

	return true;
}

//____ slotChanges() __________________________________________________________

bool FrostTest::slotChanges(std::ostream& output)
{
	Scene s;
	s.render();

	// Frost turned off and on again.

	s.overlay->frostSlots[0].setFrost(false);
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.isSharp({ 0,0,160,120 }));

	s.overlay->frostSlots[0].setFrost(true);
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.blurs() > 0);
	TEST_ASSERT(s.contrast({ 44,34,72,52 }) < 48);

	// Second pane on top, partly over the first one.

	auto pTopPane = Filler::create(WGBP(Filler, _.skin = ColorSkin::create(HiColor(0, 4096, 0, 1024))));
	s.overlay->frostSlots.pushFront(pTopPane, WGBP(FrostOverlay::Slot, _.pos = { 100,70 }, _.size = { 50,40 }));
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.at(120, 90, 1) > s.at(120, 90, 2));		// Tinted green, on top of frost.
	TEST_ASSERT(s.contrast({ 104,74,42,32 }) < 48);
	TEST_ASSERT(s.contrast({ 44,34,52,32 }) < 48);
	TEST_ASSERT(s.isSharp({ 120,30,40,40 }));

	// Panes removed.

	pTopPane->releaseFromParent();
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.overlay->frostSlots.size() == 1);
	TEST_ASSERT(s.isSharp({ 120,30,40,90 }));

	s.overlay->frostSlots.erase(0);
	s.render();

	TEST_ASSERT(s.errors == 0);
	TEST_ASSERT(s.isSharp({ 0,0,160,120 }));

	return true;
}
