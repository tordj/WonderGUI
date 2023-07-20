### Skins

Skins are a very important part of WonderGUI and defines the look and (to some extent) the feel of a widget. A typical widget doesn't draw anything at all unless you apply a skin to it. All widgets have at least one skin, which is considered the background skin, covering the whole area of the widget. This is enough for most widgets, but some widgets have additional skins that covers different parts of the widget. A slider for example has two skins, one for the background and one for the handle.

Skins are very generic in that the same skin can be applied to widgets of all different kinds, we don't have a specific type of skin for say Buttons, although some skins have features that are only useful for certain categories of widgets. 

There are many kinds of skins to choose from. The most simple skin is just a filled rectangle and is suitable for backgrounds and such. Other skins draws bitmaps, boxes, circles and even pie-charts. More advanced skins change their look depending on the state of the widget (like a pressed Button) or the value of the widget (like the progress of a progress-bar or the rotation of a knob). Skins can be animated and layered to create advanced effects.

Skins are *Immutable Resource Objects* and are therefore created once, never modified and then applied to any number of Widgets.



