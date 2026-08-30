#include "FrequencyDivider.h"

// Header-only DSP helpers live in FrequencyDivider.h. This translation unit
// exists so the CMake source list is stable and the class is clearly part of
// the library even when inlined.

namespace FrequencyDividerDetail
{
    volatile int keepTu = 0;
}
