# WonderGFX – critical review before API freeze

Scope: `src/wondergfx`, `src/extensions/{softgfx,openglgfx,metalgfx,dx12gfx,gfxstream}` and the C API, as of
commit 6c7d427 on `breaking-changes`. Two goals were applied:

1. Be a complete graphics backend for a modern UI.
2. Be fast and lean enough for software rendering of a basic UI on a low-end CPU without an FPU (Cortex-M class),
   with API consistency and memory/CPU efficiency valued highly.

Findings are grouped as follows:

- **P0**: Bugs. Fix them regardless.
- **P1**: Decisions that are hard to change after the freeze.
- **P2**: Efficiency work that doesn't break the API.
- **P3**: Features and cleanup.

File references are relative to `src/`. Findings marked ✔ were re-verified by reading the code.

---

## 0. Overall assessment

The architecture is sound:

- **Recording and playback are separate.** `GfxDeviceGen2` records commands per canvas and layer, and `GfxBackend` plays them back.
- **The command stream uses separate rect, color, object and transform buffers.** This suits GPUs, trimming and streaming.
- **Geometry is fixed point:** spx, which is 26.6.
- **The soft kernels are integer only.**
- **Analytic shapes are shared,** through `GpuShapes` and `SoftShapes`.

These are the right foundations, and none of the recommendations below question them.

The weaknesses are mostly at the edges:

1. **Fixed point stops halfway.** The API and the protocol still carry floats in several places:
   - `Transform`
   - Tint geometry
   - `NinePatch` (pts)
   - Morph factor
   - Angles
   - Blur kernel

   The startup tables are also computed with `pow()` in double precision. On an FPU-less target, this puts soft-float into the two most common UI operations: nine-patch skins and tiled backgrounds.
2. **The protocol is not self-describing.** Seven different command header layouts exist, and rects and colors are consumed implicitly. As a result:
   - Backends cannot skip unknown commands.
   - Streams cannot be validated.
   - The decoder is hand-written about eight times.
3. **The GPU backends are three copies of the same translator.** Together they hold about 70% duplicated logic and already show drift: different tint-color policies, different `BlendMode::Ignore` behavior, and different mip-staleness bugs. They also issue roughly one draw call per command.
4. **The surface API is large and partly redundant:**
   - 21 blit and blur variants with gaps
   - 6 deprecated pure virtual functions
   - Inconsistent set, clear, has and get naming
5. **A few modern-UI essentials are missing:** soft shadows and large-radius blur, rounded or mask clipping, astral-plane text with glyph-index APIs, and conic gradients.

---

## 1. P0 – Bugs found during the review

### 1.1 Front end (`wondergfx/wg_gfxdevice_gen2.cpp`)

| # | Location | Bug |
|---|---|---|
| ✔ B1 | `fill(HiColor)` ~1290-1310 | **Color buffer desync.** When the call merges with the previous Fill, it still `push_back`s its color, but backends read one color per Fill *command*. `fill(red); fill(red); fill(r, blue)` therefore draws the last fill red. Two adjacent widgets that clear their background with the same color trigger this. Fix: push the color only in the `else` branch, as `fill(rect,color)` does. |
| ✔ B2 | ~3601 | **Transform index overflow.** Every complex blit appends a `Transform` with no deduplication, and its index is written as `uint16_t`. After about 65k stretch, nine-patch or rotScale blits in one canvas session, the index silently wraps. Fix: deduplicate against the previous transform and widen the index or flush. |
| ✔ B3 | ~1128 | Colors are uploaded under `if(!baseLayer.rects.empty())`. That is a copy-paste slip and should test `colors`. |
| B4 | ~1102 vs 1254 | `_doFlattenLayers` returns early when `nRects==0` without releasing retained objects. A state change with an empty clip list leaks a blit source reference. |
| B5 | ~227 | `resetClipList()` iterates `m_pActiveCanvas->layers` without a null check, so it crashes outside a canvas update. |
| B6 | 1489, 1504, 1662, 1787, 3094, 3202 | Fields are narrowed to `uint16` without checks: line thickness (≥1024 px or negative), `nRects`, and the morph factor and blur taps at 3740/3773. Negative blur weights are undefined behavior. |
| B7 | 2082 / 2168 / 2671 | Bilinear stretch maths differs per entry point: `src.w-65`, `src.w-64` and `srcW-1.f`. The same geometry samples differently depending on which function you call. |
| B8 | 3243 | `flipDrawWave` dereferences `pTopBorder` and `pBottomBorder` without null checks. |
| H1 | 203, 263, 756 | `setClipList`, `pushClipList` and the update rects store the **caller's pointer** and read it until pop or `endCanvasUpdate`. This is undocumented. Either document it in `wg_gfxdevice.h` or copy the rects (cheap). |
| H2 | `wondergear/wg_dynamicbuffer.h` | Has a destructor but no copy or move operations, and lives in `RenderLayer` inside a `std::vector`. It is safe only by accident today. |

