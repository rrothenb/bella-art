// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_API_H
#define DL_CORE_API_H

// Include all the main headers.
//
#include "dl_defines.h"
#include "dl_platform.h"
#include "dl_types.h"
#include "dl_math.h"
#include "dl_hash.h"
#include "dl_string.h"
#include "dl_stringio.h"
#include "dl_array.h"
#include "dl_vector.h"
#include "dl_hashmap.h"
#include "dl_compress.h"
#include "dl_hw.h"
#include "dl_os.h"
#include "dl_fs.h"
#include "dl_file.h"
#include "dl_path.h"
#include "dl_time.h"
#include "dl_logging.h"
#include "dl_version.h"

namespace dl {

DL_API Version dlCoreVersion();
DL_C_EXPORT UInt dlCoreBuildDate();

}

#endif // DL_CORE_API_H
