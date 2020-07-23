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

#include <wg_metalgfxdevice.h>

namespace wg {


const char MetalGfxDevice::shaders[] =

"\n"
"#include <metal_stdlib>\n"
"using namespace metal; \n"
"\n"
"\n"
"typedef struct\n"
"{\n"
"    vector_int2     coord;\n"
"    int             extrasOfs;                        // Offset into extras buffer.\n"
"    vector_float2   uv;\n"
"} Vertex;\n"
"\n"
"typedef struct             // Uniform buffer object for canvas information.\n"
"{                               // DO NOT CHANGE ORDER OF MEMBERS!!!\n"
"    vector_float2   canvasDim;\n"
"    int        canvasYOfs;\n"
"    int        canvasYMul;\n"
"\n"
"    vector_float4 flatTint;\n"
"\n"
"    vector_int4     tintRect;\n"
"\n"
"    vector_float4   topLeftTint;\n"
"    vector_float4   topRightTint;\n"
"    vector_float4   bottomRightTint;\n"
"    vector_float4   bottomLeftTint;\n"
"\n"
"    vector_int2     textureSize;\n"
"} UniformBuffer;\n"
"\n"
"\n"
"typedef struct \n"
"{\n"
"    float4 position [[position]];\n"
"    float4 color;\n"
"} RasterizerData;\n"
"\n"
"\n"
"vertex RasterizerData\n"
"fillVertexShader(uint vertexID [[vertex_id]],\n"
"             constant Vertex *pVertices [[buffer(0)]],\n"
"             constant float  *pExtras [[buffer(1)]],\n"
"             constant UniformBuffer * pUniform[[buffer(2)]])\n"
"{\n"
"    RasterizerData out;\n"
"\n"
"    float2 pixelSpacePosition = (vector_float2) pVertices[vertexID].coord.xy;\n"
"\n"
"    vector_float2 viewportSize = pUniform->canvasDim;\n"
"    \n"
"    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);\n"
"    out.position.x = pixelSpacePosition.x*2 / viewportSize.x - 1.0;\n"
"    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pixelSpacePosition.y)*2 / viewportSize.y - 1.0;\n"
"\n"
"    out.color = pUniform->flatTint * vector_float4(pExtras[0],pExtras[1],pExtras[2],pExtras[3]);\n"
"\n"
"    return out;\n"
"}\n"
"\n"
"fragment float4 fillFragmentShader(RasterizerData in [[stage_in]])\n"
"{\n"
"    // Return the interpolated color.\n"
"    return in.color;\n"
"};\n"
"\n"
;}
