//
//  GameViewController.m
//  gfxdevice_testapp_metal
//
//  Created by Tord Jansson on 2020-08-07.
//  Copyright © 2020 Nordic UI Technologies AB. All rights reserved.
//

#import "GameViewController.h"
#import "Renderer.h"

@implementation GameViewController
{
    MTKView *_view;

    Renderer *_renderer;
    
    
    
    
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

    _renderer = [[Renderer alloc] initWithMetalKitView:_view];

    [_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

    _view.delegate = _renderer;



    //
    
    auto trackingArea = [[NSTrackingArea alloc] initWithRect:NSMakeRect(0,0, 20000, 20000)

        options: (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow )

        owner:self userInfo:nil];

    [_view addTrackingArea:trackingArea];

    
}

- (void)mouseDown:(NSEvent *)theEvent {
    int x = 0;

}

- (void)mouseUp:(NSEvent *)theEvent {
    int x = 0;

}

- (void)rightMouseDown:(NSEvent *)theEvent {
    int x = 0;

}

- (void)rightMouseUp:(NSEvent *)theEvent {
    int x = 0;

}


- (void)mouseEntered:(NSEvent *)theEvent {
    int x = 0;

}

- (void)mouseExited:(NSEvent *)theEvent {
    int x = 0;

}

- (void)mouseMoved:(NSEvent *)theEvent {
    int x = 0;

}

- (void)mouseDragged:(NSEvent *)theEvent {
    int x = 0;

}


@end
