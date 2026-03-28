#include "bella_sdk/bella_scene.h"

using namespace dl;
using namespace bella_sdk;

// This is the global dl log callback, where all messages will be received, and you can print
// them as you see fit.
//
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
            if (dl::logIsProgressBegin(msg))    { /* Open a progress dialog. */ }
            else if (dl::logIsProgressEnd(msg)) { /* Close a progress dialog. */ }
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

// The SceneObserver is called when various events occur in the scene. There is also a NodeObserver
// to capture events from a single node, which can be useful when writing a user interface.
//
struct Observer : public SceneObserver
{
    bool inEventGroup = false;

    void onNodeAdded( Node node ) override
    {
        logInfo("%sNode added: %s", inEventGroup ? "  " : "", node.name().buf());
    }
    void onNodeRemoved( Node node ) override
    {
        logInfo("%sNode removed: %s", inEventGroup ? "  " : "", node.name().buf());
    }
    void onInputChanged( Input input ) override
    {
        logInfo("%sInput changed: %s", inEventGroup ? "  " : "", input.path().buf());
    }
    void onInputConnected( Input input ) override
    {
        logInfo("%sInput connected: %s", inEventGroup ? "  " : "", input.path().buf());
    }
    void onBeginEventGroup() override
    {
        inEventGroup = true;
        logInfo("Event group begin.");
    }
    void onEndEventGroup() override
    {
        inEventGroup = false;
        logInfo("Event group end.");
    }
};

// Helpers to dump Inputs & Outputs.
//
static void printInput(Input input, String indent = "    ")
{
    logInfo("%s%s %s", indent.buf(), input.typeString().buf(), input.name().buf());

    // Array/Object inputs have child inputs, so print those.
    //
    for (UInt j = 0; j < input.inputCount(); ++j)
        printInput(input.input(j), indent + "  ");
};

static void printOutput(Output output, String indent = "    ")
{
    logInfo("%s%s %s", indent.buf(), output.typeString().buf(), output.name().buf());
};

// We will use the dl_core main helper here. This gives us a helpful Args instance to use, and
// also hides the confusing details of dealing with main vs. WinMain on windows, and gives us
// utf8-encoded args when the application is unicode. See the engine sdk for a more extensive
// example of how to use the Args class.
//
#include "dl_core/dl_main.inl"
int DL_main(Args& /*args*/)
{
    // Very early on, we will subscribe to the global bella logging callback, and ask to flush
    // any messages that have accumulated prior to this point.
    //
    subscribeLog(&s_logCtx, log);
    flushStartupMessages();

    logBanner("Bella Scene SDK (version: %s, build date: %llu)",
        bellaSdkVersion().toString().buf(),
        bellaSdkBuildDate()
    );

    // We will get warnings from node enum implementations not being available, since this is the
    // the scene SDK, and the node implementations are contained in the engine SDK.
    //
    Scene sceneWrite;
    sceneWrite.loadDefs();

    // Run through and see which node defninitions we have.
    //
    for (auto def: sceneWrite.defs())
    {
        if (def.isAbstract() || def.isInternal())
            continue;

        logInfo("%s [bases: %s]", def.type().buf(), def.bases().join(", ").buf());
        logInfo("  has eval: %s", def.hasNodeEval() ? "yes" : "no"); // Always "no" in this SDK.

        logInfo("  inputs: %d", (int)def.inputCount());
        for (UInt i = 0; i < def.inputCount(); ++i)
            printInput(def.input(i));

        logInfo("  outputs: %d", (int)def.outputCount());
        for (UInt i = 0; i < def.outputCount(); ++i)
            printOutput(def.output(i));
    }

    // Set up a scene observer to see events occurring in the scene as we alter it.
    //
    Observer observer;
    sceneWrite.subscribe(&observer);
    
    // Create a few textures, connect them, and set some attr values.
    //
    auto checker = sceneWrite.createNode("checker");
    auto grid    = sceneWrite.createNode("grid");
    auto texform = sceneWrite.createNode("texform");

    // Scope for event scope helper. Using this is equivalent to calling Scene::pushEventGroup()
    // and Scene::popEventGroup(), but you cannot accidentally forget to pop the group, since the
    // EventScope will do that in its destructor.
    {
        Scene::EventScope es(sceneWrite);

        checker["color1"]  |= grid.output("outColor");
        checker["color2"]   = Rgba::magenta;
        checker["uvCoord"] |= texform.output("outUV");
        texform["repeat"]   = Vec2 {2.0, 2.0};
        texform["rotate"]   = 45.0;
    }

    // Now write this scene out to disk and then read it back in again.
    //
    sceneWrite.write("sdktest.bsa");
    
    // Next let's create a second scene for reading the file. If we do not load definitions here,
    // the file reead will succeed, and ad hoc nodes will be defined to handle those read from the
    // file, but they will not have output attrs, so we would not be able to find the input/output
    // connections.
    //
    Scene sceneRead;
    sceneRead.loadDefs();
    sceneRead.read("sdktest.bsa");

    // Find nodes corresponding to those we added in the writing scene.
    //
    auto checker2 = sceneRead.findNode(checker.name());
    auto grid2    = sceneRead.findNode(grid.name());
    auto texform2 = sceneRead.findNode(texform.name());

    // Confirm that they are connected as expected, and that they have the expected attr values.
    //
    logInfo("Textures are connected: %s",
        texform2.output("outUV").connectedTo(checker2.input("uvCoord")) ? "yes" : "no"
        );

    logInfo("Attr value as expected: %s",
        math::zerolike(texform2["rotate"].asReal() - 45.0) ? "yes" : "no"
        );

    // Now let's write this scene back out to disk, then read the 2 files and make sure they match.
    //
    if (!sceneRead.write("sdktest_roundtrip.bsa"))
    {
        logError("Failed to write sdktest_roundtrip.bsa.");
        return 1;
    }

    // Scope to ensure files are closed, so we will not fail to remove them when we are finished.
    {
        File bsaIn, bsaOut;
        if (!bsaIn.openReadText("sdktest.bsa") || !bsaOut.openReadText("sdktest_roundtrip.bsa"))
        {
            logError("Failed to open sdktext.bsa or sdktest_roundtrip.bsa.");
            return 1;
        }

        auto linesIn = bsaIn.readLines();
        auto linesOut = bsaOut.readLines();
        auto allOK = true;

        for (UInt i = 0; i < linesIn.size(); ++i)
        {
            if (i == 2)
                continue; // Skip "# time: " line.
                
            auto a = linesIn[i];
            auto b = linesOut[i];

            if (a == b)
                continue;;
                
            allOK = false;
            logError("File mismatch: %s -> %s", a.buf(), b.buf());
        }

        if (allOK)
            logInfo("Files matched.");
    }

    // Clean up our files.
    //
    fs::rmFile("sdktest.bsa");
    fs::rmFile("sdktest_roundtrip.bsa");

    return 0;
}
