
#include <metal_stdlib>
using namespace metal; 


//____ Vertex ___________________________________________________

typedef struct
{
    vector_int2     coord;
    int             extrasOfs;              // Offset into extras buffer.
    vector_float2   uv;
} Vertex;

//____ Uniform ______________________________________________

typedef struct             // Uniform buffer object for canvas information.
{                               // DO NOT CHANGE ORDER OF MEMBERS!!!
    vector_float2   canvasDim;
    int        canvasYOfs;
    int        canvasYMul;

    vector_float4 flatTint;

    vector_int2     tintRectPos;
    vector_int2     tintRectSize;

    vector_float4   topLeftTint;
    vector_float4   topRightTint;
    vector_float4   bottomRightTint;
    vector_float4   bottomLeftTint;

    vector_int2     texSize;
} Uniform;


//____ PlotFragInput ______________________________________________

typedef struct 
{
    float4 position [[position]];
    float pointSize [[point_size]];
    float4 color;
} PlotFragInput;

//____ LineFromToFragInput ______________________________________________

typedef struct 
{
    float4 position [[position]];
    float4 color;
    float s;
    float w;
    float slope;
    float ifSteep;
    float ifMild;

} LineFromToFragInput;

//____ FillFragInput ______________________________________________

typedef struct 
{
    float4 position [[position]];
    float4 color;
} FillFragInput;

//____ FillAAFragInput ______________________________________________

typedef struct 
{
    float4 position [[position]];
    float4 color;
    float4 rect;
} FillAAFragInput;


//____ BlitFragInput ______________________________________________

typedef struct 
{
    float4 position [[position]];
    float4 color;
    float2 texUV;
} BlitFragInput;

//____ ClutBlitInterpolateFragInput ______________________________________________

typedef struct 
{
    float4 position [[position]];
    float4 color;
    float2 texUV00;
    float2 texUV11;
    float2 uvFrac;
 } ClutBlitInterpolateFragInput;


//____ plotVertexShader() ____________________________________________

vertex PlotFragInput
plotVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    PlotFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;
    out.color = pUniform->flatTint * pExtras[eOfs];

    out.pointSize = 1.f;

    return out;
}

//____ plotFragmentShader() ____________________________________________

fragment float4 plotFragmentShader(PlotFragInput in [[stage_in]])
{
    return in.color;
};

//____ plotFragmentShader_A8() ____________________________________________

fragment float4 plotFragmentShader_A8(PlotFragInput in [[stage_in]])
{
    return { in.color.a, 0.0, 0.0, 0.0 };
};


//____ lineFromToVertexShader() ____________________________________________

vertex LineFromToFragInput
lineFromToVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    LineFromToFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;
    out.color = pUniform->flatTint * pExtras[eOfs];

    float4 x = pExtras[eOfs+1];
    out.s = x.x;
    out.w = x.y;
    out.slope = x.z;
    out.ifSteep = x.w;
    out.ifMild = 1.0 - out.ifSteep;

    return out;
}

//____ lineFromToFragmentShader() ____________________________________________

fragment float4 lineFromToFragmentShader(LineFromToFragInput in [[stage_in]])
{
   float4 outColor;

   outColor.rgb = in.color.rgb;
   outColor.a = in.color.a * clamp(in.w - abs(in.position.x*in.ifSteep + in.position.y*in.ifMild - in.s - (in.position.x*in.ifMild + in.position.y*in.ifSteep) * in.slope), 0.0, 1.0);

   return outColor;
};

//____ lineFromToFragmentShader_A8() ____________________________________________

fragment float4 lineFromToFragmentShader_A8(LineFromToFragInput in [[stage_in]])
{
   float4 outColor;
   outColor.r = in.color.a * clamp(in.w - abs(in.position.x*in.ifSteep + in.position.y*in.ifMild - in.s - (in.position.x*in.ifMild + in.position.y*in.ifSteep) * in.slope), 0.0, 1.0);

   return outColor;
};


//____ fillVertexShader() ____________________________________________

vertex FillFragInput
fillVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    FillFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;
    out.color = pUniform->flatTint * pExtras[eOfs];

    return out;
}

//____ fillGradientVertexShader() ____________________________________________

vertex FillFragInput
fillGradientVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    FillFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;

    float2   tintOfs = (pos - float2(pUniform->tintRectPos)) / float2(pUniform->tintRectSize);
    float4   lineStartTint = pUniform->topLeftTint + (pUniform->bottomLeftTint - pUniform->topLeftTint) * tintOfs.y;
    float4   lineEndTint = pUniform->topRightTint + (pUniform->bottomRightTint - pUniform->topRightTint) * tintOfs.y;
    float4   gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;

    int     eOfs = pVertices[vertexID].extrasOfs;
    out.color = pUniform->flatTint * gradientTint * pExtras[eOfs];

    return out;
}

