---
title: 'Blending and transparency'
weight: 25
---

# Blending and transparency

*This section defines how GfxDevice combines what you draw with what is already on the canvas: what each BlendMode does, how transparency and anti-aliasing work, and what to expect when you draw onto transparent surfaces. All graphics backends are meant to follow these definitions. ReferenceBackend implements them as directly as possible and is used to test the other backends against them.*



## Colors

Blending happens in **linear light**, with every channel going from 0.0 to 1.0. Colors given to GfxDevice (HiColor) are sRGB and pixels of sRGB surfaces are converted to linear before blending and back again afterwards. Mixing of colors in Tint gradients is the exception, it happens in sRGB before the result is converted to linear.

Pixels are stored with **straight alpha**, e.g. the color channels are not multiplied with alpha. A surface without alpha (XRGB_8, RGB_565, BGR_565) is always opaque. An Alpha_8 surface used as a blit source is white with the alpha of the surface.

In the formulas below:

| Symbol | Meaning |
|---|---|
| Cb, αb | Color and alpha of the canvas pixel (b as in backdrop). |
| Cs, αs | Color and alpha of what we draw (s as in source), with the tint color and Tint already multiplied in. |
| B(Cb, Cs) | The effect of a blend mode at full strength. |
| mix(a, b, t) | a + (b − a) · t |

Color formulas apply to each of the color channels separately. All results are clamped to 0.0 -> 1.0.



## The glass model

Think of a transparent canvas as a perfect, non-reflective sheet of glass that we paint on.

**Paint modes** put paint on the glass. Where paint is semi-transparent, more paint makes the glass more opaque. Blend is the normal way of painting, Replace scrapes off what was there and paints anew.

**Effect modes** don't add any paint. They change the paint that is already on the glass, like shining a light on it (Add), filtering it (Multiply) or inverting it (Invert). Transparent parts of the glass just let the light through, opaque paint catches it fully and semi-transparent paint changes its hue just like opaque paint does, but stays as transparent as it was. The alpha of the canvas is never changed by an effect mode.

The transparency of the source controls the *strength* of an effect. An alpha-only surface with a soft round shape, blitted with BlendMode::Add and a tint color, works as a point light shining on the canvas.

One rule holds for all modes: **the color of a fully transparent pixel never affects the result**. It can't be seen and it doesn't bleed into anything else, whether through blending, filtering or anti-aliasing.



## Paint modes

| BlendMode | out.rgb | out.α |
|---|---|---|
| **Replace** | Cs | αs |
| **Blend** | (Cs · αs + Cb · αb · (1 − αs)) / out.α | αs + αb · (1 − αs) |
| **BlendFixedColor** | As Blend, with the fixed blend color in place of the canvas pixel. | |
| **Morph** | *See open decisions.* | |
| **Ignore** | Cb | αb |
| **Undefined** | Treated as Blend. | |

**Blend** is Porter-Duff *over*. On an opaque canvas pixel it is the familiar `Cs · αs + Cb · (1 − αs)`. The division matters where the canvas is transparent: drawing onto a transparent surface and blending that surface onto a background gives the same result as drawing directly onto the background.

**Replace** writes the source as it is, alpha included. It can make the canvas transparent.



## Effect modes

All effect modes follow the same rule:

```
out.rgb = clamp( Cb + αs · (B(Cb, Cs) − Cb) )
out.α   = αb
```

| BlendMode | B(Cb, Cs) | out.rgb | Meaning |
|---|---|---|---|
| **Add** | Cb + Cs | Cb + αs · Cs | Light shining on the paint. |
| **Subtract** | Cb − Cs | Cb − αs · Cs | Light taken away. |
| **Multiply** | Cb · Cs | Cb · mix(1, Cs, αs) | A filter. A transparent source is a clear filter. |
| **Invert** | Cs · (1 − Cb) + Cb · (1 − Cs) | Cb + αs · Cs · (1 − 2 · Cb) | Inverts the canvas, Cs gives the amount for each channel. |
| **Min** | min(Cb, Cs) | *See open decisions.* | Darkens to the source. |
| **Max** | max(Cb, Cs) | *See open decisions.* | Lightens to the source. |

Since the canvas alpha is kept, an effect mode has no visible effect on fully transparent parts of the canvas. To light up an empty layer, draw the light onto it with Blend and let the layer be blended onto the canvas below with Add (see Layers below).



## Anti-aliasing

