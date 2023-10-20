---
title: 'BlockSkin'
weight: 30
---


# BlockSkin

A BlockSkin uses blocks of bitmap graphics in a Surface to represent the different states. One block of graphics (from the same surface) is used for each state specified in the Blueprint. All the blocks have the same size.

The location and size of the first block is specified by the *firstBlock* property in the Blueprint. The rest of the blocks are lined up either vertically or horizontally as specified by *axis* with optionally some spacing specified by the *spacing* property.

## Properties

| Property       | Set in state    | Description                                                  |
| -------------- | -------------- | ------------------------------------------------------------ |
| axis           | no             | Axis along which blocks in surface are line up.              |
| blendMode      | no             | BlendMode to use for all blocks.                             |
| blockless | only | Set in state if state has no block defined. |
| color          | yes   | Tint color for the block.                                    |
| firstBlock     | no             | Rectangle for first block of line of blocks to use. Mandatory. |
| frame          | no             | Border around nine patch center.                             |
| gradient       | no             | Tint gradient for all blocks.                                |
| rigidPartX     | no             | Specifies an optional vertical column that doesn't stretch along X-axis. |
| rigidPartY     | no             | Specifies an optional horizontal row that doesn't stretch along Y-axis. |
| spacing        | no             | Spacing between blocks in the surface.                       |
| surface        | no             | Surface containing the blocks.                               |

*RigidPartX* & *rigidPartY* are structs with properties:

| Property | Description                                                  |
| -------- | ------------------------------------------------------------ |
| begin    | offset along the axis in points for start of rigid section.  |
| length   | length in points of rigid section.                           |
| sections | What sections of nine patch that should be rigid. For rigidPartX the alternatives are: Top, Center, Bottom or All. For rigidPartY the alternatives are Left, Center, Right and All. |



## NinePatches

The blocks in a BlockSkin are so called "NinePatches" or "9-patches" (commonly used in Android UI interfaces among other places), consisting of 9 sections which allows us to resize them without stretching the corners and only stretching the sides in one dimension. 

When stretching a NinePatch image, the corners are just moved without any stretching. The sides are just stretched in one dimension to connect the corners and the center does the rest of the stretching.

This makes nine-patches very suitable for resizing rectangular shapes with a single-colored center since there will be none or minor artifacts from stretching.

In WonderGUI we simply define the size of the four sections with a Border called *frame* in the Blueprint. The frame is set to mark the center patch and the rest of the patches can be deducted from that:

![9-patch](/docs/manual/9-patch.svg)

## Reusing blocks between states

Sometimes you want use the same block in two or more states, but still needs to specify state-specific data. This can be done by setting the property *blockless* to true. The specified state will the not be counted when configuring the blocks.

As an example, imagine that we have an image with only two blocks: one for Default and one for Pressed. We want to use the Default block for states Hovered and Disabled but with some color tinting for effect. The relevant part of the blueprint could look something like this: 

```c++
.states = { State::Default, 
          { State::Hovered, { .blockless = true, .color = Color::Yellow }},
          { State::Pressed, 
          { State::Disabled, { .blockless = true, .color = Color::DarkGrey }}}
```



## Rigid parts

Sometimes you have a part of the NinePatch besides the corners that you don't want to stretch. For this we can define a rigid part along the X- and/or Y-axis.

Consider the following block and imagine that you want the cross in the center to remain the same size no matter how the skin is stretched:

[image]

The solution to this is to define a section along the Y-axis that is not stretched horizontally and a section along the X-axis that is not stretched vertically, like this:

[image]

The section specified may only cover the parts normally stretching along that axis, which means Top, Center and Bottom for the X-axis and Left, Center and Right for the Y-axis.



In this case the sections both starts 4 points along their axis and have a length of 5 points, so the relevant code in the blueprint could look something like this:

```c++
.rigidPartX = { .begin = 4, .length = 5, .sections = YSections::Center }
.rigidPartY = { .begin = 4, .length = 5, .sections = XSections::Center }
```

Now the cross won't stretch, but the area around will. We could set sections to All instead of Center and it would look the same, but limiting the rigid part to as few sections as possible will speed up rendering since the block will be split into less sections before rendering.

A more typical example might be a tooltip in the shape of a speech bubble:

[image]

In this case we only need to define a rigid section along the X-axis and set sections to Bottom:

```c++
.rigidPartX = { .begin = 4, .length = 5, .sections = YSections::Bottom }
```



## Examples

In this example we have four blocks representing the states Default, Hovered, Pressed and Disabled of a simple button. Each block is 10x10 pixels, they are arranged horizontally and there is no spacing between them. The border around the center piece of the NinePatch is 4 pixels all around.

[image]

In the code we define the block as follows:

```c++

auto pButtonSkin = BlockSkin::create( { 
    .axis = Axis::X,
    .firstBlock = {0,0,10,10},
    .frame = 4,
    .states ={ State::Default, State::Hovered, 
               State::Pressed, State::Disabled }
    .surface = pSurf,

});
```

As you can see we can just specify the states without any curly brackets or data when there is no state-specific data. We need to specify the same amount of states as we have blocks and in the right order.

When stretching this graphics and changing states we now get the desired result:

[animated gif]

However, there is one more thing we want to do before we are done. If we add some text to the button we get the following effect:

[animated gif]

As you can see, the skin has a nice 3D-effect when pressed, but the text remains static, which looks weird. We solve this by adding some contentShifting to the pressed state:

```c++
auto pButtonSkin = BlockSkin::create( { 
    .axis = Axis::X,
    .firstBlock = {0,0,10,10},
    .frame = 4,
    .states ={ State::Default, State::Hovered, 
              { State::Pressed, { .contentShift = {2,2} }, State::Disabled }
    .surface = pSurf,
});
```

And finally we have a good looking button:

[animated gif]

