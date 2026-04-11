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
// strength(x) = sin(x)*1.25 + 1.25  (single-arg, unlike series1/2)
//=================================================================================================

static Double strength(Double x)
{
    return sin(x) * 1.25 + 1.25;
}

//=================================================================================================
// Torus-knot shape functions (series102.go).
// circle, torusKnot, pathWrapper come from the shared header.
// outerKnot/innerKnot are series-specific — they thread s_globalT via the global.
//=================================================================================================

static Vec3 outerKnot(Double t)
{
    return torusKnot(t, 1.0, 0.45 + sin(19 * s_globalT) * 0.05, 2, 3, circle);
}

static Vec3 innerKnot(Double t)
{
    return torusKnot(t, 1.0, 0.75 + sin(17 * s_globalT) * 0.05, 3, 2, circle);
}

//=================================================================================================
// Texture and surface functions (series102.go).
//=================================================================================================

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
// limitedV restricts the tube to an animated arc of the knot path.
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

static Vec3 cameraPath(Double t)
{
    return {0.1, 0.1, 1};
}

static Vec3 focusPath(Double t)
{
    Double pi      = dl::math::nc::pi;
    return uv2xyz((sin(2*t)+1)*pi,(sin(3*t)+1)*pi,t);
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
    cam.moveTo(cameraLoc);
    cam.lookAt(focusPoint);

    Double distance = (cameraLoc - focusPoint).norm();

    logInfo("Frame %d: camera=(%f,%f,%f), focus=(%f,%f,%f), dist=%f, t=%f",
            frameNumber,
            cameraLoc.x, cameraLoc.y, cameraLoc.z,
            focusPoint.x, focusPoint.y, focusPoint.z,
            distance, t);

    //---------------------------------------------------------------------------------------------
    // Camera lens parameters — computed early so the coarse pass can use CoC weighting.
    //---------------------------------------------------------------------------------------------
    Double fovDeg     = 20.0 + 10 * sin(37 * t);
    Double fovRad     = fovDeg * dl::math::nc::pi / 180.0;
    Double focalLenMm = (24.0 / 2.0) / tan(fovRad / 2.0);
    Double fStop      = 16 * pow(4, sin(31 * t));
    Double pixelMm    = 24.0 / 2400.0;  // sensor size / resolution

    //---------------------------------------------------------------------------------------------
    // Coarse pass: 500×500 sample grid.
    // Accumulates arc-lengths in u and v (to derive the nU/nV aspect ratio for square patches)
    // and CoC-weighted distance² from the camera (to build adaptive breakpoints — finer near
    // camera and in-focus regions).  CoC blur factor penalises out-of-focus geometry so that
    // vertices are concentrated where geometric detail is actually visible.
    //---------------------------------------------------------------------------------------------
    const Int coarseN = 500;

    ds::Vector<Double> uArcLen, vArcLen, uWeight, vWeight;
    uArcLen.resize(coarseN); vArcLen.resize(coarseN);
    uWeight.resize(coarseN); vWeight.resize(coarseN);
    for (Int i = 0; i < coarseN; ++i)
        uArcLen[i] = vArcLen[i] = uWeight[i] = vWeight[i] = 0.0;

    for (Int ui = 0; ui < coarseN; ++ui)
    {
        for (Int vi = 0; vi < coarseN; ++vi)
        {
            Double u  = index2radians(Double(ui),     coarseN);
            Double v  = index2radians(Double(vi),     coarseN);
            Double u1 = index2radians(Double(ui + 1), coarseN);
            Double v1 = index2radians(Double(vi + 1), coarseN);
            Vec3   p  = uv2xyz(u,  v,  t);
            Vec3   pu = uv2xyz(u1, v,  t);
            Vec3   pv = uv2xyz(u,  v1, t);
            Double d  = (p - cameraLoc).norm();
            Double coc = focalLenMm / fStop * abs(1.0 / distance - 1.0 / d);
            Double blur = max(1.0, coc / pixelMm);
            Double w  = d * d * blur;
            uArcLen[ui] += (pu - p).norm();
            vArcLen[vi] += (pv - p).norm();
            uWeight[ui] += w;
            vWeight[vi] += w;
        }
    }

    Double totalArcU = 0.0, totalArcV = 0.0;
    for (Int i = 0; i < coarseN; ++i) { totalArcU += uArcLen[i]; totalArcV += vArcLen[i]; }
    Double ratio = (totalArcV > 1e-10) ? totalArcU / totalArcV : 1.0;
    ratio = max(0.001, min(1000.0, ratio));

    const Int minDim = 10;
    Int nU = Int(sqrt(Double(desiredTriangles) / 2.0 * ratio));
    Int nV = Int(sqrt(Double(desiredTriangles) / 2.0 / ratio));

    // Redistribute: if one dimension falls below the minimum, give the other all the budget.
    if (nU < minDim) { nU = minDim; nV = max(minDim, Int(Double(desiredTriangles) / 2.0 / Double(nU))); }
    if (nV < minDim) { nV = minDim; nU = max(minDim, Int(Double(desiredTriangles) / 2.0 / Double(nV))); }

    logInfo("Adaptive mesh: nU=%d nV=%d ratio=%f", nU, nV, ratio);

    auto uBreaks = buildBreakpoints(uWeight, nU);
    auto vBreaks = buildBreakpoints(vWeight, nV);

    //---------------------------------------------------------------------------------------------
    // Blend texture — evaluated at the adaptive breakpoint grid.
    // 1 - pow(spow(texture,...)/2+.5, pow(4, cos(17t)))
    //---------------------------------------------------------------------------------------------
    char cwdBuf[1024];
    getcwd(cwdBuf, sizeof(cwdBuf));

    Int blendW = max(Int(1), nU - 1);
    Int blendH = max(Int(1), nV - 1);

    ds::Vector<float> blendRgb;
    for (Int vi = 0; vi < nV - 1; ++vi)
    {
        for (Int ui = 0; ui < nU - 1; ++ui)
        {
            Double u  = uBreaks[ui];
            Double v  = vBreaks[vi];
            Double bv = 1.0 - pow(spow(texture(u, v, t), pow(2, cos(13*t))) / 2.0 + 0.5,
                                  pow(4, cos(17*t)));
            float bvf = Float(bv);
            blendRgb.push_back(bvf);
            blendRgb.push_back(bvf);
            blendRgb.push_back(bvf);
        }
    }

    //---------------------------------------------------------------------------------------------
    // Generate mesh using adaptive breakpoints.
    //---------------------------------------------------------------------------------------------
    ds::Vector<Pos3f> points;
    ds::Vector<Vec3f> normals;
    ds::Vector<Vec2f> uvs;
    ds::Vector<Vec4u> polygons;

    generateMeshData(uv2xyz, uBreaks, vBreaks, t, points, normals, uvs, polygons);

    auto meshNode = buildBellaMesh(scene, "surface_mesh", points, normals, uvs, polygons);

    {
        String objPath = String::format("%s/series3_mesh_%d.obj", cwdBuf, frameNumber);
        if (writeMeshOBJ(objPath.buf(), points, normals, uvs, polygons))
            logInfo("Wrote mesh OBJ: %s", objPath.buf());
        else
            logError("Failed to write mesh OBJ: %s", objPath.buf());
    }

    //---------------------------------------------------------------------------------------------
    // Materials — two conductors blended via texture.
    //---------------------------------------------------------------------------------------------
    Double rough1 = pow(10.0, sin(5*t) - 1.0);
    Double rough2 = pow(10.0, cos(7*t) - 1.0);

    auto conductorMat1 = scene.createNode("conductor", "conductorMat1");
    conductorMat1["reflectance"] = Rgba{Float(cos(2*t)/2 + 0.5), Float(cos(3*t)/2 + 0.5), Float(cos(5*t)/2 + 0.5), 1.0f};
    conductorMat1["roughness"]   = Real(rough1);

    auto conductorMat2 = scene.createNode("conductor", "conductorMat2");
    conductorMat2["reflectance"] = Rgba{Float(0.5 - cos(13*t)/2), Float(0.5 - cos(11*t)/2), Float(0.5 - cos(7*t)/2), 1.0f};
    conductorMat2["roughness"]   = Real(rough2);

    String blendFile    = String::format("series3_blend_%d", frameNumber);
    String blendHdrPath = String::format("%s/%s.hdr", cwdBuf, blendFile.buf());
    if (writeHDR(blendHdrPath.buf(), blendW, blendH, &blendRgb[0]))
        logInfo("Wrote blend map: %s", blendHdrPath.buf());
    else
        logError("Failed to write blend map: %s", blendHdrPath.buf());

    auto blendTex = scene.createNode("fileTexture", "blendTex");
    blendTex["dir"]           = String(cwdBuf);
    blendTex["file"]          = blendFile;
    blendTex["ext"]           = String(".hdr");
    blendTex["interpolation"] = String("bilinear");

    // blendMaterial: conductorMat2 base, conductorMat1 overlay.
    auto meshMat = scene.createNode("blendMaterial", "meshMaterial");
    meshMat["mixing"] = String("ordered");
    meshMat["materials"].appendElement();
    meshMat["materials"][0]["material"] = conductorMat2;
    meshMat["materials"][0]["opacity"]  = Real(0);
    meshMat["materials"].appendElement();
    meshMat["materials"][1]["material"] = conductorMat1;
    meshMat["materials"][1]["opacity"] |= blendTex.output("outAverage");

    //---------------------------------------------------------------------------------------------
    // World (created before children reference it).
    //---------------------------------------------------------------------------------------------
    auto world = scene.createNode("xform", "world");
    world["steps"].appendElement();
    world["steps"][0]["xform"] = Mat4::identity;

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
    // Camera — series102.go: FOV=5° telephoto, Z-up, pinhole.
    //---------------------------------------------------------------------------------------------
    auto tonemap = scene.createNode("filmicHable", "tonemap");

    auto sensor = scene.createNode("sensor", "sensor");
    sensor["size"]        = Vec2{24.0, 24.0};
    sensor["tonemapping"] = tonemap;

    auto lens = scene.createNode("thinLens", "thinLens");
    lens["steps"].appendElement();
    lens["steps"][0]["focalLen"]  = Real(focalLenMm);
    lens["steps"][0]["fStop"]     = Real(fStop);
    lens["steps"][0]["focusDist"] = Real(distance);

    auto cameraNode = scene.createNode("camera", "cameraObj");
    cameraNode["sensor"]       = sensor;
    cameraNode["lens"]         = lens;
    cameraNode["resolution"]   = Vec2{2400.0, 2400.0};
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
    // Environment map — series102.go: sin^4 shape, animated 3-axis rotation.
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
    {
        Double ax = sin(19*t) * 45.0 * dl::math::nc::pi / 180.0;
        Double ay = sin(23*t) * 45.0 * dl::math::nc::pi / 180.0;
        Double az = sin(29*t) * 45.0 * dl::math::nc::pi / 180.0;
        Double cx = cos(ax), sx = sin(ax);
        Double cy = cos(ay), sy = sin(ay);
        Double cz = cos(az), sz = sin(az);
        envmapNode["xform"] = Mat3::make(
             cy*cz,                  cy*sz,                 -sy,
             sy*cz*sx - sz*cx,       sy*sz*sx + cz*cx,       cy*sx,
             sy*cz*cx + sz*sx,       sy*sz*cx - cz*sx,       cy*cx
        );
    }

    buildBellaRenderSettings(scene, cameraNode, envmapNode, world);
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

    args.add("f", "frame",            "0",     "Frame number to render.");
    args.add("p", "pixels",           "256",   "Pixel resolution.");
    args.add("m", "maxsubdivisions",  "1000",  "Maximum subdivisions.");
    args.add("n", "maxframes",        "512",   "Maximum frames in animation.");
    args.add("t", "desiredtriangles", "50000", "Desired number of triangles.");

    Int frame = 0, pixels = 256, maxSubdivisions = 1000, maxFrames = 512, desiredTriangles = 50000;
    args.value("--frame",            "0"    ).parse(frame);
    args.value("--pixels",           "256"  ).parse(pixels);
    args.value("--maxsubdivisions",  "1000" ).parse(maxSubdivisions);
    args.value("--maxframes",        "512"  ).parse(maxFrames);
    args.value("--desiredtriangles", "50000").parse(desiredTriangles);

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
