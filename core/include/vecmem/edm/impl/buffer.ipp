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
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/details/view_traits.hpp"

// System include(s).
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace vecmem {
namespace edm {

template <typename... VARTYPES>
buffer<schema<VARTYPES...>>::buffer(size_type capacity, memory_resource& mr,
                                    vecmem::data::buffer_type type)
    : view_type(capacity) {

    // Make sure that this constructor is not used for a container that has
    // jagged vectors in it.
    static_assert(
        std::disjunction_v<type::details::is_jagged_vector<VARTYPES>...> ==
            false,
        "Use the other buffer constructor with jagged vector variables!");

    // Perform the appropriate setup.
    switch (type) {
        case vecmem::data::buffer_type::fixed_size:
            setup_fixed(std::vector<std::size_t>(capacity), mr, nullptr,
                        std::index_sequence_for<VARTYPES...>{});
            break;
        case vecmem::data::buffer_type::resizable:
            setup_resizable(std::vector<std::size_t>(capacity), mr, nullptr,
                            std::index_sequence_for<VARTYPES...>{});
            break;
        default:
            throw std::runtime_error("Unknown buffer type");
    }
}

template <typename... VARTYPES>
template <typename SIZE_TYPE,
          std::enable_if_t<std::is_integral<SIZE_TYPE>::value &&
                               std::is_unsigned<SIZE_TYPE>::value,
                           bool>>
buffer<schema<VARTYPES...>>::buffer(const std::vector<SIZE_TYPE>& capacities,
                                    memory_resource& main_mr,
                                    memory_resource* host_mr,
                                    vecmem::data::buffer_type type)
    : view_type(capacities.size()) {

    // Make sure that this constructor is only used for a container that has
    // jagged vectors in it.
    static_assert(
        std::disjunction_v<type::details::is_jagged_vector<VARTYPES>...>,
        "Use the other buffer constructor without jagged vector variables!");

    // Perform the appropriate setup.
    switch (type) {
        case vecmem::data::buffer_type::fixed_size:
            setup_fixed(capacities, main_mr, host_mr,
                        std::index_sequence_for<VARTYPES...>{});
            break;
        case vecmem::data::buffer_type::resizable:
            setup_resizable(capacities, main_mr, host_mr,
                            std::index_sequence_for<VARTYPES...>{});
            break;
        default:
            throw std::runtime_error("Unknown buffer type");
    }
}

template <typename... VARTYPES>
template <typename SIZE_TYPE, std::size_t... INDICES>
void buffer<schema<VARTYPES...>>::setup_fixed(
    const std::vector<SIZE_TYPE>& capacities, memory_resource& mr,
    memory_resource* host_mr, std::index_sequence<INDICES...>) {

    // Sanity check.
    static_assert(sizeof...(VARTYPES) == sizeof...(INDICES),
                  "Invalid number of indices");

    // Tuple of pointers to the allocated "layout objects" and "payloads".
    std::tuple<typename details::view_type<VARTYPES>::layout_ptr...>
        layout_ptrs, host_layout_ptrs;
    std::tuple<typename details::view_type<VARTYPES>::payload_ptr...>
        payload_ptrs;

    // Return value for the main allocation command.
    auto main_alloc_result =
        std::tie(m_memory, std::ignore, std::get<INDICES>(layout_ptrs)...,
                 std::get<INDICES>(payload_ptrs)...);

    // Allocation sizes for the layouts and payloads.
    const std::vector<std::size_t> layout_sizes = {
        details::buffer_alloc<VARTYPES>::layout_size(capacities)...};
    const std::vector<std::size_t> payload_sizes = {
        details::buffer_alloc<VARTYPES>::payload_size(capacities)...};

    // Allocate memory for fixed sized variables.
    main_alloc_result = vecmem::details::aligned_multiple_placement<
        typename details::view_type<VARTYPES>::layout_type...,
        typename details::view_type<VARTYPES>::payload_type...>(
        mr, details::buffer_alloc<VARTYPES>::layout_size(capacities)...,
        details::buffer_alloc<VARTYPES>::payload_size(capacities)...);

    // Get the pointers for the typeless views.
    char* const layout_begin =
        reinterpret_cast<char*>(std::get<0>(layout_ptrs));
    char* const layout_end = reinterpret_cast<char*>(
        std::get<sizeof...(VARTYPES) - 1>(layout_ptrs) + layout_sizes.back());
    char* const payload_begin =
        reinterpret_cast<char*>(std::get<0>(payload_ptrs));
    char* const payload_end = reinterpret_cast<char*>(
        std::get<sizeof...(VARTYPES) - 1>(payload_ptrs) + payload_sizes.back());

    // Set the base class's memory views.
    if ((layout_begin != nullptr) && (layout_end != nullptr) &&
        (layout_begin < layout_end)) {
        view_type::m_layout = {
            static_cast<typename decltype(view_type::m_layout)::size_type>(
                layout_end - layout_begin),
            layout_begin};
    }
    if ((payload_begin != nullptr) && (payload_end != nullptr) &&
        (payload_begin < payload_end)) {
        view_type::m_payload = {
            static_cast<typename decltype(view_type::m_payload)::size_type>(
                payload_end - payload_begin),
            payload_begin};
    }

    // If requested, allocate host memory for the layouts.
    if (host_mr != nullptr) {

        // Return value for the host allocation command, reusing the memory
        // pointers from the main allocation.
        auto host_alloc_result = std::tie(
            m_host_memory, std::ignore, std::get<INDICES>(host_layout_ptrs)...);

        // Allocate memory for just the layout in host memory.
        host_alloc_result = vecmem::details::aligned_multiple_placement<
            typename details::view_type<VARTYPES>::layout_type...>(
            *host_mr,
            details::buffer_alloc<VARTYPES>::layout_size(capacities)...);

        // Get the pointers for the typeless view.
        char* const host_layout_begin =
            reinterpret_cast<char*>(std::get<0>(host_layout_ptrs));
        char* const host_layout_end = reinterpret_cast<char*>(
            std::get<sizeof...(VARTYPES) - 1>(host_layout_ptrs) +
            layout_sizes.back());

        // Set the base class's memory view.
        if ((host_layout_begin != nullptr) && (host_layout_end != nullptr) &&
            (host_layout_begin < host_layout_end)) {
            view_type::m_host_layout = {
                static_cast<typename decltype(view_type::m_layout)::size_type>(
                    host_layout_end - host_layout_begin),
                host_layout_begin};
        }
    }

    // Initialize the views from all the raw pointers.
    view_type::m_views = details::make_buffer_views<SIZE_TYPE, VARTYPES...>(
        capacities, layout_ptrs, host_layout_ptrs, payload_ptrs);
}

template <typename... VARTYPES>
template <typename SIZE_TYPE, std::size_t... INDICES>
void buffer<schema<VARTYPES...>>::setup_resizable(
    const std::vector<SIZE_TYPE>& capacities, memory_resource& mr,
    memory_resource* host_mr, std::index_sequence<INDICES...>) {

    // Sanity check(s).
    static_assert(sizeof...(VARTYPES) == sizeof...(INDICES),
                  "Invalid number of indices");
    static_assert(
        std::disjunction_v<type::details::is_vector<VARTYPES>...>,
        "Trying to create a resizable container without any vector variables!");

    // Does the container have jagged vectors in it?
    constexpr bool has_jagged_vectors =
        std::disjunction_v<type::details::is_jagged_vector<VARTYPES>...>;

    // Pointers to the allocated "size variables".
    std::tuple<typename details::view_type<VARTYPES>::size_ptr...> sizes_ptrs;

    // Tuple of pointers to the allocated "layout objects" and "payloads".
    std::tuple<typename details::view_type<VARTYPES>::layout_ptr...>
        layout_ptrs, host_layout_ptrs;
    std::tuple<typename details::view_type<VARTYPES>::payload_ptr...>
        payload_ptrs;

    // Allocation sizes for the layouts and payloads.
    const std::vector<std::size_t> layout_sizes = {
        details::buffer_alloc<VARTYPES>::layout_size(capacities)...};
    const std::vector<std::size_t> payload_sizes = {
        details::buffer_alloc<VARTYPES>::payload_size(capacities)...};

    // Allocate memory for fixed sized variables. A little differently for
    // containers that have some jagged vectors, versus ones that only have
    // 1D vectors.
    if constexpr (has_jagged_vectors) {
        // Set the size of the array that the base class's size pointer will
        // point to.
        view_type::m_size_size =
            (details::buffer_alloc<VARTYPES>::layout_size(capacities) + ...);
        // Perform the allocation.
        std::tie(m_memory, std::ignore, std::get<INDICES>(sizes_ptrs)...,
                 std::get<INDICES>(layout_ptrs)...,
                 std::get<INDICES>(payload_ptrs)...) =
            vecmem::details::aligned_multiple_placement<
                typename details::view_type<VARTYPES>::size_type...,
                typename details::view_type<VARTYPES>::layout_type...,
                typename details::view_type<VARTYPES>::payload_type...>(
                mr, details::buffer_alloc<VARTYPES>::layout_size(capacities)...,
                details::buffer_alloc<VARTYPES>::layout_size(capacities)...,
                details::buffer_alloc<VARTYPES>::payload_size(capacities)...);
        // Point the base class at the size array.
        view_type::m_size = details::find_size_pointer<VARTYPES...>(sizes_ptrs);
    } else {
        // Set the size of the array that the base class's size pointer will
        // point to.
        view_type::m_size_size = 1u;
        // Perform the allocation.
        std::tie(m_memory, std::ignore, view_type::m_size,
                 std::get<INDICES>(layout_ptrs)...,
                 std::get<INDICES>(payload_ptrs)...) =
            vecmem::details::aligned_multiple_placement<
                typename view_type::size_type,
                typename details::view_type<VARTYPES>::layout_type...,
                typename details::view_type<VARTYPES>::payload_type...>(
                mr, view_type::m_size_size,
                details::buffer_alloc<VARTYPES>::layout_size(capacities)...,
                details::buffer_alloc<VARTYPES>::payload_size(capacities)...);
        // Set all size pointers to point at the one allocated number.
        ((std::get<INDICES>(sizes_ptrs) = view_type::m_size), ...);
    }

    // Get the pointers for the typeless views.
    char* const layout_begin =
        reinterpret_cast<char*>(std::get<0>(layout_ptrs));
    char* const layout_end = reinterpret_cast<char*>(
        std::get<sizeof...(VARTYPES) - 1>(layout_ptrs) + layout_sizes.back());
    char* const payload_begin =
        reinterpret_cast<char*>(std::get<0>(payload_ptrs));
    char* const payload_end = reinterpret_cast<char*>(
        std::get<sizeof...(VARTYPES) - 1>(payload_ptrs) + payload_sizes.back());

    // Set the base class's memory views.
    if ((layout_begin != nullptr) && (layout_end != nullptr) &&
        (layout_begin < layout_end)) {
        view_type::m_layout = {
            static_cast<typename decltype(view_type::m_layout)::size_type>(
                layout_end - layout_begin),
            layout_begin};
    }
    if ((payload_begin != nullptr) && (payload_end != nullptr) &&
        (payload_begin < payload_end)) {
        view_type::m_payload = {
            static_cast<typename decltype(view_type::m_payload)::size_type>(
                payload_end - payload_begin),
            payload_begin};
    }

    // If requested, allocate host memory for the layouts.
    if (host_mr != nullptr) {

        // Allocate memory for just the layout in host memory.
        std::tie(m_host_memory, std::ignore,
                 std::get<INDICES>(host_layout_ptrs)...) =
            vecmem::details::aligned_multiple_placement<
                typename details::view_type<VARTYPES>::layout_type...>(
                *host_mr,
                details::buffer_alloc<VARTYPES>::layout_size(capacities)...);

        // Get the pointers for the typeless view.
        char* const host_layout_begin =
            reinterpret_cast<char*>(std::get<0>(host_layout_ptrs));
        char* const host_layout_end = reinterpret_cast<char*>(
            std::get<sizeof...(VARTYPES) - 1>(host_layout_ptrs) +
            layout_sizes.back());

        // Set the base class's memory view.
        if ((host_layout_begin != nullptr) && (host_layout_end != nullptr) &&
            (host_layout_begin < host_layout_end)) {
            view_type::m_host_layout = {
                static_cast<typename decltype(view_type::m_layout)::size_type>(
                    host_layout_end - host_layout_begin),
                host_layout_begin};
        }
    }

    // Initialize the views from all the raw pointers.
    view_type::m_views = details::make_buffer_views<SIZE_TYPE, VARTYPES...>(
        capacities, sizes_ptrs, layout_ptrs, host_layout_ptrs, payload_ptrs);
}

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