### 1.2 Software backends (`extensions/softgfx`)

| # | Location | Bug |
|---|---|---|
| ✔ S1 | `wg_softbackend.cpp:603,612`, `wg_linearbackend.cpp:531,540` | `return` inside `case Command::Line` when the blend mode is `Ignore` or a kernel is missing. This **drops every remaining command in the buffer** and skips saving the rect and color cursors. It should advance `pRects` and `pColors`, skip the payload and `break`. |
| ✔ S2 | `*_extras.cpp` (3 files, e.g. `bgr565srgb_extras.cpp:398`) | `updateFixedBlendCache` stores `bgCol` into the *foreground* cache key. The cache never hits, so a 256-entry table with `/255` divides is rebuilt per glyph per clip rect. On M0 there is no hardware divide. The cache is also a file-static global, so it is not per backend and not thread safe. |
| S3 | `wg_linearbackend.cpp:370` (+5 copies) | The segment search tests only the top-left corner and loops forever if the patch is outside every segment. |
| S4 | `wg_softbackend.cpp:1059` | A ClipBlit with a standard transform dispatches to the Blur op. It works only because the front end never emits that combination. |

### 1.3 GPU backends

| # | Location | Bug |
|---|---|---|
| ✔ G1 | GL `wg_glbackend.cpp:1974`, Metal `:735` | Dereferences `pInfo` (SessionInfo), whose default is `nullptr`. Also, an early return on an invalid canvas leaves null buffers for `processCommands`. DX12 handles both. |
| G2 | GL / Metal | No bounds checks against SessionInfo. An under-estimate means heap overflow or `MTLBuffer` overrun. gfxstream truncates the counts to `uint16`, which makes this reachable. |
| G3 | GL `:708-744` | Legacy lines read the active canvas at record time, but GL switches canvas only at replay. Lines in a layer canvas get the wrong Y-flip. |
| G4 | Metal `:2208` | Re-entering the default canvas after a layer or mip pass reuses the caller's load action. `wappsdl2` uses `DontCare`, which discards what has already been drawn. Fix: force `Load` on re-entry. |
| G5 | GL `:293`, `wg_glsurface.cpp:727`; Metal `:838` | Mipmap and readback staleness is not marked for canvases rendered mid-session (GL never, Metal only for the last canvas). DX12 does this correctly. |
| G6 | Metal and DX12 edgemaps and warpmaps | Shared buffers are overwritten while frames are in flight (there are TODOs for it), so animated graphs tear. |
| G7 | GL `:2210`, Metal `:2054` | A blit with no blit source dereferences null. |

### 1.4 Resources, gfxstream and the C API

| # | Location | Bug |
|---|---|---|
| ✔ R1 | `c-api/wg_c_gfxtypes.h:91` | **`wg_blendMode` lacks `BLEND` and `BLENDFIXEDCOLOR`** and is cast straight to `BlendMode`. As a result `WG_BLENDMODE_ADD` arrives as `Blend` and everything after it is off by one. `wg_placement` starts at `NORTHWEST=0` while the C++ enum starts at `Undefined=0`. Fix: match the enums and `static_assert` every mirrored enum. |
| R2 | `c-api/wg_c_blurbrush.h:43` | Uses a C++ default member initializer, so the C API does not compile as C. Add a C11 compile test. |
| R3 | `wg_surfacereader.cpp:94,337`; `wg_lzcompression.cpp:320` | `headerBytes` from the file is used unclamped in `memcpy` and `read`, which overflows the stack. The decompressors have no destination bound, and `readSurfaceFromMemory` takes no size. |
| R4 | `wg_bitmapcache.*` | Four bugs: `nbSurfaces()` counts only category 0; eviction is not oldest-first because `lowestCreationNb` is never updated; listeners receive dangling `Surface*`; and `removeListener` uses `erase(remove())` in its single-iterator form, which is undefined behavior. The cache limit is also not enforced on growth. |
| ✔ R5 | `wg_bitmapfont.cpp:406` | `maxAdvance * 54` should be `* 64`. Separately, `SysFont` mixes spx and pixels in `setSize()` and `size()`. |
| ✔ R6 | `wg_font.h` | Codepoints are `uint16_t`, so the Printer silently maps U+1F600 to U+F600. The UTF-8 parser is unbounded, and a tab with `whitespaceAdvance==0` divides by zero. |
| R7 | `wg_patches.h` | `PatchesT` has a copy constructor but no `operator=`, so assignment double-frees. |
| R8 | gfxstream (`streamplayer.cpp:480`, `streamencoder.h:52/142`) | Null object id 0 is a valid table entry, but the player drops the whole session when it sees it. `Blueprint::objectIdStart` defaults to 0 while the constructor uses 1, so the first object collides with null. `m_idCounter` is a signed `short`. |
| R9 | gfxstream (`gfxstream.cpp loadDecompressData`, `streamdecoder.h:140,443`) | Chunk sizes and offsets are unchecked, so a malformed or truncated stream causes a heap overflow. `StreamBackend::beginSession` dereferences a null `pInfo`. `StreamBackend::TYPEINFO` has `Object` as its parent instead of `GfxBackend`. |
| ✔ R10 | `wg_gfxutil.cpp:222` | `squareRoot` and `powerOfTen` are **CPOL-licensed CodeProject code**. CPOL is generally considered GPL-incompatible, which matters for the dual GPL/commercial licence. The code is also slow (digit by digit in double). Replace it with an integer `isqrt`. |

