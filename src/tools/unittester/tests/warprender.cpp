#include "warprender.h"

#include <wondergui.h>
#include <wg_gfxdevice_gen2.h>
#include <wg_softbackend.h>
#include <wg_linearbackend.h>
#include <wg_referencebackend.h>
#include <wg_softsurface.h>
#include <wg_softedgemap.h>
#include <wg_softedgemapfactory.h>
#include <wg_softwarpmap.h>
#include <wg_softwarpmapfactory.h>
#include <wg_softkernels_default.h>
#include <wg_circularwarp.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

using namespace wg;

/*
	Draws through Warpmaps with GfxDeviceGen2 and the software backends:

	- A Warpmap that maps its rectangle straight onto the source draws as drawEdgemap() and blit().
	- SoftBackend estimates coverage from the mapping, ReferenceBackend measures it by supersampling.
	  They are compared with a tolerance, allowing for a few pixels where edges have sharp corners.
	- LinearBackend draws exactly as SoftBackend.

	Canvases are ARGB_8 in linear color space.
*/

namespace
{
	const SizeI c_canvasSize = { 160, 96 };

	const HiColor c_background = HiColor(800, 1500, 2600, 4096);

	enum class Backend
	{
		Soft,
		Linear,
		Reference
	};

	using Scene = std::function<void(GfxDevice* pDevice)>;
	using Pixels = std::vector<uint8_t>;

	//____ render() ___________________________________________________________
	// Renders a scene on a cleared canvas. LinearBackend renders in two update rectangles.

	Pixels render(Backend backend, const Scene& scene, PixelFormat canvasFormat = PixelFormat::ARGB_8)
	{
		Pixels pixels(c_canvasSize.w * c_canvasSize.h * 4, 0);

		if (backend == Backend::Linear)
		{
			std::vector<uint8_t> buffer(c_canvasSize.w * c_canvasSize.h * 4 * 2);

			auto pBackend = LinearBackend::create(
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
							std::memcpy(pixels.data() + ((seg.rect.y + y) * c_canvasSize.w + seg.rect.x) * 4, seg.pBuffer + y * seg.pitch, seg.rect.w * 4);
					}
				});

			addDefaultSoftKernels(pBackend);
			pBackend->defineCanvas(CanvasRef::Default, c_canvasSize * 64, PixelFormat::ARGB_8, 64, ColorSpace::Linear);

			auto pDevice = GfxDeviceGen2::create(pBackend);

			RectSPX updateRects[2] = { { 0, 0, c_canvasSize.w * 64, 64 * 37 }, { 0, 64 * 37, c_canvasSize.w * 64, (c_canvasSize.h - 37) * 64 } };

