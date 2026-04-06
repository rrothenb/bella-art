// Bella Series 2 Generator - Port of series144.go
// Generates animated procedural surfaces for the Bella renderer

#include "bella_series_util.h"

//=================================================================================================
// Math helpers — series144.go specific.
//=================================================================================================

// series144.go: pow(8, sin(n*(x+n/10)))
static Double strength(Int n, Double x)
{
    return pow(8.0, sin(Double(n) * (x + Double(n) / 10.0)));
}

// HSB to RGB (from series144.go hsb2rgb)
static Vec3 hsb2rgb(Double hue, Double sat, Double bri)
{
    Double u = bri;
    if (sat == 0.0)
        return Vec3{u, u, u};
    Double h  = (hue - floor(hue)) * 6.0;
    Double f  = h - floor(h);
    Double p  = bri * (1.0 - sat);
    Double q  = bri * (1.0 - sat * f);
    Double tt = bri * (1.0 - sat * (1.0 - f));
    switch (int(h))
    {
    case 0: return Vec3{u, tt, p};
    case 1: return Vec3{q,  u, p};
    case 2: return Vec3{p,  u, tt};
    case 3: return Vec3{p,  q, u};
    case 4: return Vec3{tt, p, u};
    case 5: return Vec3{u,  p, q};
    default: return Vec3{u, u, u};
    }
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
        , position(Vec3{0, 0, 0})
        , target(Vec3{0, 0, -5})
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
        Vec3 f     = (position - target).unit();
        Vec3 zAxis = Vec3{0, 0, 1};
        Vec3 r     = zAxis.cross(f).unit();
        Vec3 u     = f.cross(r).unit();

        Mat4 orient = Mat4::make(
            Vec4{r.x, r.y, r.z, 0},
            Vec4{u.x, u.y, u.z, 0},
            Vec4{f.x, f.y, f.z, 0},
            Vec4{0,   0,   0,   1}
        );
        Mat4 trans = Mat4::make(
            Vec4{1, 0, 0, position.x},
            Vec4{0, 1, 0, position.y},
            Vec4{0, 0, 1, position.z},
            Vec4{0, 0, 0, 1}
        );
        transform = trans * orient;
    }

    Bool invisible(Vec3 /*point*/) { return false; }
};

//=================================================================================================
// Procedural shape functions (series144.go).
//=================================================================================================


// series144.go: texture(u, v, t)
static Double texture(Double u, Double v, Double t)
{
    return sin(
        strength(2, t)*sin(11*u + strength(19, t)*sin(u)) * sin(17*v + strength(23, t)*16*v)
        + strength(3, t)*sin(12*u + strength(17, t)*sin(v))
        + strength(5, t)*sin(13*v + strength(13, t)*sin(u))
        + strength(7, t)*sin(14*u + 15*v + strength(11, t)*sin(u + v + strength(29, t)*sin(u - v))));
}

// series144.go: shaper(x, a, b)
static Double shaper(Double x, Double a, Double b)
{
    return spow(pow(x/2.0 + 0.5, a) * 2.0 - 1.0, b);
}


// series144.go: cube(u, v, t) - fixed .6 distortion (t unused)
static Vec3 cube(Double u, Double v, Double /*t*/)
{
    return Vec3{
        sin(v/2.0 + 0.6*sin(v)) * cos(u - 0.6*sin(2*u)),
        sin(v/2.0 + 0.6*sin(v)) * sin(u + 0.6*sin(2*u)),
        cos(v/2.0 - 0.6*sin(v))
    };
}

// series144.go: shape(u, v, t) - cube displaced by texture
static Vec3 shape(Double u, Double v, Double t)
{
    Vec3 c = cube(u, v, t);
    Double disp = 1.0 + 0.05 * shaper(texture(u, v, t),
                                       pow(2.0, sin(31*t)),
                                       pow(3.0, sin(37*t) - 1.0));
    return c * disp;
}

static Vec3 uv2xyz(Double u, Double v, Double t)
{
    return shape(u, v, t);
}

