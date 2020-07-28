/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of a platform independent renderer class, which performs Metal setup and per frame rendering
*/

#import "simd/simd.h"
#import "MetalKit/MetalKit.h"

#import "Renderer.h"

// Header shared between C code here, which executes Metal API commands, and .metal files, which
// uses these types as inputs to the shaders.
#import "ShaderTypes.h"

#include <wondergui.h>
#include <wg_metalgfxdevice.h>


using namespace wg;

// Main class performing the rendering
@implementation Renderer
{
    id<MTLDevice> _device;

    // The render pipeline generated from the vertex and fragment shaders in the .metal shader file.
    id<MTLRenderPipelineState> _pipelineState;

    // The command queue used to pass commands to the device.
    id<MTLCommandQueue> _commandQueue;

    // The current size of the view, used as an input to the vertex shader.
    vector_uint2 _viewportSize;
    
    wg::MetalGfxDevice_p    m_pDevice;
    wg::MetalSurface_p      m_pSurface;
    wg::MetalSurface_p      m_pClutSurface;
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        Base::init();
    
        auto pContext = Context::create();
        pContext->setGammaCorrection(false);
        
        Base::setActiveContext(pContext);

    
        
        MetalGfxDevice::setMetalDevice( mtkView.device );
        m_pDevice = MetalGfxDevice::create( SizeI(512,512), mtkView.currentRenderPassDescriptor );
 
        Color myTexture[16] {   Color::Red, Color::Green, Color::Blue, Color::White,
                                Color::White, Color::Black, Color::White, Color::Black,
                                Color::Black, Color::White, Color::Black, Color::White,
                                Color::Yellow, Color::Pink, Color::Brown, Color::AntiqueWhite };
        
        m_pSurface = MetalSurface::create({4,4}, PixelFormat::BGRA_8, (uint8_t*) myTexture, 16);
 
        uint8_t myIndexedTexture[16] { 1, 0, 1, 1,
                                1, 0, 2, 1,
                                1, 3, 4, 1,
                                1, 1, 0, 1 };

        Color   myClut[256] = { Color::Transparent, Color::Yellow, Color::Red, Color::Green, Color::Blue };
        
        m_pClutSurface = MetalSurface::create({4,4}, PixelFormat::CLUT_8, myIndexedTexture, 4, nullptr, SurfaceFlag::Static, myClut);
        m_pClutSurface->setScaleMode(ScaleMode::Nearest);
    }

    return self;
}

/// Called whenever view changes orientation or is resized
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    // Save the size of the drawable to pass to the vertex shader.
    _viewportSize.x = size.width;
    _viewportSize.y = size.height;
}

/// Called whenever the view needs to render a frame.
- (void)drawInMTKView:(nonnull MTKView *)view
{
    m_pDevice->setRenderPassDescriptor(view.currentRenderPassDescriptor);
    m_pDevice->setDrawableToAutopresent(view.currentDrawable);

    m_pDevice->setCanvas( {(int)_viewportSize.x,(int)_viewportSize.y} );
    m_pDevice->beginRender();

    m_pDevice->setTintGradient({10,300,200,200}, Color::Red, Color::Red, Color::Red, Color::Red);
    m_pDevice->clearTintGradient();

    
    m_pDevice->fill( RectI(10,10,100,100), Color::GreenYellow );
    m_pDevice->fill( RectI(50,50,100,100), Color::Blue );
    m_pDevice->fill( RectI(90,90,100,100), Color::Red );
    m_pDevice->fill( RectI(130,130,100,100), Color::White );

    m_pDevice->setTintGradient({10,300,100,100}, Color::Red, Color::Blue, Color::Blue, Color::Red);
    m_pDevice->fill( RectI(10,300,100,100), Color::White );

    m_pDevice->setTintGradient({120,300,100,100}, Color::Green, Color::Green, Color::White, Color::White);
    m_pDevice->fill( RectI(120,300,100,100), Color::White );

    m_pDevice->setTintGradient({230,300,100,100}, Color::Red, Color::Red, Color::Yellow, Color::Yellow);
    m_pDevice->fill( RectI(230,300,100,100), Color::White );

    m_pDevice->clearTintGradient();

    
    m_pDevice->setBlitSource(m_pClutSurface);
//    m_pDevice->blit({1,1});


    m_pDevice->stretchBlit({300,10,128,128});

    m_pDevice->setTintGradient({300,140,128,128}, Color::Black, Color::Black, Color::White, Color::White);
    m_pDevice->stretchBlit({300,140,128,128});
    m_pDevice->clearTintGradient();

    m_pDevice->fill( RectF(200.5f,2.25f,20.f,10.f), Color::White);
    
    CoordI plotCoords[5] { {1,1}, {2,2}, {4,4}, {6,6}, {8,8} };
    Color  plotColors[5] { Color::White, Color::White, Color::White, Color::White, Color::White };

    m_pDevice->plotPixels(5, plotCoords, plotColors);
    
    m_pDevice->drawLine( CoordI(20,20), CoordI(400,40), {255,255,255,255}, 5.f );
    
    m_pDevice->fill( RectI(200,200,100,100), Color::DarkBlue );
    m_pDevice->drawElipse( {200,200,100,100}, 20.f, Color::Red, 4.f, Color::White );
    
    m_pDevice->endRender();
    
}

@end