//____ fillFragmentShader() ____________________________________________

fragment float4 fillFragmentShader(FillFragInput in [[stage_in]])
{
    return in.color;
};

//____ fillFragmentShader_A8() ____________________________________________

fragment float4 fillFragmentShader_A8(FillFragInput in [[stage_in]])
{
    return {in.color.a,0.0,0.0,0.0};
};


//____ fillAAVertexShader() ____________________________________________

vertex FillAAFragInput
fillAAVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    FillAAFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;
    out.color = pUniform->flatTint * pExtras[eOfs];

    out.rect = pExtras[eOfs+1];
//    out.rect.y = pUniform->canvasYOfs + pUniform->canvasYMul*out.rect.y;
    out.rect.zw += float2(0.5f,0.5f);

    return out;
}


//____ fillGradientAAVertexShader() ____________________________________________

vertex FillAAFragInput
fillGradientAAVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    FillAAFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;

    float2   tintOfs = (pos - float2(pUniform->tintRectPos)) / float2(pUniform->tintRectSize);
    float4   lineStartTint = pUniform->topLeftTint + (pUniform->bottomLeftTint - pUniform->topLeftTint) * tintOfs.y;
    float4   lineEndTint = pUniform->topRightTint + (pUniform->bottomRightTint - pUniform->topRightTint) * tintOfs.y;
    float4   gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;

    int     eOfs = pVertices[vertexID].extrasOfs;
    out.color = pUniform->flatTint * gradientTint * pExtras[eOfs];

    out.rect = pExtras[eOfs+1];
//    out.rect.y = pUniform->canvasYOfs + pUniform->canvasYMul*out.rect.y;
    out.rect.zw += float2(0.5f,0.5f);

    return out;
}

//____ fillAAFragmentShader() ____________________________________________

fragment float4 fillAAFragmentShader(FillAAFragInput in [[stage_in]])
{
    float4 color = in.color;
    float2 middleofs = abs(in.position.xy - in.rect.xy);
    float2 alphas = clamp(in.rect.zw  - middleofs, 0.f, 1.f);
    color.a = in.color.a * alphas.x * alphas.y;

    return color;
};

//____ fillAAFragmentShader_A8() ____________________________________________

fragment float4 fillAAFragmentShader_A8(FillAAFragInput in [[stage_in]])
{
    float2 middleofs = abs(in.position.xy - in.rect.xy);
    float2 alphas = clamp(in.rect.zw  - middleofs, 0.f, 1.f);
    
    return { in.color.a * alphas.x * alphas.y, 0.0, 0.0, 0.0 };
};

//____ blitVertexShader() _______________________________________________

vertex BlitFragInput
blitVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    BlitFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;

    vector_float4 srcDst = pExtras[eOfs];
    vector_float4 transform = pExtras[eOfs+1];
    vector_float2 src = srcDst.xy;
    vector_float2 dst = srcDst.zw;

    out.texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / pUniform->texSize.x;      //TODO: Replace this ugly +0.02f fix with whatever is correct.
    out.texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / pUniform->texSize.y;      //TODO: Replace this ugly +0.02f fix with whatever is correct.

    out.color = pUniform->flatTint;

    return out;
}

//____ blitGradientVertexShader() _______________________________________________


vertex BlitFragInput
blitGradientVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    BlitFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;

    vector_float4 srcDst = pExtras[eOfs];
    vector_float4 transform = pExtras[eOfs+1];
    vector_float2 src = srcDst.xy;
    vector_float2 dst = srcDst.zw;

    out.texUV.x = (src.x + 0.0001f + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / pUniform->texSize.x;      //TODO: Replace this ugly +0.02f fix with whatever is correct.
    out.texUV.y = (src.y + 0.0001f + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / pUniform->texSize.y;      //TODO: Replace this ugly +0.02f fix with whatever is correct.

    float2   tintOfs = (pos - float2(pUniform->tintRectPos)) / float2(pUniform->tintRectSize);
    float4   lineStartTint = pUniform->topLeftTint + (pUniform->bottomLeftTint - pUniform->topLeftTint) * tintOfs.y;
    float4   lineEndTint = pUniform->topRightTint + (pUniform->bottomRightTint - pUniform->topRightTint) * tintOfs.y;
    float4   gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;

    out.color = pUniform->flatTint * gradientTint;

    return out;
}

//____ blitFragmentShader() ____________________________________________

fragment float4 blitFragmentShader(BlitFragInput in [[stage_in]],
                                    texture2d<half> colorTexture [[ texture(0) ]])
{
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);

    const half4 colorSample = colorTexture.sample(textureSampler, in.texUV);

    return float4(colorSample) * in.color;
};

