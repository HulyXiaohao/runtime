/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef SIMD_MEMSETD32_H
#define SIMD_MEMSETD32_H

#include <cstddef>
#include <cstdint>

namespace cce {
namespace runtime {

// Optimized 32-bit memory fill (automatically handles alignment and SIMD selection)
void MemsetD32Optimized(uint32_t* dst, uint32_t value, size_t count);

}  // namespace runtime
}  // namespace cce

#endif  // SIMD_MEMSETD32_H
