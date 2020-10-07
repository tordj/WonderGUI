//
//  GameViewController.m
//  gfxdevice_testapp_metal
//
//  Created by Tord Jansson on 2020-08-07.
//  Copyright © 2020 Nordic UI Technologies AB. All rights reserved.
//

#import "GameViewController.h"
#import "Renderer.h"

#include <wondergui.h>
#include <wg_metalgfxdevice.h>
#include <wg_metalsurfacefactory.h>

#include "device.h"
#include "gfxdevicetester.h"
#include "app.h"

using namespace wg;

//____ MetalDevice _______________________________________________________________________

class MetalDevice : public Device
{
public:
    MetalDevice()
    {
    }

    const char * name() const
    {
        return m_pName;
    }

    bool init(SizeI canvasSize, PixelFormat canvasFormat)
    {
        m_pCanvas = MetalSurface::create(canvasSize, canvasFormat, SurfaceFlag::Canvas);
        m_pDevice = MetalGfxDevice::create();              // Needs a UBO binding point separate from other GfxDevice.
        m_pDevice->setCanvas(m_pCanvas);
        return true;
    }

    void exit()
    {
        m_pDevice = nullptr;
        m_pCanvas = nullptr;
    }

    GfxDevice_p beginRender() const
    {
        m_pDevice->beginRender();
        return m_pDevice;
    }

    void endRender() const
    {
        m_pDevice->endRender();
    }

    GfxDevice_p    gfxDevice() const
    {
        return m_pDevice;
    }

    Surface_p canvas() const
    {
        return m_pCanvas;
    }

private:

    const char * m_pName = { "Metal" };

    MetalGfxDevice_p    m_pDevice;
    MetalSurface_p        m_pCanvas;
};



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



@implementation GameViewController
{
    MTKView *_view;

    Renderer *_renderer;
    
    RootPanel_p             m_pRoot;

    GfxDeviceTester*        m_pApp;
    MyAppVisitor*           m_pVisitor;

    wg::MetalGfxDevice_p    m_pDevice;


    SizeI                   m_windowSize;

    
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _view = (MTKView *)self.view;

    _view.device = MTLCreateSystemDefaultDevice();

    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        self.view = [[NSView alloc] initWithFrame:self.view.frame];
        return;
    }

            Base::init();

            MetalGfxDevice::setMetalDevice( _view.device );
            m_pDevice = MetalGfxDevice::create();

            
            auto pContext = Context::create();
            pContext->setGammaCorrection(true);
            pContext->setGfxDevice(m_pDevice);
            pContext->setSurfaceFactory(MetalSurfaceFactory::create());
            
            Base::setActiveContext(pContext);

        
            m_pRoot = RootPanel::create(m_pDevice);
            
            m_pRoot->setGeo( {0,0,1000,500});
            
            MyAppVisitor * pVisitor = new MyAppVisitor(nullptr);
         
            // Create app and visitor, make any app-specif initialization

            m_pApp = new GfxDeviceTester();
            m_pVisitor = new MyAppVisitor(m_pRoot);

            // Get apps window size before we continue

    //        m_windowSize = m_pApp->startWindowSize();

            m_pApp->addTestDevice(new SoftwareDevice());
            m_pApp->addTestDevice(new MetalDevice());

            // Initialize the app

            bool bContinue = m_pApp->init( m_pVisitor );

    //
    
    _renderer = [[Renderer alloc] initWithMetalKitView:_view rootPanel:m_pRoot];

    [_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

    _view.delegate = _renderer;



    //
    
    auto trackingArea = [[NSTrackingArea alloc] initWithRect:NSMakeRect(0,0, 20000, 20000)

        options: (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow )

        owner:self userInfo:nil];

    [_view addTrackingArea:trackingArea];

    //
    
    [NSTimer scheduledTimerWithTimeInterval:0.016
    target:self
    selector:@selector(updateApp)
    userInfo:nil
     repeats:YES];
    
}

- (void)updateApp
{
    // Uppdate the app

    /*bContinue =*/ m_pApp->update();

}

- (void)scrollWheel:(NSEvent *)theEvent {
    
    float fDeltaX;
    
    fDeltaX = [theEvent deltaX];

    // Convert to an integer representing the number of scrolls events
    int iDeltaX;
    if(fDeltaX > 0.0f)
        iDeltaX = 1;
    else if(fDeltaX < 0.0f)
        iDeltaX = -1;
    else
        iDeltaX = 0;

    iDeltaX = 1*((int)fDeltaX + iDeltaX);

    
    
    float fDeltaY = [theEvent deltaY];

    // Convert to an integer representing the number of scrolls events
    int iDeltaY;
    if(fDeltaY > 0.0f)
        iDeltaY = 1;
    else if(fDeltaY < 0.0f)
        iDeltaY = -1;
    else
        iDeltaY = 0;

    iDeltaY = 1*((int)fDeltaY + iDeltaY);

    
    
    
    Base::inputHandler()->setWheelRoll(1, {iDeltaX,iDeltaY} );
        
}


- (void)mouseDown:(NSEvent *)theEvent {

    Base::inputHandler()->setButton(MouseButton::Left, true);
}

- (void)mouseUp:(NSEvent *)theEvent {

    Base::inputHandler()->setButton(MouseButton::Left, false);
}

- (void)rightMouseDown:(NSEvent *)theEvent {

    Base::inputHandler()->setButton(MouseButton::Right, true);
}

- (void)rightMouseUp:(NSEvent *)theEvent {

    Base::inputHandler()->setButton(MouseButton::Right, false);
}


- (void)mouseEntered:(NSEvent *)theEvent {

    NSPoint mouseLoc = [_view convertPoint:[theEvent locationInWindow] fromView:nil];
         
         // mouseLoc has bottom, left orientation. GUI needs top, left
         NSRect r = [_view bounds];
         mouseLoc.y = r.size.height - mouseLoc.y;

    //     mouseLoc = pkCocoaAPI->ConvertMousePos(mouseLoc);

        Base::inputHandler()->setPointer(m_pRoot, {(float)mouseLoc.x, (float)mouseLoc.y} );

}

- (void)mouseExited:(NSEvent *)theEvent {

    Base::inputHandler()->setPointer(m_pRoot, { -1, -1 } );

}

- (void)mouseMoved:(NSEvent *)theEvent {
   
    NSPoint mouseLoc = [_view convertPoint:[theEvent locationInWindow] fromView:nil];
     
     // mouseLoc has bottom, left orientation. GUI needs top, left
     NSRect r = [_view bounds];
     mouseLoc.y = r.size.height - mouseLoc.y;

//     mouseLoc = pkCocoaAPI->ConvertMousePos(mouseLoc);

    Base::inputHandler()->setPointer(m_pRoot, {(float)mouseLoc.x, (float)mouseLoc.y} );
}

- (void)mouseDragged:(NSEvent *)theEvent {

     NSPoint mouseLoc = [_view convertPoint:[theEvent locationInWindow] fromView:nil];
             
             // mouseLoc has bottom, left orientation. GUI needs top, left
             NSRect r = [_view bounds];
             mouseLoc.y = r.size.height - mouseLoc.y;

        //     mouseLoc = pkCocoaAPI->ConvertMousePos(mouseLoc);

            Base::inputHandler()->setPointer(m_pRoot, {(float)mouseLoc.x, (float)mouseLoc.y} );

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