---

## 2. P1 – Decisions to make before freezing the API

### 2.1 One consistent unit system, with no floats in the core

Today the public API and the wire format mix the following units:

| Quantity | Today |
|---|---|
| Geometry | spx 26.6 (good) |
| Blit source on the wire | 22.10 (`src*16`, undocumented) |
| Transform | `float[4]` (API and wire) |
| `precisionBlit`, `transformBlit` source | `RectF`/`CoordF` in spx |
| NinePatch | `pts` = `float`, plus `int scale` |
| Rotation | float degrees |
| Ring angles and slices | float turns in the API, `uint32` (2^31 per turn) on the wire, via `double` in `drawRing` |
| Morph factor | float in the API, 1/4096 on the wire |
| Blur taps | float in `Blurbrush`, Q15 on the wire, converted at every encode |
| Tint geometry and stops | float in the API **and on the wire** (`TintTools` encodes floats) |
| Warpmap nodes, CircularWarp | float and double |

Recommendation: define a small set of named fixed-point types in `wg_gfxtypes.h` and use them everywhere in GfxDevice, GfxBackend and the resource blueprints.

- `spx`: geometry (unchanged).
- `frac12` (0..4096): fractions such as morph factor, tint positions, ring slices, alpha and weights. The soft backend already works in 1/4096.
- `angle32` (`uint32`, 2^32 per turn): wraps for free and needs no FPU.
- `Transform`: 16.16 fixed point. The front end converts once for rot and scale; the backends receive integers.
- Keep float convenience overloads as inline wrappers in the header, so applications on desktop lose nothing and FPU-less targets never link them.
- Store `NinePatch` in source pixels or spx, not pts. It is converted with soft-float on every skin draw today.

This is the single most important freeze decision for the low-end goal. Every float in the protocol forces soft-float into every backend, including the soft backend playing back a gfxstream.

### 2.2 Make the command protocol self-describing

Today there are seven header layouts and implicit rect and color cursors. `default: break` in the decoders reinterprets payload words as opcodes, which produces garbage, not a skip.

- **Uniform header** for every command: `{uint16 cmd, uint16 nWords, uint16 nRects, uint16 nColors}`. DrawShape already works this way. This enables:
  - generic skipping of unknown commands
  - forward compatibility
  - one validation pass
  - generic trimming in StreamTrimBackend
- **Write down the 32-bit alignment rule** for command starts and assert it. It already holds, but only by accident, and `pushUnchecked` would fault on M0 if it broke.
- **Widen `StateChange` to 16 bits** and reserve one bit for a length-prefixed extension block, so new state can be added later. Today 7 of 8 bits are used and `0xFF` means "all".
- **Document the Fill, Blit, Tile, ClipBlit and Blur layouts and every StateChange payload** in `wg_gfxbackend.h`. Today only some commands are documented.
- **Drop the `version` argument** of `processCommands`. Nobody passes it, the `version==1` branches are dead, and it is a default argument on a pure virtual. Replace it with capability queries on the backend, e.g. `supportsShape(ShapeKind)`, so GfxDeviceGen2 can fall back on minimal backends (RoundedBox → fills or an edgemap).
- **SessionInfo**:
  - Decide whether it is exact or a hint (DX12 treats it as a hint with bounds checks, and the others should too).
  - Rename the counters so they say whether they count rects or commands (`nFill` and `nBlit` count *rects* today).
  - Make the counters `uint32` and the stream fields 32-bit.
  - Drop the counts that just repeat buffer sizes.
