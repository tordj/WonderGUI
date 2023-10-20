---
title: 'PieMeterSkin'
weight: 40
---


# PieMeterSkin

PieMeterSkin can draw pie charts and donut charts. This is more useful than it sounds since a donut chart can be used for rounded meters or progress indicator in a GUI.





## Properties

| Property  | Description                 |
| --------- | --------------------------- |
| backColor | Color for background of pie-meter. Defaults to transparent. |
| blendMode | BlendMode used for all rendering, including slices, hub and background. |
| defaultSize | Default size for this skin. |
| emptyColor | Color for the "empty" slice of the piechart where there are no slices. Defaults to DarkBlue. |
| gfxPadding | Like contentPadding but for the pie-chart graphics. |
| hubColor | Color for the hole at the center of a donut chart. Defaults to transparent. |
| hubSize | Size of the hole at the center of a donut chart, expressed as a fraction of the radius in the range of 0.0 - 1.0. Defaults to 0.75. Set to 0.0 to create a pie chart. |
| maxPie | Fraction of pie filled to represent value 1.0. |
| minPie | Fraction of pie filled to represent value 0.0. |
| movingSlices | Set to true if slices should move as the pie grows. Otherwise they represent static areas, gradually being filled as the pie grows. |
| rectangular | Set to true to make outside of pie/donut rectangular. |
| rotation | Rotation of pie/donut in degrees. |
| slices | Array of slices for the donut, including their relative sizes and colors. |

Each slice has the following properties:

| Property | Description                                                  |
| -------- | ------------------------------------------------------------ |
| size     | Size of this slice relative to the rest. The size of all slices is summed up to become a total for a full circle pie. |
| minColor | Color of this slice when value of widget is 0.0.             |
| maxColor | Color of this slice when value of widget is 1.0.             |









## Examples