			pDevice->beginRender();
			pDevice->beginCanvasUpdate(CanvasRef::Default, 2, updateRects);
			pDevice->fill(c_background);
			scene(pDevice);
			pDevice->endCanvasUpdate();
			pDevice->endRender();
			return pixels;
		}

		GfxBackend_p pBackend;

		if (backend == Backend::Soft)
		{
			auto pSoft = SoftBackend::create();
			addDefaultSoftKernels(pSoft);
			pBackend = pSoft;
		}
		else
			pBackend = ReferenceBackend::create();

		auto pDevice = GfxDeviceGen2::create(pBackend);
		auto pCanvas = SoftSurface::create(WGBP(Surface, _.size = c_canvasSize, _.format = canvasFormat, _.colorSpace = ColorSpace::Linear, _.canvas = true));

		pDevice->beginRender();
		pDevice->beginCanvasUpdate(pCanvas);
		pDevice->fill(c_background);
		scene(pDevice);
		pDevice->endCanvasUpdate();
		pDevice->endRender();

		auto buffer = pCanvas->allocPixelBuffer();
		pCanvas->pushPixels(buffer);

		for (int y = 0; y < c_canvasSize.h; y++)
			std::memcpy(pixels.data() + y * c_canvasSize.w * 4, buffer.pixels + y * buffer.pitch, c_canvasSize.w * 4);

		if (canvasFormat == PixelFormat::XRGB_8)
		{
			for (size_t i = 3; i < pixels.size(); i += 4)
				pixels[i] = 255;
		}

		pCanvas->freePixelBuffer(buffer);
		return pixels;
	}

	//____ compare() __________________________________________________________
	// Compares two renderings. Channels may differ by tolerance, and by up to maxDiff on at most
	// maxOutliers pixels. Colors are compared premultiplied by alpha.

	bool compare(std::ostream& output, const char* pWhat, const Pixels& a, const char* pNameA, const Pixels& b, const char* pNameB,
				 int tolerance, int maxOutliers = 0, int maxDiff = 0)
	{
		int outliers = 0;

		for (int y = 0; y < c_canvasSize.h; y++)
		{
			for (int x = 0; x < c_canvasSize.w; x++)
			{
				int diff = 0;
				int channel = 0;

				// Colors are compared premultiplied, as their precision drops with alpha.

				int alphaA = a[(y * c_canvasSize.w + x) * 4 + 3];
				int alphaB = b[(y * c_canvasSize.w + x) * 4 + 3];

				for (int ch = 0; ch < 4; ch++)
				{
					int i = (y * c_canvasSize.w + x) * 4 + ch;
					int d = (ch == 3) ? std::abs(alphaA - alphaB) : std::abs(int(a[i]) * alphaA / 255 - int(b[i]) * alphaB / 255);
					if (d > diff)
					{
						diff = d;
						channel = ch;
					}
				}

				if (diff <= tolerance)
					continue;

				int i = (y * c_canvasSize.w + x) * 4 + channel;

				if (diff <= maxDiff && ++outliers <= maxOutliers)
					continue;

				int p = (y * c_canvasSize.w + x) * 4;

				output << pWhat << ": pixel " << x << "," << y << " channel " << channel << " (BGRA) is " << int(a[i]) << " with " << pNameA
					   << ", " << int(b[i]) << " with " << pNameB << " (" << outliers << " outliers allowed up to " << maxOutliers << "). BGRA "
					   << int(a[p]) << "," << int(a[p + 1]) << "," << int(a[p + 2]) << "," << int(a[p + 3]) << " vs "
					   << int(b[p]) << "," << int(b[p + 1]) << "," << int(b[p + 2]) << "," << int(b[p + 3]) << "." << std::endl;
				return false;
			}
		}
		return true;
	}

	//____ Resources __________________________________________________________

	// An edgemap with three edges: straight, a slow wave and a faster one.

	Edgemap_p makeEdgemap(bool bTiling, bool bTinted = false)
	{
		const int w = 48, h = 24, nEdges = 3;
		std::vector<spx> samples(nEdges * (w + 1));

		for (int i = 0; i <= w; i++)
		{
			double a = 2 * M_PI * i / w;
			samples[i] = spx(std::lround(h * 0.2 * 64));
			samples[(w + 1) + i] = spx(std::lround(h * (0.5 + 0.15 * std::sin(a * 2)) * 64));
			samples[2 * (w + 1) + i] = spx(std::lround(h * (0.8 + 0.08 * std::sin(a * 4)) * 64));
		}

		HiColor colors[4] = { HiColor(4096, 4096, 4096, 4096), HiColor(400, 1800, 4000, 3500), HiColor(4000, 2000, 300, 4096), HiColor(0, 0, 0, 0) };

		Tint_p tints[4];
		if (bTinted)
		{
			tints[1] = Tint::create(HiColor(4096, 4096, 0, 4096), HiColor(0, 4096, 4096, 2500), { 0.f, 0.f }, { 1.f, 0.f });

			Tint::Blueprint radial;
			radial.shape = TintShape::Radial;
			radial.center = { 0.3f, 0.6f };
			radial.stops = { { 0.f, HiColor(4096, 0, 4096, 4096) }, { 1.f, HiColor(0, 4096, 0, 3000) } };
			tints[2] = Tint::create(radial);
		}

		Edgemap::Blueprint bp;
		bp.size = { w, h };
		bp.segments = 4;
		bp.colors = colors;
		bp.tints = bTinted ? tints : nullptr;
		bp.tiling = bTiling;

		return SoftEdgemapFactory::create()->createEdgemap(bp, SampleOrigo::Top, samples.data(), nEdges, w + 1);
	}

	// A surface with a pattern that has no two neighbors alike.

	Surface_p makeSurface(bool bTiling, SampleMethod sampleMethod)
	{
		const int w = 32, h = 16;

		auto pSurface = SoftSurface::create(WGBP(Surface, _.format = PixelFormat::ARGB_8, _.sampleMethod = sampleMethod, _.size = { w, h }, _.tiling = bTiling));

		auto buffer = pSurface->allocPixelBuffer();
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				uint8_t* p = buffer.pixels + y * buffer.pitch + x * 4;
				bool bChecker = ((x / 4) + (y / 4)) & 1;
				p[0] = bChecker ? 40 : 230;
				p[1] = uint8_t(x * 8);
				p[2] = uint8_t(255 - y * 14);
				p[3] = uint8_t(255 - ((x * 7 + y * 3) % 5) * 30);
			}
		}
		pSurface->pullPixels(buffer);
		pSurface->freePixelBuffer(buffer);
		return pSurface;
	}

	Warpmap_p makeRing(int size, float sweep = 360.f, float startAngle = 0.f)
	{
		return CircularWarp::create(WGBP(CircularWarp,
											_.size = { size, size },
											_.topRadius = (size / 2 - 2) * 64,
											_.bottomRadius = (size / 4) * 64,
											_.startAngle = startAngle,
											_.sweep = sweep), SoftWarpmapFactory::create())->refresh();
	}

	// A smooth, non-periodic warp of a rectangle, partly outside the source.

	Warpmap_p makeWave(int size)
	{
		auto pWarpmap = SoftWarpmapFactory::create()->createWarpmap(WGBP(Warpmap, _.size = { size, size }, _.cellSize = 8));

		SizeI n = pWarpmap->nodes();
		std::vector<CoordF> nodes(n.w * n.h);

		for (int r = 0; r < n.h; r++)
		{
			for (int c = 0; c < n.w; c++)
			{
				float x = pWarpmap->nodeX(c) / float(size);
				float y = pWarpmap->nodeY(r) / float(size);
				nodes[r * n.w + c] = { (x - 0.5f) * 1.3f + 0.5f + 0.1f * std::sin(y * 9.f), (y - 0.5f) * 1.25f + 0.5f + 0.08f * std::sin(x * 7.f) };
			}
		}

		pWarpmap->importNodes({ 0, 0, n.w, n.h }, nodes.data());
		return pWarpmap;
	}

	// Scene drawing all kinds of warps.

	void warpScene(GfxDevice* pDevice)
	{
		auto pEdgemap = makeEdgemap(true);
		auto pTinted = makeEdgemap(false, true);
		auto pRing = makeRing(44);
		auto pArc = makeRing(44, 270.f, -135.f);
		auto pWave = makeWave(44);

		pDevice->warpDrawEdgemap({ 2 * 64, 2 * 64 }, pRing, pEdgemap);
		pDevice->warpDrawEdgemap({ 50 * 64, 2 * 64 }, pArc, pEdgemap, { 11 * 64, 3 * 64 });
		pDevice->warpDrawEdgemap({ 98 * 64, 2 * 64 }, pWave, pTinted);

		pDevice->setBlitSource(makeSurface(true, SampleMethod::Bilinear));
		pDevice->warpBlit({ 2 * 64, 50 * 64 }, pRing);

		pDevice->setTint({ 50 * 64, 50 * 64, 44 * 64, 44 * 64 }, Tint::create(HiColor(4096, 4096, 4096, 4096), HiColor(4096, 1000, 1000, 2000)));
		pDevice->warpBlit({ 50 * 64, 50 * 64 }, pArc, { 8 * 64, 0, 16 * 64, 16 * 64 }, { 5 * 64, 0 });
		pDevice->clearTint();

		pDevice->setBlitSource(makeSurface(false, SampleMethod::Nearest));
		pDevice->warpBlit({ 98 * 64, 50 * 64 }, pWave);
	}

	// The default Warpmap: straight onto the source.

	Warpmap_p identity(SizeI size, int cellSize)
	{
		return SoftWarpmapFactory::create()->createWarpmap(WGBP(Warpmap, _.size = size, _.cellSize = cellSize));
	}
}

