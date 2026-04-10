// Shared utilities for Bella series generators.
// Each series is a separate executable (one .cpp file), so static/inline definitions here
// are safe — no multiple-definition issues.
// NOTE: using-directives in headers are acceptable for application-level executables like these.
#pragma once

#include "bella_sdk/bella_scene.h"
#include "dl_core/dl_math.h"
#include "dl_core/dl_string.h"
#include "dl_core/dl_file.h"
#include "dl_core/dl_args.h"
#include "dl_core/dl_vector.h"

#include <cstdio>
#include <cmath>
#include <cstdint>
#include <unistd.h>

#include "src/dl_core/dl_math.h"
#include "src/dl_core/dl_string.h"

using namespace dl;
using namespace bella_sdk;
using namespace dl::math;

//=================================================================================================
// Logging.
//=================================================================================================

static int s_logCtx = 0;

static void log(void* /*ctx*/, LogType type, const char* msg)
{
    switch (type)
    {
    case LogType_Info:
        DL_PRINT("[INFO] %s\n", msg);
        break;
    case LogType_Warning:
        DL_PRINT("[WARN] %s\n", msg);
        break;
    case LogType_Error:
        DL_PRINT("[ERROR] %s\n", msg);
        break;
    case LogType_Custom:
        DL_PRINT("%s\n", msg);
        break;
    case LogType_Progress:
        {
            if (dl::logIsProgressBegin(msg))    { }
            else if (dl::logIsProgressEnd(msg)) { }
            else
            {
                auto val  = dl::logProgressValue(msg);
                auto text = dl::logProgressText(msg);
                DL_PRINT("[%f] %s\n", val, text.buf());
            }
        }
        break;
    }
}

//=================================================================================================
// Math helpers common to all series.
//=================================================================================================

static inline Double sign(Double x)
{
    return x < 0 ? -1.0 : 1.0;
}

static inline Double spow(Double x, Double y)
{
    return sign(x) * pow(abs(x), y);
}

//=================================================================================================
// Radiance HDR (RGBE) file writer.
//=================================================================================================

static inline void rgbeEncode(float r, float g, float b, uint8_t out[4])
{
    float maxComp = r > g ? (r > b ? r : b) : (g > b ? g : b);
    if (maxComp < 1e-32f)
    {
        out[0] = out[1] = out[2] = out[3] = 0;
        return;
    }
    int   e;
    float scale = frexpf(maxComp, &e) * 256.0f / maxComp;
    out[0] = (uint8_t)(r * scale);
    out[1] = (uint8_t)(g * scale);
    out[2] = (uint8_t)(b * scale);
    out[3] = (uint8_t)(e + 128);
}

static inline bool writeHDR(const char* path, int width, int height, const float* rgb)
{
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    fprintf(f, "#?RADIANCE\nFORMAT=32-bit_rle_rgbe\n\n-Y %d +X %d\n", height, width);
    for (int i = 0; i < width * height; ++i)
    {
        uint8_t pixel[4];
        rgbeEncode(rgb[i*3+0], rgb[i*3+1], rgb[i*3+2], pixel);
        fwrite(pixel, 1, 4, f);
    }
    fclose(f);
    return true;
}

//=================================================================================================
// UV index to angle.
//=================================================================================================

static inline Double index2radians(Double index, Int n)
{
    return index / Double(n) * dl::math::nc::pi * 2;
}

//=================================================================================================
// Bella camera lookat matrix (row-vector convention).
// Row0=right, Row1=down (right×forward), Row2=forward toward target, Row3=position.
// Pass worldUp={0,0,1} for Z-up scenes (series1, series3) or {0,1,0} for Y-up (series2).
//=================================================================================================

static inline Mat4 bellaLookAt(Vec3 from, Vec3 to, Vec3 worldUp)
{
    Vec3 forward = (to - from).unit();
    Vec3 right   = worldUp.cross(forward).unit();
    Vec3 down    = right.cross(forward);
    return Mat4::make(
        Vec4{right.x,   right.y,   right.z,   0},
        Vec4{down.x,    down.y,    down.z,    0},
        Vec4{forward.x, forward.y, forward.z, 0},
        Vec4{from.x,    from.y,    from.z,    1}
    );
}

//=================================================================================================
// HSB to RGB (standard algorithm).
//=================================================================================================

