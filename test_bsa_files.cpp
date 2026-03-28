// test_bsa_files.cpp - Test that both simple.bsa and simple_generated.bsa can be loaded
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

    logBanner("Test BSA Files (version: %s)", bellaSdkVersion().toString().buf());

    String fileA = "simple.bsa";
    String fileB = "simple_generated.bsa";
    
    Bool allOK = true;

    // Test first file
    {
        logInfo("Testing file: %s", fileA.buf());
        Scene sceneA;
        sceneA.loadDefs();
        
        if (sceneA.read(fileA))
        {
            logInfo("SUCCESS: %s loaded successfully", fileA.buf());
            
            // Count nodes
            UInt nodeCount = 0;
            for (auto node : sceneA.nodes())
                ++nodeCount;
            logInfo("  Node count: %d", nodeCount);
            
            // Check for key nodes
            auto world = sceneA.world();
            if (world.impl())
                logInfo("  World node: found");
            else
                logInfo("  World node: NOT found");
        }
        else
        {
            logError("FAILED: Could not load %s", fileA.buf());
            allOK = false;
        }
    }

    // Test second file
    {
        logInfo("Testing file: %s", fileB.buf());
        Scene sceneB;
        sceneB.loadDefs();
        
        if (sceneB.read(fileB))
        {
            logInfo("SUCCESS: %s loaded successfully", fileB.buf());
            
            // Count nodes
            UInt nodeCount = 0;
            for (auto node : sceneB.nodes())
                ++nodeCount;
            logInfo("  Node count: %d", nodeCount);
            
            // Check for key nodes
            auto world = sceneB.world();
            if (world.impl())
                logInfo("  World node: found");
            else
                logInfo("  World node: NOT found");
        }
        else
        {
            logError("FAILED: Could not load %s", fileB.buf());
            allOK = false;
        }
    }

    if (allOK)
        logInfo("All tests PASSED!");
    else
        logError("Some tests FAILED!");

    return allOK ? 0 : 1;
}
