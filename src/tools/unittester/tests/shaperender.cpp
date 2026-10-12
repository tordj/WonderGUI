#include "shaperender.h"

#include <wondergui.h>
#include <wg_gfxdevice_gen2.h>
#include <wg_gfxutil.h>
#include <wg_softbackend.h>
#include <wg_linearbackend.h>
#include <wg_softsurface.h>
#include <wg_softsurfacefactory.h>
#include <wg_softedgemapfactory.h>
#include <wg_softkernels_default.h>
#include <wg_softshapes.h>
#include <wg_streambackend.h>
#include <wg_streamplayer.h>
#include <wg_streamencoder.h>

#include <algorithm>
#include <functional>

using namespace wg;

/*
	Draws shapes through GfxDeviceGen2 and the software backends and checks the pixels
	against the coverage from SoftShapes and the rule for partly covered pixels:

	  out = dst * (1 - coverage) + blend(src, dst) * coverage

	Canvases are ARGB_8 in linear color space, so channels can be compared without
	color space conversions.
*/

namespace
{
	const SizeI c_canvasSize = { 64, 48 };

	struct C12
	{
		int		b, g, r, a;		// 0 -> 4096, linear.
	};

	C12 toC12(HiColor c)
	{
		HiColor l = c.toLinear();
		return { l.b, l.g, l.r, l.a };
	}

	//____ Canvas _____________________________________________________________

	class Canvas
	{
	public:
		Canvas()
		{
			surface = SoftSurface::create(WGBP(Surface, _.size = c_canvasSize, _.format = PixelFormat::ARGB_8, _.colorSpace = ColorSpace::Linear, _.canvas = true));
		}

		std::vector<uint8_t> read() const
		{
			std::vector<uint8_t> out(c_canvasSize.w * c_canvasSize.h * 4);

			auto buffer = surface->allocPixelBuffer();
			surface->pushPixels(buffer);

			for (int y = 0; y < c_canvasSize.h; y++)
				std::memcpy(out.data() + y * c_canvasSize.w * 4, buffer.pixels + y * buffer.pitch, c_canvasSize.w * 4);

			surface->freePixelBuffer(buffer);
			return out;
		}

		SoftSurface_p	surface;
	};

	// Channel of 8-bit BGRA pixel as 12-bit linear.

	inline int unpack(const std::vector<uint8_t>& pixels, int x, int y, int channel)
	{
		return HiColor::unpackLinearTab[pixels[(y * c_canvasSize.w + x) * 4 + channel]];
	}

	inline int byteAt(const std::vector<uint8_t>& pixels, int x, int y, int channel)
	{
		return pixels[(y * c_canvasSize.w + x) * 4 + channel];
	}

	GfxDeviceGen2_p createSoftDevice()
	{
		auto pBackend = SoftBackend::create();
		addDefaultSoftKernels(pBackend);
		return GfxDeviceGen2::create(pBackend);
	}

	//____ Coverage ___________________________________________________________

	class Coverage : public SoftShapes::RowSink
	{
	public:
		Coverage() : fill(c_canvasSize.w * c_canvasSize.h, 0), outline(c_canvasSize.w * c_canvasSize.h, 0) {}

		void row(int y, int nSpans, const SoftShapes::Span* pSpans) override
		{
			for (int i = 0; i < nSpans; i++)
			{
				auto& span = pSpans[i];
				for (int x = span.begin; x < span.end; x++)
				{
					int idx = y * c_canvasSize.w + x;
					if (span.segment >= 0)
						(span.segment == 0 ? fill : outline)[idx] = 4096;
					else
					{
						const uint16_t* p = span.pCoverage + (span.bUniform ? 0 : (x - span.begin) * span.nLayers);
						for (int l = 0; l < span.nLayers; l++)
							(span.pLayerSegments[l] == 0 ? fill : outline)[idx] += p[l];
					}
				}
			}
		}

		std::vector<int>	fill;
		std::vector<int>	outline;
	};

	Coverage coverageOf(const RoundedBoxSpec& spec)
	{
		SoftShapes::RoundedBox box;
		box.rect = spec.rect;
		for (int i = 0; i < 4; i++)
			box.radius[i] = spec.radius[i];
		box.outline = spec.outline;

		Coverage cov;
		SoftShapes shapes;
		shapes.rasterizeRoundedBox(box, RectI(0, 0, c_canvasSize.w, c_canvasSize.h), &cov);
		return cov;
	}

	//____ blendFull() ________________________________________________________
	// Result of blending src onto dst with full coverage, as the fill kernels do.

