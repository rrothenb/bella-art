// Bella Series 2 Generator - Port of series144.go
// Generates animated procedural surfaces for the Bella renderer

#include "bella_series_util.h"

//=================================================================================================
// Math helpers — series144.go specific.
// strength(n,x) = 8^(sin(n*(x+n/10)))
//=================================================================================================

static Double strength(Int n, Double x)
{
    return pow(8.0, sin(Double(n) * (x + Double(n) / 10.0)));
}

//=================================================================================================
// Texture and shape functions (series144.go).
//=================================================================================================

static Double texture(Double u, Double v, Double t)
{
    return sin(
        strength(2, t)*sin(11*u + strength(19, t)*sin(u)) * sin(17*v + strength(23, t)*16*v)
        + strength(3, t)*sin(12*u + strength(17, t)*sin(v))
        + strength(5, t)*sin(13*v + strength(13, t)*sin(u))
        + strength(7, t)*sin(14*u + 15*v + strength(11, t)*sin(u + v + strength(29, t)*sin(u - v))));
}

static Double shaper(Double x, Double a, Double b)
{
    return spow(pow(x/2.0 + 0.5, a) * 2.0 - 1.0, b);
}

// cube(u,v,0.6) from shared header; texture-displaced surface.
static Vec3 uv2xyz(Double u, Double v, Double t)
{
    Vec3   c    = cube(u, v, 0.6);
    Double disp = 1.0 + 0.05 * shaper(texture(u, v, t),
                                       pow(2.0, sin(31*t)),
                                       pow(3.0, sin(37*t) - 1.0));
    return c * disp;
}

//=================================================================================================
// Camera and focus paths (series144.go). Y-up convention.
//=================================================================================================

static Vec3 cameraPath(Double t)
{
    return Vec3{sin(t)*0.5, 1.5 + cos(t)*0.25, cos(t)*0.5 - 0.25};
}

static Vec3 focusPath(Double t)
{
    Vec3 cp = cameraPath(t + 0.1);
    return Vec3{cp.x, cp.y - 0.1, cp.z};
}

static Double blendValue(Double u, Double v, Double t)
{
    return shaper(texture(u, v, t), pow(4.0, sin(71*t)), pow(2.0, cos(73*t)) - 1.0) / 2.0 + 0.5;
}

//=================================================================================================
// Main rendering function.
//=================================================================================================