static inline Vec3 hsb2rgb(Double hue, Double sat, Double bri)
{
    Double u = bri;
    if (sat == 0.0) return Vec3{u, u, u};
    Double h  = (hue - floor(hue)) * 6.0;
    Double f  = h - floor(h);
    Double p  = bri * (1.0 - sat);
    Double q  = bri * (1.0 - sat * f);
    Double tt = bri * (1.0 - sat * (1.0 - f));
    switch (int(h))
    {
    case 0: return Vec3{u,  tt, p};
    case 1: return Vec3{q,  u,  p};
    case 2: return Vec3{p,  u,  tt};
    case 3: return Vec3{p,  q,  u};
    case 4: return Vec3{tt, p,  u};
    case 5: return Vec3{u,  p,  q};
    default: return Vec3{u, u,  u};
    }
}

//=================================================================================================
// Camera (SLR2).
// FOV == 0.0  → no frustum culling (invisible() always returns false).
// FOV >  0.0  → degrees; culls points outside the FOV frustum (series102 convention).
//=================================================================================================

struct SLR2
{
    Double width  = 0.054;   // sensor width  (m)
    Double height = 0.036;   // sensor height (m)
    Double lens   = 0.050;
    Double fStop  = 4.0;
    Double focus  = 1.0;
    Double FOV    = 0.0;     // degrees; 0 = no culling

    Vec3 position;
    Vec3 target;
    Vec3 rightVec;
    Vec3 upVec;
    Vec3 fwdVec;   // (position - target).unit() — points away from scene
    Mat4 transform;

    SLR2() : position(Vec3{0,0,0}), target(Vec3{0,0,-5}) { computeTransform(); }

    void moveTo(Vec3 p) { position = p; computeTransform(); }
    void lookAt(Vec3 t) { target   = t; computeTransform(); }

    void computeTransform()
    {
        fwdVec   = (position - target).unit();
        Vec3 z   = {0, 0, 1};
        rightVec = z.cross(fwdVec).unit();
        upVec    = fwdVec.cross(rightVec).unit();
        transform = Mat4::make(
            Vec4{rightVec.x, rightVec.y, rightVec.z, 0},
            Vec4{upVec.x,    upVec.y,    upVec.z,    0},
            Vec4{fwdVec.x,   fwdVec.y,   fwdVec.z,   0},
            Vec4{position.x, position.y, position.z, 1}
        );
    }

    Bool invisible(Vec3 point) const
    {
        if (FOV <= 0.0) return false;
        Vec3   d  = point - position;
        Double px = d.x*rightVec.x + d.y*rightVec.y + d.z*rightVec.z;
        Double py = d.x*upVec.x    + d.y*upVec.y    + d.z*upVec.z;
        Double pz = d.x*fwdVec.x   + d.y*fwdVec.y   + d.z*fwdVec.z;
        if (pz > 0.0) return true;   // behind camera
        Double factor = tan(FOV * 1.25 / 360.0 * dl::math::nc::pi);
        Double aspect = width / height;
        if (px < pz * factor * aspect) return true;
        if (px > -pz * factor * aspect) return true;
        if (py < pz * factor) return true;
        if (py > -pz * factor) return true;
        return false;
    }
};

//=================================================================================================
// Generalised sphere/cube parametric shape.
// a=0.0  → sphere
// a=0.6  → cube-ish distortion (series144)
// a=f(t) → animated cube (series110)
//=================================================================================================

static inline Vec3 cube(Double u, Double v, Double a)
{
    return Vec3{
        sin(v/2.0 + a*sin(v)) * cos(u - a*sin(2*u)),
        sin(v/2.0 + a*sin(v)) * sin(u + a*sin(2*u)),
        cos(v/2.0 - a*sin(v))
    };
}

//=================================================================================================
// Unit circle in the XY plane.
//=================================================================================================

static inline Vec3 circle(Double t)
{
    return Vec3{sin(t), cos(t), 0};
}

//=================================================================================================
// Torus-knot parametric curve.
//=================================================================================================

static inline Vec3 torusKnot(Double t, Double R, Double r, Int p, Int q, Vec3(*path)(Double))
{
    Vec3   pp = path(Double(q) * t);
    Double rp = R + r * cos(Double(p) * t);
    return Vec3{rp * pp.x, rp * pp.y, r * sin(Double(p) * t) + pp.z};
}

//=================================================================================================
// Frenet-frame tube around a parametric path.
//=================================================================================================