	C12 blendFull(BlendMode mode, C12 s, C12 d, int morph, C12 fixed)
	{
		auto lim = [](int v) { return std::clamp(v, 0, 4096); };

		switch (mode)
		{
			case BlendMode::Replace:
				return s;
			case BlendMode::Blend:
				return { (d.b * (4096 - s.a) + s.b * s.a) / 4096, (d.g * (4096 - s.a) + s.g * s.a) / 4096,
						 (d.r * (4096 - s.a) + s.r * s.a) / 4096, (d.a * (4096 - s.a) + 4096 * s.a) / 4096 };
			case BlendMode::BlendFixedColor:
				return { (fixed.b * (4096 - s.a) + s.b * s.a) / 4096, (fixed.g * (4096 - s.a) + s.g * s.a) / 4096,
						 (fixed.r * (4096 - s.a) + s.r * s.a) / 4096, (fixed.a * (4096 - s.a) + 4096 * s.a) / 4096 };
			case BlendMode::Add:
				return { lim(d.b + s.b * s.a / 4096), lim(d.g + s.g * s.a / 4096), lim(d.r + s.r * s.a / 4096), d.a };
			case BlendMode::Subtract:
				return { lim(d.b - s.b * s.a / 4096), lim(d.g - s.g * s.a / 4096), lim(d.r - s.r * s.a / 4096), d.a };
			case BlendMode::Multiply:
				return { d.b * s.b / 4096, d.g * s.g / 4096, d.r * s.r / 4096, d.a };
			case BlendMode::Invert:
				return { (s.b * (4096 - d.b) + d.b * (4096 - s.b)) / 4096, (s.g * (4096 - d.g) + d.g * (4096 - s.g)) / 4096,
						 (s.r * (4096 - d.r) + d.r * (4096 - s.r)) / 4096, d.a };
			case BlendMode::Min:
				return { std::min(d.b, s.b), std::min(d.g, s.g), std::min(d.r, s.r), d.a };
			case BlendMode::Max:
				return { std::max(d.b, s.b), std::max(d.g, s.g), std::max(d.r, s.r), d.a };
			case BlendMode::Morph:
				return { (d.b * (4096 - morph) + s.b * morph) / 4096, (d.g * (4096 - morph) + s.g * morph) / 4096,
						 (d.r * (4096 - morph) + s.r * morph) / 4096, (d.a * (4096 - morph) + s.a * morph) / 4096 };
			default:
				return d;
		}
	}

	C12 mix(C12 d, C12 m, int coverage)
	{
		int inv = 4096 - coverage;
		return { (d.b * inv + m.b * coverage) / 4096, (d.g * inv + m.g * coverage) / 4096,
				 (d.r * inv + m.r * coverage) / 4096, (d.a * inv + m.a * coverage) / 4096 };
	}

	// Compares canvas to expected 12-bit colors, with tolerance in 8-bit units.

	bool compare(std::ostream& output, const char* pWhat, const std::vector<uint8_t>& pixels, int tolerance,
				 const std::function<C12(int x, int y)>& expected)
	{
		for (int y = 0; y < c_canvasSize.h; y++)
		{
			for (int x = 0; x < c_canvasSize.w; x++)
			{
				C12 e = expected(x, y);
				int want[4] = { HiColor::packLinearTab[std::clamp(e.b, 0, 4096)], HiColor::packLinearTab[std::clamp(e.g, 0, 4096)],
								HiColor::packLinearTab[std::clamp(e.r, 0, 4096)], HiColor::packLinearTab[std::clamp(e.a, 0, 4096)] };

				for (int ch = 0; ch < 4; ch++)
				{
					int got = byteAt(pixels, x, y, ch);
					if (std::abs(got - want[ch]) > tolerance)
					{
						output << pWhat << ": pixel " << x << "," << y << " channel " << ch << " (BGRA) is " << got << ", expected " << want[ch] << "." << std::endl;
						return false;
					}
				}
			}
		}
		return true;
	}

	RoundedBoxSpec testBox()
	{
		RoundedBoxSpec spec;
		spec.rect = { 64 * 5 + 19, 64 * 4 + 45, 64 * 40 + 32, 64 * 30 + 13 };
		spec.radius[0] = 64 * 9 + 32;
		spec.radius[1] = 64 * 4;
		spec.radius[2] = 64 * 12 + 7;
		spec.radius[3] = 64 * 2 + 10;
		return spec;
	}

	const HiColor c_background = HiColor(800, 2500, 3600, 4096);
}

//____ Constructor ____________________________________________________________

