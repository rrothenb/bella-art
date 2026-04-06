// Bella Series 3 Generator - Port of series102.go
// Animated torus-knot tube surfaces for the Bella renderer

#include "bella_series_util.h"

//=================================================================================================
// Global animation time.
// outerKnot/innerKnot are called as bare function pointers from pathWrapper, so they can't take
// a 't' parameter for the outer knot radius.  s_globalT is set once per frame before any shape
// evaluation.
//=================================================================================================

static Double s_globalT = 0.0;

//=================================================================================================
// Math helpers — series102.go specific.
//=================================================================================================

// series102.go: strength(x) = sin(x)*1.25 + 1.25  (single-arg, unlike series1/2)
static Double strength(Double x)
{
    return sin(x) * 1.25 + 1.25;
}


//=================================================================================================
// Camera (SLR2) with FOV-based visibility culling.
// series102.go uses Z-up and a narrow 5° FOV.
//=================================================================================================

struct SLR2
{
    Double width  = 0.03;    // sensor width (m) — from Go source
    Double height = 0.02;    // sensor height (m) — from Go source
    Double FOV    = 5.0;     // field of view in degrees

    Vec3 position;
    Vec3 target;
    Vec3 rightVec;
    Vec3 upVec;
    Vec3 fwdVec;    // forward = (position - target).unit()  (points away from scene)
    Mat4 transform;

    SLR2()
        : position(Vec3{0, 0, 0})
        , target(Vec3{0, 0, -5})
    {
        computeTransform();
    }

    void moveTo(Vec3 p) { position = p; computeTransform(); }
    void lookAt(Vec3 t) { target   = t; computeTransform(); }

    void computeTransform()
    {
        fwdVec   = (position - target).unit();
        Vec3 z   = {0, 0, 1};
        rightVec = z.cross(fwdVec).unit();
        upVec    = fwdVec.cross(rightVec).unit();

        transform = Mat4::make(
            Vec4{rightVec.x,  rightVec.y,  rightVec.z,  0},
            Vec4{upVec.x,     upVec.y,     upVec.z,     0},
            Vec4{fwdVec.x,    fwdVec.y,    fwdVec.z,    0},
            Vec4{position.x,  position.y,  position.z,  1}
        );
    }

    // Project point into camera space and test against FOV frustum.
    // Matches series102.go SLR2.invisible exactly.
    Bool invisible(Vec3 point) const
    {
        Vec3 d  = point - position;
        Double px = d.x*rightVec.x + d.y*rightVec.y + d.z*rightVec.z;
        Double py = d.x*upVec.x    + d.y*upVec.y    + d.z*upVec.z;
        Double pz = d.x*fwdVec.x   + d.y*fwdVec.y   + d.z*fwdVec.z;

        // pz > 0 → behind camera (fwdVec points away from scene)
        if (pz > 0.0) return true;

        Double factor = tan(FOV * 1.25 / 360.0 * dl::math::nc::pi);
        Double aspect = width / height;   // 0.03/0.02 = 1.5

        if (px < pz * factor * aspect) return true;
        if (px > -pz * factor * aspect) return true;
        if (py < pz * factor) return true;
        if (py > -pz * factor) return true;

        return false;
    }
};

//=================================================================================================
// Torus-knot shape functions (series102.go).
//=================================================================================================

static Vec3 circle(Double t)
{
    return Vec3{sin(t), cos(t), 0};
}

static Vec3 torusKnot(Double t, Double R, Double r, Int p, Int q, Vec3(*path)(Double))
{
    Vec3   pp = path(Double(q) * t);
    Double rp = R + r * cos(Double(p) * t);
    return Vec3{rp * pp.x, rp * pp.y, r * sin(Double(p) * t) + pp.z};
}

// outerKnot and innerKnot use s_globalT for their radius animation.
// They are passed as function pointers to torusKnot/pathWrapper, so the outer time
// is threaded via the global rather than a closure.
static Vec3 outerKnot(Double t)
{
    return torusKnot(t, 1.0, 0.45 + sin(19 * s_globalT) * 0.25, 2, 3, circle);
}