//____ blitFragmentShader_A8() ____________________________________________

fragment float4 blitFragmentShader_A8(BlitFragInput in [[stage_in]],
                                    texture2d<half> colorTexture [[ texture(0) ]])
{
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);

    const float colorSample = float(colorTexture.sample(textureSampler, in.texUV).a);

    return { colorSample * in.color.a, 0.0, 0.0, 0.0 };
};

//____ alphaBlitFragmentShader() ____________________________________________

fragment float4 alphaBlitFragmentShader(BlitFragInput in [[stage_in]],
                                    texture2d<half> colorTexture [[ texture(0) ]])
{
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);

    float4 color = in.color;
    color.a *= colorTexture.sample(textureSampler, in.texUV).r;

    return color;
};

//____ alphaBlitFragmentShader_A8() ____________________________________________

fragment float4 alphaBlitFragmentShader_A8(BlitFragInput in [[stage_in]],
                                    texture2d<half> colorTexture [[ texture(0) ]])
{
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);

    const float colorSample = float(colorTexture.sample(textureSampler, in.texUV).r);

    return { colorSample * in.color.a, 0.0, 0.0, 0.0 };
};



//____ clutBlitNearestFragmentShader() ____________________________________________

fragment float4 clutBlitNearestFragmentShader(BlitFragInput in [[stage_in]],
                                    texture2d<float> colorTexture [[ texture(0) ]],
                                    texture2d<half> clutTexture [[ texture(1) ]])
{
    constexpr sampler textureSampler (mag_filter::nearest,
                                      min_filter::nearest);

    const float colorIndex = colorTexture.sample(textureSampler, in.texUV).r;
    const half4 colorSample = clutTexture.sample(textureSampler, {colorIndex,0.5f} );

//    return in.color;
    return float4(colorSample) * in.color;
};

//____ clutBlitNearestFragmentShader_A8() ____________________________________________

fragment float4 clutBlitNearestFragmentShader_A8(BlitFragInput in [[stage_in]],
                                    texture2d<float> colorTexture [[ texture(0) ]],
                                    texture2d<half> clutTexture [[ texture(1) ]])
{
    constexpr sampler textureSampler (mag_filter::nearest,
                                      min_filter::nearest);

    const float colorIndex = colorTexture.sample(textureSampler, in.texUV).r;
    const float colorSample = clutTexture.sample(textureSampler, {colorIndex,0.5f} ).a;

    return { colorSample * in.color.a, 0.0, 0.0, 0.0 };
};


//____ clutBlitInterpolateVertexShader() _______________________________________________

vertex ClutBlitInterpolateFragInput
clutBlitInterpolateVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    ClutBlitInterpolateFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;

    vector_float4 srcDst = pExtras[eOfs];
    vector_float4 transform = pExtras[eOfs+1];
    vector_float2 src = srcDst.xy;
    vector_float2 dst = srcDst.zw;

    float2 texUV = src + (pos-dst) * transform.xw + (pos.yx - dst.yx) * transform.zy;
    texUV -= 0.5f;

    out.uvFrac = texUV;
    out.texUV00 = texUV/ (float2) pUniform->texSize;
    out.texUV11 = (texUV+1)/ (float2) pUniform->texSize;

    out.color = pUniform->flatTint;

    return out;
}

//____ clutBlitInterpolateGradientVertexShader() _______________________________________________

