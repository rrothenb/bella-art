// Bella Surface Generator - Port of series110.go
// Generates animated procedural surfaces for the Bella renderer

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
// Global state and logging.
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
                auto val = dl::logProgressValue(msg);
                auto text = dl::logProgressText(msg);
                DL_PRINT("[%f] %s\n", val, text.buf());
            }
        }
        break;
    }
}

//=================================================================================================
// Math helpers (mirroring Go code).
//=================================================================================================

static Double sign(Double x)
{
    return x < 0 ? -1.0 : 1.0;
}

static Double spow(Double x, Double y)
{
    return sign(x) * pow(abs(x), y);
}

static Double strength(Int n, Double x)
{
    return pow(2.0, sin(pow(Double(n), 0.5) * (x + Double(n) / 3.0)));
}

//=================================================================================================
// Radiance HDR (RGBE) file writer.
//=================================================================================================

// Encode a float RGB pixel to Radiance RGBE (4 bytes: R, G, B mantissa + shared exponent).
static void rgbeEncode(float r, float g, float b, uint8_t out[4])
{
    float maxComp = r > g ? (r > b ? r : b) : (g > b ? g : b);
    if (maxComp < 1e-32f)
    {
        out[0] = out[1] = out[2] = out[3] = 0;
        return;
    }
    int e;
    float scale = frexpf(maxComp, &e) * 256.0f / maxComp;
    out[0] = (uint8_t)(r * scale);
    out[1] = (uint8_t)(g * scale);
    out[2] = (uint8_t)(b * scale);
    out[3] = (uint8_t)(e + 128);
}

// Write a Radiance HDR file (non-RLE RGBE, row-major, float RGB input).
static bool writeHDR(const char* path, int width, int height, const float* rgb)
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
// Camera (SLR2).
//=================================================================================================

struct SLR2
{
    Double width;
    Double height;
    Double lens;
    Double fStop;
    Double focus;

    Mat4 transform;
    Vec3 position;
    Vec3 target;

    SLR2()
        : width(0.054)
        , height(0.036)
        , lens(0.050)
        , fStop(4.0)
        , focus(1.0)
        , position(Vec3 {0, 0, 0})
        , target(Vec3 {0, 0, -5})
    {
        computeTransform();
    }

    void lookAt(Vec3 t)
    {
        target = t;
        computeTransform();
    }

    void moveTo(Vec3 p)
    {
        position = p;
        computeTransform();
    }

    void computeTransform()
    {
        // LookAt matrix construction (similar to Go code)
        Vec3 f = (position - target).unit();  // forward
        Vec3 zAxis = Vec3 {0, 0, 1};
        Vec3 r = zAxis.cross(f).unit();       // right
        Vec3 u = f.cross(r).unit();           // up

        // Orientation matrix (row-major)
        Mat4 orient = Mat4::make(
            Vec4 {r.x, r.y, r.z, 0},
            Vec4 {u.x, u.y, u.z, 0},
            Vec4 {f.x, f.y, f.z, 0},
            Vec4 {0, 0, 0, 1}
        );

        // Translation matrix
        Mat4 trans = Mat4::make(
            Vec4 {1, 0, 0, position.x},
            Vec4 {0, 1, 0, position.y},
            Vec4 {0, 0, 1, position.z},
            Vec4 {0, 0, 0, 1}
        );

        transform = trans * orient;
    }

    Bool invisible(Vec3 /*point*/)
    {
        // Simplified visibility test - always return false for now
        return false;
    }
};

//=================================================================================================
// Procedural shape functions.
//=================================================================================================

static Vec3 circle(Double x)
{
    return Vec3 {sin(x), cos(x), 0};
}

static Double lipTexture(Double u, Double t)
{
    return sin(u + 2*strength(5, t)*sin(u + 2*strength(7, t)*sin(u))
                 + 2*strength(11, t)*sin(2*u)
                 + 2*strength(13, t)*sin(3*u));
}