- **Shared `CommandReader`** (header only, optional bounds checking) used by all backends, BackendLogger, StreamPlayer and validation. StateChange alone is decoded by hand in about eight places today.
- **Define byte order** in the gfxstream ProtocolVersion chunk. Command buffers are shipped in native byte order, so streams don't work between hosts of different byte order.

### 2.3 Shrink and regularize the GfxDevice surface

- **Collapse the blit family** into orthogonal calls:

  ```cpp
  blit(dest, src, GfxFlip = None)
  stretchBlit(dest, src, GfxFlip = None)
  precisionBlit(dest, srcSPX)                 // fixed point
  transformBlit(dest, src, Transform)          // fixed point
  tile(dest, shift, GfxFlip = None, scale = 1)
  drawEdgemap(dest, e, GfxFlip = None);  tileEdgemap(dest, e, shift, GfxFlip = None)
  ```

  Make blur a render state (`setBlitOp(Blit|Blur)`, or a brush set means blur) instead of a parallel family. That removes about 10 virtual functions and fixes the gaps: there is no `flipBlur`, `stretchFlipBlur` or `scaleBlur`. Each existing entry point repeats the same three precondition checks, about 25 copies.
- **Remove the deprecated pure virtuals before the freeze:** `drawWave`, `flipDrawWave`, `drawElipse` (sic), `drawPieChart`, `drawSegments`, `flipDrawSegments`. If they are still needed, keep them as non-virtual helpers in a utility built on `drawRing` and `drawEdgemap`. `drawWave` and `drawSegments` currently create an Edgemap **per call**. Decide the fate of `TintMode`: it is marked "DEPRECATED!!!" but is still used by the soft kernels, the GL backend and `drawSegments`.
- **State API**:
  - Use the same pattern for every state: `set(x)` / `x()`, with "none" expressed as `nullptr` or `HiColor::Undefined`.
  - Either drop `has*`/`clear*`, or provide them for every state. Today TintColor and Tint have them, but BlitSource, Blurbrush and FixedBlendColor don't.
  - `tintColor()` returns White when undefined, so a get doesn't round-trip a set.
  - Setters should return `void` and report errors through `GfxBase::throwError`. Today `setBlendMode` returns `bool` (always `true`), `setRenderLayer` returns `void` although it can fail, and so on.
  - Getters should return raw `T*`, valid while the state is set, instead of `_p`. That avoids refcount churn.
  - The structs should hold `const Tint*`.
- **Add state save and restore** (`pushState()`/`popState()`, or a returned token). Every skin and widget that touches tint, blend mode or blit source has to save and restore by hand today. This is both an API wart and a source of redundant state-change commands.
- **Remove redundant members:**
  - `isIdle()`, which is `!isRendering()`.
  - `flush()`, which does nothing.
  - `canvasSize()`, which is `canvas().size`.
  - `canvas(ref)` returns `CanvasInfo` **by value**, which copies a `Surface_p`. It should return a pointer or reference, like the backend does.
- **Make `isRendering()` and the other getters `const`, and mark `GfxDeviceGen2` `final`.** Keep the abstract `GfxDevice`: `PluginGfxDevice` needs it, and one indirect call is negligible next to the recording cost.
- **Name the magic defaults:**
  - `thickness = 64` → `c_onePixel`, or `spx(64)` via a named constant.
  - `startLayer = -1` → a named constant.
- **Decide what `BlendFixedColor` means.** It is documented as "blend against a fixed color and *replace*", but all three GPU backends draw it as plain Blend. Either implement it, or redefine it as an optimization hint ("destination is known to be X"). Also define `Ignore` as "draws nothing" in all backends; GL and Metal still issue the draw today.

### 2.4 Resource API consistency

- **Creation:**
  - One pattern: every object has a `Blueprint` plus `static create(bp)`, and generators take `(bp, factory = nullptr)`.
  - Positional `create()` for `BitmapFont`, `SysFont`, `Printer` and `BitmapCache` should get blueprints.
  - `CanvasLayers::Blueprint` needs a `finalizer`.
  - Enforce the alphabetical field rule from `docs/basics/blueprints.md`. `Surface`, `Waveform`, `ColorTransition` and `SurfaceWriter` violate it.
- **Count naming:** `nbStops()` and `nbGlyphs()` sit next to `segments()` and `size()`. Pick one convention.
- **Spelling:**
  - `maxAscend`/`maxDescend` → Ascent/Descent
  - `LemonCiffon` → LemonChiffon
  - `MediumPurple` has the wrong value
  - `drawElipse` → drawEllipse
  - `wg_printCRFL…` → CRLF
