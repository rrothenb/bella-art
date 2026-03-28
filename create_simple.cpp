// create_simple.cpp - Creates a BSA file similar to simple.bsa
// Uses the Bella SDK to create a minimal scene with sphere, plane, camera, and environment

#include "bella_sdk/bella_scene.h"
#include "dl_core/dl_main.inl"

using namespace dl;
using namespace bella_sdk;

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
    }
}

int DL_main(Args& /*args*/)
{
    subscribeLog(&s_logCtx, log);
    flushStartupMessages();

    logBanner("Create Simple BSA (version: %s, build date: %llu)",
        bellaSdkVersion().toString().buf(),
        bellaSdkBuildDate()
    );

    // Create scene and load definitions
    Scene scene;
    scene.loadDefs();

    // Use event scope for atomic updates
    Scene::EventScope es(scene);

    //=========================================================================
    // Create geometry nodes
    //=========================================================================

    // Sphere (small decorative sphere)
    auto sphereObj = scene.createNode("sphere", "__sphereObj__");
    sphereObj["radius"] = Real(0.03945);

    // Disk (flat circular plane)
    auto disk = scene.createNode("disk", "__disk__");
    disk["radius"] = Real(0.4);

    //=========================================================================
    // Create environment/background
    //=========================================================================

    // Color dome (solid color background)
    auto colorDome = scene.createNode("colorDome", "__colorDome__");
    colorDome["multiplier"] = Real(0.0);

    // Image dome (environment map) - use mat3 for xform directly
    auto background = scene.createNode("imageDome", "__background__");
    
    // Set up a rotation transform for the background (mat3)
    // Rotation matrix around Z axis (approximately 1.344 radians = 77 degrees)
    Double rot = 1.343903524035;
    Mat3 bgMat3 = Mat3::make(
        cos(rot), sin(rot), 0.0,
        -sin(rot), cos(rot), 0.0,
        0.0, 0.0, 1.0
    );
    background["xform"] = bgMat3;

    //=========================================================================
    // Create plane with material
    //=========================================================================

    // Layer material for the plane
    auto planeMat = scene.createNode("layer", "__planeMat__");
    planeMat["roughness"] = Real(40.0);

    // Xform to hold the plane
    auto planeXform = scene.createNode("xform", "__plane__");
    planeXform["steps"].appendElement();
    
    // Translation matrix (moving plane down slightly) - matches simple.bsa format
    // mat4(1 0 0 0 0 1 0 0 0 0 1 0 0 0 -0.059326 1)
    Mat4 planeMat4 = Mat4::make(
        Vec4 {1, 0, 0, 0},
        Vec4 {0, 1, 0, 0},
        Vec4 {0, 0, 1, 0},
        Vec4 {0, 0, -0.059326, 1}
    );
    planeXform["steps"][0]["xform"] = planeMat4;
    planeXform["children"].appendElement().set(disk);
    planeXform["material"] = planeMat;
    
    // Remove any extra steps
    auto planeSteps = planeXform["steps"];
    while (planeSteps.inputCount() > 1)
        planeSteps.removeLastInput();
    
    // Add meta information for transform decomposition
    auto planeMeta = planeXform.meta();
    auto stepsMeta = planeMeta.createInput("steps", AttrType_Object);
    auto stepsMeta0 = stepsMeta.appendElement();
    auto xformMeta = stepsMeta0.createInput("xform", AttrType_Object);
    xformMeta.createInput("pos", AttrType_Pos3).set(Pos3 {0, 0, -0.059326});
    auto scaMeta = stepsMeta0.createInput("sca", AttrType_Buffer);
    ds::Vector<Vec3> scaVals;
    scaVals.push_back(Vec3 {1, 1, 1});
    scaMeta.setBuffer(scaVals);

    //=========================================================================
    // Create camera
    //=========================================================================

    // Sensor
    auto sensor = scene.createNode("sensor", "__sensor__");
    sensor["size"] = Vec2 {24.0, 24.0};

    // Thin lens
    auto thinLens = scene.createNode("thinLens", "__thinLens__");
    thinLens["steps"].appendElement();
    thinLens["steps"][0]["focalLen"] = Real(50.0);
    thinLens["steps"][0]["fStop"] = Real(2.8);
    thinLens["steps"][0]["focusDist"] = Real(1.0);
    
    // Remove any extra steps (thinLens has default step values)
    auto thinLensSteps = thinLens["steps"];
    while (thinLensSteps.inputCount() > 1)
        thinLensSteps.removeLastInput();

    // Camera
    auto camera = scene.createNode("camera", "__cameraObj__");
    camera["resolution"] = Vec2 {1280.0, 1280.0};
    camera["sensor"] = sensor;
    camera["lens"] = thinLens;
    camera["pinhole"] = true;

    //=========================================================================
    // Create world hierarchy
    //=========================================================================

    // World xform
    auto world = scene.createNode("xform", "__world__");
    
    // Add meta information for world transform
    auto worldMeta = world.meta();
    auto worldStepsMeta = worldMeta.createInput("steps", AttrType_Object);
    auto worldStepsMeta0 = worldStepsMeta.appendElement();
    auto worldScaMeta = worldStepsMeta0.createInput("sca", AttrType_Buffer);
    ds::Vector<Vec3> worldScaVals;
    worldScaVals.push_back(Vec3 {1, 1, 1});
    worldScaMeta.setBuffer(worldScaVals);
    
    // Camera xform
    auto cameraXform = scene.createNode("xform", "__camera__");
    cameraXform["steps"].appendElement();
    Mat4 camMat = Mat4::make(
        Vec4 {1, 0, 0, 0},
        Vec4 {0, 1, 0, 0},
        Vec4 {0, 0, 1, 1.0},
        Vec4 {0, 0, 0, 1}
    );
    cameraXform["steps"][0]["xform"] = camMat;
    cameraXform["children"].appendElement().set(camera);
    
    // Remove any extra steps
    auto cameraSteps = cameraXform["steps"];
    while (cameraSteps.inputCount() > 1)
        cameraSteps.removeLastInput();

    // Model xform to hold geometry
    auto modelXform = scene.createNode("xform", "__model__");
    modelXform["steps"].appendElement();
    Mat4 modelMat = Mat4::make(
        Vec4 {1, 0, 0, 0},
        Vec4 {0, 1, 0, 0},
        Vec4 {0, 0, 1, 0},
        Vec4 {0, 0, 0, 1}
    );
    modelXform["steps"][0]["xform"] = modelMat;
    
    // Remove any extra steps
    auto modelSteps = modelXform["steps"];
    while (modelSteps.inputCount() > 1)
        modelSteps.removeLastInput();

    // Sphere xform with rotation (90 degrees around Z)
    auto sphereMat = scene.createNode("sphereMat", "__sphereMat__");
    
    auto sphereXform = scene.createNode("xform", "__sphere__");
    sphereXform["steps"].appendElement();
    // mat4(0 1 0 0 -1 0 0 0 0 0 1 0 0 0 0 1) - 90 degree rotation around Z
    Mat4 sphereMat4 = Mat4::make(
        Vec4 {0, 1, 0, 0},
        Vec4 {-1, 0, 0, 0},
        Vec4 {0, 0, 1, 0},
        Vec4 {0, 0, 0, 1}
    );
    sphereXform["steps"][0]["xform"] = sphereMat4;
    sphereXform["children"].appendElement().set(sphereObj);
    sphereXform["material"] = sphereMat;
    
    // Remove any extra steps
    auto sphereSteps = sphereXform["steps"];
    while (sphereSteps.inputCount() > 1)
        sphereSteps.removeLastInput();
    
    // Add meta information for rotation
    auto sphereMeta = sphereXform.meta();
    auto sphereStepsMeta = sphereMeta.createInput("steps", AttrType_Object);
    auto sphereStepsMeta0 = sphereStepsMeta.appendElement();
    auto sphereRotMeta = sphereStepsMeta0.createInput("rot", AttrType_Buffer);
    ds::Vector<Vec3> sphereRotVals;
    sphereRotVals.push_back(Vec3 {0, 0, 1.570796326795}); // 90 degrees in radians
    sphereRotMeta.setBuffer(sphereRotVals);
    auto sphereScaMeta = sphereStepsMeta0.createInput("sca", AttrType_Buffer);
    ds::Vector<Vec3> sphereScaVals;
    sphereScaVals.push_back(Vec3 {1, 1, 1});
    sphereScaMeta.setBuffer(sphereScaVals);

    // Add sphere to model
    modelXform["children"].appendElement().set(sphereXform);

    // Add camera and model to world
    world["children"].appendElement().set(cameraXform);
    world["children"].appendElement().set(modelXform);

    //=========================================================================
    // Create render settings
    //=========================================================================

    // Settings
    auto settings = scene.createNode("settings", "__settings__");
    settings["camera"] = camera;
    settings["environment"] = background;

    // State node linking settings and world
    auto state = scene.createNode("state", "__state__");
    state["settings"] = settings;
    state["world"] = world;

    // Global node
    auto global = scene.createNode("global", "__global__");
    global["states"].appendElement().set(state);

    //=========================================================================
    // Write scene to file
    //=========================================================================

    String outputPath = "simple_generated.bsa";
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
