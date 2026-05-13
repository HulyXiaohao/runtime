/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "gtest/gtest.h"
#include "mockcpp/mockcpp.hpp"
#include "securec.h"
#include "driver/ascend_hal.h"
#include "runtime/rt.h"
#define private public
#include "kernel.hpp"
#include "program.hpp"
#include "uma_arg_loader.hpp"
#include "raw_device.hpp"
#undef private
#include "runtime.hpp"
#include "api.hpp"
#include "cmodel_driver.h"
#include "thread_local_container.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "simd_memsetd32.h"

// 测试 MemsetD32Optimized 对齐地址填充
TEST(SimdUtilsTest, OptimizedAligned) {
    const size_t count = 1024;
    uint32_t* buf = (uint32_t*)aligned_alloc(32, count * sizeof(uint32_t));
    ASSERT_NE(buf, nullptr);
    uint32_t value = 0xCAFEBABE;
    MemsetD32Optimized(buf, value, count);
    for (size_t i = 0; i < count; ++i) {
        EXPECT_EQ(buf[i], value);
    }
    free(buf);
}

// 测试 MemsetD32Optimized 非对齐地址（32字节未对齐）
TEST(SimdUtilsTest, OptimizedUnaligned) {
    const size_t count = 1024;
    uint32_t* aligned = (uint32_t*)aligned_alloc(32, (count + 2) * sizeof(uint32_t));
    ASSERT_NE(aligned, nullptr);
    uint32_t* unaligned = aligned + 1;  // 偏移4字节，相对32字节未对齐
    uint32_t value = 0x12345678;
    MemsetD32Optimized(unaligned, value, count);
    for (size_t i = 0; i < count; ++i) {
        EXPECT_EQ(unaligned[i], value);
    }
    free(aligned);
}

// 测试 count = 0
TEST(SimdUtilsTest, OptimizedZeroCount) {
    uint32_t buf[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    uint32_t original = buf[0];
    MemsetD32Optimized(buf, 0xDEADBEEF, 0);
    EXPECT_EQ(buf[0], original); // 未被修改
}

// 测试各种 count 值，覆盖 SIMD 循环尾部逻辑
TEST(SimdUtilsTest, OptimizedVariousCounts) {
    std::vector<size_t> counts = {1, 3, 4, 5, 7, 8, 9, 12, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 1023, 1024};
    size_t maxCount = *std::max_element(counts.begin(), counts.end());
    uint32_t* buf = (uint32_t*)aligned_alloc(32, (maxCount + 64) * sizeof(uint32_t));
    ASSERT_NE(buf, nullptr);
    for (size_t n : counts) {
        uint32_t value = static_cast<uint32_t>(n) | 0xAAAA0000;
        MemsetD32Optimized(buf, value, n);
        for (size_t i = 0; i < n; ++i) {
            EXPECT_EQ(buf[i], value) << "Failed at n=" << n;
        }
    }
    free(buf);
}

// 大块内存压力测试
TEST(SimdUtilsTest, OptimizedLarge) {
    const size_t count = 64 * 1024 * 1024 / sizeof(uint32_t); // 64MB
    uint32_t* buf = (uint32_t*)aligned_alloc(32, count * sizeof(uint32_t));
    if (buf == nullptr) {
        GTEST_SKIP() << "Cannot allocate 64MB for large test";
    }
    uint32_t value = 0x5A5A5A5A;
    MemsetD32Optimized(buf, value, count);
    EXPECT_EQ(buf[0], value);
    EXPECT_EQ(buf[count/2], value);
    EXPECT_EQ(buf[count-1], value);
    free(buf);
}
