---
title: 'Blending and transparency'
weight: 25
---

# Blending and transparency

*This section defines how GfxDevice combines what you draw with what is already on the canvas: what each BlendMode does, how transparency and anti-aliasing work, and what to expect when you draw onto transparent surfaces. All graphics backends are meant to follow these definitions. ReferenceBackend implements them as directly as possible and is used to test the other backends against them.*



## Colors

Blending happens in **linear light**, with every channel going from 0.0 to 1.0. Colors given to GfxDevice (HiColor) are sRGB and pixels of sRGB surfaces are converted to linear before blending and back again afterwards. Mixing of colors in Tint gradients is the exception, it happens in sRGB before the result is converted to linear.

Pixels are stored with **straight alpha**, e.g. the color channels are not multiplied with alpha.

- A missing color is white. An Alpha_8 surface used as a blit source is white with the alpha of the surface.
- A missing alpha is opaque. A surface without alpha (XRGB_8, RGB_565, BGR_565) is always opaque.
- **The color of a fully transparent pixel never matters.** It can't be seen and it doesn't bleed into anything else, whether through blending, filtering or anti-aliasing.

In the formulas below:

| Symbol | Meaning |
|---|---|
| Cb, αb | Color and alpha of the canvas pixel (b as in backdrop). |
| Cs, αs | Color and alpha of what we draw (s as in source), with the tint color and Tint already multiplied in. |
| mix(a, b, t) | a + (b − a) · t |

Color formulas apply to each of the color channels separately. All results are clamped to 0.0 -> 1.0.



## The glass model

Think of a transparent canvas as a perfect, non-reflective sheet of glass that we paint on.

**Paint modes** put paint on the glass. Where paint is semi-transparent, more paint makes the glass more opaque. Blend is the normal way of painting, Replace scrapes off what was there and paints anew.

**Effect modes** don't add any paint. They change the paint that is already on the glass, like shining a light on it (Add), filtering it (Multiply) or inverting it (Invert). Transparent parts of the glass just let the light through, opaque paint catches it fully and semi-transparent paint changes its hue just like opaque paint does, but stays as transparent as it was. The alpha of the canvas is never changed by an effect mode.

In both groups, the alpha of the source always means **how much**: a fully transparent source has no effect at all.



## Paint modes

| BlendMode | out.rgb | out.α |
|---|---|---|
| **Replace** | Cs | αs |
| **Blend** | (Cs · αs + Cb · αb · (1 − αs)) / out.α | αs + αb · (1 − αs) |
| **BlendFixedColor** | As Blend, with the fixed blend color in place of the canvas pixel. | |
| **Morph** | (Cb · αb · (1 − m) + Cs · αs · m) / out.α | αb · (1 − m) + αs · m |
| **Ignore** | Cb | αb |
| **Undefined** | Treated as Blend. | |

**Blend** is Porter-Duff *over*. On an opaque canvas pixel it is the familiar `Cs · αs + Cb · (1 − αs)`. The division matters where the canvas is transparent: drawing onto a transparent surface and blending that surface onto a background gives the same result as drawing directly onto the background.

**Replace** writes the source as it is, alpha included. It can make the canvas transparent.

**Morph** cross-fades from the canvas to the source by the morph factor m, alpha included. It mixes premultiplied colors, so when one side is transparent the other side simply fades out instead of being darkened by the color of the transparent side.



## Effect modes

The alpha of the source is the strength of the effect. It fades the source towards the color that has no effect: black for Add, Subtract, Invert and Max, white for Multiply and Min.

| BlendMode | out.rgb | Meaning |
|---|---|---|
| **Add** | Cb + αs · Cs | Light shining on the paint. |
| **Subtract** | Cb − αs · Cs | Light taken away. |
| **Multiply** | Cb · mix(1, Cs, αs) | A filter. A transparent source is a clear filter. |
| **Invert** | Cb + αs · Cs · (1 − 2 · Cb) | Inverts the canvas, each channel by the amount given by Cs. |
| **Min** | min(Cb, mix(1, Cs, αs)) | Limits the canvas to at most Cs. |
| **Max** | max(Cb, αs · Cs) | Raises the canvas to at least Cs. |

out.α is always αb.

An alpha-only surface with a soft round shape, blitted with Add and a tint color, works as a point light shining on the canvas. Blitted with Min and a dark tint color it caps the brightness like a shadow, with Max and a light tint color it raises the brightness like a fill light.

Since the canvas alpha is kept, an effect mode has no visible effect on fully transparent parts of the canvas. To light up an empty layer, draw the light onto it with Blend and let the layer be blended onto the canvas below with Add (see Layers below).



## Anti-aliasing

Edges of fills, lines and edgemaps are anti-aliased by the exact area of each pixel that is covered. **A partially covered pixel is drawn as if the alpha of the source was multiplied by the coverage.** Coverage and source alpha therefore always behave the same.