// series144.go: cameraPath(t) — used as-is; Y-up convention matched by worldUp={0,1,0} in camera matrix
static Vec3 cameraPath(Double t)
{
    return Vec3{sin(t)*0.5, 1.5 + cos(t)*0.25, cos(t)*0.5 - 0.25};
}

// series144.go: focusPath(t) = cameraPath(t+.1) - {0, .1, 0}
static Vec3 focusPath(Double t)
{
    Vec3 cp = cameraPath(t + 0.1);
    return Vec3{cp.x, cp.y - 0.1, cp.z};
}

// series144.go: blendValue(u, v, t)
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

    // First pass: count visible triangles and compute bounds (500x500 sample grid)
    Int    numTriangles = 0;
    Double totalWidth   = 0.0;
    Double totalHeight  = 0.0;

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

    // Adaptive resolution
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

    // Second pass: generate vertices and faces
    // Bella mesh requires float (f32) — pos3d/vec3d causes silent render failure
    ds::Vector<Pos3f> points;
    ds::Vector<Vec3f> normals;
    ds::Vector<Vec2f> uvs;
    ds::Vector<Vec4u> polygons;

    Int startUIndex = 0, endUIndex = nU;
    Int startVIndex = 0, endVIndex = nV;

    ds::Vector<Int32> vertexIndices;
    vertexIndices.resize((nU + 1) * (nV + 1));

    Int numVertices = 0;
    Int numFaces    = 0;

    for (Int uIndex = startUIndex; uIndex <= endUIndex; ++uIndex)
    {
        for (Int vIndex = startVIndex; vIndex <= endVIndex; ++vIndex)
        {
            Double u      = index2radians(Double(uIndex), nU);
            Double v      = index2radians(Double(vIndex), nV);
            Vec3   vertex = uv2xyz(u, v, t);

            Int32 vertexIdx = -1;

            if (!cam.invisible(vertex))
            {
                Double delta  = 0.1;
                Vec3   left   = uv2xyz(index2radians(Double(uIndex) - delta, nU), v, t);
                Vec3   right  = uv2xyz(index2radians(Double(uIndex) + delta, nU), v, t);
                Vec3   up     = uv2xyz(u, index2radians(Double(vIndex) + delta, nV), t);
                Vec3   down   = uv2xyz(u, index2radians(Double(vIndex) - delta, nV), t);

                Vec3 tangentU = right - left;
                Vec3 tangentV = up - down;
                Vec3 normalRaw = tangentV.cross(tangentU);
                Vec3 normal = (normalRaw.norm() > 1e-10) ? normalRaw.unit() : vertex.unit();

                Int rangeU = max(Int(1), endUIndex - startUIndex);
                Int rangeV = max(Int(1), endVIndex - startVIndex);
                Double uvU = index2radians(Double(uIndex - startUIndex), rangeU) / (dl::math::nc::pi * 2);
                Double uvV = index2radians(Double(vIndex - startVIndex), rangeV) / (dl::math::nc::pi * 2);

                vertexIdx = numVertices++;
                points.push_back(Pos3f{Float(vertex.x), Float(vertex.y), Float(vertex.z)});
                normals.push_back(Vec3f{Float(normal.x), Float(normal.y), Float(normal.z)});
                uvs.push_back(Vec2f{Float(uvU), Float(uvV)});
            }

            vertexIndices[uIndex * (nV + 1) + vIndex] = vertexIdx;
        }
    }

    // Generate faces (quads as two triangles)
    for (Int vIndex = startVIndex; vIndex < endVIndex; ++vIndex)
    {
        for (Int uIndex = startUIndex; uIndex < endUIndex; ++uIndex)
        {
            Int32 topRight = vertexIndices[ uIndex      * (nV + 1) +  vIndex     ];
            Int32 topLeft  = vertexIndices[(uIndex + 1) * (nV + 1) +  vIndex     ];
            Int32 botRight = vertexIndices[ uIndex      * (nV + 1) + (vIndex + 1)];
            Int32 botLeft  = vertexIndices[(uIndex + 1) * (nV + 1) + (vIndex + 1)];

            if (topRight == -1 || topLeft == -1 || botRight == -1 || botLeft == -1)
                continue;

            polygons.push_back(Vec4u{UInt32(topRight), UInt32(botLeft),  UInt32(topLeft),  UInt32(topLeft)});
            ++numFaces;
            polygons.push_back(Vec4u{UInt32(topRight), UInt32(botRight), UInt32(botLeft),  UInt32(botLeft)});
            ++numFaces;
        }
    }

    logInfo("Generated %d vertices and %d faces", numVertices, numFaces);

    // Create Bella mesh node
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
    // Material — series144.go: blend(roughconductor, roughdielectric, blendTexture)
    //
    // Effective material with Weight1=Weight2=0: conductor blended with dielectric by blendTexture.
    // Colors cycle via HSB: eta=hsb(47t/2π, cos(41t)*.4+.5, .5-sin(43t)*.4)
    //                       k  =hsb(eta_hue+.5+sin(t)*.25, ...)
    // IOR   = cos(59t) + 2.5
    // Alpha1= 10^(sin(67t)-2)  (dielectric roughness)
    // Alpha2= 10^(cos(79t)-2)  (conductor roughness)
    //---------------------------------------------------------------------------------------------

    Double h1  = 47.0 * t / 2.0 / dl::math::nc::pi;
    Double s1  = cos(41*t)*0.4 + 0.5;
    Double b1  = 0.5 - sin(43*t)*0.4;
    Double h2  = h1 + 0.5 + sin(t)*0.25;
    Double s2  = s1 + 0.4; if (s2 > 0.9) s2 = s1 - 0.4;
    Double b2  = b1 + 0.4; if (b2 > 0.9) b2 = b1 - 0.4;
    Vec3 etaColor = hsb2rgb(h1, s1, b1);
    Vec3 kColor   = hsb2rgb(h2, s2, b2);

    Double intIOR  = cos(59*t) + 2.5;
    Double alpha1  = pow(10.0, sin(67*t) - 2.0);
    Double alpha2  = pow(10.0, cos(79*t) - 2.0);

    // Conductor (roughconductor in Go: k=etaColor, eta=kColor, roughness=alpha2)
    auto conductorMat = scene.createNode("conductor", "conductorMat");
    conductorMat["reflectance"] = Rgba{Float(kColor.x), Float(kColor.y), Float(kColor.z), 1.0f};
    conductorMat["roughness"]   = Real(alpha2);

    // Dielectric (roughdielectric in Go: int_ior=2, ext_ior=intIOR, roughness=alpha1)
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
    // Blend texture — series144.go: blendValue(u, v, t) per face center
    //---------------------------------------------------------------------------------------------
    int blendW = endUIndex - startUIndex;
    int blendH = endVIndex - startVIndex;
    if (blendW < 1) blendW = 1;
    if (blendH < 1) blendH = 1;

    ds::Vector<float> blendRgb;
    for (Int vIdx = startVIndex; vIdx < endVIndex; ++vIdx)
    {
        for (Int uIdx = startUIndex; uIdx < endUIndex; ++uIdx)
        {
            Double u  = Double(uIdx) / Double(nU) * 2.0 * dl::math::nc::pi;
            Double v  = Double(vIdx) / Double(nV) * 2.0 * dl::math::nc::pi;
            float  bv = Float(blendValue(u, v, t));
            blendRgb.push_back(bv);
            blendRgb.push_back(bv);
            blendRgb.push_back(bv);
        }
    }

    char cwdBuf[1024];
    getcwd(cwdBuf, sizeof(cwdBuf));

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

    // blendMaterial: ordered — dielectric base, conductor overlay driven by blend texture
    // series144.go effective material (Weight1=0, Weight2=0):
    //   blend(blendTex, roughdielectric, diffuse) — high blend → glass, low → etaColor diffuse
    auto meshMat = scene.createNode("blendMaterial", "meshMaterial");
    meshMat["mixing"] = String("ordered");
    meshMat["materials"].appendElement();
    meshMat["materials"][0]["material"] = dielectricMat;
    meshMat["materials"][0]["opacity"]  = Real(1);
    meshMat["materials"].appendElement();
    meshMat["materials"][1]["material"] = conductorMat;
    meshMat["materials"][1]["opacity"] |= blendTex.output("outAverage");

    //---------------------------------------------------------------------------------------------
    // World
    //---------------------------------------------------------------------------------------------
    auto world = scene.createNode("xform", "world");
    world["steps"].appendElement();
    world["steps"][0]["xform"] = Mat4::identity;

    //---------------------------------------------------------------------------------------------
    // Mesh xform
    //---------------------------------------------------------------------------------------------
    auto meshXform = scene.createNode("xform", "mesh_xform");
    meshXform["steps"].appendElement();
    meshXform["steps"][0]["xform"] = Mat4::identity;
    meshXform["children"].appendElement().set(meshNode);
    meshXform["material"] = meshMat;
    {
        auto meshXformSteps = meshXform["steps"];
        while (meshXformSteps.inputCount() > 1) meshXformSteps.removeLastInput();
    }
    world["children"].appendElement().set(meshXform);

    //---------------------------------------------------------------------------------------------
    // Camera
    //---------------------------------------------------------------------------------------------
    auto tonemap = scene.createNode("filmicHable", "tonemap");

    auto sensor = scene.createNode("sensor", "sensor");
    sensor["size"]        = Vec2{24.0, 24.0};
    sensor["tonemapping"] = tonemap;

    // series144.go: fov = 136° (fixed), aperture ≈ 0 (pinhole)
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

    // series144.go uses Y-up lookat (up="0,1,0").
    cam.transform = bellaLookAt(cameraLoc, focusPoint, Vec3{0, 1, 0});

    auto cameraXform = scene.createNode("xform", "camera");
    cameraXform["steps"].appendElement();
    cameraXform["steps"][0]["xform"] = cam.transform;
    cameraXform["children"].appendElement().set(cameraNode);
    {
        auto cameraSteps = cameraXform["steps"];
        while (cameraSteps.inputCount() > 1) cameraSteps.removeLastInput();
    }
    world["children"].appendElement().set(cameraXform);

    //---------------------------------------------------------------------------------------------
    // Environment map — series144.go:
    //   power = 2^(cos(t)+3)
    //   envmapValue = |sin(u/2)|^power * |sin(v)|^power
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
                double eu = double(uIdx) / double(envSize) * 2.0 * dl::math::nc::pi;
                double ev = double(vIdx) / double(envSize) * dl::math::nc::pi;
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

    //---------------------------------------------------------------------------------------------
    // Render settings
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
        auto worldSteps = world["steps"];
        while (worldSteps.inputCount() > 1) worldSteps.removeLastInput();
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

    logBanner("Bella Series 2 Generator (version: %s, build date: %llu)",
        bellaSdkVersion().toString().buf(),
        bellaSdkBuildDate()
    );

    args.add("f", "frame",           "0",     "Frame number to render.");
    args.add("p", "pixels",          "256",   "Pixel resolution.");
    args.add("m", "maxsubdivisions", "1000",  "Maximum subdivisions.");
    args.add("n", "maxframes",       "1000",  "Maximum frames in animation.");
    args.add("t", "desiredtriangles","50000", "Desired number of triangles.");

    Int frame = 0, pixels = 256, maxSubdivisions = 1000, maxFrames = 1000, desiredTriangles = 50000;
    args.value("--frame",           "0"    ).parse(frame);
    args.value("--pixels",          "256"  ).parse(pixels);
    args.value("--maxsubdivisions", "1000" ).parse(maxSubdivisions);
    args.value("--maxframes",       "1000" ).parse(maxFrames);
    args.value("--desiredtriangles","50000").parse(desiredTriangles);

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