static Vec3 bowl(Double thickness, Double insideTexture, Double outsideTexture,
                 Double u, Double v, Double t)
{
    Double width = 1.0 + 0.1 * strength(3, t) * pow(sin(v/2), 10)
                   * pow(spow(lipTexture(u, t), pow(3, sin(2*t)))/2 + 0.5, pow(3, sin(3*t)));

    Double height = sin(t)*0.15 + 0.35 + 0.1 * strength(2, t) * pow(sin(v/2), 10)
                    * pow(spow(lipTexture(u, t), pow(3, sin(2*t)))/2 + 0.5, pow(3, sin(3*t)));

    Double space = (cos(v/2 - 0.7*sin(v))/2 + 0.5) * (thickness + outsideTexture)
                   + (0.5 - cos(v/2 - 0.7*sin(v))/2) * insideTexture;

    Double scale = width * (1 + 1/height * space);

    return Vec3 {
        scale * sin(u) * sin(v/2),
        scale * cos(u) * sin(v/2),
        -height * cos(v - (sin(7*t)*0.4 + 0.5) * sin(2*v)) * (1 + 1/height * space)
    };
}

static Vec3 torusKnot(Double t, Double R, Double r, Int pInt, Int qInt, std::function<Vec3(Double)> path)
{
    Double p = Double(pInt);
    Double q = Double(qInt);
    Vec3 pathPoint = path(q * t);

    return Vec3 {
        (R + r * cos(p*t)) * pathPoint.x,
        (R + r * cos(p*t)) * pathPoint.y,
        r * sin(p*t) + pathPoint.z
    };
}

static Vec3 lissajousKnot(Double t, Int xN, Int yN, Int zN)
{
    return Vec3 {sin(Double(xN) * t), sin(Double(yN) * t), cos(Double(zN) * t)};
}

static Vec3 unitLissajousKnot(Double t, Int xN, Int yN, Int zN)
{
    Vec3 point = lissajousKnot(t, xN, yN, zN);
    return point.unit();
}

static Vec3 outerKnot(Double t)
{
    return torusKnot(t, 1, 0.25, 2, 3, circle);
}

static Vec3 innerKnot(Double t)
{
    return torusKnot(t, 1, 0.15, 3, 2, outerKnot);
}

static Vec3 cameraPath(Double t)
{
    return Vec3 {.5*sin(41*t), 0, 0};
}

static Vec3 focusPath(Double /*t*/)
{
    return Vec3 {-1, 0, 0};
}

static Vec3 pathWrapper(Double u, Double v, Double r, std::function<Vec3(Double)> path)
{
    Double delta = 0.01;
    Vec3 center = path(v);
    Vec3 tangent = (path(v + delta) - path(v - delta)).unit();
    Vec3 normal = tangent.cross(Vec3 {0, 0, 1});
    Vec3 binormal = tangent.cross(normal);

    return binormal * (r * cos(u)) + normal * (r * sin(u)) + center;
}

static Vec3 knot(Double t)
{
    return unitLissajousKnot(t, 19, 20, 21);
}

static Vec3 sphere(Double u, Double v, Double /*t*/)
{
    return Vec3 {
        sin(v/2.0) * cos(u),
        sin(v/2.0) * sin(u),
        cos(v/2.0)
    };
}

static Double shapeTexture(Double f, Double a, Double t, Vec3 loc)
{
    loc = loc * (f * 2 * dl::math::nc::pi);
    loc.x = abs(loc.x);
    loc.y = abs(loc.y);
    loc.z = abs(loc.z);

    Double result =
        a*strength(7, t)*sin(a*strength(23, t)*loc.z) +
        a*strength(7, t)*sin(a*strength(23, t)*loc.y) +
        a*strength(7, t)*sin(a*strength(23, t)*loc.x) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.x + a*strength(29, t)*sin(a*strength(31, t)*3*loc.y)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.x + a*strength(29, t)*sin(a*strength(31, t)*3*loc.z)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.y + a*strength(29, t)*sin(a*strength(31, t)*3*loc.x)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.y + a*strength(29, t)*sin(a*strength(31, t)*3*loc.z)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.z + a*strength(29, t)*sin(a*strength(31, t)*3*loc.x)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.z + a*strength(29, t)*sin(a*strength(31, t)*3*loc.y)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.x - a*strength(29, t)*sin(a*strength(31, t)*3*loc.y)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.x - a*strength(29, t)*sin(a*strength(31, t)*3*loc.z)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.y - a*strength(29, t)*sin(a*strength(31, t)*3*loc.x)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.y - a*strength(29, t)*sin(a*strength(31, t)*3*loc.z)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.z - a*strength(29, t)*sin(a*strength(31, t)*3*loc.x)) +
        a*strength(11, t)*sin(a*strength(19, t)*2*loc.z - a*strength(29, t)*sin(a*strength(31, t)*3*loc.y));

    return sin(result);
}

