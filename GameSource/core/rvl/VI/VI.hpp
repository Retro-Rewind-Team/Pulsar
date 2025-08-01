#ifndef _VIDEOINTERFACE_
#define _VIDEOINTERFACE_
#include <types.hpp>
namespace VI {
typedef void (*RetraceCallback)(u32 retraceCount);
void* GetNextFrameBuffer();  // 801bab24

enum TVMode {
    VI_TVMODE_NTSC_INT,  // NTSC INTERLACING
    VI_TVMODE_NTSC_DS,  // NTSC NO INTERLACING
    VI_TVMODE_NTSC_PROG,  // NTSC PROGRESSIVE

    VI_TVMODE_PAL_INT = 4,  // PAL INTERLACING
    VI_TVMODE_PAL_DS,  // PAL NO INTERLACING

    VI_TVMODE_MPAL_INT = 8,  // BRAZIL PAL INTERLACING
    VI_TVMODE_MPAL_DS,  // BRAZIL PAL NO INTERLACING
    VI_TVMODE_MPAL_PROG,  // BRAZIL PAL PROGRESSIVE

    VI_TVMODE_EURGB60_INT = 20,  // PAL60 INTERLACING
    VI_TVMODE_EURGB60_DS,  // PAL60 NO INTERLACING
    VI_TVMODE_EURGB60_PROG,  // PAL60 PROGRESSIVE
};

enum XFBMode {
    VI_XFBMODE_SF,
    VI_XFBMODE_DF
};
BOOL ResetSIIdle();
}  // namespace VI

#endif