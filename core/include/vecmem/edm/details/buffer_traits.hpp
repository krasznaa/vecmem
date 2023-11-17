/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/schema_traits.hpp"
#include "vecmem/edm/details/tuple_traits.hpp"
#include "vecmem/edm/details/view_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/memory/unique_ptr.hpp"

// System include(s).
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>

namespace vecmem::edm::details {

/// @name Traits for making allocations inside of buffers
/// @{

template <typename TYPE>
struct buffer_alloc {};  // struct buffer_alloc

template <typename TYPE>
struct buffer_alloc<type::scalar<TYPE> > {
    /// The number of @c TYPE elements to allocate for the payload
    template <typename SIZE_TYPE = std::size_t>
    static std::size_t payload_size(const std::vector<SIZE_TYPE>&) {
        return 1u;
    }
    /// The number of "layout meta-objects" to allocate for the payload
    template <typename SIZE_TYPE = std::size_t>
    static std::size_t layout_size(const std::vector<SIZE_TYPE>&) {
        return 0u;
    }
    /// Construct a view for a scalar variable.
    template <typename SIZE_TYPE = std::size_t>
    static typename view_type<type::scalar<TYPE> >::type make_view(
        const std::vector<SIZE_TYPE>&, unsigned int*,
        typename view_type<type::scalar<TYPE> >::layout_ptr,
        typename view_type<type::scalar<TYPE> >::layout_ptr,
        typename view_type<type::scalar<TYPE> >::payload_ptr ptr) {
        return ptr;
    }
};  // struct buffer_alloc

template <typename TYPE>
struct buffer_alloc<type::vector<TYPE> > {
    /// The number of @c TYPE elements to allocate for the payload
    template <typename SIZE_TYPE = std::size_t>
    static std::size_t payload_size(const std::vector<SIZE_TYPE>& sizes) {
        return sizes.size();
    }
    /// The number of "layout meta-objects" to allocate for the payload
    template <typename SIZE_TYPE = std::size_t>
    static std::size_t layout_size(const std::vector<SIZE_TYPE>&) {
        return 0u;
    }
    /// Construct a view for a vector variable.
    template <typename SIZE_TYPE = std::size_t>
    static typename view_type<type::vector<TYPE> >::type make_view(
        const std::vector<SIZE_TYPE>& capacity, unsigned int* size,
        typename view_type<type::vector<TYPE> >::layout_ptr,
        typename view_type<type::vector<TYPE> >::layout_ptr,
        typename view_type<type::vector<TYPE> >::payload_ptr ptr) {
        return {static_cast<
                    typename view_type<type::vector<TYPE> >::type::size_type>(
                    capacity.size()),
                size, ptr};
    }
};  // struct buffer_alloc

template <typename TYPE>
struct buffer_alloc<type::jagged_vector<TYPE> > {
    /// The number of @c TYPE elements to allocate for the payload
    template <typename SIZE_TYPE = std::size_t>
    static std::size_t payload_size(const std::vector<SIZE_TYPE>& sizes) {
        return std::accumulate(sizes.begin(), sizes.end(),
                               static_cast<std::size_t>(0));
    }
    /// The number of "layout meta-objects" to allocate for the payload
    template <typename SIZE_TYPE = std::size_t>
    static std::size_t layout_size(const std::vector<SIZE_TYPE>& sizes) {
        return sizes.size();
    }
    /// Construct a view for a jagged vector variable.
    template <typename SIZE_TYPE = std::size_t>
    static typename view_type<type::jagged_vector<TYPE> >::type make_view(
        const std::vector<SIZE_TYPE>& capacities, unsigned int* sizes,
        typename view_type<type::jagged_vector<TYPE> >::layout_ptr layout,
        typename view_type<type::jagged_vector<TYPE> >::layout_ptr host_layout,
        typename view_type<type::jagged_vector<TYPE> >::payload_ptr ptr) {

        // Set up the "layout objects" for use by the view.
        typename view_type<type::jagged_vector<TYPE> >::layout_ptr used_layout =
            (host_layout != nullptr ? host_layout : layout);
        std::ptrdiff_t ptrdiff = 0;
        for (std::size_t i = 0; i < capacities.size(); ++i) {
            new (used_layout + i)
                typename view_type<type::jagged_vector<TYPE> >::layout_type(
                    static_cast<typename view_type<
                        type::jagged_vector<TYPE> >::layout_type::size_type>(
                        capacities[i]),
                    (sizes != nullptr ? &(sizes[i]) : nullptr), ptr + ptrdiff);
            ptrdiff += capacities[i];
        }

        // Create the jagged vector view.
        return {static_cast<
                    typename view_type<type::vector<TYPE> >::type::size_type>(
                    capacities.size()),
                layout, host_layout};
    }
};  // struct buffer_alloc

/// @}

/// Helper function for @c vecmem::edm::details::make_buffer_views
template <typename SIZE_TYPE, typename... TYPES, std::size_t... INDICES>
auto make_buffer_views_impl(
    const std::vector<SIZE_TYPE>& sizes,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& layouts,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& host_layouts,
    const std::tuple<typename view_type<TYPES>::payload_ptr...>& payloads,
    std::index_sequence<INDICES...>) {

    return details::make_tuple(buffer_alloc<TYPES>::make_view(
        sizes, nullptr, std::get<INDICES>(layouts),
        std::get<INDICES>(host_layouts), std::get<INDICES>(payloads))...);
}

/// Function constructing fixed size view objects for @c vecmem::edm::buffer
template <typename SIZE_TYPE, typename... TYPES>
auto make_buffer_views(
    const std::vector<SIZE_TYPE>& sizes,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& layouts,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& host_layouts,
    const std::tuple<typename view_type<TYPES>::payload_ptr...>& payloads) {

    return make_buffer_views_impl<SIZE_TYPE, TYPES...>(
        sizes, layouts, host_layouts, payloads,
        std::index_sequence_for<TYPES...>());
}

/// Helper function for @c vecmem::edm::details::make_buffer_views
template <typename SIZE_TYPE, typename... TYPES, std::size_t... INDICES>
auto make_buffer_views_impl(
    const std::vector<SIZE_TYPE>& capacities,
    const std::tuple<typename view_type<TYPES>::size_ptr...>& sizes,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& layouts,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& host_layouts,
    const std::tuple<typename view_type<TYPES>::payload_ptr...>& payloads,
    const std::index_sequence<INDICES...>) {

    // Helper variable(s).
    constexpr auto is_jagged_vector =
        std::make_tuple(type::details::is_jagged_vector<TYPES>::value...);
    constexpr bool has_jagged_vector =
        std::disjunction_v<type::details::is_jagged_vector<TYPES>...>;

    // The logic here is that if there are any jagged vectors in the schema,
    // then only the jagged vectors are resizable, the "normal vectors" are not.
    // But the received "sizes" variable would be hard to set up like that
    // outside of this function, so the logic has to sit here.
    return details::make_tuple(buffer_alloc<TYPES>::make_view(
        capacities,
        ((has_jagged_vector && (!std::get<INDICES>(is_jagged_vector)))
             ? nullptr
             : std::get<INDICES>(sizes)),
        std::get<INDICES>(layouts), std::get<INDICES>(host_layouts),
        std::get<INDICES>(payloads))...);
}

/// Function constructing resizable view objects for @c vecmem::edm::buffer
template <typename SIZE_TYPE, typename... TYPES>
auto make_buffer_views(
    const std::vector<SIZE_TYPE>& capacities,
    const std::tuple<typename view_type<TYPES>::size_ptr...>& sizes,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& layouts,
    const std::tuple<typename view_type<TYPES>::layout_ptr...>& host_layouts,
    const std::tuple<typename view_type<TYPES>::payload_ptr...>& payloads) {

    return make_buffer_views_impl<SIZE_TYPE, TYPES...>(
        capacities, sizes, layouts, host_layouts, payloads,
        std::index_sequence_for<TYPES...>());
}

template <typename... TYPES>
auto find_size_pointer_impl(
    const std::tuple<typename view_type<TYPES>::size_ptr...>&,
    std::index_sequence<>) {

    return static_cast<unsigned int*>(nullptr);
}

template <typename... TYPES, std::size_t INDEX, std::size_t... INDICES>
auto find_size_pointer_impl(
    const std::tuple<typename view_type<TYPES>::size_ptr...>& sizes,
    std::index_sequence<INDEX, INDICES...>) {

    if (std::get<INDEX>(sizes) != nullptr) {
        return std::get<INDEX>(sizes);
    } else {
        return find_size_pointer_impl<TYPES...>(
            sizes, std::index_sequence<INDICES...>());
    }
}

/// Function finding the first non-nullptr size pointer in a tuple.
template <typename... TYPES>
auto find_size_pointer(
    const std::tuple<typename view_type<TYPES>::size_ptr...>& sizes) {
    return find_size_pointer_impl<TYPES...>(
        sizes, std::index_sequence_for<TYPES...>());
}

}  // namespace vecmem::edm::details
