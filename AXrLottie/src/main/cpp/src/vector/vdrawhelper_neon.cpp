#include "vdrawhelper.h"

// اگر ARM7 باشد و NEON غیرفعال باشد، این اجرا می‌شود
#if defined(DISABLE_NEON)

// --------------------
//   NON-NEON (armeabi-v7a)
// --------------------
#warning "NEON disabled → using fallback C++ implementations"

void memfill32(uint32_t *dest, uint32_t value, int length)
{
    for (int i = 0; i < length; ++i)
        dest[i] = value;
}

void RenderFuncTable::neon()
{
    // هیچ NEON ثبت نمی‌کنیم
    // فقط خالی باشد تا لینک خطا ندهد
}

#else
// --------------------
//   REAL NEON (arm64 / v8a)
// --------------------

#if defined(__ARM_NEON__)

extern "C" void pixman_composite_src_n_8888_asm_neon(int32_t w, int32_t h,
                                                     uint32_t *dst,
                                                     int32_t   dst_stride,
                                                     uint32_t  src);

extern "C" void pixman_composite_over_n_8888_asm_neon(int32_t w, int32_t h,
                                                      uint32_t *dst,
                                                      int32_t   dst_stride,
                                                      uint32_t  src);

void memfill32(uint32_t *dest, uint32_t value, int length)
{
    pixman_composite_src_n_8888_asm_neon(length, 1, dest, length, value);
}

static void color_SourceOver(uint32_t *dest, int length,
                             uint32_t color,
                             uint32_t const_alpha)
{
    if (const_alpha != 255)
        color = BYTE_MUL(color, const_alpha);

    pixman_composite_over_n_8888_asm_neon(length, 1, dest, length, color);
}

void RenderFuncTable::neon()
{
    updateColor(BlendMode::Src , color_SourceOver);
}

#endif // __ARM_NEON__
#endif // DISABLE_NEON
