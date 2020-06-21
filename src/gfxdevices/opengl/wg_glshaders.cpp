/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/
#ifndef WG_GLSHADERS_DOT_H
#define WG_GLSHADERS_DOT_H

#include <wg_glgfxdevice.h>  

namespace wg {


const char GlGfxDevice::fillVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"


"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   fragColor = flatTint * texelFetch(extrasId, extrasOfs);		   "
"}                                                         ";

const char GlGfxDevice::fillGradientVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"


"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;    "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "

"	vec2 tintOfs = (pos - tintRectPos) / vec2(tintRectSize); "
"   vec4 lineStartTint = topLeftTint + (bottomLeftTint - topLeftTint) * tintOfs.y;"
"   vec4 lineEndTint = topRightTint + (bottomRightTint - topRightTint) * tintOfs.y;"
"   vec4 gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;"

"   fragColor = flatTint * gradientTint * texelFetch(extrasId, extrasOfs);		   "
"}                                                         ";



const char GlGfxDevice::fillFragmentShader[] =

"#version 330 core\n"
"out vec4 outColor;                     "
"in vec4 fragColor;                         "
"void main()                            "
"{                                      "
"   outColor = fragColor;                   "
"}                                      ";

const char GlGfxDevice::fillFragmentShader_A8[] =

"#version 330 core\n"
"out vec4 outColor;                     "
"in vec4 fragColor;                         "
"void main()                            "
"{                                      "
"   outColor.r = fragColor.a;                   "
"}                                      ";



const char GlGfxDevice::blitVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"   ivec2  texSize;"
"};"


"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec2 texUV;                                           "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasId, extrasOfs);	   "
"   vec4 transform = texelFetch(extrasId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   fragColor = flatTint;									"
"}                                                         ";

const char GlGfxDevice::blitGradientVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"   ivec2  texSize;"
"};"


"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec2 texUV;                                           "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasId, extrasOfs);	   "
"   vec4 transform = texelFetch(extrasId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.
"   texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "      //TODO: Replace this ugly +0.02f fix with whatever is correct.

"	vec2 tintOfs = (pos - tintRectPos) / vec2(tintRectSize); "
"   vec4 lineStartTint = topLeftTint + (bottomLeftTint - topLeftTint) * tintOfs.y;"
"   vec4 lineEndTint = topRightTint + (bottomRightTint - topRightTint) * tintOfs.y;"
"   vec4 gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;"

"   fragColor = flatTint * gradientTint;		   "
"}                                                         ";


const char GlGfxDevice::blitFragmentShader[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color = texture(texId, texUV) * fragColor;  "
"}												";

const char GlGfxDevice::blitFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color.r = texture(texId, texUV).a * fragColor.a;  "
"}												";


const char GlGfxDevice::alphaBlitFragmentShader[] =

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

const char GlGfxDevice::alphaBlitFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color.r = fragColor.a * texture(texId, texUV).r;         "
"}												";



const char GlGfxDevice::clutBlitNearestVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"   ivec2  texSize;"
"};"

"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec2 texUV;                                           "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasId, extrasOfs);		   "
"   vec4 transform = texelFetch(extrasId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "
"   texUV.y = (src.y + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "
"   fragColor = flatTint;		   "
"}                                                         ";

const char GlGfxDevice::clutBlitNearestGradientVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"   ivec2  texSize;"
"};"

"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec2 texUV;                                           "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasId, extrasOfs);		   "
"   vec4 transform = texelFetch(extrasId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
"   texUV.x = (src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "
"   texUV.y = (src.y + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "

"	vec2 tintOfs = (pos - tintRectPos) / vec2(tintRectSize); "
"   vec4 lineStartTint = topLeftTint + (bottomLeftTint - topLeftTint) * tintOfs.y;"
"   vec4 lineEndTint = topRightTint + (bottomRightTint - topRightTint) * tintOfs.y;"
"   vec4 gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;"

"   fragColor = flatTint * gradientTint;		   "

"}                                                         ";