//____ constructor ____________________________________________________________

WarpRenderTest::WarpRenderTest()
{
	ADD_TEST(identityEdgemapTest);
	ADD_TEST(identityBlitTest);
	ADD_TEST(softVsReferenceTest);
	ADD_TEST(blendModesTest);
	ADD_TEST(linearBackendTest);
	ADD_TEST(offsetPeriodTest);
	ADD_TEST(clipTest);
	ADD_TEST(opaqueCanvasTest);
}

WarpRenderTest::~WarpRenderTest()
{
}

//____ identityEdgemapTest() __________________________________________________
// A Warpmap mapping straight onto an edgemap draws it as drawEdgemap() does.

bool WarpRenderTest::identityEdgemapTest(std::ostream& output)
{
	auto pEdgemap = makeEdgemap(false, true);
	auto pWarpmap = identity(pEdgemap->pixelSize(), 7);

	auto direct = [&](GfxDevice* pDevice)
	{
		pDevice->drawEdgemap({ 5 * 64, 9 * 64 }, pEdgemap);
		pDevice->setTint({ 0, 0, 160 * 64, 96 * 64 }, Tint::create(HiColor(4096, 4096, 4096, 4096), HiColor(1000, 4096, 2000, 3000), { 0.f, 0.f }, { 1.f, 1.f }));
		pDevice->drawEdgemap({ 70 * 64 + 20, 40 * 64 - 10 }, pEdgemap);
	};

	auto warped = [&](GfxDevice* pDevice)
	{
		pDevice->warpDrawEdgemap({ 5 * 64, 9 * 64 }, pWarpmap, pEdgemap);
		pDevice->setTint({ 0, 0, 160 * 64, 96 * 64 }, Tint::create(HiColor(4096, 4096, 4096, 4096), HiColor(1000, 4096, 2000, 3000), { 0.f, 0.f }, { 1.f, 1.f }));
		pDevice->warpDrawEdgemap({ 70 * 64 + 20, 40 * 64 - 10 }, pWarpmap, pEdgemap);
	};

	bool bOk = compare(output, "SoftBackend", render(Backend::Soft, warped), "warp", render(Backend::Soft, direct), "drawEdgemap", 2);
	bOk = bOk && compare(output, "ReferenceBackend", render(Backend::Reference, warped), "warp", render(Backend::Reference, direct), "drawEdgemap", 2);

	return bOk;
}