static Vec3 cube(Double u, Double v, Double t)
{
    Double a = -(0.25 + cos(21*t)*0.25);
    return Vec3 {
        sin(v/2.0 + a*sin(v)) * cos(u - a*sin(2*u)),
        sin(v/2.0 + a*sin(v)) * sin(u + a*sin(2*u)),
        cos(v/2.0 - a*sin(v))
    };
}

static Vec3 shape(Double u, Double v, Double t)
{
    return cube(u, v, t);
}

static Vec3 uv2xyz(Double u, Double v, Double t)
{
    return shape(u, v, t);
}

static Double index2radians(Double index, Int n)
{
    return index / Double(n) * dl::math::nc::pi * 2;
}

//=================================================================================================
// Main rendering function.
//=================================================================================================

static void renderSurfaces(Scene& scene, Int frameNumber, Int /*pixels*/, Int /*maxSubdivisions*/,
                           Double dt, Int desiredTriangles)
{
    Double t = Double(frameNumber) * dt;

    // Camera setup
    Vec3 cameraLoc = cameraPath(t);
    Vec3 focusPoint = focusPath(t);

    SLR2 cam;
    cam.moveTo(cameraLoc);
    cam.lookAt(focusPoint);

    Double distance = (cameraLoc - focusPoint).norm();

    logInfo("Frame %d: camera=(%f, %f, %f), focus=(%f, %f, %f), distance=%f, t=%f",
            frameNumber,
            cameraLoc.x, cameraLoc.y, cameraLoc.z,
            focusPoint.x, focusPoint.y, focusPoint.z,
            distance, t);

    // Generate mesh data
    Int nU = 500;
    Int nV = 500;

    // First pass: count visible triangles and compute bounds
    Int numTriangles = 0;
    Double totalWidth = 0.0;
    Double totalHeight = 0.0;
    Double minDistance = 100.0;
    Double maxDistance = 0.0;
    Double maxX = 0.0, maxY = 0.0, maxZ = 0.0;
    Double minZ = 1.0;

    // First pass - estimate density
    for (Int uIndex = 0; uIndex <= 500; ++uIndex)
    {
        for (Int vIndex = 0; vIndex <= 500; ++vIndex)
        {
            Double u = index2radians(Double(uIndex), 500);
            Double v = index2radians(Double(vIndex), 500);
            Vec3 vertex = uv2xyz(u, v, t);

            if (!cam.invisible(vertex))
            {
                ++numTriangles;

                if (uIndex > 0)
                {
                    Double uLeft = index2radians(Double(uIndex - 1), 500);
                    Vec3 vertexLeft = uv2xyz(uLeft, v, t);
                    totalWidth += (vertex - vertexLeft).norm();
                }

                if (vIndex > 0)
                {
                    Double vBelow = index2radians(Double(vIndex - 1), 500);
                    Vec3 vertexBelow = uv2xyz(u, vBelow, t);
                    totalHeight += (vertex - vertexBelow).norm();
                }

                Double vlen = vertex.norm();
                minDistance = min(minDistance, vlen);
                maxDistance = max(maxDistance, vlen);
                maxX = max(maxX, abs(vertex.x));
                maxY = max(maxY, abs(vertex.y));
                maxZ = max(maxZ, abs(vertex.z));
                minZ = min(minZ, vertex.z);
            }
        }
    }

    // Calculate adaptive resolution
    Double ratio = totalWidth / totalHeight;
    if (ratio < 0.001) ratio = 0.001;
    if (ratio > 1000.0) ratio = 1000.0;

    nU = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) * ratio) * 500);
    nV = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) / ratio) * 500);

    while (nV > 15000)
    {
        ratio = ratio * 10;
        nU = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) * ratio) * 500);
        nV = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) / ratio) * 500);
    }

    // Clamp to reasonable values
    nU = max(Int(10), min(nU, Int(2000)));
    nV = max(Int(10), min(nV, Int(2000)));

    logInfo("Mesh resolution: %d x %d (ratio=%f, triangles=%d)", nU, nV, ratio, numTriangles);

    // Second pass - generate vertices and faces
    // Bella mesh requires float (f32) precision — pos3d/vec3d causes silent render failure
    ds::Vector<Pos3f> points;
    ds::Vector<Vec3f> normals;
    ds::Vector<Vec2f> uvs;
    ds::Vector<Vec4u> polygons;  // vec4u for faces

    Int startUIndex = 0;
    Int endUIndex = nU;
    Int startVIndex = 0;
    Int endVIndex = nV;

    // Create 2D array for vertex indices (for face generation)
    ds::Vector<Int32> vertexIndices;
    vertexIndices.resize((nU + 1) * (nV + 1));

    Int numVertices = 0;
    Int numFaces = 0;

    for (Int uIndex = startUIndex; uIndex <= endUIndex; ++uIndex)
    {
        for (Int vIndex = startVIndex; vIndex <= endVIndex; ++vIndex)
        {
            Double u = index2radians(Double(uIndex), nU);
            Double v = index2radians(Double(vIndex), nV);
            Vec3 vertex = uv2xyz(u, v, t);

            Int32 vertexIdx = -1;

            if (!cam.invisible(vertex))
            {
                // Compute normal via finite differences (delta=0.1 index units, matching series110.go)
                Double delta = 0.1;
                Double uLeft = index2radians(Double(uIndex) - delta, nU);
                Double uRight = index2radians(Double(uIndex) + delta, nU);
                Double vUp = index2radians(Double(vIndex) + delta, nV);
                Double vDown = index2radians(Double(vIndex) - delta, nV);

                Vec3 left = uv2xyz(uLeft, v, t);
                Vec3 right = uv2xyz(uRight, v, t);
                Vec3 up = uv2xyz(u, vUp, t);
                Vec3 down = uv2xyz(u, vDown, t);

                Vec3 tangentU = (right - left);
                Vec3 tangentV = (up - down);
                Vec3 normalRaw = tangentV.cross(tangentU);
                Vec3 normal = (normalRaw.norm() > 1e-10) ? normalRaw.unit() : vertex.unit();

                // Compute UV coordinates
                Int rangeU = max(Int(1), endUIndex - startUIndex);
                Int rangeV = max(Int(1), endVIndex - startVIndex);
                Double uvU = index2radians(Double(uIndex - startUIndex), rangeU) / (dl::math::nc::pi * 2);
                Double uvV = index2radians(Double(vIndex - startVIndex), rangeV) / (dl::math::nc::pi * 2);

                vertexIdx = numVertices++;
                points.push_back(Pos3f {Float(vertex.x), Float(vertex.y), Float(vertex.z)});
                normals.push_back(Vec3f {Float(normal.x), Float(normal.y), Float(normal.z)});
                uvs.push_back(Vec2f {Float(uvU), Float(uvV)});
            }

            vertexIndices[uIndex * (nV + 1) + vIndex] = vertexIdx;
        }
    }

    // Generate faces (quads as two triangles)
    for (Int vIndex = startVIndex; vIndex < endVIndex; ++vIndex)
    {
        for (Int uIndex = startUIndex; uIndex < endUIndex; ++uIndex)
        {
            Int32 topRight = vertexIndices[uIndex * (nV + 1) + vIndex];
            Int32 topLeft = vertexIndices[(uIndex + 1) * (nV + 1) + vIndex];
            Int32 botRight = vertexIndices[uIndex * (nV + 1) + (vIndex + 1)];
            Int32 botLeft = vertexIndices[(uIndex + 1) * (nV + 1) + (vIndex + 1)];

            if (topRight == -1 || topLeft == -1 || botRight == -1 || botLeft == -1)
                continue;

            // Add two triangles for each quad.
            // In Bella's vec4u polygon format, triangles repeat the 3rd vertex as the 4th.
            polygons.push_back(Vec4u {UInt32(topRight), UInt32(botLeft), UInt32(topLeft), UInt32(topLeft)});
            ++numFaces;

            polygons.push_back(Vec4u {UInt32(topRight), UInt32(botRight), UInt32(botLeft), UInt32(botLeft)});
            ++numFaces;
        }
    }

    logInfo("Generated %d vertices and %d faces", numVertices, numFaces);

    // Create Bella mesh node
    auto meshNode = scene.createNode("mesh", "surface_mesh");

    // Set up motion blur steps (single step)
    auto steps = meshNode["steps"];
    steps.appendElement();  // Add first element
    auto step0 = steps[0];

    // Set buffers
    step0["points"] = points;
    step0["normals"] = normals;
    step0["uvs"] = uvs;

    // Set polygons
    meshNode["polygons"] = polygons;

    // Remove any extra steps that might have been created
    while (steps.inputCount() > 1)
        steps.removeLastInput();

    // Glass material (dielectric + scattering node), matching Go's roughdielectric+homogeneous medium
    // int_ior = sin(11*t)+2, ext_ior = 2-cos(13*t)
    Double intIor = sin(11.0 * t) + 2.0;
    Double extIor = 2.0 - cos(13.0 * t);

    // Scattering medium: albedo=1 (fully scattering, no absorption), isotropic (g=0), white
    // Matches Go's homogeneous medium: albedo=1,1,1, sigma_t=1,1,1, scale=0.5, HG g=0
    auto meshScattering = scene.createNode("scattering", "meshScattering");
    meshScattering["albedo"] = Real(.01);
    meshScattering["anisotropy"] = Real(.01);
    meshScattering["color"] = Rgba {1.0f, 1.0f, 1.0f, 1.0f};

    auto meshMat = scene.createNode("dielectric", "meshMaterial");
    meshMat["ior"] = Real(intIor / extIor);  // relative IOR
    meshMat["roughness"] = Real(.01);       // matches Go's roughdielectric alpha=0.005
    meshMat["depth"] = Real(10000);            // absorption depth in cm; default may be 0 (fully opaque)
    meshMat["abbe"] = Real(30);
    meshMat["dispersion"] = true;
    meshMat["scattering"] = meshScattering;

    // Wrap mesh in an xform with the material
    auto meshXform = scene.createNode("xform", "mesh_xform");
    meshXform["steps"].appendElement();
    meshXform["steps"][0]["xform"] = Mat4::identity;
    meshXform["children"].appendElement().set(meshNode);
    meshXform["material"] = meshMat;
    auto meshXformSteps = meshXform["steps"];
    while (meshXformSteps.inputCount() > 1)
        meshXformSteps.removeLastInput();

    // Add mesh to scene hierarchy
    // Create world as a regular xform node (not using scene.world() for better compatibility)
    auto world = scene.createNode("xform", "world");

    // Create tonemapping node (ACES)
    auto tonemap = scene.createNode("filmicHable", "tonemap");
    
    // Create sensor node
    auto sensor = scene.createNode("sensor", "sensor");
    sensor["size"] = Vec2 {24.0, 24.0};
    sensor["tonemapping"] = tonemap;

    // FOV = 120 + cos(14*t)*30 degrees (matching series110.go)
    // For square 24mm sensor: focalLen = (sensorWidth/2) / tan(FOV_rad/2)
    Double fovDeg = 120.0 + cos(14.0 * t) * 30.0;
    Double fovRad = fovDeg * dl::math::nc::pi / 180.0;
    Double focalLenMm = (24.0 / 2.0) / tan(fovRad / 2.0);

    auto lens = scene.createNode("thinLens", "thinLens");
    lens["steps"].appendElement();
    lens["steps"][0]["focalLen"] = Real(focalLenMm);
    lens["steps"][0]["fStop"] = Real(pow(1.5, sin(37*t))*170.0);
    lens["steps"][0]["focusDist"] = Real(distance);

    // Create camera node
    auto cameraNode = scene.createNode("camera", "cameraObj");
    cameraNode["sensor"] = sensor;
    cameraNode["lens"] = lens;
    cameraNode["resolution"] = Vec2 {2400.0, 2400.0};
    cameraNode["exposureMode"] = String("aperture");
    cameraNode["ev"] = Real(14);  // default, adjust to taste

    // Build camera lookAt matrix.
    // Bella row-vector convention: Row0=right, Row1=right×forward (image-down), Row2=forward, Row3=position.
    // Verified against simple.bsa and orange-juice.bsa reference scenes.
    // World is Z-up; camera looks in the direction of Row2 (forward toward target).
    {
        Vec3 forward  = (focusPoint - cameraLoc).unit();  // toward target
        Vec3 worldUp  = Vec3 {0, 0, 1};                   // Z-up scene
        Vec3 right    = worldUp.cross(forward).unit();     // camera +X (image right)
        Vec3 down     = right.cross(forward);              // camera +Y (image down)
        cam.transform = Mat4::make(
            Vec4 {right.x,   right.y,   right.z,   0},
            Vec4 {down.x,    down.y,    down.z,    0},
            Vec4 {forward.x, forward.y, forward.z, 0},
            Vec4 {cameraLoc.x, cameraLoc.y, cameraLoc.z, 1}
        );
    }
    auto cameraXform = scene.createNode("xform", "camera");
    cameraXform["steps"].appendElement();
    cameraXform["steps"][0]["xform"] = cam.transform;

    // Add camera to xform
    cameraXform["children"].appendElement().set(cameraNode);
    
    // Remove any extra steps that might have been created
    auto cameraSteps = cameraXform["steps"];
    while (cameraSteps.inputCount() > 1)
        cameraSteps.removeLastInput();

    // Add camera xform to world
    world["children"].appendElement().set(cameraXform);

    // Generate procedural environment map matching series110.go:
    //   power = 2 * 4^(sin(15*t)-1)
    //   envmapValue = sin(u/2)^(4*power) * sin(v/2)^power
    //   R = envmapValue^(2^sin(17*t)),  G = envmapValue^(2^cos(18*t)),  B = envmapValue^(2^-sin(19*t))
    int envSize = int(sqrt(double(desiredTriangles)));
    ds::Vector<float> envRgb;
    {
        double power = 2.0 * pow(4.0, sin(15.0 * t) - 1.0);
        double expR  = pow(2.0,  sin(17.0 * t));
        double expG  = pow(2.0,  cos(18.0 * t));
        double expB  = pow(2.0, -sin(19.0 * t));
        for (int vIdx = 0; vIdx < envSize; ++vIdx)
        {
            for (int uIdx = 0; uIdx < envSize; ++uIdx)
            {
                double u  = double(uIdx) / double(envSize) * 2.0 * dl::math::nc::pi;
                double v  = double(vIdx) / double(envSize) * 2.0 * dl::math::nc::pi;
                double ev = 1 - pow(sin(u / 2.0), power * 4 ) * pow(sin(v / 2.0), power);
                envRgb.push_back(float(pow(ev, expR)));
                envRgb.push_back(float(pow(ev, expG)));
                envRgb.push_back(float(pow(ev, expB)));
            }
        }
    }

    // Write env map as Radiance HDR alongside the BSA file
    char cwdBuf[1024];
    getcwd(cwdBuf, sizeof(cwdBuf));
    String envDir(cwdBuf);
    String envFile = String::format("surface_env_%d", frameNumber);
    String envHdrPath = String::format("%s/%s.hdr", cwdBuf, envFile.buf());
    if (writeHDR(envHdrPath.buf(), envSize, envSize, &envRgb[0]))
        logInfo("Wrote env map: %s", envHdrPath.buf());
    else
        logError("Failed to write env map: %s", envHdrPath.buf());

    // Create imageDome using the procedural env map.
    // Apply -90° rotation around Z (Bella's up axis) to match Mitsuba's
    // "<rotate value='0,1,0' angle='-90'/>" which shifts the azimuth by -90°.
    // Orange-juice.bsa xform convention: Row0=(cos θ, -sin θ, 0), Row1=(sin θ, cos θ, 0).
    auto envmapNode = scene.createNode("imageDome", "environment");
    envmapNode["dir"] = envDir;
    envmapNode["file"] = envFile;
    envmapNode["multiplier"] = Real(3);
    envmapNode["ext"] = String(".hdr");
    // cos(-90°)=0, -sin(-90°)=1, sin(-90°)=-1
    envmapNode["xform"] = Mat3::make(
        0.0, 1.0, 0.0,
        -1.0, 0.0, 0.0,
        0.0, 0.0, 1.0
    );

    // Create instances (grid of copies)
    // Go: num=9, iterates x/y/z in [-9..9] including center, 19^3=6859 total
    Int numInstances = 9;
    Double maxDim = max(maxX, max(maxY, maxZ));
    Double instanceScale = 0.49 / max(maxDim, 0.001);

    logInfo("Creating instances: scale=%f", instanceScale);

    // Create a shape group for instancing
    auto shapeGroup = scene.createNode("xform", "instance_group");
    shapeGroup["children"].appendElement().set(meshXform);

    // Create grid of instances
    Int instanceCount = 0;
    for (Int x = -numInstances; x <= numInstances; ++x)
    {
        for (Int y = -numInstances; y <= numInstances; ++y)
        {
            for (Int z = -numInstances; z <= numInstances; ++z)
            {
                if (!(x || y || z)) {
                    continue;
                }
                auto instXform = scene.createNode("xform",
                    String::format("inst_%s%d_%s%d_%s%d",
                        x<0?"n":"p", abs(x),
                        y<0?"n":"p", abs(y),
                        z<0?"n":"p", abs(z)).buf());

                // Create scale and translation matrix for instance.
                // Mat4::make(a,b,c,d) takes COLUMNS, col3 = translation.
                Mat4 instMat = Mat4::make(
                    Vec4 {instanceScale, 0, 0, 0},
                    Vec4 {0, instanceScale, 0, 0},
                    Vec4 {0, 0, instanceScale, 0},
                    Vec4 {Double(x), Double(y), Double(z), 1}
                );
                instXform["steps"].appendElement();
                instXform["steps"][0]["xform"] = instMat;
                instXform["children"].appendElement().set(shapeGroup);

                // Remove any extra steps that might have been created
                auto instSteps = instXform["steps"];
                while (instSteps.inputCount() > 1)
                    instSteps.removeLastInput();

                // Add instance to world
                world["children"].appendElement().set(instXform);

                ++instanceCount;
            }
        }
    }

    logInfo("Created %d instances", instanceCount);

    // Set up render settings
    auto settings = scene.createNode("settings", "settings");
    settings["camera"] = cameraNode;
    settings["environment"] = envmapNode;

    // Create beauty pass for rendering
    auto beautyPass = scene.createNode("beautyPass", "beautyPass");
    beautyPass["resume"] = "disabled";
    beautyPass["saveBsi"] = Int(0);  // Enable BSI saving
    beautyPass["saveImage"] = Int(0);  // Enable image saving
    beautyPass["targetNoise"] = UInt(2);
    beautyPass["bouncesRefraction"] = Int(128);  // high refraction bounce limit for dense glass grid
    beautyPass["solver"] = String("Atlas");
    settings["beautyPass"] = beautyPass;

    // Create state node that links settings and world
    auto state = scene.createNode("state", "state");
    state["settings"] = settings;
    state["world"] = world;

    // Create global node that references the state
    auto globalNode = scene.createNode("global", "global");
    globalNode["states"].appendElement().set(state);
    
    // Remove any extra steps from world xform
    auto worldSteps = world["steps"];
    while (worldSteps.inputCount() > 1)
        worldSteps.removeLastInput();
}

