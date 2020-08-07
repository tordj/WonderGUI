//
//  Renderer.h
//  gfxdevice_testapp_metal
//
//  Created by Tord Jansson on 2020-08-07.
//  Copyright © 2020 Nordic UI Technologies AB. All rights reserved.
//

#import <MetalKit/MetalKit.h>

#include <wondergui.h>

// Our platform independent renderer class.   Implements the MTKViewDelegate protocol which
//   allows it to accept per-frame update and drawable resize callbacks.
@interface Renderer : NSObject <MTKViewDelegate>

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view rootPanel:(wg::RootPanel_p)pRootPanel;

@end