Where several segments of an edgemap cover the same pixel, their colors are first averaged into one source color, weighted by the area each segment covers and as premultiplied colors. That color then covers the whole pixel.

Replace and BlendFixedColor don't look at the canvas, so their edges aren't mixed with what was there. The edges of a Replace become partially transparent, the edges of a BlendFixedColor show the fixed blend color.



## Sampling

Bilinear sampling interpolates premultiplied colors, so transparent pixels don't bleed their color into their neighbours. Mipmaps should be generated the same way.

Outside the source of a ClipBlit is transparent. That has no effect in any effect mode and doesn't darken the edges of a bilinear blit.



## Canvases without alpha

A canvas without alpha can't become transparent. Where a result isn't opaque, the pixel that was there shows through, as if the result was blended onto it. That only concerns paint modes, so on such a canvas:

- Replace is the same as Blend. A transparent Replace leaves the canvas as it was.
- Morph is the same as Blend with the alpha of the source multiplied by the morph factor.
- BlendFixedColor with a fixed blend color that isn't opaque is blended onto the canvas.

The anti-aliased edges of a Replace are therefore mixed with what was there, just like those of a Blend.



## Alpha-only canvases

An Alpha_8 canvas is the one exception to the glass model. It has no color, so it is a one-channel image and its alpha is its value.

- **Paint modes** work on alpha as on any other canvas.
- **Effect modes** would do nothing if they kept alpha. Instead they work on alpha with the alpha of the source as the value: Add grows the mask, Subtract cuts holes in it, Multiply and Min intersect and Max unites.

Coverage fades the value of the source towards the value that has no effect, 1.0 for Multiply and Min and 0.0 for the others, just like alpha does on canvases with color. Paint modes multiply the alpha of the source with the coverage as usual.

| BlendMode | out.α, with v = mix(neutral, αs, coverage) |
|---|---|
| Add | αb + v |
| Subtract | αb − v |
| Multiply | αb · v |
| Invert | v · (1 − αb) + αb · (1 − v) |
| Min, Max | min(αb, v), max(αb, v) |



## Layers

Since Blend is Porter-Duff over, content drawn with paint modes onto a transparent layer (a CanvasLayers layer or a surface of your own) and then blended onto the canvas looks exactly as if it was drawn directly onto the canvas.

That can't hold for effect modes, by their nature: an effect changes what is below it, and on a transparent layer nothing is below. If the content of a layer should work as an effect on what is below the layer, draw it onto the layer with Blend and blend the layer with the effect mode, using `blendFunc` in CanvasLayers::LayerBP.



## Relation to other standards

*Porter-Duff compositing* (1984) defines how the coverage of two images combines. Blend is its *over* operator. The effect modes resemble its *atop* operator, which keeps the alpha of the destination, but change the destination instead of placing the source on it.

*W3C Compositing and Blending* (used by CSS, SVG, PDF and HTML canvas) treats blend modes as paint: the blended color is composited with *over*, so a blend mode paints normally onto transparent parts of the canvas and makes the canvas more opaque. Multiply, Min, Max and Invert correspond to W3C's multiply, darken, lighten and exclusion, applied as effects instead of paint. Add corresponds most closely to the compositing operator plus-lighter, which also adds alpha.

Neither standard has a mode that takes away alpha by the alpha of the source, like Porter-Duff *destination-in* does. That could be added as a separate paint mode if it is needed for masking.



## Exactness

ReferenceBackend and the software backends can follow these definitions exactly everywhere.

GPU backends can follow them exactly with fixed-function blending, provided that canvases with alpha store premultiplied colors. The exception is semi-transparent pixels of canvases with alpha, where Invert, Min, Max and an Add that saturates are approximated. Canvases without alpha are exact in all modes.



## Implementation status

| | ReferenceBackend | SoftBackend, LinearBackend | OpenGL, Metal, DirectX 12 |
|---|---|---|---|
| Blend as Porter-Duff over | Yes | Only onto opaque pixels | Only onto opaque pixels |
| Source alpha as strength of effect modes | All | Add, Subtract | Add, Subtract |
| Coverage as strength of effect modes | All | Add, Subtract | Add, Subtract |
| Morph as premultiplied cross-fade | Yes | No | No |
| Premultiplied bilinear sampling | Yes | No | No |
| Transparent outside of ClipBlit has no effect | Yes | Add, Subtract | Add, Subtract |
| Canvases without alpha: results that aren't opaque blended onto the canvas | Yes | No | No |
| Alpha_8: coverage fades the value towards the neutral one | Yes | Add, Subtract, Invert | Add, Subtract, Invert |

GfxDeviceTester2 has tests for these rules in BlendRuleTests and LayerRoundTripTests.