- **Observers:** three mechanisms exist today:
  - Surface: `std::function` in a heap list.
  - BitmapCache: a raw listener interface, which has use-after-free bugs.
  - Edgemap and Warpmap: none public.

  Pick one lightweight mechanism, such as a function pointer plus `void*`, which also maps to C.
- **Surface:**
  - `paletteCapacity` exists but there is no `setPalette()`, which contradicts the docs. Add it or drop the capacity.
  - `pullPixels` says "needs to be overridden" but is not pure virtual.
  - `PixelBuffer` must be freed by hand; consider an RAII wrapper.
  - Decide whether a surface may report a different `pixelFormat()` than requested. DX12 and Metal silently widen 565 to XRGB_8.
- **Backend surface parity:**
  - `unload`/`reload`/`isLoaded` exist on GL and Metal only.
  - `syncTexture`/`notifyRendered` exist on DX12 only.
  - The texture getter is `getTexture()` on GL and Metal but `texture()` on DX12.
  - `setDefaultCanvas` has three different shapes and units (pixels on Metal, spx on the others), and DX12 has no ColorSpace parameter.
  - DX12 has no GfxDeviceFactory.
- **Font, the biggest API debt.** The API should support the following before the freeze, even where the implementation comes later:
  - `char32_t` codepoints.
  - Glyph-index access (`glyphIndex(cp)`, `getGlyphByIndex`), so shaping (HarfBuzz), ligatures and bidi can be attached.
  - Size passed per call instead of a stateful `setSize()`, so one font object can be shared and used `const`.
  - A pixel format per cache slot, so colour emoji can be cached. `BitmapCache` only creates Alpha_8 today.
  - A fallback chain instead of a single backup font.

### 2.5 The C API

Beyond R1 and R2:

- **Missing wrappers:** Transitions, Waveform, EdgemapTools, compressors, QOI, SurfaceFileInspector, PixelTools, `Surface::convert`, and finalizer and baggage in blueprints.
- **Naming:**
  - The `get` prefix is used inconsistently: `wg_getSurfaceIdentity` versus `wg_surfacePixelSize`.
  - Some names are unprefixed and generic: `wg_lineGap`, `wg_nbGlyphs`, `wg_baseLayer`.
- **Signatures:** Blurbrush and `printInBox` take structs by value while everything else takes pointers.
- Prefer binary-equivalent blueprint structs with `static_assert`s over the many "NOT BINARY EQUIVALENT" comments.

---

## 3. P2 – Low-end, FPU-less software rendering

Integer-only today:

- The soft kernels per pixel
- `SoftShapes`, which uses constexpr trig and reciprocal tables
- Per-pixel `SoftTint` stepping
- Transitions
- Clip handling

What still hurts:

### 3.1 Soft-float in the hot path

| Where | Frequency | Fix |
|---|---|---|
| ✔ `wg_softkernelstanza.impl.h:1969` Tile/Blur `srcX += pMatrix->xx` (int += float) | **per pixel** (tiled skin backgrounds) | The standard transforms are all 0 and ±1. Pass them to proxies and kernels as an `int8_t[2][2]`. |
| `wg_softbackend.cpp:1035,1048,1391,1413,1443` (+ LinearBackend copies) | per clip rect and per span, every blit and glyph | Same integer standard transforms. |
| `wg_softbackend.cpp:1077` `std::ceil(float * double(BINAL_MUL))` | per clip rect for every stretch blit, including nine-patch | A fixed-point Transform (§2.1). Until then, at least use `float` and an integer ceil. |
| Line thickness `thickness/64.f` | per line | Integer `(int64(t)*scale)>>6`. |
| `TintTools::_buildLUT`, float per LUT entry (up to 2049 per layer) | per Tint change and per tinted shape (1–3 M cycles on M0) | Fixed-point stops with integer DDA. Cache `SoftTint` by its encoded words, because UIs reuse the same tints constantly. |
| GfxDeviceGen2 stretch, precision and rotScale blits (float divides, sin/cos); `drawRing` (double); `blitNinePatch` (pts) | per call | §2.1. |
| `HiColor::_initTables`, `PixelTools::_initTables`, SoftBackend tables: about 21k `pow()` calls in **double** | at startup, likely seconds on a 48 MHz M0 | Generate them as `constexpr`/`const` arrays in flash, as `wg_softshapes.cpp` already does. |

### 3.2 Text, the most common basic-UI operation

