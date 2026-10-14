// The tint, tintColor(), evalTintBlock() and the colors buffer come from tint.hlsl,
// which is put in front of this file.
//
// Shapes (Command::DrawShape), see GpuShapes for the entries read from the extras
// buffer. The coverage of each pixel by each segment is worked out from the geometry.
// Partly covered pixels get the colors of the drawn segments covering them mixed,
// weighted by coverage and alpha, like edgemaps. Pixels not covered by any drawn
// segment are left alone.


StructuredBuffer<float4> extras : register(t1);


struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    nointerpolation uint shapeOfs : TEXCOORD1;
};


float spanCoverage(float a, float b, float p)
{
    return clamp(min(p + 0.5f, b) - max(p - 0.5f, a), 0.0f, 1.0f);
}


float rectCoverage(float4 r, float2 p)
{
    return spanCoverage(r.x, r.z, p.x) * spanCoverage(r.y, r.w, p.y);
}


// Functions below return once, at their end. FXC can take an early return from a
// function as leaving its result uninitialized (warning X4000).

// Part of a pixel inside a straight edge, s from the pixel center along its normal n.

float halfPlaneCoverage(float s, float2 n)
{
    float2 an = abs(n);
    float a = max(an.x, an.y);
    float b = min(an.x, an.y);
    float t = s + 0.5f * (a + b);

    float c = 0.0f;                                     // Outside, t <= 0.

    if (b < 0.001f)
        c = clamp(s + 0.5f, 0.0f, 1.0f);
    else if (t >= a + b)
        c = 1.0f;
    else if (t > 0.0f)
    {
        if (t < b)
            c = t * t / (2.0f * a * b);
        else if (t > a)
            c = 1.0f - (a + b - t) * (a + b - t) / (2.0f * a * b);
        else
            c = (t - 0.5f * b) / a;
    }

    return c;
}


// Part of a pixel inside a circle, v from its center to the pixel center and d its length.

float circleCoverage(float radius, float2 v, float d)
{
    float c = 0.0f;

    if (radius > 0.0f)
        c = d > 0.001f ? halfPlaneCoverage(radius - d, v / d) : clamp(radius + 0.5f, 0.0f, 1.0f);

    return c;
}


float roundedRectCoverage(float4 r, float4 radii, float2 p)
{
    // The rounded corner, if any, whose rounding the pixel center is in. Square corners are
    // left to rectCoverage(), which also covers the pixels just outside them partly.

    bool bCorner = true;
    float2 center = float2(0.0f, 0.0f);
    float radius = 0.0f;

    if (radii.x > 0.0f && p.x < r.x + radii.x && p.y < r.y + radii.x)
    {
        radius = radii.x;
        center = float2(r.x + radius, r.y + radius);
    }
    else if (radii.y > 0.0f && p.x > r.z - radii.y && p.y < r.y + radii.y)
    {
        radius = radii.y;
        center = float2(r.z - radius, r.y + radius);
    }
    else if (radii.z > 0.0f && p.x > r.z - radii.z && p.y > r.w - radii.z)
    {
        radius = radii.z;
        center = float2(r.z - radius, r.w - radius);
    }
    else if (radii.w > 0.0f && p.x < r.x + radii.w && p.y > r.w - radii.w)
    {
        radius = radii.w;
        center = float2(r.x + radius, r.w - radius);
    }
    else
        bCorner = false;

    float c = rectCoverage(r, p);

    if (bCorner)
    {
        float2 v = p - center;
        c = min(c, circleCoverage(radius, v, length(v)));
    }

    return c;
}


// Adds a segment covering c of the pixel, if drawn: color times alpha and alpha weighted
// by coverage to acc, coverage to covered. Tint offsets are kept from going negative for
// the same reason as in tintColor().

void addSegment(uint seg, float c, float2 p, inout float4 acc, inout float covered)
{
    if (c <= 0.0f)
        return;

    float4 s = extras[seg];
    if (s.x < 0.0f)
        return;

    float4 col = colors[uint(s.x)];

    [branch]
    if (s.y >= 0.0f)
        col *= evalTintBlock(colors, uint(max(s.y, 0.0f)), p);

    acc += float4(col.rgb * col.a, col.a) * c;
    covered += c;
}


void boxCoverage(uint ofs, float2 p, inout float4 acc, inout float covered)
{
    float outer = roundedRectCoverage(extras[ofs + 1], extras[ofs + 2], p);
    float fill = outer;

    if ((int(extras[ofs].y) & 1) != 0)
        fill = min(outer, roundedRectCoverage(extras[ofs + 3], extras[ofs + 4], p));

    addSegment(ofs + 5, fill, p, acc, covered);
    addSegment(ofs + 6, outer - fill, p, acc, covered);
}