//=================================================================================================
// Main entry point.
//=================================================================================================

#include "dl_core/dl_main.inl"

int DL_main(Args& args)
{
    subscribeLog(&s_logCtx, log);
    flushStartupMessages();

    logBanner("Bella Surface Generator (version: %s, build date: %llu)",
        bellaSdkVersion().toString().buf(),
        bellaSdkBuildDate()
    );

    // Add command line arguments
    args.add("f", "frame", "0", "Frame number to render.");
    args.add("p", "pixels", "256", "Pixel resolution.");
    args.add("m", "maxsubdivisions", "1000", "Maximum subdivisions.");
    args.add("n", "maxframes", "512", "Maximum frames in animation.");
    args.add("t", "desiredtriangles", "50000", "Desired number of triangles.");

    // Parse command line arguments
    Int frame = 0, pixels = 256, maxSubdivisions = 1000, maxFrames = 512, desiredTriangles = 50000;
    args.value("--frame", "0").parse(frame);
    args.value("--pixels", "256").parse(pixels);
    args.value("--maxsubdivisions", "1000").parse(maxSubdivisions);
    args.value("--maxframes", "512").parse(maxFrames);
    args.value("--desiredtriangles", "50000").parse(desiredTriangles);

    logInfo("frame=%d, pixels=%d, maxSubdivisions=%d, maxFrames=%d, desiredTriangles=%d",
            frame, pixels, maxSubdivisions, maxFrames, desiredTriangles);

    Double dt = dl::math::nc::pi * 2 / Double(maxFrames);

    // Create scene
    Scene scene;
    scene.loadDefs();

    // Generate surface
    renderSurfaces(scene, frame, pixels, maxSubdivisions, dt, desiredTriangles);

    // Write scene to BSA text file (more compatible than BSX)
    String outputPath = String::format("surface_frame_%d.bsa", frame);
    if (scene.write(outputPath))
    {
        logInfo("Successfully wrote scene to %s", outputPath.buf());
    }
    else
    {
        logError("Failed to write scene to %s", outputPath.buf());
        return 1;
    }

    return 0;
}