- **Glyph pixels lack early-outs.** `_straight_blit` has no early-out for transparent glyph pixels: the `bFastTransparent` test compares a HiColor that is never zero for Alpha_8. It also has no direct store for opaque glyph pixels with an opaque tint. Fix: add `if(srcA==0) continue;` for blend-type modes, and a direct store on full alpha.
- **Tinted text on sRGB 565 always takes the two-pass path** (HiColor staging buffer, 8 B/pixel). The single-pass Alpha_8 kernels in `bgr565srgb_extras.cpp:653` are commented out. Register single-pass `Alpha_8 → 565 {Flat} × {Blend, BlendFixedColor}` kernels.
- **The sRGB write path does two table lookups per channel.** Merge them into one table from 12-bit linear to 5/6-bit, about 1 KB.
- **Each glyph is one blit** (`wg_printer.cpp:445`), and each blit costs 16 B of rect plus a 20 B record that repeats `src`, `dest` and `transform` **per clip rect**. Two fixes:
  - Encode blits as `{nBlits}` × `{nRects, src, dest, xform}`.
  - Or add a dedicated `drawGlyphs(source, n, {src,dst}[])` command.

  Either roughly halves the stream size for text-heavy screens and lets GPU backends instance glyphs.
- **Glyph positions snap to whole pixels.** `_transformBlitSimple` does `align(dest)`, with the comment "TODO: Proper 26:6 support". That is fine for the low-end target, but decide now whether blits are *defined* to snap. Smooth scrolling and animation on GPU backends want subpixel placement; it could be a state or a per-backend capability.

### 3.3 Memory

**RAM**

- About 85 KB of static tables are computed at startup (`s_limit4096Tab` 24.6 KB, `s_reciprocalTab` 16.4 KB, HiColor ~42 KB, PixelTools ~7 KB).
  - Make them `const` so they live in ROM or flash.
  - Replace `s_limit4096Tab` with a clamp (`USAT`).
  - Move `sRGBToLinearFloatTab` (16 KB, float) to the GPU backends; only they use it.
- **Per-object sizes** (x86-64):
  - **Surface** is 144 B, including a 48 B embedded `PixelDescription` plus a pointer to itself. Point at the static `pixelDescTab` instead and pack the six bools into a bitfield.
  - **Tint** is 384 B even when flat. Allocate stops at their exact count and put the mix data in a subclass. `Tint::mix()` heap-allocates every animation frame.
  - **BitmapFont** costs about 12 KB for Latin-1, with a `Surface_p` per glyph although all glyphs share one surface. Store the surface once and keep `int16` metrics.
  - **Edgemap** always allocates its tints and stores `int32` samples. An `int16` sample mode would halve it for embedded targets.
- **`Object::m_refCount` is `std::atomic_int`.** On ARMv6-M atomics become library calls, and every `Surface_p` copy pays for that. Offer a build option for non-atomic refcounts on single-threaded targets.

**Code size** (x86-64 `-Os`; expect about 60–70% of this on Thumb-2)

- The "565" kernel set is about 0.5 MB, and `_transform_blit` alone is 275 KB (220 instantiations).
- `processCommands` unconditionally links shapes, warp, edgemap and blur, about 140 KB.
- `GfxBase::getDecompressor` hard-links all five codecs.
- `_draw_line` has 12 instantiations and `pLineKernels` is never read: this is dead code.
- There are no feature macros.

**Recommendations**

- Compile-time switches: `WG_SOFTGFX_NO_FPU`, `NO_WARP`, `NO_EDGEMAP`, `NO_BLUR`, `NO_BILINEAR`, `NO_ERRORSTRINGS`. The error strings currently pull in `snprintf` with a 1 KB stack buffer per failing span.
- A curated **"tiny565" kernel set**:
  - 565 destination only
  - Replace, Blend and BlendFixedColor
  - None and Flat tints
  - Straight, tile and nearest-neighbour stretch blits

  That should fit well under 50 KB.
- Codec registration, so unused decompressors are stripped.
- Remove the dead tint slots (`GradientY`/`GradientXY`, about 40% of the tables) and the Undefined/Ignore blend slots.

### 3.4 Structure of the soft backends

- **`LinearBackend` copies SoftBackend's command loop almost verbatim.** It is the right backend for MCUs with SPI or parallel displays and no framebuffer, so it should get the attention, not drift. Introduce a single `_resolveTarget(clip) → (origin, pitch)` hook: SoftBackend gets one segment equal to the canvas, and LinearBackend shrinks to about 150 lines.
- **`ReferenceBackend`** (3k lines, all double) is a valuable correctness oracle, but it belongs to a test or tools target, not to `wgsoftgfx`.
- **Kernel file names lie.** The "bgr565" files register `RGB_565`, and the "rgb555be" files register `BGR_565_BE`. The hand-written extras ignore byte order and truncate instead of rounding, so they differ from the template kernels.