static inline Vec3 pathWrapper(Double u, Double v, Double r, Vec3(*path)(Double))
{
    Double delta   = 0.01;
    Vec3   center  = path(v);
    Vec3   diff    = path(v + delta) - path(v - delta);
    Vec3   tangent = (diff.norm() > 1e-10) ? diff.unit() : Vec3{1, 0, 0};
    Vec3   sinVec  = tangent.cross(Vec3{0, 0, 1}).unit();
    Vec3   cosVec  = tangent.cross(sinVec);   // tangent⊥sinVec → already unit
    Double cu = r * cos(u);
    Double su = r * sin(u);
    return Vec3{
        cosVec.x * cu + sinVec.x * su + center.x,
        cosVec.y * cu + sinVec.y * su + center.y,
        cosVec.z * cu + sinVec.z * su + center.z
    };
}

//=================================================================================================
// Build adaptive breakpoints from distance-squared weights.
// weights[i] is the accumulated d² at coarse sample i (higher = farther from camera).
// Returns n angles in [0, 2π] with larger intervals where the surface is farther away,
// so nearer regions receive proportionally finer sampling.
//=================================================================================================

static inline ds::Vector<Double> buildBreakpoints(const ds::Vector<Double>& weights, Int n)
{
    Int N = (Int)weights.size();

    // Invert weights: small d² → large inverse → more breakpoints nearby
    ds::Vector<Double> invWeights;
    invWeights.resize(N);
    for (Int i = 0; i < N; ++i)
        invWeights[i] = (weights[i] > 1e-10) ? 1.0 / weights[i] : 1e10;

    ds::Vector<Double> cdf;
    cdf.resize(N + 1);
    cdf[0] = 0.0;
    for (Int i = 0; i < N; ++i)
        cdf[i + 1] = cdf[i] + invWeights[i];
    Double total = cdf[N];

    ds::Vector<Double> breaks;
    breaks.resize(n);
    for (Int k = 0; k < n; ++k)
    {
        Double target = (n > 1) ? (Double(k) / Double(n - 1) * total) : 0.0;
        Int lo = 0, hi = N - 1;
        while (hi - lo > 1)
        {
            Int mid = (lo + hi) / 2;
            if (cdf[mid] <= target) lo = mid; else hi = mid;
        }
        Double frac = 0.0;
        if (cdf[lo + 1] > cdf[lo])
            frac = (target - cdf[lo]) / (cdf[lo + 1] - cdf[lo]);
        breaks[k] = (Double(lo) + frac) / Double(N) * dl::math::nc::pi * 2.0;
    }
    return breaks;
}

//=================================================================================================
// Mesh generation.
// Builds vertex, normal, UV and polygon arrays from a UV parametric surface.
// uBreaks/vBreaks are non-uniform angle sequences produced by buildBreakpoints —
// finer near the camera, coarser farther away.  No frustum culling is performed;
// Bella's path tracer handles off-screen geometry correctly.
// Bella mesh requires float (f32) — pos3d/vec3d causes silent render failure.
//=================================================================================================

static void generateMeshData(
    Vec3(*uv2xyz)(Double, Double, Double),
    const ds::Vector<Double>& uBreaks,
    const ds::Vector<Double>& vBreaks,
    Double                    t,
    ds::Vector<Pos3f>&        points,
    ds::Vector<Vec3f>&        normals,
    ds::Vector<Vec2f>&        uvs,
    ds::Vector<Vec4u>&        polygons)
{
    Int    nU    = (Int)uBreaks.size();
    Int    nV    = (Int)vBreaks.size();
    Double delta = 1e-3;

    for (Int ui = 0; ui < nU; ++ui)
    for (Int vi = 0; vi < nV; ++vi)
    {
        Double u      = uBreaks[ui];
        Double v      = vBreaks[vi];
        Vec3   vertex = uv2xyz(u, v, t);
        Vec3   tanU   = uv2xyz(u + delta, v, t) - uv2xyz(u - delta, v, t);
        Vec3   tanV   = uv2xyz(u, v + delta, t) - uv2xyz(u, v - delta, t);
        Vec3   nrm    = tanV.cross(tanU);
        Vec3   normal = (nrm.norm() > 1e-10) ? nrm.unit() : vertex.unit();

        Double uvU = (nU > 1) ? Double(ui) / Double(nU - 1) : 0.0;
        Double uvV = (nV > 1) ? Double(vi) / Double(nV - 1) : 0.0;

        points.push_back(Pos3f{Float(vertex.x), Float(vertex.y), Float(vertex.z)});
        normals.push_back(Vec3f{Float(normal.x), Float(normal.y), Float(normal.z)});
        uvs.push_back(Vec2f{Float(uvU), Float(uvV)});
    }

    Int numFaces = 0;
    for (Int ui = 0; ui < nU - 1; ++ui)
    for (Int vi = 0; vi < nV - 1; ++vi)
    {
        UInt32 topRight = UInt32( ui      * nV + vi    );
        UInt32 topLeft  = UInt32((ui + 1) * nV + vi    );
        UInt32 botRight = UInt32( ui      * nV + vi + 1);
        UInt32 botLeft  = UInt32((ui + 1) * nV + vi + 1);

        polygons.push_back(Vec4u{topRight, botLeft,  topLeft,  topLeft });
        polygons.push_back(Vec4u{topRight, botRight, botLeft,  botLeft });
        numFaces += 2;
    }

    logInfo("Generated %d vertices and %d faces", Int(points.size()), numFaces);
}

