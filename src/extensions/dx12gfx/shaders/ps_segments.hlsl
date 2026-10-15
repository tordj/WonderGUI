cbuffer CanvasInfo : register(b0)
{
    float2 canvasScale;     // Not used here.
    float2 textureSize;     // Not used here.
    uint flags;             // Not used here.
    uint blurOfs;           // Not used here.
    uint edgemapEdges;      // Edges to walk, one fewer than the segments we render.
    uint edgemapPitch;      // Edges per column in the edgemap's buffer.
};

// writeOutput() and premul() come from output.hlsl, the tint, tintColor() and the
// colors buffer from tint.hlsl, which are put in front of this file.


StructuredBuffer<float4> edgemap : register(t3);


struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texUV : TEXCOORD0;
    nointerpolation uint2 segColorsOfs : TEXCOORD1;    // Flat colors and tint table in the edgemap buffer.
    nointerpolation float2 tileSize : TEXCOORD2;       // Size of a tiled edgemap in columns and pixels, zero when not tiled.
};


// Position in the edgemap of a pixel of a tiled edgemap, which may be outside of
// it, wrapped into it. uv.y is half a pixel back, so it is whole at pixel centers.

float2 wrapUV(float2 uv, float2 tileSize)
{
    if (tileSize.x > 0.0f)
    {
        float2 cell = float2(floor(uv.x), floor(uv.y + 0.5f));
        uv += cell - tileSize * floor(cell / tileSize) - cell;
    }

    return uv;
}


// A segment is colored either by a flat color or by a tint placed in the
// edgemap's own rectangle. The edgemap buffer has a table with the offset of
// each segment's tint block, -1 for a flat colored segment. That one is kept
// from being read as an offset for the same reason as in tintColor().

float4 segmentColor(uint seg, uint2 segColorsOfs, float2 epos)
{
    float4 col;

    int blockOfs = int(edgemap[segColorsOfs.y + seg].x);

    [branch]
    if (blockOfs < 0)
        col = edgemap[segColorsOfs.x + seg];
    else
        col = evalTintBlock(edgemap, uint(max(blockOfs, 0)), epos);

    return col;
}


PS_OUTPUT main(PS_INPUT input)
{
    // Walking down a column of the edgemap, one edge at a time. Each edge says
    // where it begins and how steeply it runs, which is enough to tell how much
    // of this pixel falls above it. The difference between one edge's coverage
    // and the next is how much of the pixel belongs to the segment between them.
    // The segments together cover the whole pixel, their premultiplied colors
    // weighted by those parts.

    uint nEdges = edgemapEdges;
    uint pitch = edgemapPitch;

    float2 texUV = wrapUV(input.texUV, input.tileSize);

    uint column = uint(texUV.x) * pitch;

    // Pixel center in edgemap space, which is where segment tints are placed.
    // V is half a pixel back already, see DX12Backend::_drawEdgemap().

    float2 epos = float2(floor(texUV.x) + 0.5f, texUV.y + 0.5f);

    float4 acc = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float factor = 1.0f;

    [loop]
    for (uint i = 0; i < nEdges; i++)
    {
        float4 col = segmentColor(i, input.segColorsOfs, epos);

        float4 edge = edgemap[column + i];

        float x = (texUV.y - edge.r) * edge.g;

        float adder = edge.g / 2.0f;

        if (x < 0.0f)
            adder = edge.b;
        else if (x + edge.g > 1.0f)
            adder = edge.a;

        float factor2 = clamp(x + adder, 0.0f, 1.0f);

        acc += premul(col) * (factor - factor2);

        factor = factor2;
    }

    // Whatever is left below the last edge belongs to the last segment.

    float4 lastCol = segmentColor(nEdges, input.segColorsOfs, epos);

    acc += premul(lastCol) * factor;

    return writeOutput(acc * premul(input.color * tintColor(input.position.xy)), 1.0f);
}