// Splits band, the coverage of the slice area, between the elements: those whose
// boundaries pass through the pixel get the part between them, the element at the
// pixel center the rest.

void ringElements(uint ofs, int n, uint base, float band, float2 p, float2 v, float d, inout float4 acc, inout float covered)
{
    if (n == 1)
    {
        addSegment(ofs + uint(extras[base].y), band, p, acc, covered);
        return;
    }

    float theta = d > 0.0001f ? frac(atan2(v.x, -v.y) * 0.15915494f) : 0.0f;

    uint lo = 0;                                        // Unsigned, FXC warns of slow signed divides.
    uint hi = uint(n);

    [loop]
    while (lo < hi)
    {
        uint mid = (lo + hi) / 2;
        if (extras[base + mid].x <= theta)
            lo = mid + 1;
        else
            hi = mid;
    }

    int first = lo > 0 ? int(lo) - 1 : n - 1;
    float w = min(0.25f, 0.70710678f / (6.2831853f * max(d, 0.0001f)));
    float behind = frac(theta - extras[base + uint(first)].x);

    [loop]
    for (int i = 0; i < n - 1 && behind < w; i++)
    {
        int prev = first > 0 ? first - 1 : n - 1;
        behind += frac(extras[base + uint(first)].x - extras[base + uint(prev)].x);
        first = prev;
    }

    float g = 1.0f;
    float ahead = -behind;
    int k = first;
    float4 element = extras[base + uint(k)];

    [loop]
    for (int j = 0; j < n; j++)
    {
        int next = k + 1 < n ? k + 1 : 0;
        float4 nextElement = extras[base + uint(next)];
        ahead += frac(nextElement.x - element.x);

        float gEnd = 0.0f;
        if (j < n - 1 && ahead < w)
        {
            float a = 6.2831853f * nextElement.x;
            gEnd = min(g, halfPlaneCoverage(-d * sin(6.2831853f * ahead), float2(cos(a), sin(a))));
        }

        addSegment(ofs + uint(element.y), band * (g - gEnd), p, acc, covered);

        if (gEnd <= 0.0f)
            break;

        g = gEnd;
        k = next;
        element = nextElement;
    }
}


void ringCoverage(uint ofs, float2 p, inout float4 acc, inout float covered)
{
    float4 head = extras[ofs];
    int flags = int(head.y);
    float4 circles = extras[ofs + 1];
    float4 inner = extras[ofs + 2];
    float4 rect = extras[ofs + 3];

    float2 v = p - circles.xy;
    float d = length(v);
    bool bSquare = (flags & 1) != 0;

    float c0 = bSquare ? rectCoverage(rect, p) : circleCoverage(circles.z, v, d);
    float c1 = min(bSquare ? rectCoverage(extras[ofs + 4], p) : circleCoverage(circles.w, v, d), c0);
    float c2 = min(circleCoverage(inner.x, v, d), c1);
    float c3 = min(circleCoverage(inner.y, v, d), c2);

    float back = (flags & 2) != 0 ? max(rectCoverage(rect, p) - c0, 0.0f) : 0.0f;
    float outline = c0 - c1 + c2 - c3;
    float band = c1 - c2;

    if ((flags & 16) == 0)
    {
        outline += band;
        band = 0.0f;
    }

    addSegment(ofs + 5, back, p, acc, covered);
    addSegment(ofs + 6, outline, p, acc, covered);
    addSegment(ofs + 7, (flags & 4) != 0 ? c3 : 0.0f, p, acc, covered);

    if (band > 0.0f)
        ringElements(ofs, int(head.z), ofs + uint(head.w), band, p, v, d, acc, covered);
}


// Coverage of the drawn segments of a shape at a pixel center, their colors in acc.

float shapeCoverage(uint ofs, float2 p, out float4 acc)
{
    acc = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float covered = 0.0f;

    int kind = int(extras[ofs].x);

    if (kind == 1)
        boxCoverage(ofs, p, acc, covered);
    else if (kind == 2)
        ringCoverage(ofs, p, acc, covered);

    return covered;
}


float4 main(PS_INPUT input) : SV_TARGET
{
    float4 acc;

    if (shapeCoverage(input.shapeOfs, input.position.xy, acc) < 1.0f / 256.0f)
        discard;

    float4 outColor;

    outColor.a = acc.a;
    outColor.rgb = acc.a > 0.0f ? acc.rgb / acc.a : float3(0.0f, 0.0f, 0.0f);

    return outColor * input.color * tintColor(input.position.xy);
}
