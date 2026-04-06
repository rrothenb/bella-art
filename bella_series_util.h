// Shared utilities for Bella series generators.
// Each series is a separate executable (one .cpp file), so static/inline definitions here
// are safe — no multiple-definition issues.
// NOTE: using-directives in headers are acceptable for application-level executables like these.
#pragma once

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
// Logging.
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
                auto val  = dl::logProgressValue(msg);
                auto text = dl::logProgressText(msg);
                DL_PRINT("[%f] %s\n", val, text.buf());
            }
        }
        break;
    }
}

//=================================================================================================
// Math helpers common to all series.
//=================================================================================================

static inline Double sign(Double x)
{
    return x < 0 ? -1.0 : 1.0;
}

static inline Double spow(Double x, Double y)
{
    return sign(x) * pow(abs(x), y);
}

//=================================================================================================
// Radiance HDR (RGBE) file writer.
//=================================================================================================

static inline void rgbeEncode(float r, float g, float b, uint8_t out[4])
{
    float maxComp = r > g ? (r > b ? r : b) : (g > b ? g : b);
    if (maxComp < 1e-32f)
    {
        out[0] = out[1] = out[2] = out[3] = 0;
        return;
    }
    int   e;
    float scale = frexpf(maxComp, &e) * 256.0f / maxComp;
    out[0] = (uint8_t)(r * scale);
    out[1] = (uint8_t)(g * scale);
    out[2] = (uint8_t)(b * scale);
    out[3] = (uint8_t)(e + 128);
}

static inline bool writeHDR(const char* path, int width, int height, const float* rgb)
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
// UV index to angle.
//=================================================================================================

static inline Double index2radians(Double index, Int n)
{
    return index / Double(n) * dl::math::nc::pi * 2;
}

//=================================================================================================
// Bella camera lookat matrix (row-vector convention).
// Row0=right, Row1=down (right×forward), Row2=forward toward target, Row3=position.
// Pass worldUp={0,0,1} for Z-up scenes (series1, series3) or {0,1,0} for Y-up (series2).
//=================================================================================================

static inline Mat4 bellaLookAt(Vec3 from, Vec3 to, Vec3 worldUp)
{
    Vec3 forward = (to - from).unit();
    Vec3 right   = worldUp.cross(forward).unit();
    Vec3 down    = right.cross(forward);
    return Mat4::make(
        Vec4{right.x,   right.y,   right.z,   0},
        Vec4{down.x,    down.y,    down.z,    0},
        Vec4{forward.x, forward.y, forward.z, 0},
        Vec4{from.x,    from.y,    from.z,    1}
    );
}
