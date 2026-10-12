// Plugin side of the PluginGfxDevice test. Drives a PluginGfxDevice wrapping a
// host GfxDeviceGen2 and checks what it reports back.

#include <wondergui.h>
#include <wonderplugin.h>

#include <iostream>

using namespace wg;

#define DLLEXPORTPREFIX __attribute__ ((visibility("default")))

static int	g_nFails = 0;
static int	g_nChecks = 0;
static int	g_pluginErrors = 0;
static int	(*g_hostErrors)() = nullptr;

#define CHECK(cond) do { g_nChecks++; if(!(cond)) { g_nFails++; std::cout << "  FAIL (plugin) line " << __LINE__ << ": " #cond "\n"; } } while(0)

extern "C" DLLEXPORTPREFIX int runTests( wg_plugin_interface * pInterface, void * pHostBridge, wg_obj hDevice,
										 wg_obj hSurfaceFactory, wg_obj hEdgemapFactory, wg_obj hSurfA, wg_obj hSurfB,
										 int (*hostErrors)() )
{
	if( !PluginBase::init(pInterface, pHostBridge) )
		return -1;

	g_hostErrors = hostErrors;
	Base::setErrorHandler([](Error& e) { g_pluginErrors++; std::cout << "  (plugin error: " << e.message << ")\n"; });

	{
		auto pSF = PluginSurfaceFactory::create(hSurfaceFactory);
		auto pEF = PluginEdgemapFactory::create(hEdgemapFactory);
		auto pDev = PluginGfxDevice::create(hDevice, pSF, pEF);

		Surface_p pA = PluginSurface::create(hSurfA);		// 64x64
		Surface_p pB = PluginSurface::create(hSurfB);		// 16x16

		// Idle state

		CHECK( pDev->canvas().pSurface == nullptr );
		CHECK( pDev->canvas().ref == CanvasRef::None );
		CHECK( pDev->canvasSize() == SizeSPX(0,0) );
		CHECK( pDev->canvasLayers() == nullptr );
		CHECK( !pDev->isRendering() );
		CHECK( pDev->isIdle() );
		CHECK( pDev->blitSource() == nullptr );
		CHECK( pDev->blurbrush() == nullptr );

		CHECK( pDev->beginRender() );
		CHECK( pDev->isRendering() );
		CHECK( !pDev->isIdle() );

		// Update rects outside canvas are refused and leave state untouched

		{
			int errs = g_pluginErrors;
			RectSPX bad = { 0, 0, 65*64, 64 };
			CHECK( !pDev->beginCanvasUpdate(pA, 1, &bad) );
			CHECK( g_pluginErrors == errs + 1 );
			CHECK( pDev->canvas().pSurface == nullptr );
			CHECK( pDev->canvasSize() == SizeSPX(0,0) );
		}

		// Canvas A

		CHECK( pDev->beginCanvasUpdate(pA) );
		CHECK( pDev->canvas().pSurface == pA );
		CHECK( pDev->canvas().ref == CanvasRef::None );
		CHECK( pDev->canvasSize() == SizeSPX(64*64,64*64) );
		CHECK( pDev->canvas().format == PixelFormat::ARGB_8 );
		CHECK( pDev->canvasLayers() == nullptr );

		pDev->setBlendMode(BlendMode::Replace);
		CHECK( pDev->blendMode() == BlendMode::Replace );
		pDev->fill(Color::Red);

		// Morph factor is clamped before it reaches the host

		pDev->setMorphFactor(2.f);
		CHECK( pDev->morphFactor() == 1.f );
		pDev->setMorphFactor(-1.f);
		CHECK( pDev->morphFactor() == 0.f );
		pDev->setMorphFactor(0.25f);
		CHECK( pDev->morphFactor() == 0.25f );

		// Render layer out of bounds is stopped on plugin side

		{
			int pErrs = g_pluginErrors, hErrs = g_hostErrors();
			pDev->setRenderLayer(1);
			CHECK( g_pluginErrors == pErrs + 1 );
			CHECK( g_hostErrors() == hErrs );
			CHECK( pDev->renderLayer() == 0 );
			pDev->setRenderLayer(-1);
			CHECK( g_pluginErrors == pErrs + 2 );
			pDev->setRenderLayer(0);
			CHECK( g_pluginErrors == pErrs + 2 );
			CHECK( pDev->renderLayer() == 0 );
		}

		// Clip lists

		{
			RectSPX c1 = { 64*8, 64*8, 64*16, 64*16 };
			RectSPX c2 = { 64*10, 64*10, 64*4, 64*4 };

			CHECK( pDev->setClipList(1, &c1) );
			CHECK( pDev->clipListSize() == 1 );
			CHECK( pDev->clipBounds() == c1 );
			CHECK( pDev->pushClipList(1, &c2) );
			CHECK( pDev->clipBounds() == c2 );
			CHECK( pDev->popClipList() );
			CHECK( pDev->clipBounds() == c1 );

			int hErrs = g_hostErrors();
			CHECK( !pDev->popClipList() );					// Underflow, refused by host
			CHECK( g_hostErrors() == hErrs + 1 );

			pDev->fill(Color::Green);						// Green square at 8,8 - 24,24
			pDev->resetClipList();
			CHECK( pDev->clipBounds() == RectSPX(0,0,64*64,64*64) );
		}

		// Tint color

		pDev->setTintColor(HiColor(4096,0,0,4096));
		CHECK( pDev->hasTintColor() );
		CHECK( pDev->tintColor() == HiColor(4096,0,0,4096) );
		pDev->clearTintColor();
		CHECK( !pDev->hasTintColor() );

		// Nested canvas B with canvas layers

		{
			auto pLayers = PluginCanvasLayers::create(pDev, { .layers = { { .format = PixelFormat::ARGB_8 } } });
			CHECK( pLayers != nullptr );

			CHECK( pDev->beginCanvasUpdate(pB, 0, nullptr, pLayers) );
			CHECK( pDev->canvas().pSurface == pB );
			CHECK( pDev->canvasSize() == SizeSPX(16*64,16*64) );
			CHECK( pDev->canvasLayers() == pLayers );
			CHECK( pDev->blendMode() == BlendMode::Blend );		// Reset by new canvas update

			pDev->setRenderLayer(1);
			CHECK( pDev->renderLayer() == 1 );

			int pErrs = g_pluginErrors;
			pDev->setRenderLayer(2);
			CHECK( g_pluginErrors == pErrs + 1 );
			CHECK( pDev->renderLayer() == 1 );

			pDev->setRenderLayer(0);
			pDev->setBlendMode(BlendMode::Replace);
			pDev->fill(Color::Blue);

			pDev->endCanvasUpdate();
		}

		CHECK( pDev->canvas().pSurface == pA );
		CHECK( pDev->canvasSize() == SizeSPX(64*64,64*64) );
		CHECK( pDev->canvasLayers() == nullptr );
		CHECK( pDev->blendMode() == BlendMode::Replace );		// Restored by host
		CHECK( pDev->morphFactor() == 0.25f );

		// Blit B into A

		CHECK( pDev->setBlitSource(pB) );
		CHECK( pDev->blitSource() == pB );
		pDev->blit({ 64*40, 64*40 });							// Blue square at 40,40 - 56,56

		// Blurbrush

		auto pBrush = Blurbrush::create( { .size = 64*2 } );
		pDev->setBlurbrush(pBrush);
		CHECK( pDev->blurbrush() == pBrush );
		pDev->setBlurbrush(pBrush);
		CHECK( pDev->blurbrush() == pBrush );
		pDev->setBlurbrush(nullptr);
		CHECK( pDev->blurbrush() == nullptr );
		pDev->setBlurbrush(pBrush);
		CHECK( pDev->blurbrush() == pBrush );

		pDev->endCanvasUpdate();

		CHECK( pDev->canvas().pSurface == nullptr );
		CHECK( pDev->canvasSize() == SizeSPX(0,0) );

		// Canvas by reference (host defines Canvas_1 as surface B)

		CHECK( pDev->beginCanvasUpdate(CanvasRef::Canvas_1) );
		CHECK( pDev->canvas().ref == CanvasRef::Canvas_1 );
		CHECK( pDev->canvas().pSurface == nullptr );
		CHECK( pDev->canvasSize() == SizeSPX(16*64,16*64) );
		pDev->endCanvasUpdate();
		CHECK( pDev->canvas().ref == CanvasRef::None );

		CHECK( pDev->endRender() );
		CHECK( !pDev->isRendering() );
		CHECK( pDev->isIdle() );

		// Leave the brush set on the host when the device goes, so the
		// host can check that our reference to it was released.

		pDev->setBlurbrush(pBrush);
	}

	std::cout << "  Plugin side: " << (g_nChecks - g_nFails) << "/" << g_nChecks << " checks passed\n";

	PluginBase::exit();
	return g_nFails;
}
