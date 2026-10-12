// Host side of the PluginGfxDevice test. Sets up a software GfxDeviceGen2,
// hands it to the test plugin and checks the results.

#include <wondergui.h>
#include <wg_softsurface.h>
#include <wg_softsurfacefactory.h>
#include <wg_softedgemapfactory.h>
#include <wg_softbackend.h>
#include <wg_softkernels_default.h>
#include <wg_gfxdevice_gen2.h>
#include <wg_plugininterface.h>

#include <dlfcn.h>
#include <iostream>

using namespace wg;

typedef int (*RunTestsFunc)( wg_plugin_interface*, void*, wg_obj, wg_obj, wg_obj, wg_obj, wg_obj, int(*)() );

static int g_hostErrors = 0;
static int g_nFails = 0;

#define CHECK(cond) do { if(!(cond)) { g_nFails++; std::cout << "  FAIL (host) line " << __LINE__ << ": " #cond "\n"; } } while(0)

static uint32_t pixel( Surface * pSurf, int x, int y )
{
	auto buf = pSurf->allocPixelBuffer();
	uint32_t pix = * (uint32_t*) (buf.pixels + y*buf.pitch + x*4);
	pSurf->freePixelBuffer(buf);
	return pix & 0xFFFFFF;
}

int main( int argc, char * argv[] )
{
	if( argc < 2 )
	{
		std::cout << "Usage: plugingfxdevicetest <path to plugin>\n";
		return -1;
	}

	Base::init(nullptr);
	Base::setErrorHandler([](Error& e) { g_hostErrors++; std::cout << "  (host error: " << e.message << ")\n"; });

	int result;
	{
		auto pBackend = SoftBackend::create();
		addDefaultSoftKernels(pBackend);
		auto pDevice = GfxDeviceGen2::create(pBackend);

		auto pSurfaceFactory = SoftSurfaceFactory::create();
		auto pEdgemapFactory = SoftEdgemapFactory::create();

		auto pA = SoftSurface::create({ .canvas = true, .format = PixelFormat::ARGB_8, .size = {64,64} });
		auto pB = SoftSurface::create({ .canvas = true, .format = PixelFormat::ARGB_8, .size = {16,16} });

		pBackend->defineCanvas(CanvasRef::Canvas_1, pB);

		void * hLib = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
		if( !hLib )
		{
			std::cout << "Failed to load plugin: " << dlerror() << "\n";
			return -1;
		}
		auto runTests = (RunTestsFunc) dlsym(hLib, "runTests");

		wg_plugin_interface calls;
		wg_populatePluginInterface(&calls);

		int pluginFails = runTests(&calls, Base::hostBridge(), static_cast<Object*>(pDevice.rawPtr()),
								   static_cast<Object*>(pSurfaceFactory.rawPtr()), static_cast<Object*>(pEdgemapFactory.rawPtr()),
								   static_cast<Object*>(pA.rawPtr()), static_cast<Object*>(pB.rawPtr()),
								   []() { return g_hostErrors; } );

		// Pixels rendered through the plugin

		CHECK( pixel(pA, 2, 2) == 0xFF0000 );			// Red fill
		CHECK( pixel(pA, 63, 63) == 0xFF0000 );
		CHECK( pixel(pA, 8, 8) == 0x008000 );			// Green, clipped fill
		CHECK( pixel(pA, 23, 23) == 0x008000 );
		CHECK( pixel(pA, 24, 24) == 0xFF0000 );
		CHECK( pixel(pA, 40, 40) == 0x0000FF );			// Blue, blitted from B
		CHECK( pixel(pA, 55, 55) == 0x0000FF );
		CHECK( pixel(pA, 56, 56) == 0xFF0000 );
		CHECK( pixel(pB, 0, 0) == 0x0000FF );

		// The plugin device is gone, so the host blurbrush it created should
		// only be referenced by our device (and pHostBrush) now.

		Blurbrush_p pHostBrush = pDevice->blurbrush();
		CHECK( pHostBrush != nullptr );
		CHECK( pHostBrush && pHostBrush->size() == 64*2 );
		CHECK( pHostBrush && pHostBrush->refcount() == 2 );
		pHostBrush = nullptr;

		// Plugin device released its reference to our device.

		CHECK( pDevice->refcount() == 1 );

		std::cout << "  Plugin side failures: " << pluginFails << ", host side failures: " << g_nFails << "\n";
		result = pluginFails + g_nFails;

		dlclose(hLib);
	}

	Base::exit();

	std::cout << (result == 0 ? "ALL PASSED\n" : "FAILED\n");
	return result;
}