static void renderSurfaces(Scene& scene, Int frameNumber, Int /*pixels*/, Int /*maxSubdivisions*/,
                           Double dt, Int desiredTriangles)
{
    Double t = Double(frameNumber) * dt;

    Vec3 cameraLoc  = cameraPath(t);
    Vec3 focusPoint = focusPath(t);

    SLR2 cam;   // FOV=0 → no frustum culling
    cam.moveTo(cameraLoc);
    cam.lookAt(focusPoint);

    Double distance = (cameraLoc - focusPoint).norm();

    logInfo("Frame %d: camera=(%f, %f, %f), focus=(%f, %f, %f), distance=%f, t=%f",
            frameNumber,
            cameraLoc.x, cameraLoc.y, cameraLoc.z,
            focusPoint.x, focusPoint.y, focusPoint.z,
            distance, t);

    // First pass: count visible triangles and estimate density (500×500 sample grid).
    Int    numTriangles = 0;
    Double totalWidth   = 0.0, totalHeight = 0.0;

    for (Int uIndex = 0; uIndex <= 500; ++uIndex)
    {
        for (Int vIndex = 0; vIndex <= 500; ++vIndex)
        {
            Double u      = index2radians(Double(uIndex), 500);
            Double v      = index2radians(Double(vIndex), 500);
            Vec3   vertex = uv2xyz(u, v, t);

            if (!cam.invisible(vertex))
            {
                ++numTriangles;
                if (uIndex > 0)
                {
                    Vec3 vL = uv2xyz(index2radians(Double(uIndex-1), 500), v, t);
                    totalWidth += (vertex - vL).norm();
                }
                if (vIndex > 0)
                {
                    Vec3 vB = uv2xyz(u, index2radians(Double(vIndex-1), 500), t);
                    totalHeight += (vertex - vB).norm();
                }
            }
        }
    }

    // Adaptive resolution.
    Double ratio = totalWidth / totalHeight;
    if (ratio < 0.001) ratio = 0.001;
    if (ratio > 1000.0) ratio = 1000.0;

    Int nU = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) * ratio) * 500);
    Int nV = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) / ratio) * 500);

    while (nV > 15000)
    {
        ratio *= 10;
        nU = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) * ratio) * 500);
        nV = Int(sqrt(Double(desiredTriangles) / Double(numTriangles * 2) / ratio) * 500);
    }

    nU = max(Int(10), min(nU, Int(2000)));
    nV = max(Int(10), min(nV, Int(2000)));

    logInfo("Mesh resolution: %d x %d (ratio=%f, triangles=%d)", nU, nV, ratio, numTriangles);

    // Second pass: generate mesh.
    ds::Vector<Pos3f> points;
    ds::Vector<Vec3f> normals;
    ds::Vector<Vec2f> uvs;
    ds::Vector<Vec4u> polygons;

    generateMeshData(uv2xyz, cam, nU, nV, 0, nU, 0, nV, t, points, normals, uvs, polygons);

    auto meshNode = buildBellaMesh(scene, "surface_mesh", points, normals, uvs, polygons);

    //---------------------------------------------------------------------------------------------
    // Material — series144.go: blend(roughconductor, roughdielectric, blendTexture).
    // Colors cycle via HSB; IOR, roughness animated.
    //---------------------------------------------------------------------------------------------
    Double h1  = 47.0 * t / 2.0 / dl::math::nc::pi;
    Double s1  = cos(41*t)*0.4 + 0.5;
    Double b1  = 0.5 - sin(43*t)*0.4;
    Double h2  = h1 + 0.5 + sin(t)*0.25;
    Double s2  = s1 + 0.4; if (s2 > 0.9) s2 = s1 - 0.4;
    Double b2  = b1 + 0.4; if (b2 > 0.9) b2 = b1 - 0.4;
    Vec3 etaColor = hsb2rgb(h1, s1, b1);
    Vec3 kColor   = hsb2rgb(h2, s2, b2);

    Double intIOR = cos(59*t) + 2.5;
    Double alpha1 = pow(10.0, sin(67*t) - 2.0);
    Double alpha2 = pow(10.0, cos(79*t) - 2.0);

    auto conductorMat = scene.createNode("conductor", "conductorMat");
    conductorMat["reflectance"] = Rgba{Float(kColor.x), Float(kColor.y), Float(kColor.z), 1.0f};
    conductorMat["roughness"]   = Real(alpha2);

    auto meshScattering = scene.createNode("scattering", "meshScattering");
    meshScattering["albedo"]     = Real(0.1);
    meshScattering["anisotropy"] = Real(-cos(61*t) * 0.9);
    meshScattering["color"]      = Rgba{Float(etaColor.x), Float(etaColor.y), Float(etaColor.z), 1.0f};

    auto dielectricMat = scene.createNode("dielectric", "dielectricMat");
    dielectricMat["ior"]        = Real(2.0 / intIOR);
    dielectricMat["roughness"]  = Real(alpha1);
    dielectricMat["depth"]      = Real(10);
    dielectricMat["abbe"]       = Real(30);
    dielectricMat["scattering"] = meshScattering;

    //---------------------------------------------------------------------------------------------
    // Blend texture.
    //---------------------------------------------------------------------------------------------
    char cwdBuf[1024];
    getcwd(cwdBuf, sizeof(cwdBuf));

    int blendW = nU, blendH = nV;
    if (blendW < 1) blendW = 1;
    if (blendH < 1) blendH = 1;

    ds::Vector<float> blendRgb;
    for (Int vIdx = 0; vIdx < nV; ++vIdx)
    {
        for (Int uIdx = 0; uIdx < nU; ++uIdx)
        {
            Double u  = Double(uIdx) / Double(nU) * 2.0 * dl::math::nc::pi;
            Double v  = Double(vIdx) / Double(nV) * 2.0 * dl::math::nc::pi;
            float  bv = Float(blendValue(u, v, t));
            blendRgb.push_back(bv);
            blendRgb.push_back(bv);
            blendRgb.push_back(bv);
        }
    }

    String blendFile    = String::format("series2_blend_%d", frameNumber);
    String blendHdrPath = String::format("%s/%s.hdr", cwdBuf, blendFile.buf());
    if (writeHDR(blendHdrPath.buf(), blendW, blendH, &blendRgb[0]))
        logInfo("Wrote blend map: %s", blendHdrPath.buf());
    else
        logError("Failed to write blend map: %s", blendHdrPath.buf());

    auto blendTex = scene.createNode("fileTexture", "blendTex");
    blendTex["dir"]  = String(cwdBuf);
    blendTex["file"] = blendFile;
    blendTex["ext"]  = String(".hdr");

    auto meshMat = scene.createNode("blendMaterial", "meshMaterial");
    meshMat["mixing"] = String("ordered");
    meshMat["materials"].appendElement();
    meshMat["materials"][0]["material"] = dielectricMat;
    meshMat["materials"][0]["opacity"]  = Real(1);
    meshMat["materials"].appendElement();
    meshMat["materials"][1]["material"] = conductorMat;
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
    // Camera — series144.go: FOV=136° (fixed), pinhole, Y-up lookat.
    //---------------------------------------------------------------------------------------------
    auto tonemap = scene.createNode("filmicHable", "tonemap");

    auto sensor = scene.createNode("sensor", "sensor");
    sensor["size"]        = Vec2{24.0, 24.0};
    sensor["tonemapping"] = tonemap;

    Double fovDeg     = 136.0;
    Double fovRad     = fovDeg * dl::math::nc::pi / 180.0;
    Double focalLenMm = (24.0 / 2.0) / tan(fovRad / 2.0);

    auto lens = scene.createNode("thinLens", "thinLens");
    lens["steps"].appendElement();
    lens["steps"][0]["focalLen"]  = Real(focalLenMm);
    lens["steps"][0]["fStop"]     = Real(1e9);
    lens["steps"][0]["focusDist"] = Real(distance);

    auto cameraNode = scene.createNode("camera", "cameraObj");
    cameraNode["sensor"]       = sensor;
    cameraNode["lens"]         = lens;
    cameraNode["resolution"]   = Vec2{720.0, 720.0};
    cameraNode["exposureMode"] = String("aperture");
    cameraNode["ev"]           = Real(14);

    // series144.go uses Y-up lookat.
    cam.transform = bellaLookAt(cameraLoc, focusPoint, Vec3{0, 1, 0});

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
    // Environment map — series144.go: |sin(u/2)|^p * |sin(v)|^p, p=2^(cos(t)+3).
    //---------------------------------------------------------------------------------------------
    int envSize = int(sqrt(double(desiredTriangles)));
    if (envSize < 2) envSize = 2;

    ds::Vector<float> envRgb;
    {
        double power = pow(2.0, cos(t) + 3.0);
        for (int vIdx = 0; vIdx < envSize; ++vIdx)
        {
            for (int uIdx = 0; uIdx < envSize; ++uIdx)
            {
                double eu  = double(uIdx) / double(envSize) * 2.0 * dl::math::nc::pi;
                double ev  = double(vIdx) / double(envSize) * dl::math::nc::pi;
                double val = pow(abs(sin(eu / 2.0)), power) * pow(abs(sin(ev)), power);
                envRgb.push_back(float(val));
                envRgb.push_back(float(val));
                envRgb.push_back(float(val));
            }
        }
    }

    String envDir(cwdBuf);
    String envFile    = String::format("series2_env_%d", frameNumber);
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
    envmapNode["xform"] = Mat3::make(
         0.0,  1.0, 0.0,
        -1.0,  0.0, 0.0,
         0.0,  0.0, 1.0
    );

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

    logBanner("Bella Series 2 Generator (version: %s, build date: %llu)",
        bellaSdkVersion().toString().buf(),
        bellaSdkBuildDate()
    );

    args.add("f", "frame",            "0",     "Frame number to render.");
    args.add("p", "pixels",           "256",   "Pixel resolution.");
    args.add("m", "maxsubdivisions",  "1000",  "Maximum subdivisions.");
    args.add("n", "maxframes",        "1000",  "Maximum frames in animation.");
    args.add("t", "desiredtriangles", "50000", "Desired number of triangles.");

    Int frame = 0, pixels = 256, maxSubdivisions = 1000, maxFrames = 1000, desiredTriangles = 50000;
    args.value("--frame",            "0"    ).parse(frame);
    args.value("--pixels",           "256"  ).parse(pixels);
    args.value("--maxsubdivisions",  "1000" ).parse(maxSubdivisions);
    args.value("--maxframes",        "1000" ).parse(maxFrames);
    args.value("--desiredtriangles", "50000").parse(desiredTriangles);

    logInfo("frame=%d, pixels=%d, maxSubdivisions=%d, maxFrames=%d, desiredTriangles=%d",
            frame, pixels, maxSubdivisions, maxFrames, desiredTriangles);

    Double dt = dl::math::nc::pi * 2 / Double(maxFrames);

    Scene scene;
    scene.loadDefs();

    renderSurfaces(scene, frame, pixels, maxSubdivisions, dt, desiredTriangles);

    String outputPath = String::format("series2_frame_%d.bsa", frame);
    if (scene.write(outputPath))
        logInfo("Successfully wrote scene to %s", outputPath.buf());
    else
    {
        logError("Failed to write scene to %s", outputPath.buf());
        return 1;
    }

    return 0;
}