---

## 4. P2 – Efficiency in the front end and the GPU backends

### 4.1 GfxDeviceGen2 recording

- **Every frame reallocates everything.** `beginCanvasUpdate` builds a fresh `RenderCanvas`: per layer, a `DynamicBuffer` (≥4 KB), three vectors and a 1 KB object cache. `endCanvasUpdate` frees them all. Pool `RenderCanvas` and `RenderLayer` by stack depth and clear them while keeping capacity. This is the largest allocator win on low-end targets.
- **✔ CanvasLayers surfaces are created eagerly, at full canvas size, every frame** (gen2.cpp:798), and cleared, although the docs say "created on demand". Make them lazy, size them to the update-rect bounds and pool them.
- **Lines copy the whole clip list into each Line command** without culling against the line bounds.
- **Every clip-list change walks all layers** to reset `latestCommand == Line`. Binding clip rects per line, or per batch, would remove this special case.
- **The object-cache hash does a 64-bit multiply.** A 32-bit Fibonacci hash is cheaper on 32-bit cores.
- **Public virtuals call other public virtuals**, for example `stretchBlit(dest)` → `stretchBlit(dest,src)`, so checks run twice. Route these through non-virtual `_impl` functions.

### 4.2 GPU backends: one shared translator

About 70% of GL, Metal and DX12 is the same command-to-geometry translation written three times:

- fills
- legacy line geometry
- thickness scaling
- capped lines
- shape plumbing
- edgemap flip and UV maths
- blit source-offset rules
- blur taps
- warp extras
- output-mode tables
- premultiply and opaque helpers

The copies already drift:

- Tint color is applied on the CPU in GL, by a uniform in Metal, and per color slot in DX12.
- `Ignore` is handled differently.
- Palettes are premultiplied differently in GL and DX12.

Recommendation: a shared **`GpuCommandTranslator`** in wondergfx.

- It decodes the stream once with the shared `CommandReader` and tracks state with one policy.
- It writes backend-neutral **instance records** (rect, kind, color, extras and tint offsets, source and transform) into spans the backend provides.
- It emits a compact draw list of `{pipelineKey, source, firstInstance, count, scissor?}` and **merges consecutive compatible draws**.
- Each backend implements only `reserve`, `setCanvas`, `bindPipeline`, `bindSource`, `setScissor` and `draw`.

This roughly halves each backend and removes the following inefficiencies, which all three backends share today:

- **About one draw call per command, with no merging.** A UI of rounded boxes means one draw plus one pipeline bind per button.
- **Six fat, non-indexed vertices per rect** (24–40 B each). Instanced quads expanded from the vertex ID would cut vertex traffic by 6–10×.
- **Lines are redrawn once per clip rect with scissor.** Clip per instance instead.

Backend-specific items:

- **GL:**
  - Calls `glGetUniformLocation` per draw ✔. Cache the locations at link time.
  - Changes texture wrap state per ClipBlit. Use sampler objects.
  - Re-specifies four `new[]` buffers and three `glBufferData` calls per session.
  - There is no GLES 3 or WebGL 2 path, so Android and Web are not covered.
- **Metal:**
  - Creates three buffers and a command buffer per session and waits with `waitUntilScheduled`. Use per-frame rings and a semaphore.
  - Compiles pipelines lazily on the render thread, which causes hitches. Pre-warm them.
- **DX12:**
  - Runs `waitForCompletionOfAll()` (a full GPU drain) before every dynamic surface upload, which stalls video and animated surfaces every frame.
  - Keeps a full upload-heap mirror and two descriptor heaps per surface.
  - Reads palettes, edgemaps and warpmaps from upload heaps across PCIe.
- **All three:**
  - `alpha(coord)` on a dirty canvas reads back the whole texture.
  - Metal and DX12 always keep CPU mirrors. Respect `Blueprint::buffered`, as GL does.
- **Shaders:** write them once (HLSL → DXC → SPIRV-Cross), or at least share the coverage maths, which already matches `GpuShapes`.
- **Dead code:**
  - `openglgfx/wg_glshaders.cpp` (1.1k lines, not built)
  - `CommandMTL`
  - `colorstripOfs`, `m_bPendingReads` and `m_bMipmappedActiveCanvas`

---

## 5. P3 – Modern-UI feature gaps

In priority order for a modern UI:

