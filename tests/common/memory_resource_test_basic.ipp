/*
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

/// Perform some very basic tests that do not need host accessibility
TEST_P(memory_resource_test_basic, allocations) {

    // Get the tested memory resource.
    vecmem::memory_resource* resource = GetParam();

    // Check that allocating zero bytes would fail.
    EXPECT_THROW(static_cast<void>(resource->allocate(0)), std::bad_alloc);

    // Check that "valid" allocations succeed.
    for (std::size_t size = 1000; size < 100000; size += 1000) {
        void* ptr = resource->allocate(size);
        EXPECT_NE(ptr, nullptr);
        resource->deallocate(ptr, size);
    }
}
