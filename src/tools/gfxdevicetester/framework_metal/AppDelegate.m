#import "AppDelegate.h"


@implementation MyApplicationDelegate : NSObject
- (id)init {
    if (self = [super init]) {
        // allocate and initialize window and stuff here ..
    }
    return self;
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    [window makeKeyAndOrderFront:self];
}

- (void)dealloc {
//    [window release];
//    [super dealloc];
}

@end