Edges of fills, lines and edgemaps are anti-aliased by the exact area of each pixel that is covered. A partially covered pixel gets the area weighted average of what the operation gives the covered part and the original pixel for the rest:

```
out = average( op(source, canvas) weighted by coverage,  canvas weighted by (1 − coverage) )
```

Averages of colors are taken as **premultiplied colors**: each color counts by its alpha as well as by its area, so that a transparent part only makes the pixel more transparent and doesn't darken it. On a canvas without alpha everything is opaque and colors are simply averaged.

For Blend and all effect modes this is the same as multiplying αs with the coverage, so coverage and source alpha behave the same.

A pixel crossed by several segments of an edgemap gets the area weighted average of what each segment gives the whole pixel.



## Sampling

Bilinear sampling interpolates premultiplied colors, so transparent pixels don't bleed their color into their neighbours. This includes the transparent black outside the source of a ClipBlit, which otherwise would give dark edges. Mipmaps should be generated the same way.



## Alpha-only canvases

An Alpha_8 canvas has no color, so here the blend modes work on alpha as if it was a color channel. This is how masks are built, for example by adding and subtracting shapes. *See open decisions.*

| BlendMode | out.α |
|---|---|
| Replace | αs |
| Blend | αs + αb · (1 − αs) |
| BlendFixedColor | αs + αfixed · (1 − αs) |
| Add | αb + αs |
| Subtract | αb − αs |
| Multiply | αb · αs |
| Invert | αs · (1 − αb) + αb · (1 − αs) |
| Min, Max | min(αb, αs), max(αb, αs) |
| Morph | mix(αb, αs, morph factor) |



## Layers

Since Blend is Porter-Duff over, content drawn with paint modes onto a transparent layer (a CanvasLayers layer or a surface of your own) and then blended onto the canvas looks exactly as if it was drawn directly onto the canvas.

That can't hold for effect modes, by their nature: an effect changes what is below it, and on a transparent layer nothing is below. If the content of a layer should work as an effect on what is below the layer, draw it onto the layer with Blend and blend the layer with the effect mode, using `blendFunc` in CanvasLayers::LayerBP.



## Relation to other standards

*Porter-Duff compositing* (1984) defines how the coverage of two images combines. Blend is its *over* operator. The effect modes resemble its *atop* operator, which keeps the alpha of the destination, but apply B(Cb, Cs) instead of the source color.

*W3C Compositing and Blending* (used by CSS, SVG, PDF and HTML canvas) treats blend modes as paint: `Cs' = (1 − αb) · Cs + αb · B(Cb, Cs)` is composited with *over*. A blend mode there paints normally onto transparent parts of the canvas and makes the canvas more opaque, and its effect is weakened by the alpha of the canvas. Multiply, Min, Max and Invert correspond to W3C's multiply, darken, lighten and exclusion, applied as effects instead of paint. Add corresponds most closely to the compositing operator plus-lighter, which also adds alpha.



## Open decisions

1. **Min and Max.** Either the rule as written, `mix(Cb, min(Cb, Cs), αs)`, which GPUs can't do with fixed-function blending, or a transparent source fading towards the mode's neutral value, `min(Cb, mix(1, Cs, αs))` and `max(Cb, αs · Cs)`, which they can. The two are equal for opaque and fully transparent sources.
2. **Invert.** The documentation of BlendMode says Invert ignores the source color and uses its alpha. The backends use the source color as the amount for each channel and ignore alpha. This page assumes the latter, with alpha as strength.
3. **Morph.** Proposed as a cross-fade between canvas and source, including alpha, taken on premultiplied colors so that a transparent side doesn't darken the other: `out.α = mix(αb, αs, m)`, `out.rgb = mix(Cb · αb, Cs · αs, m) / out.α`.
4. **Alpha-only canvases.** Keep the exception above?



## Implementation status

| | ReferenceBackend | SoftBackend, LinearBackend | OpenGL, Metal, DirectX 12 |
|---|---|---|---|
| Blend as Porter-Duff over | Yes | Only onto opaque pixels | Only onto opaque pixels |
| Premultiplied anti-aliasing | Yes | Edgemaps with Replace only | Edgemaps only |
| Premultiplied bilinear sampling | Yes | No | No |
| Effect modes with source alpha as strength | Add, Subtract | Add, Subtract | Add, Subtract |
| Effect modes with coverage as strength | All | Add, Subtract. The others only at edges between edgemap segments. | Add, Subtract |
| Morph as premultiplied cross-fade | No | No | No |
