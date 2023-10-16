---
title: 'States'
weight: 15
---

# Widget states

*The state of a widget controls how it looks and behaves. Understanding what different states means and how they relates to each other will make it easier for you to design widgets that change their appearance as expected when interacted with.*



## States and properties

There is a total of 18 states that a widget can enter, made up of combinations of six binary properties. The properties that determines the state are the following:

* *Enabled/disabled*
* *Focused (having keyboard focus)*
* *Hovered (mouse pointer placed on widget)*
* *Pressed (mouse pressed on widget)*
* *Selected (like a checkbox or entry in a list being selected or not )*
* *Targeted (hovering another dragged widget above the widget, targeted for drop action)*

This could result in 2^5 = 32 different states, but some properties can only exist in combinations. For example, a pressed widget must be hovered as well and it can't be pressed or hovered if it is disabled. So we end up with the following 18 combinations:

* *Default*
* *Focused*
* *Hovered*
* *HoveredFocused*
* *Pressed*
* *PressedFocused*
* *Selected*
* *SelectedFocused*
* *SelectedHovered*
* *SelectedHoveredFocused*
* *SelectedPressed*
* *SelectedPressedFocused*
* *Targeted*
* *TargetedFocused*
* *TargetedSelected*
* *TargetedSelectedFocused*
* *Disabled*
* *DisabledSelected*



## Controlling the state

The state of a widget can not be directly set, only the disabled-property is fully controlled by the developer by calling *setEnabled()*. The developer can also control which widgets can be selected or become drop-targets through the *setSelectable()* and *setDropTarget()* methods and change focus to or from a widget using *grabFocus()/releaseFocus()*. The selected property can also be indirectly controlled since item lists, togglebuttons etc have methods for selecting and unselecting.

The state can be read from the widget using the *state()* method, which returns an object representing the state. This object has methods for reading individual abilities, such as isDisabled() and isFocused() and also methods for setting the same, although that is hardly useful unless you write widget code.



## Where are states used?

Except for being used internally in the widget itself to control its behavior, the states are used when creating Skins and TextStyles. Certain parameters such as a texts color, size and decoration can be specified individually for various states, making it change appearance when interacting with it.



## Understanding state priority

Since there is a total of 18 individual states you can specify the same parameter up to 18 times to exactly control the appearance of each and every state. However, that is an awful lot to specify so you don't want to do that. Instead you want to specify as little as possible and let WonderGUI pick the best appearance for each state. To do that you need to understand how WonderGUI prioritizes states.

### Setting up an example

Let's say we create a TextStyle-object that we want to use on a LineEditor. We want the text to be displayed in dark gray and be highlighted to black when mouse pointer hovers so the user gets an indication that he can click to edit the text. For some reason we also want the text to be red when it has keyboard focus.

We therefore specify the colors of the different states as follows:

* *Default*: dark gray
* *Hovered*: black
* *Focused*: red

This all seems good, but the widget enters more states than these three. When mouse button is pressed it briefly enters the *Pressed* state and once focused it will be in the state *PressedFocused* until the mouse button is released and it becomes *HoveredFocused* until you move the mouse from the widget and it finally is just *Focused*.

If you test this out you discover that you don't get the red color until the mouse pointer has left the widget. Let us explain why.

### Priority by word order

How does WonderGUI decide which color to use for one of these unspecified states? The name of the state gives you a good hint:

*HoveredFocused*

The word "Hovered" comes before "Focused", which means that *Hovered* has higher priority than *Focused* when finding the closest match. WonderGUI will therefore choose to display the state as *Hovered* and not *Focused*.

For a longer state like this:

*SelectedHoveredFocused*

we know that *Selected* has priority over *Hovered* which has priority over *Focused*. So if no color has been specified for this specific state, WonderGUI will try to find a state that contains as many of these words as possible in priority order and will therefore check the following states in order for a color:

1. *SelectedHovered*
2. *SelectedFocused*
3. *Selected*
4. *HoveredFocused*
5. *Hovered*
6. *Focused*
7. *Default*

### Pressed implies Hovered

There is also one more thing you need to know: *Pressed* always implies *Hovered*, so when applying this rule you should think of *Pressed* as <u>*PressedHovered*</u> and *PressedFocused* as *PressedHoveredFocused*. The name has just been shortened for brevity.

### Fixing our example

Lets now revisit our example above and see what colors will be used for the unspecified states *PressedFocused* and *HoveredFocused*:

* State *PressedFocused* is replaced with state *Hovered* which gives black text.
* State *HoveredFocused* is also replaced with state *Hovered* and gives black text.

This is clearly not what we want. When we have focus we want the text to be red, no matter where the mouse pointer is or if a mouse button is pressed or not. We therefore also need to specify the following:

* *HoveredFocused*: red

This also works for *PressedFocus* since *PressedFocused* actually means *PressedHoveredFocused* and we don't have any color specified for neither *PressedHovered* nor just *Pressed* which are the only higher priority substitutes.

### Some words about the order

In this specific example the order of priority has worked to our disadvantage since *Focused* has lowest priority, and we therefore need to intervene and specify two more states. That's why we chose this example, so we could show you how to handle these situations. 

In most cases you will find that the priority order works to your advantage and that you seldom have to specify more than the basic, single word states.