ShapeRenderTest::ShapeRenderTest()
{
	ADD_TEST(blendModesTest);
	ADD_TEST(fillAndOutlineTest);
	ADD_TEST(tintColorTest);
	ADD_TEST(gradientTintTest);
	ADD_TEST(clipListTest);
	ADD_TEST(linearBackendTest);
	ADD_TEST(streamTest);
	ADD_TEST(alignedBoxTest);
	ADD_TEST(boxSkinTest);
	ADD_TEST(ringTest);
	ADD_TEST(undefinedColorTest);
}

ShapeRenderTest::~ShapeRenderTest()
{
}

//____ blendModesTest() _______________________________________________________
// Single segment box in all blend modes, partly covered pixels blended against background.

bool ShapeRenderTest::blendModesTest(std::ostream& output)
{
	BlendMode modes[] = { BlendMode::Replace, BlendMode::Blend, BlendMode::BlendFixedColor, BlendMode::Add, BlendMode::Subtract,
						  BlendMode::Multiply, BlendMode::Invert, BlendMode::Min, BlendMode::Max, BlendMode::Morph };

	RoundedBoxSpec spec = testBox();

	Coverage cov = coverageOf(spec);

	HiColor fixedColor(1000, 3000, 2000, 2500);
	float morphFactor = 0.3f;

	// Second color is bright enough to saturate Add.

	HiColor colors[2] = { HiColor(3000, 1500, 600, 3000), HiColor(4096, 3500, 3900, 3500) };

	for (auto color : colors)
	for (auto mode : modes)
	{
		spec.fillColor = color;

		Canvas canvas;
		auto pDevice = createSoftDevice();

		pDevice->beginRender();
		pDevice->beginCanvasUpdate(canvas.surface);
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->fill(c_background);
		pDevice->endCanvasUpdate();
		pDevice->endRender();

		auto before = canvas.read();

		pDevice->beginRender();
		pDevice->beginCanvasUpdate(canvas.surface);
		pDevice->setBlendMode(mode);
		pDevice->setFixedBlendColor(fixedColor);
		pDevice->setMorphFactor(morphFactor);
		pDevice->drawRoundedBox(spec);
		pDevice->endCanvasUpdate();
		pDevice->endRender();

		auto after = canvas.read();

		C12 src = toC12(spec.fillColor);
		C12 fixed = toC12(fixedColor);
		int morph = int(morphFactor * 4096);

		std::string what = std::string("BlendMode::") + toString(mode);

		bool bOk = compare(output, what.c_str(), after, 3, [&](int x, int y)
		{
			C12 dst = { unpack(before, x, y, 0), unpack(before, x, y, 1), unpack(before, x, y, 2), unpack(before, x, y, 3) };
			int c = cov.fill[y * c_canvasSize.w + x];
			return mix(dst, blendFull(mode, src, dst, morph, fixed), c);
		});

		if (!bOk)
			return false;
	}

	return true;
}

//____ fillAndOutlineTest() ___________________________________________________
// Fill and semi-transparent outline in Blend mode, colors mixed where they share pixels.

