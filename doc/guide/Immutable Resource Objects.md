### Immutable Resource Objects

Some objects in WonderGUI are *immutable*, meaning that once created they can't be modified. These are all fundamental resources meant to be shared by a large amount of widgets. Therefore, allowing them to change while in use would require expensive book-keeping to make sure any widget affected is updated accordingly.

The immutable resource objects includes the following:

* TextStyles
* All skins (ColorSkin, BoxSkin, BlockSkin etc)
* Layout objects (TextLayout, NumberLayout, PackLayout etc)
* Fonts (not technically true at the moment, but will be refactored)
* Cursors
* CanvasLayers

Although these can be created anytime during the program cycle, the intended use is to create all that will be needed once during program start and have widgets share them. Widgets can swap one for another if needed, but should rarely have new ones created specifically for them.

Some objects, i.e. Surfaces, are *semi-mutable* in that you can change their content (i.e. their pixel values) while their attributes are immutable (e.g. size, pixel format etc). These should be treated with care, understanding that changing the pixel data of a surface will not automatically update any widget or other resource depending on it.

Widgets are never immutable or semi-mutable, they can all be manipulated at all times.

A small example to clarify how resource should be handled:

```c++
*** RIGHT WAY! ***

	// At initialization
	auto pWhiteSkin = ColorSkin::create( Color::White );					
	auto pBlackSkin = ColorSkin::create( Color::Black );
										
	// Later on changing color of a widget.
	pWidget->setSkin( bIsWhite ? pWhiteSkin : pBlackSkin );
	
*** WRONG WAY! ***

	pWidget->setSkin( bIsWhite ? ColorSkin::create( bIsWhite ? Color::White : Color::Black ) );
```

Doing it the wrong way will create and destroy resources repeatedly. The cost in this case is negligible but doing the same in other places could result in the unnecessary creation of a large amount of resources, increasing memory consumption and decreasing performance. 

There is also a hard limit on some resources, e.g. there can be no more than 65535 TextStyle objects at any given time. Creating a new TextStyle for each entry in a table could make you quickly reach this limit.

Creating immutable resources on the fly might be the right thing to do in some cases, but you should always stop and think before doing it.