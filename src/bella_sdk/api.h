#ifndef DL_BELLA_SDK_API_H
#define DL_BELLA_SDK_API_H

#include "../dl_core/api.h"
#include "bella_types.h"
#include "bella_scene.h"

namespace dl {

DL_API Version bellaSdkVersion();
DL_C_EXPORT UInt bellaSdkBuildDate();

}

#endif // DL_BELLA_SDK_API_H