vertex ClutBlitInterpolateFragInput
clutBlitInterpolateGradientVertexShader(uint vertexID [[vertex_id]],
             constant Vertex *pVertices [[buffer(0)]],
             constant vector_float4  *pExtras [[buffer(1)]],
             constant Uniform * pUniform[[buffer(2)]])
{
    ClutBlitInterpolateFragInput out;

    float2 pos = (vector_float2) pVertices[vertexID].coord.xy;

    vector_float2 canvasSize = pUniform->canvasDim;
    
    out.position = vector_float4(0.0, 0.0, 0.0, 1.0);
    out.position.x = pos.x*2 / canvasSize.x - 1.0;
    out.position.y = (pUniform->canvasYOfs + pUniform->canvasYMul*pos.y)*2 / canvasSize.y - 1.0;


    int     eOfs = pVertices[vertexID].extrasOfs;

    vector_float4 srcDst = pExtras[eOfs];
    vector_float4 transform = pExtras[eOfs+1];
    vector_float2 src = srcDst.xy;
    vector_float2 dst = srcDst.zw;

    float2 texUV = src + (pos-dst) * transform.xw + (pos.yx - dst.yx) * transform.zy;
    texUV -= 0.5f;

    out.uvFrac = texUV;
    out.texUV00 = texUV/ (float2) pUniform->texSize;
    out.texUV11 = (texUV+1)/ (float2) pUniform->texSize;

    float2   tintOfs = (pos - float2(pUniform->tintRectPos)) / float2(pUniform->tintRectSize);
    float4   lineStartTint = pUniform->topLeftTint + (pUniform->bottomLeftTint - pUniform->topLeftTint) * tintOfs.y;
    float4   lineEndTint = pUniform->topRightTint + (pUniform->bottomRightTint - pUniform->topRightTint) * tintOfs.y;
    float4   gradientTint = lineStartTint + (lineEndTint - lineStartTint) * tintOfs.x;

    out.color = pUniform->flatTint * gradientTint;

    return out;
}

//____ clutBlitInterpolateFragmentShader() ____________________________________________

fragment float4 clutBlitInterpolateFragmentShader(ClutBlitInterpolateFragInput in [[stage_in]],
                                    texture2d<float> colorTexture [[ texture(0) ]],
                                    texture2d<half> clutTexture [[ texture(1) ]])
{
    constexpr sampler textureSampler (mag_filter::nearest,
                                      min_filter::nearest);

   float index00 = colorTexture.sample(textureSampler, in.texUV00).r;
   float index01 = colorTexture.sample(textureSampler, float2(in.texUV11.x,in.texUV00.y) ).r;
   float index10 = colorTexture.sample(textureSampler, float2(in.texUV00.x,in.texUV11.y) ).r;
   float index11 = colorTexture.sample(textureSampler, in.texUV11).r;
   half4 color00 = clutTexture.sample(textureSampler, float2(index00,0.5f));
   half4 color01 = clutTexture.sample(textureSampler, float2(index01,0.5f));
   half4 color10 = clutTexture.sample(textureSampler, float2(index10,0.5f));
   half4 color11 = clutTexture.sample(textureSampler, float2(index11,0.5f));

   half4 out0 = color00 * (1-fract(in.uvFrac.x)) + color01 * fract(in.uvFrac.x);
   half4 out1 = color10 * (1-fract(in.uvFrac.x)) + color11 * fract(in.uvFrac.x);
   half4 colorSample = (out0 * (1-fract(in.uvFrac.y)) + out1 * fract(in.uvFrac.y));

    return float4(colorSample) * in.color;
};

//____ clutBlitInterpolateFragmentShader_A8() ____________________________________________

fragment float4 clutBlitInterpolateFragmentShader_A8(ClutBlitInterpolateFragInput in [[stage_in]],
                                    texture2d<float> colorTexture [[ texture(0) ]],
                                    texture2d<half> clutTexture [[ texture(1) ]])
{
    constexpr sampler textureSampler (mag_filter::nearest,
                                      min_filter::nearest);

   float index00 = colorTexture.sample(textureSampler, in.texUV00).r;
   float index01 = colorTexture.sample(textureSampler, float2(in.texUV11.x,in.texUV00.y) ).r;
   float index10 = colorTexture.sample(textureSampler, float2(in.texUV00.x,in.texUV11.y) ).r;
   float index11 = colorTexture.sample(textureSampler, in.texUV11).r;
   float color00 = clutTexture.sample(textureSampler, float2(index00,0.5f)).a;
   float color01 = clutTexture.sample(textureSampler, float2(index01,0.5f)).a;
   float color10 = clutTexture.sample(textureSampler, float2(index10,0.5f)).a;
   float color11 = clutTexture.sample(textureSampler, float2(index11,0.5f)).a;

   float out0 = color00 * (1-fract(in.uvFrac.x)) + color01 * fract(in.uvFrac.x);
   float out1 = color10 * (1-fract(in.uvFrac.x)) + color11 * fract(in.uvFrac.x);
   float colorSample = (out0 * (1-fract(in.uvFrac.y)) + out1 * fract(in.uvFrac.y));

   return { colorSample * in.color.a, 0.0, 0.0, 0.0 };
};