//____ identityBlitTest() _____________________________________________________
// A Warpmap mapping straight onto a surface draws it as blit() does, for both sample methods.

bool WarpRenderTest::identityBlitTest(std::ostream& output)
{
	bool bOk = true;

	for (auto sampleMethod : { SampleMethod::Nearest, SampleMethod::Bilinear })
	{
		auto pSurface = makeSurface(false, sampleMethod);
		auto pWarpmap = identity({ 16, 12 }, 5);

		auto direct = [&](GfxDevice* pDevice)
		{
			pDevice->setBlitSource(pSurface);
			pDevice->blit({ 3 * 64, 4 * 64 }, { 64 * 7, 64 * 2, 64 * 16, 64 * 12 });
			pDevice->setTintColor(HiColor(4096, 2000, 4096, 3000));
			pDevice->blit({ 40 * 64, 50 * 64 }, { 64 * 7, 64 * 2, 64 * 16, 64 * 12 });
		};

		auto warped = [&](GfxDevice* pDevice)
		{
			pDevice->setBlitSource(pSurface);
			pDevice->warpBlit({ 3 * 64, 4 * 64 }, pWarpmap, { 64 * 7, 64 * 2, 64 * 16, 64 * 12 });
			pDevice->setTintColor(HiColor(4096, 2000, 4096, 3000));
			pDevice->warpBlit({ 40 * 64, 50 * 64 }, pWarpmap, { 64 * 7, 64 * 2, 64 * 16, 64 * 12 });
		};

		const char* pWhat = sampleMethod == SampleMethod::Nearest ? "Nearest" : "Bilinear";

		bOk = bOk && compare(output, pWhat, render(Backend::Soft, warped), "SoftBackend warp", render(Backend::Soft, direct), "SoftBackend blit", 1);
		bOk = bOk && compare(output, pWhat, render(Backend::Reference, warped), "ReferenceBackend warp", render(Backend::Reference, direct), "ReferenceBackend blit", 1);
	}

	return bOk;
}