static Vec3 innerKnot(Double t)
{
    return torusKnot(t, 1.0, 0.35 + sin(17 * s_globalT) * 0.25, 3, 2, outerKnot);
}

// Frenet-frame tube around a parametric path.
static Vec3 pathWrapper(Double u, Double v, Double r, Vec3(*path)(Double))
{
    Double delta  = 0.01;
    Vec3   center = path(v);
    Vec3   diff   = path(v + delta) - path(v - delta);
    Vec3   tangent = (diff.norm() > 1e-10) ? diff.unit() : Vec3{1, 0, 0};

    // sinVec = tangent × Z  (tube "horizontal" axis)
    // cosVec = tangent × sinVec (tube "vertical" axis)
    Vec3 sinVec = tangent.cross(Vec3{0, 0, 1}).unit();
    Vec3 cosVec = tangent.cross(sinVec);   // tangent⊥sinVec → |cosVec|=1

    Double cu = r * cos(u);
    Double su = r * sin(u);
    return Vec3{
        cosVec.x * cu + sinVec.x * su + center.x,
        cosVec.y * cu + sinVec.y * su + center.y,
        cosVec.z * cu + sinVec.z * su + center.z
    };
}

//=================================================================================================
// Texture and surface functions (series102.go).
//=================================================================================================

// series102 texture.  NOTE: uses v*10 scaling and single-arg strength.
static Double texture(Double u, Double v, Double t)
{
    Double vs = v * 10.0;
    return sin(
        3*u + 5*vs
        + strength(1.7 + 19*t) * sin(2*u  + strength(0.7 + 7*t) * sin(3*u  + strength(0.3 + 3*t) * sin(5*u  - 7*vs) * sin(11*u + 3*vs)))
        + strength(1.5 + 17*t) * sin(7*vs + strength(0.5 + 5*t) * sin(5*vs + strength(0.1 + 2*t) * sin(7*u  - 11*vs) * sin(13*u + 5*vs)))
        + strength(1.3 + 13*t) * sin(5*u + 7*vs)
        + strength(1.1 + 11*t) * sin(17*u) * sin(19*vs)
    );
}

// Tube surface wrapped around innerKnot.
// limitedV restricts the tube to a animated arc of the knot path.
// The ridge parameter and blend value modulate tube radius.
static Vec3 uv2xyz(Double u, Double v, Double t)
{
    Double pi      = dl::math::nc::pi;
    Double minV    = sin(2*t) * pi/2 + pi/2;
    Double maxV    = minV + sin(3*t) * pi/4 + pi/2;
    Double limitedV = minV + v / 2 / pi * (maxV - minV);
    Double ridges  = 20 + sin(7*t) * 15;
    Double a       = 1 - spow(cos(floor(ridges) * v), pow(10, -cos(5*t))) * 0.5;
    Double bv      = pow(spow(texture(u, v, t), pow(2, cos(13*t))) / 2 + 0.5,
                         pow(2, cos(17*t))) * sin(23*t);
    Double rParam  = 0.25 * spow(sin(v/2), pow(10, sin(17*t))) * a * (1 + 0.1 * bv);
    Double vParam  = limitedV + pow(a - 0.5, pow(10, sin(11*t))) / (15 + sin(13*t) * 5);
    return pathWrapper(u, vParam, rParam, innerKnot);
}

//=================================================================================================
// Camera and focus paths (series102.go).
//=================================================================================================

// Camera orbits at distance 9+cos(5t) on a normalized Lissajous-ish arc.
static Vec3 cameraPath(Double t)
{
    Vec3 v = {cos(3*t), cos(2*t), 1.0};
    return v.unit() * (9 + cos(5*t));
}

// Focus tracks a point along the visible arc of the inner knot.
static Vec3 focusPath(Double t)
{
    Double pi      = dl::math::nc::pi;
    Double v       = pi - cos(t) * pi;
    Double minV    = sin(2*t) * pi/2 + pi/2;
    Double maxV    = minV + sin(3*t) * pi/4 + pi/2;
    Double minV2   = 0.75*minV + 0.25*maxV;
    Double maxV2   = 0.25*minV + 0.75*maxV;
    Double limitedV = minV2 + v / 2 / pi * (maxV2 - minV2);
    return innerKnot(limitedV);
}


