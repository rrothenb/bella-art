// Bella Series 1 Generator - Port of series110.go
// Generates animated procedural surfaces for the Bella renderer

#include "bella_series_util.h"

//=================================================================================================
// Surface function — series110.go: animated cube distortion, Z-up.
// a = -(0.25 + cos(21t)*0.25); calls shared cube(u, v, a).
//=================================================================================================

static Vec3 uv2xyz(Double u, Double v, Double t)
{
    return cube(u, v, -(0.25 + cos(21*t)*0.25));
}

//=================================================================================================
// Camera and focus paths (series110.go).
//=================================================================================================

static Vec3 cameraPath(Double t)
{
    return Vec3{0.5*sin(41*t), 0, 0};
}

static Vec3 focusPath(Double /*t*/)
{
    return Vec3{-1, 0, 0};
}

//=================================================================================================
// shapeTexture — used for per-vertex color variation.
// strength(n,x) = 2^(sin(sqrt(n)*(x+n/3))) — series110 formula.
//=================================================================================================

static Double strength(Int n, Double x)
{
    return pow(2.0, sin(pow(Double(n), 0.5) * (x + Double(n) / 3.0)));
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

    // First pass: estimate mesh density and scene bounds (500×500 sample grid).
    Int    numTriangles = 0;
    Double totalWidth   = 0.0, totalHeight = 0.0;
    Double maxX = 0.0, maxY = 0.0, maxZ = 0.0;

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

                maxX = max(maxX, abs(vertex.x));
                maxY = max(maxY, abs(vertex.y));
                maxZ = max(maxZ, abs(vertex.z));
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

    // Glass material: relative IOR cycles, with scattering medium.
    Double intIor = sin(11.0 * t) + 2.0;
    Double extIor = 2.0 - cos(13.0 * t);

    auto meshScattering = scene.createNode("scattering", "meshScattering");
    meshScattering["albedo"]     = Real(.01);
    meshScattering["anisotropy"] = Real(.01);
    meshScattering["color"]      = Rgba{1.0f, 1.0f, 1.0f, 1.0f};

    auto meshMat = scene.createNode("dielectric", "meshMaterial");
    meshMat["ior"]        = Real(intIor / extIor);
    meshMat["roughness"]  = Real(.01);
    meshMat["depth"]      = Real(10000);
    meshMat["abbe"]       = Real(30);
    meshMat["dispersion"] = true;
    meshMat["scattering"] = meshScattering;

    // Mesh xform.
    auto meshXform = scene.createNode("xform", "mesh_xform");
    meshXform["steps"].appendElement();
    meshXform["steps"][0]["xform"] = Mat4::identity;
    meshXform["children"].appendElement().set(meshNode);
    meshXform["material"] = meshMat;
    {
        auto s = meshXform["steps"];
        while (s.inputCount() > 1) s.removeLastInput();
    }

    // World (created before children reference it).
    auto world = scene.createNode("xform", "world");
    world["steps"].appendElement();
    world["steps"][0]["xform"] = Mat4::identity;

    // Camera.
    auto tonemap = scene.createNode("filmicHable", "tonemap");

    auto sensor = scene.createNode("sensor", "sensor");
    sensor["size"]        = Vec2{24.0, 24.0};
    sensor["tonemapping"] = tonemap;

    // FOV = 120 + cos(14t)*30 degrees.
    Double fovDeg     = 120.0 + cos(14.0 * t) * 30.0;
    Double fovRad     = fovDeg * dl::math::nc::pi / 180.0;
    Double focalLenMm = (24.0 / 2.0) / tan(fovRad / 2.0);

    auto lens = scene.createNode("thinLens", "thinLens");
    lens["steps"].appendElement();
    lens["steps"][0]["focalLen"]  = Real(focalLenMm);
    lens["steps"][0]["fStop"]     = Real(pow(1.5, sin(37*t))*170.0);
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

    // Environment map.
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
                double ev = 1 - pow(sin(u / 2.0), power * 4) * pow(sin(v / 2.0), power);
                envRgb.push_back(float(pow(ev, expR)));
                envRgb.push_back(float(pow(ev, expG)));
                envRgb.push_back(float(pow(ev, expB)));
            }
        }
    }

    char cwdBuf[1024];
    getcwd(cwdBuf, sizeof(cwdBuf));
    String envFile    = String::format("series1_env_%d", frameNumber);
    String envHdrPath = String::format("%s/%s.hdr", cwdBuf, envFile.buf());
    if (writeHDR(envHdrPath.buf(), envSize, envSize, &envRgb[0]))
        logInfo("Wrote env map: %s", envHdrPath.buf());
    else
        logError("Failed to write env map: %s", envHdrPath.buf());

    auto envmapNode = scene.createNode("imageDome", "environment");
    envmapNode["dir"]        = String(cwdBuf);
    envmapNode["file"]       = envFile;
    envmapNode["multiplier"] = Real(3);
    envmapNode["ext"]        = String(".hdr");
    envmapNode["xform"] = Mat3::make(
         0.0,  1.0, 0.0,
        -1.0,  0.0, 0.0,
         0.0,  0.0, 1.0
    );

    // Instancing grid: series110.go — 19^3 instances around origin.
    Int    numInstances  = 9;
    Double maxDim        = max(maxX, max(maxY, maxZ));
    Double instanceScale = 0.49 / max(maxDim, 0.001);

    logInfo("Creating instances: scale=%f", instanceScale);

    auto shapeGroup = scene.createNode("xform", "instance_group");
    shapeGroup["children"].appendElement().set(meshXform);

    Int instanceCount = 0;
    for (Int x = -numInstances; x <= numInstances; ++x)
    {
        for (Int y = -numInstances; y <= numInstances; ++y)
        {
            for (Int z = -numInstances; z <= numInstances; ++z)
            {
                if (!(x || y || z)) continue;

                auto instXform = scene.createNode("xform",
                    String::format("inst_%s%d_%s%d_%s%d",
                        x<0?"n":"p", abs(x),
                        y<0?"n":"p", abs(y),
                        z<0?"n":"p", abs(z)).buf());

                Mat4 instMat = Mat4::make(
                    Vec4{instanceScale, 0, 0, 0},
                    Vec4{0, instanceScale, 0, 0},
                    Vec4{0, 0, instanceScale, 0},
                    Vec4{Double(x), Double(y), Double(z), 1}
                );
                instXform["steps"].appendElement();
                instXform["steps"][0]["xform"] = instMat;
                instXform["children"].appendElement().set(shapeGroup);
                {
                    auto s = instXform["steps"];
                    while (s.inputCount() > 1) s.removeLastInput();
                }
                world["children"].appendElement().set(instXform);
                ++instanceCount;
            }
        }
    }

    logInfo("Created %d instances", instanceCount);

    buildBellaRenderSettings(scene, cameraNode, envmapNode, world, 128);
}

//=================================================================================================
// Main entry point.
//=================================================================================================

#include "dl_core/dl_main.inl"

int DL_main(Args& args)
{
    subscribeLog(&s_logCtx, log);
    flushStartupMessages();

    logBanner("Bella Series 1 Generator (version: %s, build date: %llu)",
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

    String outputPath = String::format("series1_frame_%d.bsa", frame);
    if (scene.write(outputPath))
        logInfo("Successfully wrote scene to %s", outputPath.buf());
    else
    {
        logError("Failed to write scene to %s", outputPath.buf());
        return 1;
    }

    return 0;
}