bool ShapeRenderTest::fillAndOutlineTest(std::ostream& output)
{
	RoundedBoxSpec spec = testBox();
	spec.outline = { 64 + 20, 64 * 3 + 5, 40, 64 * 2 };
	spec.fillColor = HiColor(3000, 1500, 600, 4096);
	spec.outlineColor = HiColor(200, 4000, 1000, 2600);

	Coverage cov = coverageOf(spec);

	Canvas canvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(canvas.surface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(c_background);
	pDevice->setBlendMode(BlendMode::Blend);
	pDevice->drawRoundedBox(spec);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto pixels = canvas.read();

	C12 bg = toC12(c_background);
	C12 f = toC12(spec.fillColor);
	C12 o = toC12(spec.outlineColor);

	return compare(output, "Fill and outline", pixels, 3, [&](int x, int y)
	{
		int cf = cov.fill[y * c_canvasSize.w + x] * f.a / 4096;
		int co = cov.outline[y * c_canvasSize.w + x] * o.a / 4096;
		int back = 4096 - cf - co;

		return C12{ (bg.b * back + f.b * cf + o.b * co) / 4096, (bg.g * back + f.g * cf + o.g * co) / 4096,
					(bg.r * back + f.r * cf + o.r * co) / 4096, (bg.a * back + 4096 * (cf + co)) / 4096 };
	});
}

//____ tintColorTest() ________________________________________________________

bool ShapeRenderTest::tintColorTest(std::ostream& output)
{
	RoundedBoxSpec spec = testBox();
	spec.outline = { 64 * 2, 64 * 2, 64 * 2, 64 * 2 };
	spec.fillColor = HiColor(3000, 1500, 600, 4096);
	spec.outlineColor = HiColor(200, 4000, 1000, 4096);

	HiColor tintColor(4096, 2048, 1024, 4096);

	Coverage cov = coverageOf(spec);

	Canvas canvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(canvas.surface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(c_background);
	pDevice->setBlendMode(BlendMode::Blend);
	pDevice->setTintColor(tintColor);
	pDevice->drawRoundedBox(spec);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto pixels = canvas.read();

	C12 bg = toC12(c_background);
	C12 t = toC12(tintColor);
	C12 f = toC12(spec.fillColor);
	C12 o = toC12(spec.outlineColor);

	f = { f.b * t.b / 4096, f.g * t.g / 4096, f.r * t.r / 4096, f.a * t.a / 4096 };
	o = { o.b * t.b / 4096, o.g * t.g / 4096, o.r * t.r / 4096, o.a * t.a / 4096 };

	return compare(output, "Tint color", pixels, 3, [&](int x, int y)
	{
		int cf = cov.fill[y * c_canvasSize.w + x];
		int co = cov.outline[y * c_canvasSize.w + x];
		int back = 4096 - cf - co;

		return C12{ (bg.b * back + f.b * cf + o.b * co) / 4096, (bg.g * back + f.g * cf + o.g * co) / 4096,
					(bg.r * back + f.r * cf + o.r * co) / 4096, 4096 };
	});
}

//____ gradientTintTest() _____________________________________________________
// Gradient tints on fill and outline.

bool ShapeRenderTest::gradientTintTest(std::ostream& output)
{
	RoundedBoxSpec spec = testBox();
	spec.outline = { 64 * 3, 64 * 3, 64 * 3, 64 * 3 };
	spec.fillColor = HiColor::White;
	spec.outlineColor = HiColor::White;

	auto pFillTint = Tint::create(HiColor(4096, 0, 0, 4096), HiColor(0, 0, 4096, 4096));							// Vertical
	auto pOutlineTint = Tint::create(HiColor(0, 4096, 0, 4096), HiColor(4096, 4096, 0, 4096), { 0.f, 0.f }, { 1.f, 0.f });	// Horizontal

	spec.pFillTint = pFillTint;
	spec.pOutlineTint = pOutlineTint;

	Coverage cov = coverageOf(spec);

	Canvas canvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(canvas.surface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(c_background);
	pDevice->setBlendMode(BlendMode::Blend);
	pDevice->drawRoundedBox(spec);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto pixels = canvas.read();

	C12 bg = toC12(c_background);

	return compare(output, "Gradient tints", pixels, 4, [&](int x, int y)
	{
		C12 f = toC12(pFillTint->colorAt({ x * 64, y * 64 }, spec.rect));
		C12 o = toC12(pOutlineTint->colorAt({ x * 64, y * 64 }, spec.rect));

		int cf = cov.fill[y * c_canvasSize.w + x];
		int co = cov.outline[y * c_canvasSize.w + x];
		int back = 4096 - cf - co;

		return C12{ (bg.b * back + f.b * cf + o.b * co) / 4096, (bg.g * back + f.g * cf + o.g * co) / 4096,
					(bg.r * back + f.r * cf + o.r * co) / 4096, 4096 };
	});
}

//____ clipListTest() _________________________________________________________

bool ShapeRenderTest::clipListTest(std::ostream& output)
{
	RoundedBoxSpec spec = testBox();
	spec.outline = { 64 * 2, 64 * 2, 64 * 2, 64 * 2 };
	spec.fillColor = HiColor(3000, 1500, 600, 3500);
	spec.outlineColor = HiColor(200, 4000, 1000, 4096);

	RectSPX clips[2] = { { 64 * 3, 64 * 2, 64 * 20, 64 * 15 }, { 64 * 30, 64 * 20, 64 * 30, 64 * 25 } };

	auto render = [&](bool bClip)
	{
		Canvas canvas;
		auto pDevice = createSoftDevice();

		pDevice->beginRender();
		pDevice->beginCanvasUpdate(canvas.surface);
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->fill(c_background);
		pDevice->setBlendMode(BlendMode::Blend);

		if (bClip)
			pDevice->setClipList(2, clips);

		pDevice->drawRoundedBox(spec);
		pDevice->endCanvasUpdate();
		pDevice->endRender();

		return canvas.read();
	};

	auto clipped = render(true);
	auto whole = render(false);

	C12 bg = toC12(c_background);

	return compare(output, "Clipped", clipped, 0, [&](int x, int y)
	{
		CoordSPX pos(x * 64, y * 64);
		if (clips[0].contains(pos) || clips[1].contains(pos))
			return C12{ unpack(whole, x, y, 0), unpack(whole, x, y, 1), unpack(whole, x, y, 2), unpack(whole, x, y, 3) };
		else
			return bg;
	});
}

//____ Scene for comparing backends _______________________________________

namespace
{
	void drawScene(GfxDevice* pDevice)
	{
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->fill(c_background);

		RoundedBoxSpec spec = testBox();
		spec.outline = { 64 + 20, 64 * 3 + 5, 40, 64 * 2 };
		spec.fillColor = HiColor(3000, 1500, 600, 3500);
		spec.outlineColor = HiColor(200, 4000, 1000, 2600);

		auto pTint = Tint::create(HiColor(4096, 0, 0, 4096), HiColor(0, 0, 4096, 2000));
		spec.pOutlineTint = pTint;

		pDevice->setBlendMode(BlendMode::Blend);
		pDevice->drawRoundedBox(spec);

		RoundedBoxSpec pill;
		pill.rect = { 64 * 2 + 10, 64 * 30 + 50, 64 * 50, 64 * 12 + 20 };
		pill.radius[0] = pill.radius[1] = pill.radius[2] = pill.radius[3] = 64 * 1000;
		pill.fillColor = HiColor(4096, 4096, 0, 2048);

		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->drawRoundedBox(pill);

		pDevice->setBlendMode(BlendMode::Add);
		pDevice->setTintColor(HiColor(2048, 4096, 4096, 4096));
		pill.rect.y -= 64 * 20 + 13;
		pDevice->drawRoundedBox(pill);
		pDevice->clearTintColor();

		auto pSliceTint = Tint::create(HiColor(4096, 4096, 4096, 4096), HiColor(0, 0, 0, 4096), { 0.f, 0.f }, { 1.f, 1.f });

		RingSlice slices[4] = { { 0.2f, HiColor(4096, 0, 0, 4096) }, { 0.3f, HiColor(0, 4096, 0, 3000), pSliceTint },
								{ 0.1f, HiColor(0, 0, 4096, 4096) }, { 0.15f, HiColor(4096, 4096, 0, 4096) } };

		RingSpec ring;
		ring.rect = { 64 * 30 + 21, 64 * 3 + 9, 64 * 30 + 40, 64 * 30 + 40 };
		ring.thickness = 64 * 9;
		ring.outline = 64 + 20;
		ring.start = 0.85f;
		ring.nSlices = 4;
		ring.pSlices = slices;
		ring.outlineColor = HiColor(1000, 1000, 1000, 4096);
		ring.hubColor = HiColor(4096, 4096, 4096, 2000);
		ring.backColor = HiColor(0, 0, 0, 1500);

		pDevice->setBlendMode(BlendMode::Blend);
		pDevice->drawRing(ring);
	}
}

//____ linearBackendTest() ____________________________________________________
// LinearBackend must draw exactly what SoftBackend draws.

bool ShapeRenderTest::linearBackendTest(std::ostream& output)
{
	Canvas canvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(canvas.surface);
	drawScene(pDevice);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto softPixels = canvas.read();

	// Same with LinearBackend, rendered in two update rectangles.

	std::vector<uint8_t> buffer(c_canvasSize.w * c_canvasSize.h * 4 * 2);
	std::vector<uint8_t> linearPixels(c_canvasSize.w * c_canvasSize.h * 4, 0);

	auto pLinearBackend = LinearBackend::create(
		[&](CanvasRef ref, int bytes)
		{
			return (void*)buffer.data();
		},
		[&](CanvasRef ref, int nSegments, const LinearBackend::Segment* pSegments)
		{
			for (int i = 0; i < nSegments; i++)
			{
				auto& seg = pSegments[i];
				for (int y = 0; y < seg.rect.h; y++)
					std::memcpy(linearPixels.data() + ((seg.rect.y + y) * c_canvasSize.w + seg.rect.x) * 4, seg.pBuffer + y * seg.pitch, seg.rect.w * 4);
			}
		});

	addDefaultSoftKernels(pLinearBackend);
	pLinearBackend->defineCanvas(CanvasRef::Default, c_canvasSize * 64, PixelFormat::ARGB_8, 64, ColorSpace::Linear);

	auto pLinearDevice = GfxDeviceGen2::create(pLinearBackend);

	RectSPX updateRects[2] = { { 0, 0, c_canvasSize.w * 64, 64 * 20 }, { 0, 64 * 20, c_canvasSize.w * 64, (c_canvasSize.h - 20) * 64 } };

	pLinearDevice->beginRender();
	pLinearDevice->beginCanvasUpdate(CanvasRef::Default, 2, updateRects);
	drawScene(pLinearDevice);
	pLinearDevice->endCanvasUpdate();
	pLinearDevice->endRender();

	for (size_t i = 0; i < softPixels.size(); i++)
	{
		if (softPixels[i] != linearPixels[i])
		{
			int pixel = int(i / 4);
			output << "Pixel " << pixel % c_canvasSize.w << "," << pixel / c_canvasSize.w << " channel " << i % 4
				   << " is " << int(linearPixels[i]) << " with LinearBackend, " << int(softPixels[i]) << " with SoftBackend." << std::endl;
			return false;
		}
	}

	return true;
}

//____ streamTest() ___________________________________________________________
// Shapes streamed through StreamBackend and StreamPlayer must draw exactly as directly.

bool ShapeRenderTest::streamTest(std::ostream& output)
{
	Canvas directCanvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(directCanvas.surface);
	drawScene(pDevice);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto directPixels = directCanvas.read();

	// Through a stream

	Canvas streamedCanvas;

	auto pBackend = SoftBackend::create();
	addDefaultSoftKernels(pBackend);
	pBackend->defineCanvas(CanvasRef::Default, streamedCanvas.surface);

	auto pStreamPlayer = StreamPlayer::create(pBackend, SoftSurfaceFactory::create(), SoftEdgemapFactory::create());
	auto pStreamEncoder = StreamEncoder::create({ pStreamPlayer, pStreamPlayer->input });
	auto pStreamBackend = StreamBackend::create(pStreamEncoder);
	pStreamBackend->defineCanvas(CanvasRef::Default, c_canvasSize, PixelFormat::ARGB_8, 64, ColorSpace::Linear);

	auto pStreamDevice = GfxDeviceGen2::create(pStreamBackend);

	pStreamDevice->beginRender();
	pStreamDevice->beginCanvasUpdate(CanvasRef::Default);
	drawScene(pStreamDevice);
	pStreamDevice->endCanvasUpdate();
	pStreamDevice->endRender();

	auto streamedPixels = streamedCanvas.read();

	for (size_t i = 0; i < directPixels.size(); i++)
	{
		if (directPixels[i] != streamedPixels[i])
		{
			int pixel = int(i / 4);
			output << "Pixel " << pixel % c_canvasSize.w << "," << pixel / c_canvasSize.w << " channel " << i % 4
				   << " is " << int(streamedPixels[i]) << " streamed, " << int(directPixels[i]) << " direct." << std::endl;
			return false;
		}
	}

	return true;
}

//____ alignedBoxTest() _______________________________________________________
// Square boxes aligned to pixels are drawn as fills.

bool ShapeRenderTest::alignedBoxTest(std::ostream& output)
{
	RoundedBoxSpec spec;
	spec.rect = { 64 * 4, 64 * 5, 64 * 30, 64 * 20 };
	spec.outline = { 64 * 2, 64 * 3, 64 * 1, 64 * 4 };
	spec.fillColor = HiColor(3000, 1500, 600, 4096);
	spec.outlineColor = HiColor(200, 4000, 1000, 4096);

	Canvas canvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(canvas.surface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(c_background);
	pDevice->setBlendMode(BlendMode::Blend);
	pDevice->drawRoundedBox(spec);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto pixels = canvas.read();

	RectI outer = spec.rect / 64;
	RectI inner = (spec.rect - spec.outline) / 64;

	return compare(output, "Aligned box", pixels, 1, [&](int x, int y)
	{
		if (inner.contains(CoordI(x, y)))
			return toC12(spec.fillColor);
		if (outer.contains(CoordI(x, y)))
			return toC12(spec.outlineColor);
		return toC12(c_background);
	});
}

//____ boxSkinTest() __________________________________________________________
// BoxSkin with corner radii draws a rounded box and respects the corners when mark testing.

bool ShapeRenderTest::boxSkinTest(std::ostream& output)
{
	auto pSkin = BoxSkin::create(WGBP(BoxSkin,
		_.color = HiColor(3000, 1500, 600, 4096),
		_.outlineColor = HiColor(200, 4000, 1000, 4096),
		_.outlineThickness = { 2, 3, 2, 1 },
		_.cornerRadius = { 8, 8, 3, 0 }));

	RectSPX canvas = { 64 * 5, 64 * 4, 64 * 40, 64 * 30 };
	int scale = 64;

	// Skin

	Canvas skinCanvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(skinCanvas.surface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(c_background);
	pDevice->setBlendMode(BlendMode::Blend);
	pSkin->_render(pDevice, canvas, scale, State::Default);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	// Same box drawn directly

	RoundedBoxSpec spec;
	spec.rect = canvas;
	spec.radius = { 64 * 8, 64 * 8, 64 * 3, 0 };
	spec.outline = { 64 * 2, 64 * 3, 64 * 2, 64 * 1 };
	spec.fillColor = HiColor(3000, 1500, 600, 4096);
	spec.outlineColor = HiColor(200, 4000, 1000, 4096);

	Canvas boxCanvas;

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(boxCanvas.surface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(c_background);
	pDevice->setBlendMode(BlendMode::Blend);
	pDevice->drawRoundedBox(spec);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto skinPixels = skinCanvas.read();
	auto boxPixels = boxCanvas.read();

	TEST_ASSERT(skinPixels == boxPixels);

	// Mark test

	TEST_ASSERT(!pSkin->_markTest({ canvas.x + 64, canvas.y + 64 }, canvas, scale, State::Default));					// Outside top-left corner.
	TEST_ASSERT(pSkin->_markTest({ canvas.x + 64 * 3, canvas.y + 64 * 3 }, canvas, scale, State::Default));				// Inside it.
	TEST_ASSERT(pSkin->_markTest({ canvas.x + 64, canvas.y + canvas.h - 64 }, canvas, scale, State::Default));			// Square bottom-left corner.
	TEST_ASSERT(pSkin->_markTest({ canvas.x + canvas.w / 2, canvas.y + canvas.h / 2 }, canvas, scale, State::Default));

	// Coverage must lie inside the rounded box.

	RectSPX coverage = pSkin->_coverage(canvas, scale, State::Default);
	TEST_ASSERT(!coverage.isEmpty());

	spx radius[4] = { 64 * 8, 64 * 8, 64 * 3, 0 };
	TEST_ASSERT(Util::isInsideRoundedBox(coverage.pos(), canvas, radius));
	TEST_ASSERT(Util::isInsideRoundedBox({ coverage.x + coverage.w - 1, coverage.y }, canvas, radius));
	TEST_ASSERT(Util::isInsideRoundedBox({ coverage.x, coverage.y + coverage.h - 1 }, canvas, radius));
	TEST_ASSERT(Util::isInsideRoundedBox({ coverage.x + coverage.w - 1, coverage.y + coverage.h - 1 }, canvas, radius));

	return true;
}

//____ ringTest() _____________________________________________________________
// Ring with slices, outline, hub and back, against coverage from SoftShapes.

namespace
{
	class SegmentCoverage : public SoftShapes::RowSink
	{
	public:
		SegmentCoverage(int nSegments) : coverage(nSegments, std::vector<int>(c_canvasSize.w * c_canvasSize.h, 0)) {}

		void row(int y, int nSpans, const SoftShapes::Span* pSpans) override
		{
			for (int i = 0; i < nSpans; i++)
			{
				auto& span = pSpans[i];
				for (int x = span.begin; x < span.end; x++)
				{
					int idx = y * c_canvasSize.w + x;
					if (span.segment >= 0)
						coverage[span.segment][idx] += 4096;
					else
					{
						const uint16_t* p = span.pCoverage + (span.bUniform ? 0 : (x - span.begin) * span.nLayers);
						for (int l = 0; l < span.nLayers; l++)
							coverage[span.pLayerSegments[l]][idx] += p[l];
					}
				}
			}
		}

		std::vector<std::vector<int>>	coverage;
	};
}

bool ShapeRenderTest::ringTest(std::ostream& output)
{
	RingSlice slices[5] = { { 0.1f, HiColor(4096, 0, 0, 4096) }, { 0.25f, HiColor(0, 4096, 0, 4096) },
							{ 0.05f, HiColor(0, 0, 4096, 4096) }, { 0.3f, HiColor(4096, 4096, 0, 4096) }, { 0.15f, HiColor(0, 4096, 4096, 4096) } };

	RingSpec ring;
	ring.rect = { 64 * 3 + 21, 64 * 2 + 9, 64 * 50 + 40, 64 * 42 + 10 };
	ring.thickness = 64 * 12 + 30;
	ring.outline = 64 + 20;
	ring.start = 0.9f;
	ring.nSlices = 5;
	ring.pSlices = slices;
	ring.outlineColor = HiColor(1000, 1000, 1000, 4096);
	ring.hubColor = HiColor(4096, 4096, 4096, 4096);
	ring.backColor = HiColor(2000, 0, 2000, 4096);

	// Coverage, converted as GfxDeviceGen2 does.

	uint32_t sizes[5];
	for (int i = 0; i < 5; i++)
		sizes[i] = uint32_t(double(slices[i].size) * 2147483648.0);

	SoftShapes::Ring shape;
	shape.rect = ring.rect;
	shape.thickness = ring.thickness;
	shape.outline = ring.outline;
	shape.start = uint32_t(double(ring.start - std::floor(ring.start)) * 2147483648.0) % 0x80000000;
	shape.nSlices = 5;
	shape.pSliceSizes = sizes;

	SegmentCoverage cov(SoftShapes::c_ringFirstSliceSegment + 5);
	SoftShapes shapes;
	shapes.rasterizeRing(shape, RectI(0, 0, c_canvasSize.w, c_canvasSize.h), &cov);

	// Render

	Canvas canvas;
	auto pDevice = createSoftDevice();

	pDevice->beginRender();
	pDevice->beginCanvasUpdate(canvas.surface);
	pDevice->setBlendMode(BlendMode::Replace);
	pDevice->fill(c_background);
	pDevice->setBlendMode(BlendMode::Blend);
	pDevice->drawRing(ring);
	pDevice->endCanvasUpdate();
	pDevice->endRender();

	auto pixels = canvas.read();

	HiColor segmentColors[8] = { ring.backColor, ring.outlineColor, ring.hubColor, slices[0].color, slices[1].color, slices[2].color, slices[3].color, slices[4].color };
	C12 colors[8];
	for (int i = 0; i < 8; i++)
		colors[i] = toC12(segmentColors[i]);

	C12 bg = toC12(c_background);

	return compare(output, "Ring", pixels, 3, [&](int x, int y)
	{
		int idx = y * c_canvasSize.w + x;
		int total = 0;
		C12 sum = { 0, 0, 0, 0 };

		for (int s = 0; s < 8; s++)
		{
			int c = cov.coverage[s][idx];
			total += c;
			sum.b += colors[s].b * c;
			sum.g += colors[s].g * c;
			sum.r += colors[s].r * c;
		}

		int back = 4096 - total;
		return C12{ (bg.b * back + sum.b) / 4096, (bg.g * back + sum.g) / 4096, (bg.r * back + sum.r) / 4096, 4096 };
	});
}

//____ undefinedColorTest() ___________________________________________________
// Fill, hub and back left undefined are not drawn, also in Replace mode where a
// transparent color is written as is.

bool ShapeRenderTest::undefinedColorTest(std::ostream& output)
{
	RoundedBoxSpec box;
	box.rect = { 64 * 4, 64 * 4, 64 * 40, 64 * 30 };
	box.radius = 64 * 8;
	box.outline = { 64 * 3, 64 * 3, 64 * 3, 64 * 3 };

	RingSlice slice = { 1.f, HiColor(4096, 0, 0, 4096) };

	RingSpec ring;
	ring.rect = { 64 * 4, 64 * 2, 64 * 44, 64 * 44 };
	ring.thickness = 64 * 10;
	ring.nSlices = 1;
	ring.pSlices = &slice;

	struct Probe { const char* what; int x, y; };

	auto render = [&](bool bRing, bool bTransparent)
	{
		Canvas canvas;
		auto pDevice = createSoftDevice();

		RoundedBoxSpec b = box;
		RingSpec r = ring;

		if (bTransparent)
		{
			b.fillColor = HiColor::Transparent;
			r.hubColor = HiColor::Transparent;
			r.backColor = HiColor::Transparent;
		}

		pDevice->beginRender();
		pDevice->beginCanvasUpdate(canvas.surface);
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->fill(c_background);
		if (bRing)
			pDevice->drawRing(r);
		else
			pDevice->drawRoundedBox(b);
		pDevice->endCanvasUpdate();
		pDevice->endRender();

		return canvas.read();
	};

	auto check = [&](const char* shape, bool bRing, std::initializer_list<Probe> probes)
	{
		auto untouched = render(bRing, false);
		auto cleared = render(bRing, true);

		for (auto& p : probes)
		{
			int ofs = (p.y * c_canvasSize.w + p.x) * 4;

			if (std::memcmp(untouched.data() + ofs, untouched.data(), 4) != 0)		// Pixel 0,0 is outside shape.
			{
				output << shape << ": " << p.what << " left undefined was drawn." << std::endl;
				return false;
			}

			if (cleared[ofs + 3] != 0)
			{
				output << shape << ": transparent " << p.what << " was not written in Replace mode." << std::endl;
				return false;
			}
		}
		return true;
	};

	return check("RoundedBox", false, { { "fill", 24, 19 } }) &&
		   check("Ring", true, { { "hub", 26, 24 }, { "back", 5, 3 } });
}
