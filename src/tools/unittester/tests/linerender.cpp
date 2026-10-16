#include "linerender.h"

#include <wondergui.h>
#include <wg_gfxdevice_gen2.h>
#include <wg_gfxbackend.h>
#include <wg_backendlogger.h>
#include <wg_softbackend.h>
#include <wg_softsurface.h>
#include <wg_softkernels_default.h>

#include <sstream>
#include <string>

using namespace wg;

/*
	Draws lines with caps through GfxDeviceGen2 and the software backends.
*/

LineRenderTest::LineRenderTest()
{
	ADD_TEST(capsCodecTest);
	ADD_TEST(capsCommandTest);
}

LineRenderTest::~LineRenderTest()
{
}

//____ capsCodecTest() ________________________________________________________
// Caps survive the caps word, with spear lengths rounded to multiples of 25 within 25 to 800.

bool LineRenderTest::capsCodecTest(std::ostream& output)
{
	TEST_ASSERT(GfxBackend::encodeLineCaps(LineCaps()) == 0);
	TEST_ASSERT(GfxBackend::encodeLineCaps(LineCaps(LineCap::Default, LineCap::Default)) == 0);

	// Every shape at both ends.

	for (int b = 0; b < LineCapShape_size; b++)
	{
		for (int e = 0; e < LineCapShape_size; e++)
		{
			LineCaps caps(LineCap(LineCapShape(b), 125), LineCap(LineCapShape(e), 650));
			LineCaps decoded = GfxBackend::decodeLineCaps(GfxBackend::encodeLineCaps(caps));

			TEST_ASSERT(decoded.begin.shape == caps.begin.shape);
			TEST_ASSERT(decoded.end.shape == caps.end.shape);

			if (caps.begin.shape == LineCapShape::Spear)
				TEST_ASSERT(decoded.begin.spearLength == 125);
			if (caps.end.shape == LineCapShape::Spear)
				TEST_ASSERT(decoded.end.spearLength == 650);
		}
	}

	// Spear lengths.

	struct { uint16_t in, out; } lengths[] = { {0, 25}, {1, 25}, {25, 25}, {37, 25}, {38, 50}, {200, 200}, {212, 200}, {213, 225},
											   {787, 775}, {788, 800}, {800, 800}, {801, 800}, {65535, 800} };

	for (auto& l : lengths)
	{
		LineCaps decoded = GfxBackend::decodeLineCaps(GfxBackend::encodeLineCaps(LineCaps(LineCap::Flat, LineCap::Spear(l.in))));

		if (decoded.end.spearLength != l.out)
		{
			output << "Spear length " << l.in << " decoded as " << decoded.end.spearLength << ", expected " << l.out << "." << std::endl;
			return false;
		}
		TEST_ASSERT(decoded.begin.shape == LineCapShape::Flat);
	}

	// Unused shape values decode as Default.

	TEST_ASSERT(GfxBackend::decodeLineCaps(0x0707).isDefault());
	TEST_ASSERT(GfxBackend::encodeLineCaps(LineCaps(LineCap(LineCapShape(7)))) == 0);

	return true;
}

//____ capsCommandTest() ______________________________________________________
// GfxDeviceGen2 encodes caps into its line commands, also from the direction overload.

bool LineRenderTest::capsCommandTest(std::ostream& output)
{
	auto pSoft = SoftBackend::create();
	addDefaultSoftKernels(pSoft);

	std::ostringstream log;
	auto pLogger = BackendLogger::create(&log, pSoft);
	auto pDevice = GfxDeviceGen2::create(pLogger);
	auto pCanvas = SoftSurface::create(WGBP(Surface, _.size = { 32, 32 }, _.format = PixelFormat::ARGB_8, _.canvas = true));

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(pCanvas);
	pDevice->drawLine({ 64, 64 }, { 64 * 20, 64 * 8 }, Color::Black, 64);
	pDevice->drawLine({ 64, 64 }, { 64 * 20, 64 * 8 }, Color::Black, 64, LineCap::Round);
	pDevice->drawLine({ 64, 64 }, { 64 * 20, 64 * 8 }, Color::Black, 64, { LineCap::Flat, LineCap::Spear(50) });
	pDevice->drawLine({ 64 * 4, 64 * 4 }, Direction::Down, 64 * 10, Color::Black, 128, LineCap::Square);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	std::string text = log.str();

	TEST_ASSERT(text.find("from (64, 64) to (1280, 512) with thickness 64 and color 0, 0, 0, 4096\n") != std::string::npos);
	TEST_ASSERT(text.find("caps Round and Round\n") != std::string::npos);
	TEST_ASSERT(text.find("caps Flat and Spear(50%)\n") != std::string::npos);
	TEST_ASSERT(text.find("from (256, 256) to (256, 896) with thickness 128 and color 0, 0, 0, 4096, caps Square and Square\n") != std::string::npos);

	return true;
}