//=================================================================================================
// Main rendering function.
//=================================================================================================

static void renderSurfaces(Scene& scene, Int frameNumber, Int /*pixels*/, Int /*maxSubdivisions*/,
                           Double dt, Int desiredTriangles)
{
    Double t = Double(frameNumber) * dt;
    s_globalT = t;

    Vec3 cameraLoc  = cameraPath(t);
    Vec3 focusPoint = focusPath(t);

    SLR2 cam;
    cam.FOV = 5.0;
    cam.moveTo(cameraLoc);
    cam.lookAt(focusPoint);

    Double distance = (cameraLoc - focusPoint).norm();

    logInfo("Frame %d: camera=(%f,%f,%f), focus=(%f,%f,%f), dist=%f, t=%f",
            frameNumber,
            cameraLoc.x, cameraLoc.y, cameraLoc.z,
            focusPoint.x, focusPoint.y, focusPoint.z,
            distance, t);

    //---------------------------------------------------------------------------------------------
    // First pass: 1001×1001 sample grid — count visible triangles and find UV bounds.
    // series102.go uses minU/maxU/minV/maxV to restrict the mesh to the visible arc.
    //---------------------------------------------------------------------------------------------
    Int    numTriangles = 0;
    Double totalWidth   = 0.0, totalHeight = 0.0;
    Int    minU = 1000, maxU = 0, minV = 1000, maxV = 0;

    for (Int uIdx = 0; uIdx <= 1000; ++uIdx)
    {
        for (Int vIdx = 0; vIdx <= 1000; ++vIdx)
        {
            Vec3 vertex = uv2xyz(
                index2radians(Double(uIdx),   1000),
                index2radians(Double(vIdx),   1000), t);

            if (!cam.invisible(vertex))
            {
                ++numTriangles;
                Vec3 vL = uv2xyz(index2radians(Double(uIdx-1), 1000), index2radians(Double(vIdx),   1000), t);
                Vec3 vB = uv2xyz(index2radians(Double(uIdx),   1000), index2radians(Double(vIdx-1), 1000), t);
                totalWidth  += (vertex - vL).norm();
                totalHeight += (vertex - vB).norm();
                if (minU > uIdx) minU = uIdx;
                if (minV > vIdx) minV = vIdx;
                if (maxU < uIdx) maxU = uIdx;
                if (maxV < vIdx) maxV = vIdx;
            }
        }
    }

    if (numTriangles == 0)
    {
        logError("Frame %d: no visible triangles — skipping", frameNumber);
        return;
    }

    Double ratio = totalWidth / totalHeight;
    if (ratio < 0.001) ratio = 0.001;
    if (ratio > 1000.0) ratio = 1000.0;

    // Compute nU/nV and the visible UV index range, matching series102.go exactly.
    Double scaleU = sqrt(Double(desiredTriangles) / Double(numTriangles * 2) * ratio);
    Double scaleV = sqrt(Double(desiredTriangles) / Double(numTriangles * 2) / ratio);

    Int nU          = Int(scaleU * 1000);
    Int nV          = Int(scaleV * 1000);
    Int startUIndex = Int(scaleU * Double(minU));
    Int endUIndex   = Int(scaleU * Double(maxU));
    Int startVIndex = Int(scaleV * Double(minV));
    Int endVIndex   = Int(scaleV * Double(maxV));

    nU = max(Int(10), min(nU, Int(5000)));
    nV = max(Int(10), min(nV, Int(5000)));
    if (endUIndex >= nU) endUIndex = nU - 1;
    if (endVIndex >= nV) endVIndex = nV - 1;
    if (startUIndex >= endUIndex) startUIndex = 0;
    if (startVIndex >= endVIndex) startVIndex = 0;

    logInfo("Mesh: nU=%d nV=%d startU=%d endU=%d startV=%d endV=%d ratio=%f triangles=%d",
            nU, nV, startUIndex, endUIndex, startVIndex, endVIndex, ratio, numTriangles);

    //---------------------------------------------------------------------------------------------
    // Second pass: generate vertices and faces.
    //---------------------------------------------------------------------------------------------
    // Offset-based vertex index array covering [startUIndex..endUIndex] × [startVIndex..endVIndex].
    Int dV = endVIndex - startVIndex + 2;
    Int dU = endUIndex - startUIndex + 2;
    ds::Vector<Int32> vertexIndices;
    vertexIndices.resize(dU * dV);
    for (Int i = 0; i < dU * dV; ++i) vertexIndices[i] = Int32(-1);

    ds::Vector<Pos3f> points;
    ds::Vector<Vec3f> normals;
    ds::Vector<Vec2f> uvs;

    Int numVertices = 0;

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
                Double delta = 0.1;
                Vec3 left  = uv2xyz(index2radians(Double(uIndex) - delta, nU), v, t);
                Vec3 right = uv2xyz(index2radians(Double(uIndex) + delta, nU), v, t);
                Vec3 up    = uv2xyz(u, index2radians(Double(vIndex) + delta, nV), t);
                Vec3 down  = uv2xyz(u, index2radians(Double(vIndex) - delta, nV), t);
                Vec3 tanU      = right - left;
                Vec3 tanV      = up - down;
                Vec3 normalRaw = tanV.cross(tanU);
                Vec3 normal    = (normalRaw.norm() > 1e-10) ? normalRaw.unit() : vertex.unit();

                // UV coordinates normalised to [0,1] within the visible range.
                Double uvU = Double(uIndex - startUIndex) / Double(max(Int(1), endUIndex - startUIndex));
                Double uvV = Double(vIndex - startVIndex) / Double(max(Int(1), endVIndex - startVIndex));

                vertexIdx = numVertices++;
                points.push_back(Pos3f{Float(vertex.x), Float(vertex.y), Float(vertex.z)});
                normals.push_back(Vec3f{Float(normal.x), Float(normal.y), Float(normal.z)});
                uvs.push_back(Vec2f{Float(uvU), Float(uvV)});
            }
            vertexIndices[(uIndex - startUIndex) * dV + (vIndex - startVIndex)] = vertexIdx;
        }
    }

    // Generate quads (as two triangles) and blend texture values simultaneously.
    Int blendW = max(Int(1), endUIndex - startUIndex);
    Int blendH = max(Int(1), endVIndex - startVIndex);

    ds::Vector<Vec4u> polygons;
    ds::Vector<float> blendRgb;
    Int numFaces = 0;

    for (Int vIndex = startVIndex; vIndex < endVIndex; ++vIndex)
    {
        for (Int uIndex = startUIndex; uIndex < endUIndex; ++uIndex)
        {
            // Blend value: series102.go formula
            // 1 - pow(spow(texture,...)/2+.5, pow(4, cos(17t)))
            Double u  = index2radians(Double(uIndex), nU);
            Double v  = index2radians(Double(vIndex), nV);
            Double bv = 1.0 - pow(spow(texture(u, v, t), pow(2, cos(13*t))) / 2.0 + 0.5,
                                  pow(4, cos(17*t)));
            float  bvf = Float(bv);
            blendRgb.push_back(bvf);
            blendRgb.push_back(bvf);
            blendRgb.push_back(bvf);

            Int32 topRight = vertexIndices[(uIndex     - startUIndex) * dV + (vIndex     - startVIndex)];
            Int32 topLeft  = vertexIndices[(uIndex + 1 - startUIndex) * dV + (vIndex     - startVIndex)];
            Int32 botRight = vertexIndices[(uIndex     - startUIndex) * dV + (vIndex + 1 - startVIndex)];
            Int32 botLeft  = vertexIndices[(uIndex + 1 - startUIndex) * dV + (vIndex + 1 - startVIndex)];

            if (topRight == -1 || topLeft == -1 || botRight == -1 || botLeft == -1)
                continue;

            polygons.push_back(Vec4u{UInt32(topRight), UInt32(botLeft),  UInt32(topLeft),  UInt32(topLeft)});
            ++numFaces;
            polygons.push_back(Vec4u{UInt32(topRight), UInt32(botRight), UInt32(botLeft),  UInt32(botLeft)});
            ++numFaces;
        }
    }

    logInfo("Generated %d vertices and %d faces", numVertices, numFaces);

    //---------------------------------------------------------------------------------------------
    // Bella mesh node.
    //---------------------------------------------------------------------------------------------
    auto meshNode = scene.createNode("mesh", "surface_mesh");
    auto steps = meshNode["steps"];
    steps.appendElement();
    auto step0 = steps[0];
    step0["points"]  = points;
    step0["normals"] = normals;
    step0["uvs"]     = uvs;
    meshNode["polygons"] = polygons;
    while (steps.inputCount() > 1) steps.removeLastInput();

    //---------------------------------------------------------------------------------------------
    // Materials.
    // Color1 = (cos(2t)/2+.5, cos(3t)/2+.5, cos(5t)/2+.5)  rough1 = 10^(sin(5t)-2)
    // Color2 = (.5-cos(13t)/2, .5-cos(11t)/2, .5-cos(7t)/2) rough2 = 10^(cos(7t)-2)
    //---------------------------------------------------------------------------------------------
    Double rough1 = pow(10.0, sin(5*t) - 2.0);
    Double rough2 = pow(10.0, cos(7*t) - 2.0);

    auto conductorMat1 = scene.createNode("conductor", "conductorMat1");
    conductorMat1["reflectance"] = Rgba{Float(cos(2*t)/2 + 0.5), Float(cos(3*t)/2 + 0.5), Float(cos(5*t)/2 + 0.5), 1.0f};
    conductorMat1["roughness"]   = Real(rough1);

    auto conductorMat2 = scene.createNode("conductor", "conductorMat2");
    conductorMat2["reflectance"] = Rgba{Float(0.5 - cos(13*t)/2), Float(0.5 - cos(11*t)/2), Float(0.5 - cos(7*t)/2), 1.0f};
    conductorMat2["roughness"]   = Real(rough2);

    //---------------------------------------------------------------------------------------------
    // Blend texture.
    //---------------------------------------------------------------------------------------------
    char cwdBuf[1024];
    getcwd(cwdBuf, sizeof(cwdBuf));

    String blendFile    = String::format("series3_blend_%d", frameNumber);
    String blendHdrPath = String::format("%s/%s.hdr", cwdBuf, blendFile.buf());
    if (writeHDR(blendHdrPath.buf(), blendW, blendH, &blendRgb[0]))
        logInfo("Wrote blend map: %s", blendHdrPath.buf());
    else
        logError("Failed to write blend map: %s", blendHdrPath.buf());

    auto blendTex = scene.createNode("fileTexture", "blendTex");
    blendTex["dir"]  = String(cwdBuf);
    blendTex["file"] = blendFile;
    blendTex["ext"]  = String(".hdr");

    // blendMaterial: conductorMat2 base, conductorMat1 overlay driven by blend texture.
    // high blend → conductorMat1; low blend → conductorMat2 (matches Mitsuba outer blendbsdf).
    auto meshMat = scene.createNode("blendMaterial", "meshMaterial");
    meshMat["mixing"] = String("ordered");
    meshMat["materials"].appendElement();
    meshMat["materials"][0]["material"] = conductorMat2;
    meshMat["materials"][0]["opacity"]  = Real(1);
    meshMat["materials"].appendElement();
    meshMat["materials"][1]["material"] = conductorMat1;
    meshMat["materials"][1]["opacity"] |= blendTex.output("outAverage");

    //---------------------------------------------------------------------------------------------
    // World.
    //---------------------------------------------------------------------------------------------
    auto world = scene.createNode("xform", "world");
    world["steps"].appendElement();
    world["steps"][0]["xform"] = Mat4::identity;

    //---------------------------------------------------------------------------------------------
    // Mesh xform.
    //---------------------------------------------------------------------------------------------
    auto meshXform = scene.createNode("xform", "mesh_xform");
    meshXform["steps"].appendElement();
    meshXform["steps"][0]["xform"] = Mat4::identity;
    meshXform["children"].appendElement().set(meshNode);
    meshXform["material"] = meshMat;
    {
        auto s = meshXform["steps"];
        while (s.inputCount() > 1) s.removeLastInput();
    }
    world["children"].appendElement().set(meshXform);

    //---------------------------------------------------------------------------------------------
    // Camera.
    // series102.go: FOV=5° (telephoto), Z-up lookat, aperture≈0.
    //---------------------------------------------------------------------------------------------
    auto tonemap = scene.createNode("filmicHable", "tonemap");

    auto sensor = scene.createNode("sensor", "sensor");
    sensor["size"]        = Vec2{24.0, 24.0};
    sensor["tonemapping"] = tonemap;

    Double fovDeg     = 5.0;
    Double fovRad     = fovDeg * dl::math::nc::pi / 180.0;
    Double focalLenMm = (24.0 / 2.0) / tan(fovRad / 2.0);

    auto lens = scene.createNode("thinLens", "thinLens");
    lens["steps"].appendElement();
    lens["steps"][0]["focalLen"]  = Real(focalLenMm);
    lens["steps"][0]["fStop"]     = Real(1e9);        // pinhole
    lens["steps"][0]["focusDist"] = Real(distance);

    auto cameraNode = scene.createNode("camera", "cameraObj");
    cameraNode["sensor"]       = sensor;
    cameraNode["lens"]         = lens;
    cameraNode["resolution"]   = Vec2{720.0, 720.0};
    cameraNode["exposureMode"] = String("aperture");
    cameraNode["ev"]           = Real(14);

    cam.transform = bellaLookAt(cameraLoc, focusPoint, Vec3{0, 0, 1});

    auto cameraXform = scene.createNode("xform", "camera");
    cameraXform["steps"].appendElement();
    cameraXform["steps"][0]["xform"] = cam.transform;
    cameraXform["children"].appendElement().set(cameraNode);
    {
        auto s = cameraXform["steps"];
        while (s.inputCount() > 1) s.removeLastInput();
    }
    world["children"].appendElement().set(cameraXform);

    //---------------------------------------------------------------------------------------------
    // Environment map.
    // series102.go: pow(sin(u/2),4)*pow(sin(v),4) — constant shape, animated rotation.
    // Rotation: X=sin(19t)*45°, Y=sin(23t)*45°, Z=sin(29t)*45°.
    //---------------------------------------------------------------------------------------------
    int envSize = max(int(sqrt(double(desiredTriangles))), 2);
    ds::Vector<float> envRgb;
    {
        for (int vIdx = 0; vIdx < envSize; ++vIdx)
        {
            for (int uIdx = 0; uIdx < envSize; ++uIdx)
            {
                double eu  = double(uIdx) / double(envSize) * 2.0 * dl::math::nc::pi;
                double ev  = double(vIdx) / double(envSize) * dl::math::nc::pi;
                double val = pow(abs(sin(eu / 2.0)), 4.0) * pow(abs(sin(ev)), 4.0);
                envRgb.push_back(float(val));
                envRgb.push_back(float(val));
                envRgb.push_back(float(val));
            }
        }
    }

    String envDir(cwdBuf);
    String envFile    = String::format("series3_env_%d", frameNumber);
    String envHdrPath = String::format("%s/%s.hdr", cwdBuf, envFile.buf());
    if (writeHDR(envHdrPath.buf(), envSize, envSize, &envRgb[0]))
        logInfo("Wrote env map: %s", envHdrPath.buf());
    else
        logError("Failed to write env map: %s", envHdrPath.buf());

    auto envmapNode = scene.createNode("imageDome", "environment");
    envmapNode["dir"]        = envDir;
    envmapNode["file"]       = envFile;
    envmapNode["multiplier"] = Real(3);
    envmapNode["ext"]        = String(".hdr");

    // Animated 3-axis rotation matching Mitsuba's sequential X→Y→Z extrinsic rotations.
    // In Bella row-major: combined = Rx_row * Ry_row * Rz_row = (Rz_col*Ry_col*Rx_col)^T.
    {
        Double ax = sin(19*t) * 45.0 * dl::math::nc::pi / 180.0;
        Double ay = sin(23*t) * 45.0 * dl::math::nc::pi / 180.0;
        Double az = sin(29*t) * 45.0 * dl::math::nc::pi / 180.0;
        Double cx = cos(ax), sx = sin(ax);
        Double cy = cos(ay), sy = sin(ay);
        Double cz = cos(az), sz = sin(az);
        envmapNode["xform"] = Mat3::make(
             cy*cz,                   cy*sz,                  -sy,
             sy*cz*sx - sz*cx,        sy*sz*sx + cz*cx,        cy*sx,
             sy*cz*cx + sz*sx,        sy*sz*cx - cz*sx,        cy*cx
        );
    }

    //---------------------------------------------------------------------------------------------
    // Render settings.
    //---------------------------------------------------------------------------------------------
    auto settings = scene.createNode("settings", "settings");
    settings["camera"]      = cameraNode;
    settings["environment"] = envmapNode;

    auto beautyPass = scene.createNode("beautyPass", "beautyPass");
    beautyPass["resume"]            = "disabled";
    beautyPass["saveBsi"]           = Int(0);
    beautyPass["saveImage"]         = Int(0);
    beautyPass["targetNoise"]       = UInt(2);
    beautyPass["bouncesRefraction"] = Int(128);
    beautyPass["solver"]            = String("Atlas");
    settings["beautyPass"] = beautyPass;

    auto state = scene.createNode("state", "state");
    state["settings"] = settings;
    state["world"]    = world;

    auto globalNode = scene.createNode("global", "global");
    globalNode["states"].appendElement().set(state);

    {
        auto s = world["steps"];
        while (s.inputCount() > 1) s.removeLastInput();
    }
}

