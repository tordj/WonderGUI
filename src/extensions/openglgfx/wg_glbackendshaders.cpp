/*=========================================================================

                             >>> WonderGUI <<<

  This file is part of Tord Bärnfors' WonderGUI UI Toolkit and copyright
  Tord Bärnfors, Sweden [mail: first name AT barnfors DOT c_o_m].

                                -----------

  The WonderGUI UI Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                                -----------

  The WonderGUI UI Toolkit is also available for use in commercial
  closed source projects under a separate license. Interested parties
  should contact Bärnfors Technology AB [www.barnfors.com] for details.

=========================================================================*/
#ifndef WG_GLSHADERS_DOT_H
#define WG_GLSHADERS_DOT_H

#include <wg_glbackend.h>  


// GLSL function evaluating a tint block (see TintTools::writeGpuTintBlock()) at a pixel center.

#define WG_GL_TINT_FUNC \
"vec4 evalTint(samplerBuffer buf, int ofs, vec2 pos)								" \
"{																					" \
"	int nLayers = int(texelFetch(buf, ofs).x);										" \
"	int p = ofs + 1;																" \
"	vec4 result = vec4(0.0);														" \
"	for (int l = 0; l < nLayers; l++)												" \
"	{																				" \
"		vec4 info = texelFetch(buf, p);												" \
"		vec4 geo = texelFetch(buf, p + 1);											" \
"		float srgb = texelFetch(buf, p + 2).x;										" \
"		int nStops = int(info.z);													" \
"		int posOfs = p + 3;															" \
"		int colOfs = posOfs + (nStops + 3) / 4;										" \
"		if (nStops > 0)																" \
"		{																			" \
"			float t = info.x < 0.5 ? geo.x * pos.x + geo.y * pos.y + geo.z : length((pos - geo.xy) * geo.zw);	" \
"			t = clamp(t, -1e7, 1e7);												" \
"			int spread = int(info.y);												" \
"			if (spread == 0)														" \
"				t = clamp(t, 0.0, 1.0);												" \
"			else if (spread == 1)													" \
"				t = t - floor(t);													" \
"			else																	" \
"			{																		" \
"				float r = t - 2.0 * floor(t * 0.5);									" \
"				t = r > 1.0 ? 2.0 - r : r;											" \
"			}																		" \
"			int k = -1;																" \
"			for (int i = 0; i < nStops; i++)										" \
"			{																		" \
"				if (texelFetch(buf, posOfs + i / 4)[i % 4] <= t)					" \
"					k = i;															" \
"			}																		" \
"			vec4 c;																	" \
"			if (k < 0)																" \
"				c = texelFetch(buf, colOfs);										" \
"			else if (k == nStops - 1)												" \
"				c = texelFetch(buf, colOfs + k);									" \
"			else																	" \
"			{																		" \
"				float p0 = texelFetch(buf, posOfs + k / 4)[k % 4];					" \
"				float p1 = texelFetch(buf, posOfs + (k + 1) / 4)[(k + 1) % 4];		" \
"				c = mix(texelFetch(buf, colOfs + k), texelFetch(buf, colOfs + k + 1), (t - p0) / (p1 - p0));	" \
"			}																		" \
"			if (srgb > 0.5)															" \
"			{																		" \
"				vec3 lo = c.rgb / 12.92;											" \
"				vec3 hi = pow((c.rgb + 0.055) / 1.055, vec3(2.4));					" \
"				c.rgb = mix(hi, lo, vec3(lessThanEqual(c.rgb, vec3(0.04045))));		" \
"			}																		" \
"			result += c * info.w;													" \
"		}																			" \
"		p = colOfs + nStops;														" \
"	}																				" \
"	return result;																	" \
"}																					"

// GLSL function reading a blit source. A source without alpha (RGB8 texture) reads as opaque
// also outside the source, where the transparent border of a ClipBlit should make it
// transparent. So when told to (rgbxClip bit 0), the alpha is worked out from the position:
// 1 inside and 0 outside with nearest sampling, and the part of the bilinear footprint that
// lies inside with bilinear sampling (bit 1), which is what the sampler does with the colors.
// Needs uniform texId declared first.

#define WG_GL_SOURCE_SAMPLE \
"uniform int rgbxClip;																" \
"vec4 sampleSource(vec2 uv)															" \
"{																					" \
"	vec4 c = texture(texId, uv);													" \
"	if ((rgbxClip & 1) != 0)														" \
"	{																				" \
"		if ((rgbxClip & 2) == 0)													" \
"			c.a = (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0)))) ? 0.0 : 1.0;	" \
"		else																		" \
"		{																			" \
"			vec2 size = vec2(textureSize(texId, 0));								" \
"			vec2 p = uv * size;														" \
"			vec2 coverage = clamp(min(p + 0.5, size - p + 0.5), 0.0, 1.0);			" \
"			c.a = coverage.x * coverage.y;											" \
"		}																			" \
"	}																				" \
"	return c;																		" \
"}																					"

// GLSL function looking up the palette entry for an index read from an R8 (Index_8) or R16
// (Index_16) index texture. The palette texture is up to 256 texels wide with as many rows as
// needed for paletteCapacity() entries (see GlSurface::_uploadPalette()), so the normalized
// index can't be used as a texture coordinate. Instead the index is turned into the coordinate
// of its texel's center, with paletteInfo set by GlBackend::_setPaletteInfo():
//
//   x: largest index of the index texture (255 or 65535), y: width, z: height, w: entries.
//
// Indexes beyond the palette get its last entry, like DX12 and Metal do. Needs uniform
// paletteId declared first.
//
// This samples with texture() rather than texelFetch() and textureSize(), which on a 2013
// Mac Pro (AMD, macOS OpenGL 4.1) gave random groups of black pixels.

#define WG_GL_PALETTE_FUNC \
"uniform vec4 paletteInfo;															" \
"vec4 paletteLookup(float normIndex)												" \
"{																					" \
"	float index = min(floor(normIndex * paletteInfo.x + 0.5), paletteInfo.w - 1.0);	" \
"	float row = floor(index / paletteInfo.y);										" \
"	float column = index - row * paletteInfo.y;										" \
"	return texture(paletteId, vec2((column + 0.5) / paletteInfo.y, (row + 0.5) / paletteInfo.z));	" \
"}																					"

namespace wg {


const char GlBackend::fillVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;					   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 2) in int colorOfs;                     "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   fragColor = texelFetch(colorBufferId, colorOfs);	   "
"}                                                         ";