const char GlGfxDevice::clutBlitNearestFragmentShader[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform sampler2D clutId;						"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color = texture(clutId, vec2(texture(texId, texUV).r,0.5f)) * fragColor;	"
"}												";

const char GlGfxDevice::clutBlitNearestFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform sampler2D clutId;						"
"in vec2 texUV;									"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   color.r = texture(clutId, vec2(texture(texId, texUV).r,0.5f)).a * fragColor.a;	"
"}												";


const char GlGfxDevice::clutBlitInterpolateVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"   ivec2  texSize;"
"};"

"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec2 texUV00;                                         "
"out vec2 texUV11;                                         "
"out vec2 uvFrac;                                         "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasId, extrasOfs);		   "
"   vec4 transform = texelFetch(extrasId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w; "

//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.y - dst.y) * transform.w + (pos.x - dst.x) * transform.y; "

"   vec2 texUV = src + (pos-dst) * transform.xw + (pos.yx - dst.yx) * transform.zy;"
"   texUV -= 0.5f;"

"   uvFrac = texUV;"
"   texUV00 = texUV/texSize;				"
"   texUV11 = (texUV+1)/texSize;			"
"   fragColor = flatTint;		   "
"}                                                         ";

const char GlGfxDevice::clutBlitInterpolateGradientVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"   ivec2  texSize;"
"};"

"uniform samplerBuffer extrasId;						   "
"layout(location = 0) in ivec2 pos;                        "
"layout(location = 1) in int extrasOfs;                    "
"out vec2 texUV00;                                         "
"out vec2 texUV11;                                         "
"out vec2 uvFrac;                                         "
"out vec4 fragColor;                                       "
"void main()                                               "
"{                                                         "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;            "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                   "
"   gl_Position.w = 1.0;                                   "
"   vec4 srcDst = texelFetch(extrasId, extrasOfs);		   "
"   vec4 transform = texelFetch(extrasId, extrasOfs+1);	   "
"   vec2 src = srcDst.xy;                                  "
"   vec2 dst = srcDst.zw;                                  "
//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w; "

//"   float texU = src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z; "
//"   float texV = src.y + (pos.y - dst.y) * transform.w + (pos.x - dst.x) * transform.y; "

"   vec2 texUV = src + (pos-dst) * transform.xw + (pos.yx - dst.yx) * transform.zy;"
"   texUV -= 0.5f;"

"   uvFrac = texUV;"
"   texUV00 = texUV/texSize;				"
"   texUV11 = (texUV+1)/texSize;			"

"	vec2 tintOfs = (pos - tintRectPos) / vec2(tintRectSize); "
"   vec4 lineStartTint = topLeftTint + (bottomLeftTint - topLeftTint) * tintOfs.y;"
"   vec4 lineEndTint = topRightTint + (bottomRightTint - topRightTint) * tintOfs.y;"
"   vec4 gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;"

"   fragColor = flatTint * gradientTint;		   "

"}                                                         ";


