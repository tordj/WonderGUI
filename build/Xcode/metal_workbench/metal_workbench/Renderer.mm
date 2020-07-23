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
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        Base::init();
        MetalGfxDevice::setMetalDevice( mtkView.device );
        m_pDevice = MetalGfxDevice::create( SizeI(512,512), mtkView.currentRenderPassDescriptor );
        
        
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
    
    m_pDevice->fill( RectI(10,10,100,100), Color::GreenYellow );
    
    m_pDevice->endRender();
    
}

@end