const char GlBackend::fillTintmapVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;					   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 2) in int colorOfs;                     "
"layout(location = 4) in vec2 tintmapOfs;                  "
"out vec4 fragColor;                                       "
"flat out int tintOfs;  out vec2 tintPos;  "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;             "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   fragColor = texelFetch(colorBufferId, colorOfs);	   "
"   tintOfs = int(tintmapOfs.x);  tintPos = vec2(pos);  "
"}                                                         ";


const char GlBackend::fillFragmentShader[] =

"#version 330 core\n"
"out vec4 outColor;                     "
"in vec4 fragColor;                         "
"void main()                            "
"{                                      "
"   outColor = fragColor;                   "
"}                                      ";


const char GlBackend::fillFragmentShader_A8[] =

"#version 330 core\n"
"out vec4 outColor;                     "
"in vec4 fragColor;                         "
"void main()                            "
"{                                      "
"   outColor.r = fragColor.a;                   "
"}                                      ";

const char GlBackend::fillFragmentShaderTintmap[] =

"#version 330 core\n"
"uniform samplerBuffer tintmapBufferId;	"
"in vec4 fragColor;                     "
"flat in int tintOfs;  in vec2 tintPos;  " WG_GL_TINT_FUNC
"out vec4 outColor;                     "
"void main()                            "
"{                                      "
"   outColor = evalTint(tintmapBufferId, tintOfs, tintPos)"
"	           * fragColor;           "
"}                                      ";


const char GlBackend::fillFragmentShaderTintmap_A8[] =

"#version 330 core\n"
"uniform samplerBuffer tintmapBufferId;	"
"in vec4 fragColor;                     "
"flat in int tintOfs;  in vec2 tintPos;  " WG_GL_TINT_FUNC
"out vec4 outColor;                     "
"void main()                            "
"{                                      "
"   outColor.r = evalTint(tintmapBufferId, tintOfs, tintPos).a"
"	           * fragColor.a;           "
"}                                      ";



const char GlBackend::blitVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer extrasBufferId;						   "
"uniform samplerBuffer colorBufferId;					   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in vec2 texSize;					   "
"layout(location = 2) in int colorOfs;                     "
"layout(location = 3) in int extrasOfs;				       "
"out vec2 texUV;                                           "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasBufferId, extrasOfs);	   "
"   vec4 transform = texelFetch(extrasBufferId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   fragColor = texelFetch(colorBufferId, colorOfs);				"
"}                                                         ";


const char GlBackend::blitTintmapVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;						   "
"layout(location = 2) in int colorOfs;                     "
"uniform samplerBuffer extrasBufferId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in vec2 texSize;					   "
"layout(location = 3) in int extrasOfs;				       "
"layout(location = 4) in vec2 tintmapOfs;                  "
"out vec2 texUV;                                           "
"flat out int tintOfs;  out vec2 tintPos;  "
"flat out vec4 flatTint;                                   "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasBufferId, extrasOfs);	   "
"   vec4 transform = texelFetch(extrasBufferId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   tintOfs = int(tintmapOfs.x);  tintPos = vec2(pos);  "
"   flatTint = texelFetch(colorBufferId, colorOfs);        "
"}                                                         ";





// Blur shaders, one per [source kind][tint][canvas]. Source kinds are ordinary sources,
// Alpha_8 sources (their R8 texture is read as white with alpha, like the alpha blits do) and
// palette based sources (each tap is looked up in the palette before it is weighted, the index
// texture is read with nearest sampling so taps never blend indexes). On an Alpha_8 canvas
// the resulting alpha is written to the red channel, like the other _A8 shaders do.

#define WG_GL_BLUR_HEAD \
"#version 330 core\n" \
"struct BlurInfo { vec4 colorMtx[9]; vec2 offset[9]; };							" \
"uniform BlurInfo blurInfo;															" \
"uniform sampler2D texId;															" \
"in vec2 texUV;																		" \
"out vec4 color;																	"

#define WG_GL_BLUR_TAP \
"vec4 tap(vec2 uv) { return texture(texId, uv); }									"

#define WG_GL_BLUR_TAP_ALPHA \
"vec4 tap(vec2 uv) { return vec4(1.0, 1.0, 1.0, texture(texId, uv).r); }			"

#define WG_GL_BLUR_TAP_PALETTE \
"uniform sampler2D paletteId;														" WG_GL_PALETTE_FUNC \
"vec4 tap(vec2 uv) { return paletteLookup(texture(texId, uv).r); }					"

#define WG_GL_BLUR_FLAT \
"in vec4 fragColor;																	" \
"vec4 tintColor() { return fragColor; }												"

#define WG_GL_BLUR_TINT \
"uniform samplerBuffer tintmapBufferId;												" \
"flat in int tintOfs;  in vec2 tintPos;  flat in vec4 flatTint;						" WG_GL_TINT_FUNC \
"vec4 tintColor() { return evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint; }"

#define WG_GL_BLUR_SUM \
"vec4 blurred()																		" \
"{																					" \
"	vec4 c = vec4(0.0);																" \
"	for (int i = 0; i < 9; i++)														" \
"		c += tap(texUV + blurInfo.offset[i]) * blurInfo.colorMtx[i];				" \
"	return c;																		" \
"}																					"

#define WG_GL_BLUR_OUT \
"void main() { color = blurred() * tintColor(); }									"

#define WG_GL_BLUR_OUT_A8 \
"void main() { color.r = blurred().a * tintColor().a; }								"

#define WG_GL_BLUR_SHADERS(TAP) \
	{ { WG_GL_BLUR_HEAD TAP WG_GL_BLUR_FLAT WG_GL_BLUR_SUM WG_GL_BLUR_OUT, WG_GL_BLUR_HEAD TAP WG_GL_BLUR_FLAT WG_GL_BLUR_SUM WG_GL_BLUR_OUT_A8 }, \
	  { WG_GL_BLUR_HEAD TAP WG_GL_BLUR_TINT WG_GL_BLUR_SUM WG_GL_BLUR_OUT, WG_GL_BLUR_HEAD TAP WG_GL_BLUR_TINT WG_GL_BLUR_SUM WG_GL_BLUR_OUT_A8 } }

const char * const GlBackend::blurFragmentShaders[3][2][2] =
{
	WG_GL_BLUR_SHADERS(WG_GL_BLUR_TAP),
	WG_GL_BLUR_SHADERS(WG_GL_BLUR_TAP_ALPHA),
	WG_GL_BLUR_SHADERS(WG_GL_BLUR_TAP_PALETTE)
};




const char GlBackend::blitFragmentShader[] =

"#version 330 core\n"

"uniform sampler2D texId;						" WG_GL_SOURCE_SAMPLE
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color = sampleSource(texUV) * fragColor;  "
"}												";

const char GlBackend::blitFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						" WG_GL_SOURCE_SAMPLE
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color.r = sampleSource(texUV).a * fragColor.a;  "
"}												";