//____ softVsReferenceTest() __________________________________________________
// SoftBackend estimates coverage close to what ReferenceBackend measures. Corners of edges
// and the source within a pixel are where estimates are off the most.

bool WarpRenderTest::softVsReferenceTest(std::ostream& output)
{
	return compare(output, "Warp scene", render(Backend::Soft, warpScene), "SoftBackend", render(Backend::Reference, warpScene), "ReferenceBackend", 6, 60, 48);
}

//____ blendModesTest() _______________________________________________________

bool WarpRenderTest::blendModesTest(std::ostream& output)
{
	const BlendMode modes[] = { BlendMode::Replace, BlendMode::Blend, BlendMode::Add, BlendMode::Subtract, BlendMode::Multiply,
								BlendMode::Invert, BlendMode::Min, BlendMode::Max, BlendMode::Morph, BlendMode::BlendFixedColor };

	auto pEdgemap = makeEdgemap(true);
	auto pRing = makeRing(40);
	auto pWave = makeWave(40);
	auto pSurface = makeSurface(true, SampleMethod::Bilinear);

	for (BlendMode mode : modes)
	{
		// BlendFixedColor blends onto the fixed color in SoftBackend, which gives the same as Blend,
		// as ReferenceBackend draws it, where the canvas is that color.

		const HiColor fixedColor = HiColor(4096, 0, 2000, 4096);

		auto scene = [&](GfxDevice* pDevice)
		{
			if (mode == BlendMode::BlendFixedColor)
				pDevice->fill(fixedColor);

			pDevice->setBlendMode(mode);
			pDevice->setMorphFactor(0.3f);
			pDevice->setFixedBlendColor(fixedColor);

			pDevice->warpDrawEdgemap({ 4 * 64, 4 * 64 }, pRing, pEdgemap);
			pDevice->setBlitSource(pSurface);
			pDevice->warpBlit({ 60 * 64, 4 * 64 }, pWave);
			pDevice->warpBlit({ 110 * 64, 4 * 64 }, pRing);
		};

		std::string what = std::string("BlendMode::") + toString(mode);

		if (!compare(output, what.c_str(), render(Backend::Soft, scene), "SoftBackend", render(Backend::Reference, scene), "ReferenceBackend", 6, 40, 48))
			return false;
	}

	return true;
}

//____ linearBackendTest() ____________________________________________________
// LinearBackend draws warps exactly as SoftBackend, also across update rectangles.

bool WarpRenderTest::linearBackendTest(std::ostream& output)
{
	return compare(output, "Warp scene", render(Backend::Linear, warpScene), "LinearBackend", render(Backend::Soft, warpScene), "SoftBackend", 0);
}

