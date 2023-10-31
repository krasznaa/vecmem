/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/details/aligned_multiple_placement.hpp"
#include "vecmem/edm/details/traits.hpp"

// System include(s).
#include <iostream>
#include <stdexcept>
#include <tuple>

namespace vecmem::edm {

template <typename... VARTYPES>
buffer<VARTYPES...>::buffer(std::size_t capacity, memory_resource& mr,
                            data::buffer_type type) {

    // Temporary pointers to the individual variables.
    std::tuple<typename details::view_type<VARTYPES>::pointer_type...> pointers;

    // Make the main memory allocation.
    if (type == data::buffer_type::fixed_size) {
        // Allocate memory for fixed sized variables.
        std::tuple_cat(std::tie(m_memory), pointers) =
            vecmem::details::aligned_multiple_placement<
                typename details::view_type<VARTYPES>::raw_type...>(
                mr, details::buffer_alloc<VARTYPES>::size(capacity)...);
        m_views = details::make_buffer_views<VARTYPES...>(capacity, pointers);
    } else if (type == data::buffer_type::resizable) {
        // Allocate memory for resizable variables.
        std::tuple_cat(std::tie(m_memory, m_size), pointers) =
            vecmem::details::aligned_multiple_placement<
                unsigned int,
                typename details::view_type<VARTYPES>::raw_type...>(
                mr, 1, details::buffer_alloc<VARTYPES>::size(capacity)...);
        m_views =
            details::make_buffer_views<VARTYPES...>(capacity, m_size, pointers);
    } else {
        throw std::runtime_error("Unknown buffer type");
    }
}

template <typename... VARTYPES>
buffer<VARTYPES...>::buffer(const std::vector<std::size_t>& capacities,
                            memory_resource& mr, memory_resource* host_mr,
                            data::buffer_type type) {}

}  // namespace vecmem::edm