const char GlBackend::alphaBlitFragmentShader[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color = fragColor;							"
"   color.a *= texture(texId, texUV).r;         "
"}												";

const char GlBackend::alphaBlitFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color.r = fragColor.a * texture(texId, texUV).r;         "
"}												";


const char GlBackend::blitFragmentShaderTintmap[] =

"#version 330 core\n"

"uniform sampler2D texId;						" WG_GL_SOURCE_SAMPLE
"uniform samplerBuffer tintmapBufferId;			"
"in vec2 texUV;									"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"void main()									"
"{												"
"   vec4 fragColor = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint); "
"   color = sampleSource(texUV) * fragColor;  "
"}												";

const char GlBackend::blitFragmentShaderTintmap_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						" WG_GL_SOURCE_SAMPLE
"uniform samplerBuffer tintmapBufferId;			"
"in vec2 texUV;									"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"void main()									"
"{												"
"   float fragA = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint).a; "
"   color.r = sampleSource(texUV).a * fragA;  "
"}												";

const char GlBackend::alphaBlitFragmentShaderTintmap[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform samplerBuffer tintmapBufferId;			"
"in vec2 texUV;									"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"void main()									"
"{												"
"   color = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint); "
"   color.a *= texture(texId, texUV).r;         "
"}												";

const char GlBackend::alphaBlitFragmentShaderTintmap_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform samplerBuffer tintmapBufferId;			"
"in vec2 texUV;									"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"void main()									"
"{												"
"   float fragA = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint).a; "

"   color.r = fragA * texture(texId, texUV).r;  "
"}												";


const char GlBackend::paletteBlitNearestVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer extrasBufferId;						   "
"uniform samplerBuffer colorBufferId;					   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in vec2 texSize;					   "
"layout(location = 2) in int colorOfs;                    "
"layout(location = 3) in int extrasOfs;                    "
"out vec2 texUV;                                           "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasBufferId, extrasOfs);	   "
"   vec4 transform = texelFetch(extrasBufferId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   fragColor = texelFetch(colorBufferId, colorOfs);				"
"}                                                         ";


const char GlBackend::paletteBlitNearestTintmapVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;						   "
"layout(location = 2) in int colorOfs;                     "
"uniform samplerBuffer extrasBufferId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in vec2 texSize;					   "
"layout(location = 3) in int extrasOfs;                    "
"layout(location = 4) in vec2 tintmapOfs;                  "
"out vec2 texUV;                                           "
"flat out int tintOfs;  out vec2 tintPos;  "
"flat out vec4 flatTint;                                   "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasBufferId, extrasOfs);	   "
"   vec4 transform = texelFetch(extrasBufferId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   tintOfs = int(tintmapOfs.x);  tintPos = vec2(pos);  "
"   flatTint = texelFetch(colorBufferId, colorOfs);        "
"}														";


const char GlBackend::paletteBlitNearestFragmentShader[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;					" WG_GL_PALETTE_FUNC
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   color = paletteLookup(texture(texId, texUV).r) * fragColor;	"
"   color *= inside(texUV);	"
"}												";

const char GlBackend::paletteBlitNearestFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;						" WG_GL_PALETTE_FUNC
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   color.r = paletteLookup(texture(texId, texUV).r).a * fragColor.a;"
"   color.r *= inside(texUV);	"
"}												";


const char GlBackend::paletteBlitNearestFragmentShaderTintmap[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform samplerBuffer tintmapBufferId;			"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;					" WG_GL_PALETTE_FUNC
"in vec2 texUV;									"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   vec4 fragColor = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint); "

"   color = paletteLookup(texture(texId, texUV).r) * fragColor;	"
"   color *= inside(texUV);	"
"}												";

const char GlBackend::paletteBlitNearestFragmentShaderTintmap_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform samplerBuffer tintmapBufferId;			"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;						" WG_GL_PALETTE_FUNC
"in vec2 texUV;									"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   float fragA = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint).a; "
"   color.r = paletteLookup(texture(texId, texUV).r).a * fragA;"
"   color.r *= inside(texUV);	"
"}												";



const char GlBackend::paletteBlitInterpolateVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer extrasBufferId;						   "
"uniform samplerBuffer colorBufferId;					   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in vec2 texSize;					   "
"layout(location = 2) in int colorOfs;                    "
"layout(location = 3) in int extrasOfs;                    "
"out vec2 uvFrac;                                         "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasBufferId, extrasOfs);		   "
"   vec4 transform = texelFetch(extrasBufferId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w; "

//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.y - dst.y) * transform.w + (pos.x - dst.x) * transform.y; "

"   vec2 texUV = src + (pos-dst) * transform.xw + (pos.yx - dst.yx) * transform.zy;"
"   texUV -= 0.5f;"

"   uvFrac = texUV;"
"   fragColor = texelFetch(colorBufferId, colorOfs);				"
"}                                                         ";


const char GlBackend::paletteBlitInterpolateTintmapVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;						   "
"layout(location = 2) in int colorOfs;                     "
"uniform samplerBuffer extrasBufferId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in vec2 texSize;					   "
"layout(location = 3) in int extrasOfs;                    "
"layout(location = 4) in vec2 tintmapOfs;                  "
"out vec2 uvFrac;                                         "
"flat out int tintOfs;  out vec2 tintPos;  "
"flat out vec4 flatTint;                                   "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasBufferId, extrasOfs);		   "
"   vec4 transform = texelFetch(extrasBufferId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w; "

//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.y - dst.y) * transform.w + (pos.x - dst.x) * transform.y; "

"   vec2 texUV = src + (pos-dst) * transform.xw + (pos.yx - dst.yx) * transform.zy;"
"   texUV -= 0.5f;"

"   uvFrac = texUV;"
"   tintOfs = int(tintmapOfs.x);  tintPos = vec2(pos);  "
"   flatTint = texelFetch(colorBufferId, colorOfs);        "
"}                                                         ";