//=================================================================================================
// Write mesh geometry to a Wavefront OBJ file for inspection.
// Each Vec4u polygon is stored as a degenerate quad (indices [2]==[3] → triangle).
// OBJ indices are 1-based.
//=================================================================================================

static inline bool writeMeshOBJ(const char* path,
    const ds::Vector<Pos3f>& points,
    const ds::Vector<Vec3f>& normals,
    const ds::Vector<Vec2f>& uvs,
    const ds::Vector<Vec4u>& polygons)
{
    FILE* f = fopen(path, "w");
    if (!f) return false;

    fprintf(f, "# Generated by bella-art series generator\n");

    for (size_t i = 0; i < points.size(); ++i)
        fprintf(f, "v %f %f %f\n", points[i].x, points[i].y, points[i].z);

    for (size_t i = 0; i < uvs.size(); ++i)
        fprintf(f, "vt %f %f\n", uvs[i].x, uvs[i].y);

    for (size_t i = 0; i < normals.size(); ++i)
        fprintf(f, "vn %f %f %f\n", normals[i].x, normals[i].y, normals[i].z);

    for (size_t i = 0; i < polygons.size(); ++i)
    {
        uint32_t a = polygons[i].x + 1;
        uint32_t b = polygons[i].y + 1;
        uint32_t c = polygons[i].z + 1;
        uint32_t d = polygons[i].w + 1;
        if (c == d)
            fprintf(f, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
                    a, a, a, b, b, b, c, c, c);
        else
            fprintf(f, "f %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u\n",
                    a, a, a, b, b, b, c, c, c, d, d, d);
    }

    fclose(f);
    return true;
}

//=================================================================================================
// Create a Bella mesh node from pre-built geometry arrays.
//=================================================================================================

static inline Node buildBellaMesh(Scene& scene, const char* name,
    const ds::Vector<Pos3f>& points,
    const ds::Vector<Vec3f>& normals,
    const ds::Vector<Vec2f>& uvs,
    const ds::Vector<Vec4u>& polygons)
{
    auto meshNode = scene.createNode("mesh", name);
    auto steps    = meshNode["steps"];
    steps.appendElement();
    auto step0       = steps[0];
    step0["points"]  = points;
    step0["normals"] = normals;
    step0["uvs"]     = uvs;
    meshNode["polygons"] = polygons;
    while (steps.inputCount() > 1) steps.removeLastInput();
    return meshNode;
}

//=================================================================================================
// Build the Bella render-graph tail: beautyPass → settings → state → global.
// Also trims any extra world steps.
//=================================================================================================

static inline void buildBellaRenderSettings(Scene& scene, Node cameraNode, Node envmapNode,
    Node world, Int bouncesRefraction = 128)
{
    auto settings = scene.createNode("settings", "settings");
    settings["camera"]      = cameraNode;
    settings["environment"] = envmapNode;

    auto beautyPass = scene.createNode("beautyPass", "beautyPass");
    beautyPass["resume"]            = "disabled";
    beautyPass["saveBsi"]           = Int(0);
    beautyPass["saveImage"]         = Int(0);
    beautyPass["targetNoise"]       = UInt(2);
    beautyPass["bouncesRefraction"] = Int(bouncesRefraction);
    beautyPass["solver"]            = String("Atlas");
    settings["beautyPass"] = beautyPass;

    auto state = scene.createNode("state", "state");
    state["settings"] = settings;
    state["world"]    = world;

    auto globalNode = scene.createNode("global", "global");
    globalNode["states"].appendElement().set(state);

    auto worldSteps = world["steps"];
    while (worldSteps.inputCount() > 1) worldSteps.removeLastInput();
}
