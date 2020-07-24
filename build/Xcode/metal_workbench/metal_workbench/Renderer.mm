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
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        Base::init();
        MetalGfxDevice::setMetalDevice( mtkView.device );
        m_pDevice = MetalGfxDevice::create( SizeI(512,512), mtkView.currentRenderPassDescriptor );
 
        Color myTexture[16] {   Color::Red, Color::Green, Color::Blue, Color::White,
                                Color::White, Color::Black, Color::White, Color::Black,
                                Color::Black, Color::White, Color::Black, Color::White,
                                Color::Yellow, Color::Pink, Color::Brown, Color::AntiqueWhite };
        
        m_pSurface = MetalSurface::create({4,4}, PixelFormat::BGRA_8, (uint8_t*) myTexture, 16);
 
        
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

    
    m_pDevice->setBlitSource(m_pSurface);
    m_pDevice->blit({1,1});


    m_pDevice->stretchBlit({300,10,128,128});

    m_pDevice->setTintGradient({300,140,128,128}, Color::Black, Color::Black, Color::White, Color::White);
    m_pDevice->stretchBlit({300,140,128,128});
    m_pDevice->clearTintGradient();

    m_pDevice->fill( RectF(200.5f,2.25f,20.f,10.f), Color::White);
    
    
    m_pDevice->endRender();
    
}

@end
