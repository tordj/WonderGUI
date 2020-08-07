/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of a platform independent renderer class, which performs Metal setup and per frame rendering
*/

#import "MetalKit/MetalKit.h"

#import "Renderer.h"

#include <wondergui.h>
#include <wg_metalgfxdevice.h>
#include <wg_metalsurfacefactory.h>

#include "device.h"
#include "gfxdevicetester.h"
#include "app.h"

using namespace wg;


//____ MyAppVisitor() ________________________________________________________

class MyAppVisitor : public AppVisitor
{
public:
    MyAppVisitor( RootPanel_p pRoot ) : m_pRoot(pRoot) {}
    
    wg::RootPanel_p    rootPanel() override
    {
        return          m_pRoot;
    }

    int64_t            time() override
    {
        return 1;
    }

    wg::Blob_p       loadBlob(const char* pPath) override;
    wg::Surface_p    loadSurface(const char* pPath, SurfaceFactory* pFactory, int flags = 0) override;

protected:
    RootPanel_p          m_pRoot;

};


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
    wg::MetalSurface_p      m_pMipMapSurface;
    wg::MetalSurface_p      m_pCanvasSurface;
    wg::MetalSurface_p      m_pRGBXSurface;
    wg::MetalSurface_p      m_pLoadedSurface;


    SizeI                m_windowSize;
    RootPanel_p          m_pRoot;

    GfxDeviceTester*     m_pApp;
    MyAppVisitor*        m_pVisitor;
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        mtkView.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
        
        Base::init();

        MetalGfxDevice::setMetalDevice( mtkView.device );
        m_pDevice = MetalGfxDevice::create();

        
        auto pContext = Context::create();
        pContext->setGammaCorrection(true);
        pContext->setGfxDevice(m_pDevice);
        pContext->setSurfaceFactory(MetalSurfaceFactory::create());
        
        Base::setActiveContext(pContext);

    
        m_pRoot = RootPanel::create(m_pDevice);
        
        m_pRoot->setGeo( {0,0,1000,500});
        
        MyAppVisitor * pVisitor = new MyAppVisitor(nullptr);

        auto pLoadedBlob = pVisitor->loadBlob("../resources/splash.png");

        
        auto pSurface = pVisitor->loadSurface("../resources/blaetter.gif", MetalSurfaceFactory::create());
        m_pLoadedSurface = wg_cast<MetalSurface_p>(pSurface);
     
        // Create app and visitor, make any app-specif initialization

        m_pApp = new GfxDeviceTester();
        m_pVisitor = new MyAppVisitor(m_pRoot);

        // Get apps window size before we continue

//        m_windowSize = m_pApp->startWindowSize();

        m_pApp->addTestDevice(new SoftwareDevice());
        m_pApp->addTestDevice(new SoftwareDevice());

        // Initialize the app

        bool bContinue = m_pApp->init( m_pVisitor );

/*
        // Exit the app

        pApp->exit();

        // App specific exit calls

        pApp->destroy_testdevices();

        //

        delete pApp;
        delete pVisitor;
*/
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
    MTLRenderPassDescriptor * pDesc = view.currentRenderPassDescriptor;
    pDesc.colorAttachments[0].loadAction = MTLLoadActionLoad;

    m_pDevice->autopresent(view.currentDrawable);

    m_pDevice->setCanvas( pDesc, {(int)_viewportSize.x,(int)_viewportSize.y}, PixelFormat::BGRA_8_sRGB );
    
    m_pRoot->setGeo( {0,0,(int)_viewportSize.x,(int)_viewportSize.y} );
    
    // Handle system events

//    process_system_events(g_pRoot);

    // Uppdate the app

    /*bContinue =*/ m_pApp->update();

    // Render. We do this outside the app since we might want to
    // handle updated rectangles in a system specific way.

    m_pRoot->addDirtyPatch({0,0,(int)_viewportSize.x,(int)_viewportSize.y});
    
    m_pRoot->render();
}

//____ loadBlob() _________________________________________________________

Blob_p MyAppVisitor::loadBlob(const char* pPath)
{
    FILE* fp;

#ifdef WIN32
    errno_t err = fopen_s(&fp, pPath, "rb");
#else
    fp = fopen(pPath, "rb");
#endif
    if (!fp)
        return 0;

    fseek(fp, 0, SEEK_END);
    int size = (int)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    Blob_p pBlob = Blob::create(size);

    int nRead = (int)fread(pBlob->data(), 1, size, fp);
    fclose(fp);

    if (nRead < size)
        return 0;

    return pBlob;
}

//____ loadSurface() ______________________________________________________

Surface_p MyAppVisitor::loadSurface(const char* pPath, SurfaceFactory* pFactory, int flags)
{
    NSString * nsPath = [[NSString alloc] initWithUTF8String:pPath];
    
    
    NSImage * nsImage = [[NSImage alloc] initWithContentsOfFile:nsPath];
    
    if (!nsImage)
        return nullptr;

    // Dimensions - source image determines context size

    NSImageRep *rep = [[nsImage representations] objectAtIndex:0];
    NSSize imageSize = NSMakeSize(rep.pixelsWide, rep.pixelsHigh);
    
//    NSSize imageSize = nsImage.size;
    NSRect imageRect = NSMakeRect(0, 0, imageSize.width, imageSize.height);

    // Create a context to hold the image data

    CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);

    CGContextRef ctx = CGBitmapContextCreate(NULL,
                                             imageSize.width,
                                             imageSize.height,
                                             8,
                                             0,
                                             colorSpace,
                                             kCGImageAlphaPremultipliedLast);

    // Wrap graphics context

    NSGraphicsContext* gctx = [NSGraphicsContext graphicsContextWithCGContext:ctx flipped:NO];

    // Make our bitmap context current and render the NSImage into it

    [NSGraphicsContext setCurrentContext:gctx];
    [nsImage drawInRect:imageRect];

    
    //

    size_t width = CGBitmapContextGetWidth(ctx);
    size_t height = CGBitmapContextGetHeight(ctx);
    uint32_t* pixel = (uint32_t*)CGBitmapContextGetData(ctx);
    
    int pitch = (int) CGBitmapContextGetBytesPerRow(ctx);
    
    PixelDescription output;
    
    PixelFormat format = Base::activeContext()->gammaCorrection() ? PixelFormat::BGRA_8_sRGB : PixelFormat::BGRA_8_linear;

    output.format = format;
    output.bits = 32;
    output.bIndexed = false;
    output.bLinear = format == PixelFormat::BGRA_8_linear ? true : false;

    output.R_bits = 8;
    output.G_bits = 8;
    output.B_bits = 8;
    output.A_bits = 8;

    output.R_loss = 0;
    output.G_loss = 0;
    output.B_loss = 0;
    output.A_loss = 0;

    output.A_mask = 0xFF000000;
    output.R_mask = 0xFF;
    output.G_mask = 0xFF00;
    output.B_mask = 0xFF0000;

    output.A_shift = 24;
    output.R_shift = 0;
    output.G_shift = 8;
    output.B_shift = 16;

    
    PixelFormat px;

    if (output.A_bits > 0)
        px = PixelFormat::BGRA_8;
    else
        px = PixelFormat::BGR_8;

    if (!pFactory)
        pFactory = Base::activeContext()->surfaceFactory();

    auto pSurface = pFactory->createSurface(SizeI(width, height), px, (uint8_t*)pixel, pitch, &output, flags);

    // Clean up

    [NSGraphicsContext setCurrentContext:nil];
    CGContextRelease(ctx);
    CGColorSpaceRelease(colorSpace);



    return pSurface;
 
}


@end
