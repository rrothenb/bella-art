// test_surface_bsa.cpp - Test that surface_frame_0.bsa can be loaded
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

    logBanner("Test Surface BSA (version: %s)", bellaSdkVersion().toString().buf());

    String fileToTest = "surface_frame_0.bsa";
    
    logInfo("Testing file: %s", fileToTest.buf());
    Scene scene;
    scene.loadDefs();
    
    if (scene.read(fileToTest))
    {
        logInfo("SUCCESS: %s loaded successfully", fileToTest.buf());
        
        // Count nodes
        UInt nodeCount = 0;
        for (auto node : scene.nodes())
            ++nodeCount;
        logInfo("  Node count: %d", nodeCount);
        
        // Check for key nodes
        auto world = scene.world();
        if (world.impl())
            logInfo("  World node: found");
        else
            logInfo("  World node: NOT found");
        
        // Check for settings
        auto settingsInput = world.meta().input("settings");
        if (settingsInput.impl())
            logInfo("  Settings: found via meta");
        
        // Look for state and global nodes
        Node stateNode, globalNode;
        for (auto node : scene.nodes())
        {
            String typeName = node.type();
            if (typeName == "state")
                stateNode = node;
            else if (typeName == "global")
                globalNode = node;
        }
        
        if (stateNode.impl())
            logInfo("  State node: found");
        else
            logInfo("  State node: NOT found");
            
        if (globalNode.impl())
            logInfo("  Global node: found");
        else
            logInfo("  Global node: NOT found");
    }
    else
    {
        logError("FAILED: Could not load %s", fileToTest.buf());
        return 1;
    }

    return 0;
}
