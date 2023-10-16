/*
 * VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2021-2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// Local include(s).
#include "vecmem/memory/details/coalescing_memory_resource.hpp"

#include "coalescing_memory_resource_impl.hpp"
#include "vecmem/utils/debug.hpp"

namespace vecmem::details {

coalescing_memory_resource::coalescing_memory_resource(
    std::vector<std::reference_wrapper<memory_resource>>&& upstreams)
    : m_impl(new coalescing_memory_resource_impl{std::move(upstreams)}) {}

coalescing_memory_resource::coalescing_memory_resource(
    coalescing_memory_resource&& parent)
    : m_impl(parent.m_impl) {

    parent.m_impl = nullptr;
}

coalescing_memory_resource::~coalescing_memory_resource() {

    delete m_impl;
}

coalescing_memory_resource& coalescing_memory_resource::operator=(
    coalescing_memory_resource&& rhs) {

    delete m_impl;
    m_impl = rhs.m_impl;
    rhs.m_impl = nullptr;
    return *this;
}

void* coalescing_memory_resource::mr_allocate(std::size_t size,
                                              std::size_t align) {

    if (size == 0) {
        return nullptr;
    }

    void* ptr = m_impl->allocate(size, align);
    VECMEM_DEBUG_MSG(3, "Allocated %lu bytes at %p", size, ptr);
    return ptr;
}

void coalescing_memory_resource::mr_deallocate(void* p, std::size_t size,
                                               std::size_t align) {

    if (p == nullptr) {
        return;
    }

    VECMEM_DEBUG_MSG(3, "De-allocating memory at %p", p);
    m_impl->deallocate(p, size, align);
}

}  // namespace vecmem::details
