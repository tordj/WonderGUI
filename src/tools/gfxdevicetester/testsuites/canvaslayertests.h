#include "testsuite.h"

#include <wg_canvaslayers.h>

class CanvasLayerTests : public TestSuite
{
public:
	CanvasLayerTests()
	{
		name = "CanvasLayerTests";

		addTest("CanvasLayerTest1", &CanvasLayerTests::prep1, &CanvasLayerTests::test1, &CanvasLayerTests::cleanup);
//		addTest("BlendModeUndefined", &BlitBlendTests::setUndefined, &BlitBlendTests::blit, &BlitBlendTests::cleanup);
	}


	bool init(GfxDevice * pDevice, const RectI& canvas, AppVisitor * pAppVisitor)
	{
		m_pOne = pAppVisitor->loadSurface("resources/one.png", pDevice->surfaceFactory(), { .sampleMethod = SampleMethod::Nearest } );
		m_pTwo = pAppVisitor->loadSurface("resources/two.png", pDevice->surfaceFactory(), { .sampleMethod = SampleMethod::Nearest } );
		m_pThree = pAppVisitor->loadSurface("resources/three.png", pDevice->surfaceFactory(), { .sampleMethod = SampleMethod::Nearest } );
		if (!m_pOne || !m_pTwo || !m_pThree)
			return false;

		m_pBackground = pAppVisitor->loadSurface("resources/What-Goes-Up-3.bmp", pDevice->surfaceFactory(), { .sampleMethod = SampleMethod::Nearest } );
		if (!m_pBackground)
			return false;

		m_pCanvas = pDevice->surfaceFactory()->createSurface( WGBP(Surface,
																   _.size = canvas/64,
																   _.format = PixelFormat::BGRA_8,
																   _.canvas = true
																   ));
		
		
		m_pLayerStack1 = CanvasLayers::create( WGBP(CanvasLayers,
								   _.baseLayer = 2,
								   _.layers = { CanvasLayers::LayerBP(PixelFormat::A_8, [](GfxDevice * pDevice) { pDevice->setBlendMode(BlendMode::Subtract); pDevice->blit({0,0}); }),
												CanvasLayers::LayerBP(PixelFormat::BGRA_8, [](GfxDevice * pDevice) { pDevice->setBlendMode(BlendMode::Blend); pDevice->blit({0,0}); }),
												CanvasLayers::LayerBP(PixelFormat::A_8, [](GfxDevice * pDevice) { pDevice->setBlendMode(BlendMode::Add); pDevice->blit({0,0}); }),
												CanvasLayers::LayerBP(PixelFormat::BGRA_8, [](GfxDevice * pDevice) { pDevice->setBlendMode(BlendMode::Blend); pDevice->blit({0,0}); })
		} ) );
		
		return true;
	}

	bool exit(GfxDevice * pDevice, const RectI& canvas)
	{
		m_pCanvas = nullptr;
		m_pLayerStack1 = nullptr;
		m_pOne = nullptr;
		m_pTwo = nullptr;
		m_pThree = nullptr;
		m_pBackground = nullptr;
		return true;
	}


	bool cleanup(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->endCanvasUpdate();

		pDevice->setBlitSource(m_pCanvas);
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->blit({0,0});
		pDevice->setBlendMode(BlendMode::Blend);

		return true;
	}

	bool prep1(GfxDevice * pDevice, const RectI& canvas)
	{
		
		
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->setBlitSource(m_pBackground);
		pDevice->blit({ 0,0 });
		pDevice->setBlendMode(BlendMode::Blend);

		pDevice->beginCanvasUpdate(m_pCanvas, 0, nullptr, m_pLayerStack1 );
		return true;
	}


	
	
	
	bool test1(GfxDevice * pDevice, const RectI& canvas)
	{
		pDevice->setRenderLayer(1);
		pDevice->setBlitSource(m_pOne);
		pDevice->setTintColor(HiColor(1.f, 1.f, 1.f,0.25f) );
		pDevice->stretchBlit(canvas);

		pDevice->setRenderLayer(2);
		pDevice->setBlitSource(m_pTwo);
		pDevice->setTintColor(HiColor(0.5f, 0.5f, 0.5f,1.f) );
		pDevice->blit( {canvas.w/2,canvas.h/2} );

		pDevice->setRenderLayer(3);
		pDevice->setTintColor(HiColor::White);
		pDevice->fill( {0,0,canvas.w/2+m_pTwo->pixelWidth()*64/2,canvas.h}, HiColor(4096,4096,4096,4096) );
		
		pDevice->setRenderLayer(4);
		pDevice->setBlitSource(m_pThree);
		pDevice->setTintColor(Color8::Blue);
		pDevice->blit( {canvas.w/2,0 } );
		return true;

	}


	Surface_p	m_pCanvas;
	Surface_p	m_pBackground;
	Surface_p	m_pOne, m_pTwo, m_pThree;
	CanvasLayers_p	m_pLayerStack1;

};