const char GlBackend::paletteBlitInterpolateFragmentShader[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;						" WG_GL_PALETTE_FUNC
"in vec2 uvFrac;								"
"in vec4 fragColor;								"
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   vec2 uvBase = floor(uvFrac);	"		// Texels and weights from the same value, so they always agree.
"   vec2 uvf = uvFrac - uvBase;	"
"   vec2 texSize = vec2(textureSize(texId, 0));	"
"   vec2 texUV00 = (uvBase + 0.5) / texSize;	"		// Centers of the texels, which no rounding error can move.
"   vec2 texUV11 = (uvBase + 1.5) / texSize;	"
"   float index00 = texture(texId, texUV00).r;		"
"   float index01 = texture(texId, vec2(texUV11.x,texUV00.y) ).r;		"
"   float index10 = texture(texId, vec2(texUV00.x,texUV11.y) ).r;		"
"   float index11 = texture(texId, texUV11).r;		"
"   vec4 color00 = paletteLookup(index00) * inside(texUV00);	"
"   vec4 color01 = paletteLookup(index01) * inside(vec2(texUV11.x,texUV00.y));	"
"   vec4 color10 = paletteLookup(index10) * inside(vec2(texUV00.x,texUV11.y));	"
"   vec4 color11 = paletteLookup(index11) * inside(texUV11);	"

"   vec4 out0 = color00 * (1-uvf.x) + color01 * uvf.x;	"
"   vec4 out1 = color10 * (1-uvf.x) + color11 * uvf.x;	"
"   color = (out0 * (1-uvf.y) + out1 * uvf.y) * fragColor;	"
"}												";

const char GlBackend::paletteBlitInterpolateFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;						" WG_GL_PALETTE_FUNC
"in vec2 uvFrac;								"
"in vec4 fragColor;								"
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   vec2 uvBase = floor(uvFrac);	"		// Texels and weights from the same value, so they always agree.
"   vec2 uvf = uvFrac - uvBase;	"
"   vec2 texSize = vec2(textureSize(texId, 0));	"
"   vec2 texUV00 = (uvBase + 0.5) / texSize;	"		// Centers of the texels, which no rounding error can move.
"   vec2 texUV11 = (uvBase + 1.5) / texSize;	"
"   float index00 = texture(texId, texUV00).r;		"
"   float index01 = texture(texId, vec2(texUV11.x,texUV00.y) ).r;		"
"   float index10 = texture(texId, vec2(texUV00.x,texUV11.y) ).r;		"
"   float index11 = texture(texId, texUV11).r;		"
"   float color00 = paletteLookup(index00).a * inside(texUV00);	"
"   float color01 = paletteLookup(index01).a * inside(vec2(texUV11.x,texUV00.y));	"
"   float color10 = paletteLookup(index10).a * inside(vec2(texUV00.x,texUV11.y));	"
"   float color11 = paletteLookup(index11).a * inside(texUV11);	"

"   float out0 = color00 * (1-uvf.x) + color01 * uvf.x;	"
"   float out1 = color10 * (1-uvf.x) + color11 * uvf.x;	"
"   color.r = (out0 * (1-uvf.y) + out1 * uvf.y) * fragColor.a;	"
"}												";


const char GlBackend::paletteBlitInterpolateFragmentShaderTintmap[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform samplerBuffer tintmapBufferId;			"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;						" WG_GL_PALETTE_FUNC
"in vec2 uvFrac;								"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   vec2 uvBase = floor(uvFrac);	"		// Texels and weights from the same value, so they always agree.
"   vec2 uvf = uvFrac - uvBase;	"
"   vec2 texSize = vec2(textureSize(texId, 0));	"
"   vec2 texUV00 = (uvBase + 0.5) / texSize;	"		// Centers of the texels, which no rounding error can move.
"   vec2 texUV11 = (uvBase + 1.5) / texSize;	"
"   float index00 = texture(texId, texUV00).r;		"
"   float index01 = texture(texId, vec2(texUV11.x,texUV00.y) ).r;		"
"   float index10 = texture(texId, vec2(texUV00.x,texUV11.y) ).r;		"
"   float index11 = texture(texId, texUV11).r;		"
"   vec4 color00 = paletteLookup(index00) * inside(texUV00);	"
"   vec4 color01 = paletteLookup(index01) * inside(vec2(texUV11.x,texUV00.y));	"
"   vec4 color10 = paletteLookup(index10) * inside(vec2(texUV00.x,texUV11.y));	"
"   vec4 color11 = paletteLookup(index11) * inside(texUV11);	"

"   vec4 out0 = color00 * (1-uvf.x) + color01 * uvf.x;	"
"   vec4 out1 = color10 * (1-uvf.x) + color11 * uvf.x;	"

"   vec4 fragColor = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint); "

"   color = (out0 * (1-uvf.y) + out1 * uvf.y) * fragColor;	"
"}												";

const char GlBackend::paletteBlitInterpolateFragmentShaderTintmap_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform samplerBuffer tintmapBufferId;			"
"uniform int clipToSource;					"	// Set for a ClipBlit: nothing outside the source is read.
"uniform sampler2D paletteId;						" WG_GL_PALETTE_FUNC
"in vec2 uvFrac;								"
"flat in int tintOfs;  in vec2 tintPos;  " "flat in vec4 flatTint;  " WG_GL_TINT_FUNC
"out vec4 color;								"
"float inside(vec2 uv)							"
"{												"
"   return (clipToSource != 0 && (any(lessThan(uv, vec2(0.0))) || any(greaterThanEqual(uv, vec2(1.0))))) ? 0.0 : 1.0;	"
"}												"

"void main()									"
"{												"
"   vec2 uvBase = floor(uvFrac);	"		// Texels and weights from the same value, so they always agree.
"   vec2 uvf = uvFrac - uvBase;	"
"   vec2 texSize = vec2(textureSize(texId, 0));	"
"   vec2 texUV00 = (uvBase + 0.5) / texSize;	"		// Centers of the texels, which no rounding error can move.
"   vec2 texUV11 = (uvBase + 1.5) / texSize;	"
"   float index00 = texture(texId, texUV00).r;		"
"   float index01 = texture(texId, vec2(texUV11.x,texUV00.y) ).r;		"
"   float index10 = texture(texId, vec2(texUV00.x,texUV11.y) ).r;		"
"   float index11 = texture(texId, texUV11).r;		"
"   float color00 = paletteLookup(index00).a * inside(texUV00);	"
"   float color01 = paletteLookup(index01).a * inside(vec2(texUV11.x,texUV00.y));	"
"   float color10 = paletteLookup(index10).a * inside(vec2(texUV00.x,texUV11.y));	"
"   float color11 = paletteLookup(index11).a * inside(texUV11);	"

"   float out0 = color00 * (1-uvf.x) + color01 * uvf.x;	"
"   float out1 = color10 * (1-uvf.x) + color11 * uvf.x;	"

"   float fragA = (evalTint(tintmapBufferId, tintOfs, tintPos) * flatTint).a; "

"   color.r = (out0 * (1-uvf.y) + out1 * uvf.y) * fragA;	"
"}												";


const char GlBackend::lineFromToVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;					"
"uniform samplerBuffer extrasBufferId;					"

"layout(location = 0) in ivec2 pos;                     "
"layout(location = 2) in int colorOfs;                  "
"layout(location = 3) in int extrasOfs;                 "

"out vec4 fragColor;                                    "
"flat out float s;										"
"flat out float w;										"
"flat out float slope;									"
"flat out float ifSteep;								"
"flat out float ifMild;									"
"void main()                                            "
"{                                                      "
"   gl_Position.x = pos.x*2.0/canvasWidth - 1.0;        "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2.0/canvasHeight - 1.0;"
"   gl_Position.z = 0.0;                                "
"   gl_Position.w = 1.0;                                "
"   fragColor = texelFetch(colorBufferId, colorOfs);	"

"   vec4 x = texelFetch(extrasBufferId, extrasOfs);		"
"   s = x.x;											"
"   w = x.y;											"
"   slope = canvasYMul*x.z;								"
"   ifSteep = x.w;										"
"   ifMild = 1.0 - ifSteep;								"
"}                                                      ";


const char GlBackend::lineFromToFragmentShader[] =

"#version 330 core\n"
"in vec4 fragColor;                     "
"flat in float s;							"
"flat in float w;							"
"flat in float slope;						"
"flat in float ifSteep;						"
"flat in float ifMild;						"
"out vec4 outColor;                     "
"void main()                            "
"{										"
"   outColor.rgb = fragColor.rgb;		"
"   outColor.a = fragColor.a * clamp(w - abs(gl_FragCoord.x*ifSteep + gl_FragCoord.y*ifMild - s - (gl_FragCoord.x*ifMild + gl_FragCoord.y*ifSteep) * slope), 0.0, 1.0); "
"}                                      ";

const char GlBackend::lineFromToFragmentShader_A8[] =

"#version 330 core\n"
"in vec4 fragColor;                     "
"flat in float s;							"
"flat in float w;							"
"flat in float slope;						"
"flat in float ifSteep;						"
"flat in float ifMild;						"
"out vec4 outColor;                     "
"void main()                            "
"{										"
"   outColor.r = fragColor.a * clamp(w - abs(gl_FragCoord.x*ifSteep + gl_FragCoord.y*ifMild - s - (gl_FragCoord.x*ifMild + gl_FragCoord.y*ifSteep) * slope), 0.0, 1.0); "
"}                                      ";



const char GlBackend::aaFillVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;					    "
"uniform samplerBuffer extrasBufferId;						"
"layout(location = 0) in ivec2 pos;                         "
"layout(location = 2) in int colorOfs;                      "
"layout(location = 3) in int extrasOfs;                     "
"out vec4 fragColor;                                        "
"flat out vec4 rect;										"
"void main()                                                "
"{                                                          "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;              "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                    "
"   gl_Position.w = 1.0;                                    "
"   fragColor = texelFetch(colorBufferId, colorOfs);		"
"   rect = texelFetch(extrasBufferId, extrasOfs);			"
"   rect.y = canvasYOfs + canvasYMul*rect.y;				"
"   rect.zw += vec2(0.5f,0.5f);								"		// Adding offset here so we don't have to do it in pixel shader.
"}                                                          ";


const char GlBackend::aaFillTintmapVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;					    "
"uniform samplerBuffer extrasBufferId;						"
"layout(location = 0) in ivec2 pos;                         "
"layout(location = 2) in int colorOfs;                      "
"layout(location = 3) in int extrasOfs;                     "
"layout(location = 4) in vec2 tintmapOfs;                   "
"out vec4 fragColor;                                        "
"flat out vec4 rect;										"
"flat out int tintOfs;  out vec2 tintPos;  "
"void main()                                                "
"{                                                          "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;              "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                    "
"   gl_Position.w = 1.0;                                    "
"   fragColor = texelFetch(colorBufferId, colorOfs);		"
"   rect = texelFetch(extrasBufferId, extrasOfs);			"
"   rect.y = canvasYOfs + canvasYMul*rect.y;				"
"   rect.zw += vec2(0.5f,0.5f);								"		// Adding offset here so we don't have to do it in pixel shader.
"   tintOfs = int(tintmapOfs.x);  tintPos = vec2(pos);  "
"}                                                          ";





const char GlBackend::aaFillFragmentShader[] =

"#version 330 core\n"
"in vec4 fragColor;						"
"flat in vec4 rect;						"
"out vec4 outColor;                     "
"void main()                            "
"{										"
"   outColor.rgb = fragColor.rgb;             "
"	vec2 middleofs = abs(gl_FragCoord.xy - rect.xy);   "
"	vec2 alphas = clamp(rect.zw  - middleofs, 0.f, 1.f);  "
"	outColor.a = fragColor.a * alphas.x * alphas.y;  "
"}                                      ";

const char GlBackend::aaFillFragmentShader_A8[] =

"#version 330 core\n"
"in vec4 fragColor;						"
"flat in vec4 rect;						"
"out vec4 outColor;                     "
"void main()                            "
"{										"
"	vec2 middleofs = abs(gl_FragCoord.xy - rect.xy);   "
"	vec2 alphas = clamp(rect.zw  - middleofs, 0.f, 1.f);  "
"	outColor.r = fragColor.a * alphas.x * alphas.y;  "
"}                                      ";


const char GlBackend::aaFillFragmentShaderTintmap[] =

"#version 330 core\n"
"uniform samplerBuffer tintmapBufferId;	"
"in vec4 fragColor;						"
"flat in int tintOfs;  in vec2 tintPos;  " WG_GL_TINT_FUNC
"flat in vec4 rect;						"
"out vec4 outColor;                     "
"void main()                            "
"{										"
"   vec4 inColor = evalTint(tintmapBufferId, tintOfs, tintPos)"
"	           * fragColor;           "
"   outColor.rgb = inColor.rgb;             "
"	vec2 middleofs = abs(gl_FragCoord.xy - rect.xy);   "
"	vec2 alphas = clamp(rect.zw  - middleofs, 0.f, 1.f);  "
"	outColor.a = inColor.a * alphas.x * alphas.y;  "
"}                                      ";

const char GlBackend::aaFillFragmentShaderTintmap_A8[] =

"#version 330 core\n"
"uniform samplerBuffer tintmapBufferId;	"
"in vec4 fragColor;						"
"flat in int tintOfs;  in vec2 tintPos;  " WG_GL_TINT_FUNC
"flat in vec4 rect;						"
"out vec4 outColor;                     "
"void main()                            "
"{										"
"   float inAlpha = evalTint(tintmapBufferId, tintOfs, tintPos).a"
"	           * fragColor.a;           "
"	vec2 middleofs = abs(gl_FragCoord.xy - rect.xy);   "
"	vec2 alphas = clamp(rect.zw  - middleofs, 0.f, 1.f);  "
"	outColor.r = inAlpha * alphas.x * alphas.y;  "
"}                                      ";


const char GlBackend::segmentsVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer extrasBufferId;					"
"uniform samplerBuffer colorBufferId;					"
"layout(location = 2) in int colorOfs;                   "
"layout(location = 0) in ivec2 pos;                     "
"layout(location = 1) in vec2 uv;					    "
"layout(location = 3) in int extrasOfs;                 "
"layout(location = 4) in vec2 tintmapOfs;               "
"out vec2 texUV;										"
"flat out int edgemapPitch;								"
"flat out int flatColorsOfs;							"
"flat out int tintTableOfs;								"
"flat out int tintOfs;									"
"out vec2 tintPos;										"
"flat out vec4 flatTint;								"

"void main()											"
"{                                                      "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;              "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                    "
"   gl_Position.w = 1.0;                                    "

"   vec4 extras = texelFetch(extrasBufferId, extrasOfs);		"
"   edgemapPitch = int(extras.x);						"
"   flatColorsOfs = int(extras.y);						"
"   tintTableOfs = int(extras.z);						"
"   texUV = uv;											"
"   tintOfs = int(tintmapOfs.x);"
"   tintPos = vec2(pos);"
"   flatTint = texelFetch(colorBufferId, colorOfs);"
"}                                                      ";

// Segment colors are either flat or a tint in edgemap space. The edgemap buffer holds a
// table with the offset of each segment's tint block, -1 for flat colored segments.

#define WG_GL_SEGCOLOR_FUNC \
"vec4 segColor(int seg, int flatColorsOfs, int tintTableOfs, vec2 epos)			" \
"{																				" \
"	int tofs = int(texelFetch(edgemapId, tintTableOfs + seg).x);					" \
"	return tofs < 0 ? texelFetch(edgemapId, flatColorsOfs + seg) : evalTint(edgemapId, tofs, epos);	" \
"}																				"

const char GlBackend::segmentsFragmentShader[] =

"#version 330 core\n"
"uniform samplerBuffer tintmapBufferId;			"
"uniform samplerBuffer edgemapId;				"
"in vec2 texUV;									"
"flat in int edgemapPitch;						"
"flat in int flatColorsOfs;						"
"flat in int tintTableOfs;						"
"flat in int tintOfs;							"
"in vec2 tintPos;								"
"flat in vec4 flatTint;							"
WG_GL_TINT_FUNC
WG_GL_SEGCOLOR_FUNC

"out vec4 color;								"
"void main()									"
"{												"
"	float totalAlpha = 0.f;"
"	vec3	rgbAcc = vec3(0,0,0);"

"	float factor = 1.f;"

"	vec2 epos = vec2(floor(texUV.x) + 0.5, texUV.y + 0.5);"		// Pixel center in edgemap space.

"	for( int i = 0 ; i < $EDGES ; i++ )"
"	{"

"		vec4 col = segColor(i, flatColorsOfs, tintTableOfs, epos);"

"		vec4 edge = texelFetch(edgemapId, int(texUV.x)*edgemapPitch+i );"

"		float x = (texUV.y - edge.r) * edge.g;"
"		float adder = edge.g / 2.f;"
"		if (x < 0.f)"
"			adder = edge.b;"
"		else if (x + edge.g > 1.f)"
"			adder = edge.a;"
"		float factor2 = clamp(x + adder, 0.f, 1.f);"

"		float useFactor = (factor - factor2)*col.a;"
"		totalAlpha += useFactor;"
"		rgbAcc += col.rgb * useFactor;"

"		factor = factor2;"
"	}"

"	vec4 col = segColor($EDGES, flatColorsOfs, tintTableOfs, epos);"

"	float useFactor = factor*col.a;"
"	totalAlpha += useFactor;"
"	rgbAcc += col.rgb * useFactor;"

"   col.a = totalAlpha; "
"   col.rgb = totalAlpha > 0.0 ? rgbAcc/totalAlpha : vec3(0.0);"

"   color = (tintOfs >= 0 ? evalTint(tintmapBufferId, tintOfs, tintPos) * col : col) * flatTint;"
"}";


const char GlBackend::segmentsFragmentShader_A8[] =

"#version 330 core\n"
"uniform samplerBuffer tintmapBufferId;	"
"uniform samplerBuffer edgemapId;				"
"in vec2 texUV;									"
"flat in int edgemapPitch;						"
"flat in int flatColorsOfs;						"
"flat in int tintTableOfs;						"
"flat in int tintOfs;							"
"in vec2 tintPos;								"
"flat in vec4 flatTint;							"
WG_GL_TINT_FUNC
WG_GL_SEGCOLOR_FUNC

"out vec4 color;								"
"void main()									"
"{												"
"	float totalAlpha = 0.f;"

"	float factor = 1.f;"

"	vec2 epos = vec2(floor(texUV.x) + 0.5, texUV.y + 0.5);"

"	for( int i = 0 ; i < $EDGES ; i++ )"
"	{"

"		vec4 col = segColor(i, flatColorsOfs, tintTableOfs, epos);"

"		vec4 edge = texelFetch(edgemapId, int(texUV.x)*edgemapPitch+i );"

"		float x = (texUV.y - edge.r) * edge.g;"
"		float adder = edge.g / 2.f;"
"		if (x < 0.f)"
"			adder = edge.b;"
"		else if (x + edge.g > 1.f)"
"			adder = edge.a;"
"		float factor2 = clamp(x + adder, 0.f, 1.f);"

"		float useFactor = (factor - factor2)*col.a;"
"		totalAlpha += useFactor;"

"		factor = factor2;"
"	}"

"	vec4 col = segColor($EDGES, flatColorsOfs, tintTableOfs, epos);"
"	float useFactor = factor*col.a;"
"	totalAlpha += useFactor;"

"   color.r = (tintOfs >= 0 ? evalTint(tintmapBufferId, tintOfs, tintPos).a : 1.0) * totalAlpha * flatTint.a;"
"}";


// Shapes (Command::DrawShape), see GpuShapes for the entries read from the extras buffer. The
// coverage of each pixel by each segment is worked out from the geometry. Partly covered
// pixels get the colors of the drawn segments covering them mixed, weighted by coverage and
// alpha, like edgemaps. Pixels not covered by any drawn segment are left alone.

const char GlBackend::shapeVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	float  canvasWidth;"
"	float  canvasHeight;"
"	float  canvasYOfs;"
"	float  canvasYMul;"
"};"

"uniform samplerBuffer colorBufferId;					"
"layout(location = 0) in ivec2 pos;                     "
"layout(location = 2) in int colorOfs;                  "
"layout(location = 3) in int extrasOfs;                 "
"layout(location = 4) in vec2 tintmapOfs;               "
"flat out int shapeOfs;									"
"flat out int tintOfs;									"
"out vec2 canvasPos;									"
"flat out vec4 flatTint;								"

"void main()											"
"{                                                      "
"   gl_Position.x = pos.x*2/canvasWidth - 1.0;              "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasHeight - 1.0;    "
"   gl_Position.z = 0.0;                                    "
"   gl_Position.w = 1.0;                                    "
"   shapeOfs = extrasOfs;								"
"   tintOfs = int(tintmapOfs.x);						"
"   canvasPos = vec2(pos);								"
"   flatTint = texelFetch(colorBufferId, colorOfs);		"
"}                                                      ";

// Coverage of the segments of a shape at a pixel center, accumulated as color times alpha
// and alpha, weighted by coverage, in acc, and the coverage of drawn segments in covered.
// Needs extrasBufferId, colorBufferId and WG_GL_TINT_FUNC.

#define WG_GL_SHAPE_FUNCS \
"vec4 shapeEntry(int i)																" \
"{																					" \
"	return texelFetch(extrasBufferId, i);											" \
"}																					" \
"float spanCoverage(float a, float b, float p)										" \
"{																					" \
"	return clamp(min(p + 0.5, b) - max(p - 0.5, a), 0.0, 1.0);						" \
"}																					" \
"float rectCoverage(vec4 r, vec2 p)													" \
"{																					" \
"	return spanCoverage(r.x, r.z, p.x) * spanCoverage(r.y, r.w, p.y);				" \
"}																					" \
"float halfPlaneCoverage(float s, vec2 n)											" \
"{																					" \
"	vec2 an = abs(n);																" \
"	float a = max(an.x, an.y);														" \
"	float b = min(an.x, an.y);														" \
"	if (b < 0.001)																	" \
"		return clamp(s + 0.5, 0.0, 1.0);											" \
"	float t = s + 0.5 * (a + b);													" \
"	if (t <= 0.0)																	" \
"		return 0.0;																	" \
"	if (t >= a + b)																	" \
"		return 1.0;																	" \
"	if (t < b)																		" \
"		return t * t / (2.0 * a * b);												" \
"	if (t > a)																		" \
"		return 1.0 - (a + b - t) * (a + b - t) / (2.0 * a * b);						" \
"	return (t - 0.5 * b) / a;														" \
"}																					" \
"float circleCoverage(float radius, vec2 v, float d)								" \
"{																					" \
"	if (radius <= 0.0)																" \
"		return 0.0;																	" \
"	return d > 0.001 ? halfPlaneCoverage(radius - d, v / d) : clamp(radius + 0.5, 0.0, 1.0);	" \
"}																					" \
"float roundedRectCoverage(vec4 r, vec4 radii, vec2 p)								" \
"{																					" \
"	float c = rectCoverage(r, p);													" \
"	vec2 center;																	" \
"	float radius;																	" \
"	if (p.x < r.x + radii.x && p.y < r.y + radii.x)									" \
"	{																				" \
"		radius = radii.x;															" \
"		center = vec2(r.x + radius, r.y + radius);									" \
"	}																				" \
"	else if (p.x > r.z - radii.y && p.y < r.y + radii.y)							" \
"	{																				" \
"		radius = radii.y;															" \
"		center = vec2(r.z - radius, r.y + radius);									" \
"	}																				" \
"	else if (p.x > r.z - radii.z && p.y > r.w - radii.z)							" \
"	{																				" \
"		radius = radii.z;															" \
"		center = vec2(r.z - radius, r.w - radius);									" \
"	}																				" \
"	else if (p.x < r.x + radii.w && p.y > r.w - radii.w)							" \
"	{																				" \
"		radius = radii.w;															" \
"		center = vec2(r.x + radius, r.w - radius);									" \
"	}																				" \
"	else																			" \
"		return c;																	" \
"	vec2 v = p - center;															" \
"	return min(c, circleCoverage(radius, v, length(v)));							" \
"}																					" \
"void addSegment(int seg, float c, vec2 p, inout vec4 acc, inout float covered)		" \
"{																					" \
"	if (c <= 0.0)																	" \
"		return;																		" \
"	vec4 s = shapeEntry(seg);														" \
"	if (s.x < 0.0)																	" \
"		return;																		" \
"	vec4 col = texelFetch(colorBufferId, int(s.x));									" \
"	if (s.y >= 0.0)																	" \
"		col *= evalTint(colorBufferId, int(s.y), p);								" \
"	acc += vec4(col.rgb * col.a, col.a) * c;										" \
"	covered += c;																	" \
"}																					" \
"void boxCoverage(int ofs, vec2 p, inout vec4 acc, inout float covered)				" \
"{																					" \
"	float outer = roundedRectCoverage(shapeEntry(ofs + 1), shapeEntry(ofs + 2), p);	" \
"	float fill = outer;																" \
"	if ((int(shapeEntry(ofs).y) & 1) != 0)											" \
"		fill = min(outer, roundedRectCoverage(shapeEntry(ofs + 3), shapeEntry(ofs + 4), p));	" \
"	addSegment(ofs + 5, fill, p, acc, covered);										" \
"	addSegment(ofs + 6, outer - fill, p, acc, covered);								" \
"}																					" \
"void ringElements(int ofs, int n, int base, float band, vec2 p, vec2 v, float d, inout vec4 acc, inout float covered)	" \
"{																					" \
"	if (n == 1)																		" \
"	{																				" \
"		addSegment(ofs + int(shapeEntry(base).y), band, p, acc, covered);			" \
"		return;																		" \
"	}																				" \
"	float theta = d > 0.0001 ? fract(atan(v.x, -v.y) * 0.15915494) : 0.0;			" \
"	int lo = 0;																		" \
"	int hi = n;																		" \
"	while (lo < hi)																	" \
"	{																				" \
"		int mid = (lo + hi) / 2;													" \
"		if (shapeEntry(base + mid).x <= theta)										" \
"			lo = mid + 1;															" \
"		else																		" \
"			hi = mid;																" \
"	}																				" \
"	int first = lo > 0 ? lo - 1 : n - 1;											" \
"	float w = min(0.25, 0.70710678 / (6.2831853 * max(d, 0.0001)));					" \
"	float behind = fract(theta - shapeEntry(base + first).x);						" \
"	for (int i = 0; i < n - 1 && behind < w; i++)									" \
"	{																				" \
"		int prev = first > 0 ? first - 1 : n - 1;									" \
"		behind += fract(shapeEntry(base + first).x - shapeEntry(base + prev).x);	" \
"		first = prev;																" \
"	}																				" \
"	float g = 1.0;																	" \
"	float ahead = -behind;															" \
"	int k = first;																	" \
"	vec4 element = shapeEntry(base + k);											" \
"	for (int i = 0; i < n; i++)														" \
"	{																				" \
"		int next = k + 1 < n ? k + 1 : 0;											" \
"		vec4 nextElement = shapeEntry(base + next);									" \
"		ahead += fract(nextElement.x - element.x);									" \
"		float gEnd = 0.0;															" \
"		if (i < n - 1 && ahead < w)													" \
"		{																			" \
"			float a = 6.2831853 * nextElement.x;									" \
"			gEnd = min(g, halfPlaneCoverage(-d * sin(6.2831853 * ahead), vec2(cos(a), sin(a))));	" \
"		}																			" \
"		addSegment(ofs + int(element.y), band * (g - gEnd), p, acc, covered);		" \
"		if (gEnd <= 0.0)															" \
"			break;																	" \
"		g = gEnd;																	" \
"		k = next;																	" \
"		element = nextElement;														" \
"	}																				" \
"}																					" \
"void ringCoverage(int ofs, vec2 p, inout vec4 acc, inout float covered)			" \
"{																					" \
"	vec4 head = shapeEntry(ofs);													" \
"	int flags = int(head.y);														" \
"	vec4 circles = shapeEntry(ofs + 1);												" \
"	vec4 inner = shapeEntry(ofs + 2);												" \
"	vec4 rect = shapeEntry(ofs + 3);												" \
"	vec2 v = p - circles.xy;														" \
"	float d = length(v);															" \
"	bool bSquare = (flags & 1) != 0;												" \
"	float c0 = bSquare ? rectCoverage(rect, p) : circleCoverage(circles.z, v, d);	" \
"	float c1 = min(bSquare ? rectCoverage(shapeEntry(ofs + 4), p) : circleCoverage(circles.w, v, d), c0);	" \
"	float c2 = min(circleCoverage(inner.x, v, d), c1);								" \
"	float c3 = min(circleCoverage(inner.y, v, d), c2);								" \
"	float back = (flags & 2) != 0 ? max(rectCoverage(rect, p) - c0, 0.0) : 0.0;		" \
"	float outline = c0 - c1 + c2 - c3;												" \
"	float band = c1 - c2;															" \
"	if ((flags & 16) == 0)															" \
"	{																				" \
"		outline += band;															" \
"		band = 0.0;																	" \
"	}																				" \
"	addSegment(ofs + 5, back, p, acc, covered);										" \
"	addSegment(ofs + 6, outline, p, acc, covered);									" \
"	addSegment(ofs + 7, (flags & 4) != 0 ? c3 : 0.0, p, acc, covered);				" \
"	if (band > 0.0)																	" \
"		ringElements(ofs, int(head.z), ofs + int(head.w), band, p, v, d, acc, covered);	" \
"}																					" \
"float shapeCoverage(int ofs, vec2 p, out vec4 acc)									" \
"{																					" \
"	acc = vec4(0.0);																" \
"	float covered = 0.0;															" \
"	int kind = int(shapeEntry(ofs).x);												" \
"	if (kind == 1)																	" \
"		boxCoverage(ofs, p, acc, covered);											" \
"	else if (kind == 2)																" \
"		ringCoverage(ofs, p, acc, covered);											" \
"	return covered;																	" \
"}																					"

const char GlBackend::shapeFragmentShader[] =

"#version 330 core\n"
"uniform samplerBuffer extrasBufferId;			"
"uniform samplerBuffer colorBufferId;			"
"uniform samplerBuffer tintmapBufferId;			"
"flat in int shapeOfs;							"
"flat in int tintOfs;							"
"in vec2 canvasPos;								"
"flat in vec4 flatTint;							"
WG_GL_TINT_FUNC
WG_GL_SHAPE_FUNCS

"out vec4 color;								"
"void main()									"
"{												"
"	vec4 acc;									"
"	if (shapeCoverage(shapeOfs, canvasPos, acc) < 1.0 / 256.0)	"
"		discard;								"
"	vec4 col = vec4(acc.a > 0.0 ? acc.rgb / acc.a : vec3(0.0), acc.a);	"
"	color = (tintOfs >= 0 ? evalTint(tintmapBufferId, tintOfs, canvasPos) * col : col) * flatTint;	"
"}";

const char GlBackend::shapeFragmentShader_A8[] =

"#version 330 core\n"
"uniform samplerBuffer extrasBufferId;			"
"uniform samplerBuffer colorBufferId;			"
"uniform samplerBuffer tintmapBufferId;			"
"flat in int shapeOfs;							"
"flat in int tintOfs;							"
"in vec2 canvasPos;								"
"flat in vec4 flatTint;							"
WG_GL_TINT_FUNC
WG_GL_SHAPE_FUNCS

"out vec4 color;								"
"void main()									"
"{												"
"	vec4 acc;									"
"	if (shapeCoverage(shapeOfs, canvasPos, acc) < 1.0 / 256.0)	"
"		discard;								"
"	color.r = (tintOfs >= 0 ? evalTint(tintmapBufferId, tintOfs, canvasPos).a : 1.0) * acc.a * flatTint.a;	"
"}";




/*
const char GlBackend::segmentsFragmentShader_A8[] =

"#version 330 core\n"
"uniform samplerBuffer stripesId;				"
"uniform sampler2D	paletteId;					"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"flat in int segments;							"
"flat in int stripesOfs;						"
"in vec2 paletteOfs;"

"out vec4 color;								"
"void main()									"
"{												"
"	float totalAlpha = 0.f;"

"	float factor = 1.f;"
"   vec2 palOfs = paletteOfs;"
"	for( int i = 0 ; i < $EDGES ; i++ )"
"	{"
"  		vec4 col = texture(paletteId, palOfs);"
"  		palOfs.x += 1/$MAXSEG.f;"

"		vec4 edge = texelFetch(stripesId, stripesOfs + int(texUV.x)*(segments-1)+i );"

"		float x = (texUV.y - edge.r) * edge.g;"
"		float adder = edge.g / 2.f;"
"		if (x < 0.f)"
"			adder = edge.b;"
"		else if (x + edge.g > 1.f)"
"			adder = edge.a;"
"		float factor2 = clamp(x + adder, 0.f, 1.f);"

"		float useFactor = (factor - factor2)*col.a;"
"		totalAlpha += useFactor;"

"		factor = factor2;"
"	}"

"  	vec4 col = texture(paletteId, palOfs);"
"	float useFactor = factor*col.a;"
"	totalAlpha += useFactor;"

"   color.r = totalAlpha * fragColor.a; "
"}";
*/

}

#endif //WG_GLSHADERS_DOT_H
