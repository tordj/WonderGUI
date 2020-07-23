//
//  Renderer.h
//  metal_workbench
//
//  Created by Tord Jansson on 2020-07-16.
//  Copyright © 2020 Tord Jansson. All rights reserved.
//

#import <MetalKit/MetalKit.h>

// Our platform independent renderer class.   Implements the MTKViewDelegate protocol which
//   allows it to accept per-frame update and drawable resize callbacks.
@interface Renderer : NSObject <MTKViewDelegate>

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view;

@end

