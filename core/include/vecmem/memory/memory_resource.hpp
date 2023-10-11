/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021-2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// This header can only be used in C++17 mode. So "device compilers" that
// can't use C++17, must not see it. Which in practically all cases should
// "just" be a question of code organisation.
#if __cplusplus < 201700L
#error \
    "This header can only be used in C++17 mode. " \
         "Ideally it should only be used by the \"host compiler\"."
#endif  // < C++17

// System include(s).
#include <memory_resource>

namespace vecmem {

/// Convenience declaration for the memory resource type.
using memory_resource = std::pmr::memory_resource;

}  // namespace vecmem
