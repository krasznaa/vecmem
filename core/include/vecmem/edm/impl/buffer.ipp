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

namespace vecmem {
namespace edm {

template <typename... VARTYPES>
buffer<schema<VARTYPES...>>::buffer(size_type capacity, memory_resource& mr,
                                    vecmem::data::buffer_type type)
    : view_type() {

    // Remember the capacity of the individual variables.
    view_type::m_capacity = capacity;

    // Bytes allocated in total.
    std::size_t bytes = 0;

    // Make the main memory allocation.
    if (type == vecmem::data::buffer_type::fixed_size) {

        // Return value for the subsequent allocation command.
        auto alloc_result = std::tuple_cat(
            std::tie(m_memory, bytes),
            std::tuple<
                typename details::view_type<VARTYPES>::pointer_type...>{});

        // Allocate memory for fixed sized variables.
        alloc_result = vecmem::details::aligned_multiple_placement<
            typename details::view_type<VARTYPES>::raw_type...>(
            mr, details::buffer_alloc<VARTYPES>::size(capacity)...);

        // Initialize the views from the raw pointers.
        view_type::m_views =
            details::make_buffer_views<VARTYPES...>(capacity, alloc_result);

    } else if (type == vecmem::data::buffer_type::resizable) {

        // Return value for the subsequent allocation command.
        auto alloc_result = std::tuple_cat(
            std::tie(m_memory, bytes, view_type::m_size),
            std::tuple<
                typename details::view_type<VARTYPES>::pointer_type...>{});

        // Allocate memory for fixed sized variables.
        alloc_result = vecmem::details::aligned_multiple_placement<
            size_type, typename details::view_type<VARTYPES>::raw_type...>(
            mr, 1, details::buffer_alloc<VARTYPES>::size(capacity)...);

        // Initialize the views from the raw pointers.
        view_type::m_views = details::make_buffer_views<VARTYPES...>(
            capacity, view_type::m_size, alloc_result);

    } else {
        throw std::runtime_error("Unknown buffer type");
    }

    // Set the base class's memory view.
    view_type::m_memory = {
        static_cast<typename decltype(view_type::m_memory)::size_type>(bytes),
        m_memory.get()};
}

template <typename... VARTYPES>
buffer<schema<VARTYPES...>>::buffer(const std::vector<size_type>&,
                                    memory_resource&, memory_resource*,
                                    vecmem::data::buffer_type)
    : view_type() {}

}  // namespace edm

template <typename... VARTYPES>
edm::view<edm::schema<VARTYPES...>> get_data(
    edm::buffer<edm::schema<VARTYPES...>>& buffer) {

    return buffer;
}

template <typename... VARTYPES>
edm::view<
    edm::schema<typename edm::type::details::add_const<VARTYPES>::type...>>
get_data(const edm::buffer<edm::schema<VARTYPES...>>& buffer) {

    return buffer;
}

}  // namespace vecmem