//=================================================================================================
// Main entry point.
//=================================================================================================

#include "dl_core/dl_main.inl"

int DL_main(Args& args)
{
    subscribeLog(&s_logCtx, log);
    flushStartupMessages();

    logBanner("Bella Series 3 Generator (version: %s, build date: %llu)",
        bellaSdkVersion().toString().buf(),
        bellaSdkBuildDate()
    );

    args.add("f", "frame",           "0",     "Frame number to render.");
    args.add("p", "pixels",          "256",   "Pixel resolution.");
    args.add("m", "maxsubdivisions", "1000",  "Maximum subdivisions.");
    args.add("n", "maxframes",       "512",   "Maximum frames in animation.");
    args.add("t", "desiredtriangles","50000", "Desired number of triangles.");

    Int frame = 0, pixels = 256, maxSubdivisions = 1000, maxFrames = 512, desiredTriangles = 50000;
    args.value("--frame",           "0"    ).parse(frame);
    args.value("--pixels",          "256"  ).parse(pixels);
    args.value("--maxsubdivisions", "1000" ).parse(maxSubdivisions);
    args.value("--maxframes",       "512"  ).parse(maxFrames);
    args.value("--desiredtriangles","50000").parse(desiredTriangles);

    logInfo("frame=%d, pixels=%d, maxSubdivisions=%d, maxFrames=%d, desiredTriangles=%d",
            frame, pixels, maxSubdivisions, maxFrames, desiredTriangles);

    Double dt = dl::math::nc::pi * 2 / Double(maxFrames);

    Scene scene;
    scene.loadDefs();

    renderSurfaces(scene, frame, pixels, maxSubdivisions, dt, desiredTriangles);

    String outputPath = String::format("series3_frame_%d.bsa", frame);
    if (scene.write(outputPath))
        logInfo("Successfully wrote scene to %s", outputPath.buf());
    else
    {
        logError("Failed to write scene to %s", outputPath.buf());
        return 1;
    }

    return 0;
}