const char GlGfxDevice::clutBlitInterpolateFragmentShader[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform sampler2D clutId;						"
"in vec2 texUV00;								"
"in vec2 texUV11;								"
"in vec2 uvFrac;								"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   float index00 = texture(texId, texUV00).r;		"
"   float index01 = texture(texId, vec2(texUV11.x,texUV00.y) ).r;		"
"   float index10 = texture(texId, vec2(texUV00.x,texUV11.y) ).r;		"
"   float index11 = texture(texId, texUV11).r;		"
"   vec4 color00 = texture(clutId, vec2(index00,0.5f));	"
"   vec4 color01 = texture(clutId, vec2(index01,0.5f));	"
"   vec4 color10 = texture(clutId, vec2(index10,0.5f));	"
"   vec4 color11 = texture(clutId, vec2(index11,0.5f));	"

"   vec4 out0 = color00 * (1-fract(uvFrac.x)) + color01 * fract(uvFrac.x);	"
"   vec4 out1 = color10 * (1-fract(uvFrac.x)) + color11 * fract(uvFrac.x);	"
"   color = (out0 * (1-fract(uvFrac.y)) + out1 * fract(uvFrac.y)) * fragColor;	"
"}												";

const char GlGfxDevice::clutBlitInterpolateFragmentShader_A8[] =

"#version 330 core\n"

"uniform sampler2D texId;						"
"uniform sampler2D clutId;						"
"in vec2 texUV00;								"
"in vec2 texUV11;								"
"in vec2 uvFrac;								"
"in vec4 fragColor;								"
"out vec4 color;								"
"void main()									"
"{												"
"   float index00 = texture(texId, texUV00).r;		"
"   float index01 = texture(texId, vec2(texUV11.x,texUV00.y) ).r;		"
"   float index10 = texture(texId, vec2(texUV00.x,texUV11.y) ).r;		"
"   float index11 = texture(texId, texUV11).r;		"
"   float color00 = texture(clutId, vec2(index00,0.5f)).a;	"
"   float color01 = texture(clutId, vec2(index01,0.5f)).a;	"
"   float color10 = texture(clutId, vec2(index10,0.5f)).a;	"
"   float color11 = texture(clutId, vec2(index11,0.5f)).a;	"

"   float out0 = color00 * (1-fract(uvFrac.x)) + color01 * fract(uvFrac.x);	"
"   float out1 = color10 * (1-fract(uvFrac.x)) + color11 * fract(uvFrac.x);	"
"   color.r = (out0 * (1-fract(uvFrac.y)) + out1 * fract(uvFrac.y)) * fragColor.a;	"
"}												";



const char GlGfxDevice::plotVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"


"uniform samplerBuffer extrasId;						"
"layout(location = 0) in ivec2 pos;                     "
"layout(location = 1) in int extrasOfs;                 "
"out vec4 fragColor;									"
"void main()                                            "
"{                                                      "
"   gl_Position.x = (pos.x+0.5)*2.0/canvasSize.x - 1.0; "
"   gl_Position.y = ((canvasYOfs + canvasYMul*pos.y)+0.5)*2.0/canvasSize.y - 1,0;	"
"   gl_Position.z = 0.0;                                "
"   gl_Position.w = 1.0;                                "
"   fragColor = texelFetch(extrasId, extrasOfs);		   "
"}                                                      ";


const char GlGfxDevice::plotFragmentShader[] =

"#version 330 core\n"
"in vec4 fragColor;                     "
"out vec4 outColor;                     "
"void main()                            "
"{                                      "
"   outColor = fragColor;				"
"}                                      ";

const char GlGfxDevice::plotFragmentShader_A8[] =

"#version 330 core\n"
"in vec4 fragColor;                     "
"out vec4 outColor;                     "
"void main()                            "
"{                                      "
"   outColor.r = fragColor.a;				"
"}                                      ";



const char GlGfxDevice::lineFromToVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"


"uniform samplerBuffer extrasId;								"
"layout(location = 0) in ivec2 pos;                         "
"layout(location = 1) in int extrasOfs;                       "
"out vec4 fragColor;                                        "
"flat out float s;												"
"flat out float w;												"
"flat out float slope;											"
"flat out float ifSteep;											"
"flat out float ifMild;											"
"void main()                                                "
"{                                                          "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;             "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;             "
"   gl_Position.z = 0.0;                                    "
"   gl_Position.w = 1.0;                                    "
"   fragColor = texelFetch(extrasId, extrasOfs);			"
"   int ofs = extrasOfs+1;									"
"   vec4 x = texelFetch(extrasId, ofs);						"
"   s = x.x;												"
"   w = x.y;												"
"   slope = canvasYMul*x.z;										"
"   ifSteep = x.w;											"
"   ifMild = 1.0 - ifSteep;									"
"}                                                          ";


const char GlGfxDevice::lineFromToFragmentShader[] =

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

const char GlGfxDevice::lineFromToFragmentShader_A8[] =

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
"   outColor.t = fragColor.a * clamp(w - abs(gl_FragCoord.x*ifSteep + gl_FragCoord.y*ifMild - s - (gl_FragCoord.x*ifMild + gl_FragCoord.y*ifSteep) * slope), 0.0, 1.0); "
"}                                      ";


const char GlGfxDevice::aaFillVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"


"uniform samplerBuffer extrasId;								"
"layout(location = 0) in ivec2 pos;                         "
"layout(location = 1) in int extrasOfs;                       "
"out vec4 fragColor;                                        "
"flat out vec4 rect;										"
"void main()                                                "
"{                                                          "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;             "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;             "
"   gl_Position.z = 0.0;                                    "
"   gl_Position.w = 1.0;                                    "
"   fragColor = flatTint * texelFetch(extrasId, extrasOfs);			"
"   rect = texelFetch(extrasId, extrasOfs+1);				"
"   rect.y = canvasYOfs + canvasYMul*rect.y;							"
"   rect.zw += vec2(0.5f,0.5f);								"		// Adding offset here so we don't have to do it in pixel shader.
"}                                                          ";

const char GlGfxDevice::aaFillGradientVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"


"uniform samplerBuffer extrasId;								"
"layout(location = 0) in ivec2 pos;                         "
"layout(location = 1) in int extrasOfs;                       "
"out vec4 fragColor;                                        "
"flat out vec4 rect;										"
"void main()                                                "
"{                                                          "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;             "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;             "
"   gl_Position.z = 0.0;                                    "
"   gl_Position.w = 1.0;                                    "

"	vec2 tintOfs = (pos - tintRectPos) / vec2(tintRectSize); "
"   vec4 lineStartTint = topLeftTint + (bottomLeftTint - topLeftTint) * tintOfs.y;"
"   vec4 lineEndTint = topRightTint + (bottomRightTint - topRightTint) * tintOfs.y;"
"   vec4 gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;"

"   fragColor = flatTint * gradientTint * texelFetch(extrasId, extrasOfs);		   "

"   rect = texelFetch(extrasId, extrasOfs+1);				"
"   rect.y = canvasYOfs + canvasYMul*rect.y;							"
"   rect.zw += vec2(0.5f,0.5f);								"		// Adding offset here so we don't have to do it in pixel shader.
"}                                                          ";



const char GlGfxDevice::aaFillFragmentShader[] =

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

const char GlGfxDevice::aaFillFragmentShader_A8[] =

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


const char GlGfxDevice::segmentsVertexShader[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"

"uniform samplerBuffer extrasId;						"
"uniform samplerBuffer colorsId;					"
"layout(location = 0) in ivec2 pos;                     "
"layout(location = 1) in int extrasOfs;					"
"layout(location = 2) in vec2 uv;						"
"out vec2 texUV;										"
"out vec4 fragColor;                                    "
"flat out int segments;									"
"flat out int stripesOfs;								"
"out vec2 paletteOfs;"

"void main()											"
"{                                                      "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;         "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                "
"   gl_Position.w = 1.0;                                "
"   vec4 extras = texelFetch(extrasId, extrasOfs);		"
"   vec4 extras2 = texelFetch(extrasId, extrasOfs+1);	"		// x,y = first color ofs. z,w = texture width/height
"   segments = int(extras.x);							"
"   stripesOfs = int(extras.y);							"
"   texUV = uv;											"

"   float xTintOfs = uv.x/extras.z*1/extras2.z;"
"   float yTintOfs = uv.y/extras.w*1/extras2.w;"

"   paletteOfs = vec2(extras2.x+xTintOfs,extras2.y+yTintOfs);"
"   fragColor = flatTint;								"
"}                                                      ";

const char GlGfxDevice::segmentsVertexShaderGradient[] =

"#version 330 core\n"

"layout(std140) uniform Canvas"
"{"
"	vec2   canvasSize;"
"	int    canvasYOfs;"
"	int    canvasYMul;"
"   vec4   flatTint;"
"   ivec2  tintRectPos;"
"   ivec2  tintRectSize;"
"   vec4   topLeftTint;"
"   vec4   topRightTint;"
"   vec4   bottomRightTint;"
"   vec4   bottomLeftTint;"
"};"

"uniform samplerBuffer extrasId;						"
"uniform samplerBuffer colorsId;					"
"layout(location = 0) in ivec2 pos;                     "
"layout(location = 1) in int extrasOfs;					"
"layout(location = 2) in vec2 uv;						"
"out vec2 texUV;										"
"out vec4 fragColor;                                    "
"flat out int segments;									"
"flat out int stripesOfs;								"
"out vec2 paletteOfs;"

"void main()											"
"{                                                      "
"   gl_Position.x = pos.x*2/canvasSize.x - 1.0;         "
"   gl_Position.y = (canvasYOfs + canvasYMul*pos.y)*2/canvasSize.y - 1.0;            "
"   gl_Position.z = 0.0;                                "
"   gl_Position.w = 1.0;                                "
"   vec4 extras = texelFetch(extrasId, extrasOfs);		"
"   vec4 extras2 = texelFetch(extrasId, extrasOfs+1);	"		// x,y = first color ofs. z,w = texture width/height
"   segments = int(extras.x);							"
"   stripesOfs = int(extras.y);							"
"   texUV = uv;											"

"   float xTintOfs = uv.x/extras.z*1/extras2.z;"
"   float yTintOfs = uv.y/extras.w*1/extras2.w;"

"   paletteOfs = vec2(extras2.x+xTintOfs,extras2.y+yTintOfs);"

"	vec2 tintOfs = (pos - tintRectPos) / vec2(tintRectSize); "
"   vec4 lineStartTint = topLeftTint + (bottomLeftTint - topLeftTint) * tintOfs.y;"
"   vec4 lineEndTint = topRightTint + (bottomRightTint - topRightTint) * tintOfs.y;"
"   vec4 gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;"

"   fragColor = flatTint * gradientTint;		   "
"}                                                      ";

const char GlGfxDevice::segmentsFragmentShader[] =

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
"	vec3	rgbAcc = vec3(0,0,0);"

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
"		rgbAcc += col.rgb * useFactor;"

"		factor = factor2;"
"	}"

"  	vec4 col = texture(paletteId, palOfs);"
"	float useFactor = factor*col.a;"
"	totalAlpha += useFactor;"
"	rgbAcc += col.rgb * useFactor;"

"   color.a = totalAlpha * fragColor.a; "
"   color.rgb = (rgbAcc/totalAlpha) * fragColor.rgb;"
"}";

const char GlGfxDevice::segmentsFragmentShader_A8[] =

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


/*
static const char segmentsFragmentShader2[] =

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
"	vec4 edge1 = texelFetch(stripesId, stripesOfs + int(texUV.x)*(segments-1) ); "
"	vec4 edge2 = texelFetch(stripesId, stripesOfs + int(texUV.x)*(segments-1)+1 ); "

"   vec4 col1 = texture(paletteId, paletteOfs);  "
"   vec4 col2 = texture(paletteId, vec2(paletteOfs.x+1/16.f,paletteOfs.y));  "
"   vec4 col3 = texture(paletteId, vec2(paletteOfs.x+2/16.f,paletteOfs.y));  "


"	float factor1 = 1.f; "

"	float x1 = (texUV.y - edge1.r) * edge1.g;"
"	float adder1 = edge1.g / 2.f;"
"	if (x1 < 0.f)"
"		adder1 = edge1.b;"
"	else if (x1 + edge1.g > 1.f)"
"		adder1 = edge1.a;"
"	float factor2 = clamp(x1 + adder1, 0.f, 1.f);"

"	float x2 = (texUV.y - edge2.r) * edge2.g;"
"	float adder2 = edge2.g / 2.f;"
"	if (x2 < 0.f)"
"		adder2 = edge2.b;"
"	else if (x2 + edge2.g > 1.f)"
"		adder2 = edge2.a;"
"	float factor3 = clamp(x2 + adder2, 0.f, 1.f);"

"   factor1 = (factor1 -factor2)*col1.a;"
"   factor2 = (factor2 -factor3)*col2.a;"
"   factor3 = (factor3)*col3.a;"

"   float totalAlpha = factor1 + factor2 + factor3;"

"   vec4 result;"
"   result.a = totalAlpha; "
"   result.rgb = (col1.rgb * factor1 + col2.rgb * factor2 + col3.rgb * factor3)/totalAlpha;"
"   color = result * fragColor;"
"}												";
*/

}

#endif //WG_GLSHADERS_DOT_H