//____ offsetPeriodTest() _____________________________________________________
// A source offset of a whole period draws the same as none, also for negative offsets.

bool WarpRenderTest::offsetPeriodTest(std::ostream& output)
{
	auto pEdgemap = makeEdgemap(true);
	auto pArc = makeRing(44, 270.f, -135.f);
	auto pSurface = makeSurface(true, SampleMethod::Bilinear);

	// Edgemap is 48 pixels wide, period is 360 / 270 of that: 64 pixels. Source rectangle of the
	// blit is 24 pixels wide, period 32 pixels.

	auto scene = [&](int edgemapOffset, int blitOffset)
	{
		return [=](GfxDevice* pDevice)
		{
			pDevice->warpDrawEdgemap({ 2 * 64, 2 * 64 }, pArc, pEdgemap, { edgemapOffset + 7 * 64, 0 });
			pDevice->setBlitSource(pSurface);
			pDevice->warpBlit({ 60 * 64, 2 * 64 }, pArc, { 4 * 64, 0, 24 * 64, 16 * 64 }, { blitOffset + 3 * 64, 0 });
		};
	};

	auto base = render(Backend::Soft, scene(0, 0));

	return compare(output, "Offset one period", render(Backend::Soft, scene(64 * 64, 32 * 64)), "offset", base, "no offset", 0) &&
		   compare(output, "Offset minus two periods", render(Backend::Soft, scene(-128 * 64, -64 * 64)), "offset", base, "no offset", 0);
}

//____ clipTest() _____________________________________________________________
// Clipped warps draw the same pixels as unclipped within the clip rectangles and nothing outside.

bool WarpRenderTest::clipTest(std::ostream& output)
{
	RectSPX clips[3] = { { 10 * 64, 10 * 64, 30 * 64, 20 * 64 }, { 60 * 64, 5 * 64, 25 * 64, 80 * 64 }, { 100 * 64, 60 * 64, 50 * 64, 20 * 64 } };

	auto clipped = [&](GfxDevice* pDevice)
	{
		pDevice->setClipList(3, clips);
		warpScene(pDevice);
	};

	auto background = render(Backend::Soft, [](GfxDevice*) {});

	for (Backend backend : { Backend::Soft, Backend::Linear })
	{
		auto full = render(backend, warpScene);
		auto pixels = render(backend, clipped);

		// Expected: full scene within clip rectangles, background elsewhere.

		Pixels expected = background;

		for (auto& clip : clips)
		{
			RectI r = clip / 64;
			for (int y = r.y; y < r.y + r.h; y++)
				std::memcpy(expected.data() + (y * c_canvasSize.w + r.x) * 4, full.data() + (y * c_canvasSize.w + r.x) * 4, r.w * 4);
		}

		if (!compare(output, backend == Backend::Soft ? "SoftBackend" : "LinearBackend", pixels, "clipped", expected, "expected", 0))
			return false;
	}

	return true;
}

//____ opaqueCanvasTest() _____________________________________________________
// On a canvas without alpha, paint modes give each part of a pixel the color of what covers it,
// which for Replace is the color of the segment as if opaque, also where a transparent segment
// shares the pixel.

bool WarpRenderTest::opaqueCanvasTest(std::ostream& output)
{
	for (BlendMode mode : { BlendMode::Replace, BlendMode::Blend, BlendMode::Morph })
	{
		auto scene = [&](GfxDevice* pDevice)
		{
			pDevice->setBlendMode(mode);
			pDevice->setMorphFactor(0.6f);
			warpScene(pDevice);
		};

		std::string what = std::string("XRGB_8 canvas, BlendMode::") + toString(mode);

		if (!compare(output, what.c_str(), render(Backend::Soft, scene, PixelFormat::XRGB_8), "SoftBackend",
					 render(Backend::Reference, scene, PixelFormat::XRGB_8), "ReferenceBackend", 6, 60, 48))
			return false;
	}

	return true;
}