1. **Soft shadows and glows.** `softness` is already reserved in the RoundedBox and Ring payloads. Implementing it analytically, as an erf or Gaussian-integral approximation for rounded boxes, gives CSS-style `box-shadow` in one draw on every backend. It is cheap even in software at small radii. This is probably the most visible missing feature.
2. **Real blur.** The Blurbrush is a single 3×3 kernel at a radius, so large radii produce ghosting. Backdrop blur ("frosted glass") needs a separable Gaussian or a downsampling dual-Kawase blur with an arbitrary radius. Define it as a radius in spx plus quality. A 3×3 kernel can stay for special effects.
3. **Clipping to shapes, and masks.** Clipping only works with rectangle lists. Rounded containers that clip scrolled content are everywhere in modern UIs. Options:
   - A clip state of a rounded rect, cheaply analytic in both soft and GPU backends.
   - A general alpha-mask surface operand (`setMask(surface, rect)`).

   CanvasLayers lambdas can emulate this today, but at a full-canvas layer cost every frame (§4.1).
4. **Text** (§2.4):
   - astral plane and emoji
   - glyph indices and shaping hooks
   - colour glyphs
   - fallback chain
   - subpixel x-phase variants
   - underline, strikethrough, x-height and cap-height metrics

   SDF or MSDF glyphs would give scalable text on GPUs cheaply. Make them an optional font type.
5. **Group opacity.** "Draw this subtree at 60%" without a full-screen layer. Lazy, bounds-sized layers (§4.1) would cover it.
6. **Gradients.** Add a conic (sweep) gradient for progress rings and colour wheels, and rotated elliptical radials. Specify whether stops are interpolated premultiplied.
7. **Paths.** There is no general path rasterizer for SVG icons. An `EdgemapTools::fromPolygon`/`fromPath` (even-odd and nonzero), plus dashed lines and polylines with joins, would cover icons and charts. Line caps exist, but joins don't.
8. **Images.** Provide a reader codec registry so PNG, JPEG and WebP can come from an extension.
9. **Wide gamut and HDR.** At least reserve a `ColorSpace` value. `ARGB_16` exists but is not tied to a colour space. This is low priority for the stated goals.

---

## 6. P3 – Cleanup before the freeze

- **Remove dead code:**
  - `wg_geo.cpp` (empty)
  - `BorderT::scale()` ("WG2 compatibility")
  - The commented-out CMYK block in `wg_color.cpp`
  - The commented-out blocks in `wg_surfacefileheader.h`
  - `Surface::m_bOpaque`
  - The `LayerBP` constructors
  - BitmapFont kerning parsing and `m_pKerningTable`. The kerning table is never allocated, so either implement kerning or remove it.
  - Dead gfxstream paths: the old DataInfo heuristic, `chunkComp` (also the riskiest overflow code), the empty `wg_streamsurface.cpp` and `WG_GFXSTREAM_USE_SURFACE_UPDATE2` in a public header.
- **Move internal-only definitions out of public headers:** the `Compression` enum to `wg_spxcompression.cpp`.
- **Deduplicate the sRGB transfer functions** in `wg_color.cpp` and `wg_pixeltools.cpp`.
- **Update stale documentation:**
  - `canvaslayers.cpp` documents removed methods.
  - `surfaces.md` references `SoftGfxSurface::create` and a palette API that doesn't exist.
  - The streaming guide describes an older chunk header.
  - `wg_surface.h` has a stale `setScale` comment.
  - `DestFormatKernels` has a stale size comment.
- **Fix geometry types:**
  - Make `BorderT(Type)` `explicit`, because it allows an implicit int → Border conversion.
  - Make `BorderT` `constexpr`.
  - Drop the duplicate typedefs (`Coord`/`CoordF`, `PatchesPTS`/`PatchesF`).
- **Make gfxstream constructors consistent:** use the same pointer and reference conventions, and give `StreamPlayer::create` a warpmap factory parameter.

---

## 7. Suggested order of work

1. **P0 bug fixes**, all small:
   - B1, B2, S1, S2, G1, R1, R2, R3, R9 and R10 first.
   - Add a C11 compile test and `static_assert`s for enum mirrors.
2. **Fixed-point unit system** (§2.1) together with the **uniform command header, CommandReader and StateChange widening** (§2.2). Both change the wire format, so do them in one protocol bump.
3. **GfxDevice surface cleanup** (§2.3) and **Font API shape** (§2.4). These are the last API-breaking items.
4. **Tables to flash, glyph fast paths, integer standard transforms, tiny565 kernel set and feature macros** (§3). None of these break the API.
5. **Recording-buffer pooling, lazy layers and GpuCommandTranslator with instancing** (§4). None of these break the API.
6. **Features**: shadows (softness), real blur, rounded or mask clip, then text features.
