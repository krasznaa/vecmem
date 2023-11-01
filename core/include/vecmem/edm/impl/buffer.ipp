/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/containers/details/aligned_multiple_placement.hpp"
#include "vecmem/edm/details/buffer_traits.hpp"

// System include(s).
#include <stdexcept>
#include <tuple>

namespace vecmem::edm {

template <typename... VARTYPES>
buffer<schema<VARTYPES...>>::buffer(size_type capacity, memory_resource& mr,
                                    data::buffer_type type)
    : view_type() {

    // Remember the capacity of the individual variables.
    view_type::m_capacity = capacity;

    // Make the main memory allocation.
    if (type == data::buffer_type::fixed_size) {

        // Return value for the subsequent allocation command.
        auto alloc_result = std::tuple_cat(
            std::tie(m_memory),
            std::tuple<
                typename details::view_type<VARTYPES>::pointer_type...>{});

        // Allocate memory for fixed sized variables.
        alloc_result = vecmem::details::aligned_multiple_placement<
            typename details::view_type<VARTYPES>::raw_type...>(
            mr, details::buffer_alloc<VARTYPES>::size(capacity)...);

        // Initialize the views from the raw pointers.
        view_type::m_views =
            details::make_buffer_views<VARTYPES...>(capacity, alloc_result);

    } else if (type == data::buffer_type::resizable) {

        // Return value for the subsequent allocation command.
        auto alloc_result = std::tuple_cat(
            std::tie(m_memory, view_type::m_size),
            std::tuple<
                typename details::view_type<VARTYPES>::pointer_type...>{});

        // Allocate memory for fixed sized variables.
        alloc_result = vecmem::details::aligned_multiple_placement<
            size_type, typename details::view_type<VARTYPES>::raw_type...>(
            mr, 1, details::buffer_alloc<VARTYPES>::size(capacity)...);

        // Initialize the views from the raw pointers.
        view_type::m_views = details::make_buffer_views<VARTYPES...>(
            capacity, view_type::m_size, alloc_result);

        // !!!FIXME!!! This will need to be done by vecmem::copy later on.
        *(view_type::m_size) = 0u;

    } else {
        throw std::runtime_error("Unknown buffer type");
    }
}

template <typename... VARTYPES>
buffer<schema<VARTYPES...>>::buffer(const std::vector<size_type>&,
                                    memory_resource&, memory_resource*,
                                    data::buffer_type)
    : view_type() {}

}  // namespace vecmem::edm
