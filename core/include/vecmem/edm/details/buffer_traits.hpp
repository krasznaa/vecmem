/* VecMem project, part of the ACTS project (R&D line)
 *
 * (c) 2023 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */
#pragma once

// Local include(s).
#include "vecmem/edm/details/view_traits.hpp"
#include "vecmem/edm/schema.hpp"
#include "vecmem/memory/unique_ptr.hpp"

// System include(s).
#include <tuple>

namespace vecmem::edm::details {

/// @name Traits for making allocations inside of buffers
/// @{

template <typename TYPE>
struct buffer_alloc {
    static std::size_t size(std::size_t size) { return size; }
    static typename view_type<TYPE>::type make_view(
        std::size_t size, typename view_type<TYPE>::pointer_type ptr) {
        return {size, ptr};
    }
    static typename view_type<TYPE>::type make_view(
        std::size_t capacity, unsigned int* size,
        typename view_type<TYPE>::pointer_type ptr) {
        return {capacity, size, ptr};
    }
};  // struct buffer_alloc

template <typename TYPE>
struct buffer_alloc<type::scalar<TYPE> > {
    static std::size_t size(std::size_t) { return 1u; }
    static typename view_type<type::scalar<TYPE> >::type make_view(
        std::size_t,
        typename view_type<type::scalar<TYPE> >::pointer_type ptr) {
        return ptr;
    }
    static typename view_type<type::scalar<TYPE> >::type make_view(
        std::size_t, unsigned int*,
        typename view_type<type::scalar<TYPE> >::pointer_type ptr) {
        return ptr;
    }
};  // struct buffer_alloc

template <typename TYPE>
struct buffer_alloc<type::vector<TYPE> > {
    static std::size_t size(std::size_t size) { return size; }
    static typename view_type<type::vector<TYPE> >::type make_view(
        std::size_t size,
        typename view_type<type::vector<TYPE> >::pointer_type ptr) {
        return {
            static_cast<
                typename view_type<type::vector<TYPE> >::type::size_type>(size),
            ptr};
    }
    static typename view_type<type::vector<TYPE> >::type make_view(
        std::size_t capacity, unsigned int* size,
        typename view_type<type::vector<TYPE> >::pointer_type ptr) {
        return {static_cast<
                    typename view_type<type::vector<TYPE> >::type::size_type>(
                    capacity),
                size, ptr};
    }
};  // struct buffer_alloc

template <typename TYPE>
struct buffer_alloc<type::jagged_vector<TYPE> > {};  // struct buffer_alloc

/// @}

/// Helper function for @c vecmem::edm::details::make_buffer_views
template <typename... TYPES, std::size_t... I>
auto make_buffer_views_impl(
    std::size_t size,
    std::tuple<unique_alloc_ptr<char[]>&, std::size_t&,
               typename view_type<TYPES>::pointer_type...>& allocs,
    std::index_sequence<I...>) {

    return std::make_tuple(
        buffer_alloc<TYPES>::make_view(size, std::get<I + 2>(allocs))...);
}

/// Function turning raw pointers into views
///
/// @tparam ...TYPES The variable types
/// @param size The fixed size of the variables
/// @param allocs Return value of @c vecmem::details::aligned_multiple_placement
/// @return A tuple of view objects, describing the allocated variables
///
template <typename... TYPES>
auto make_buffer_views(
    std::size_t size,
    std::tuple<unique_alloc_ptr<char[]>&, std::size_t&,
               typename view_type<TYPES>::pointer_type...>& allocs) {

    return make_buffer_views_impl<TYPES...>(
        size, allocs, std::index_sequence_for<TYPES...>());
}

/// Helper function for @c vecmem::edm::details::make_buffer_views
template <typename... TYPES, std::size_t... I>
auto make_buffer_views_impl(
    std::size_t capacity, unsigned int* size,
    std::tuple<unique_alloc_ptr<char[]>&, size_t&, unsigned int*&,
               typename view_type<TYPES>::pointer_type...>& allocs,
    std::index_sequence<I...>) {

    return std::make_tuple(buffer_alloc<TYPES>::make_view(
        capacity, size, std::get<I + 3>(allocs))...);
}

/// Function turning raw pointers into views
///
/// @tparam ...TYPES The variable types
/// @param capacity The fixed capacity of the variables
/// @param size Pointer to the resizable size of the variables
/// @param allocs Return value of @c vecmem::details::aligned_multiple_placement
/// @return A tuple of view objects, describing the allocated variables
///
template <typename... TYPES>
auto make_buffer_views(
    std::size_t capacity, unsigned int* size,
    std::tuple<unique_alloc_ptr<char[]>&, std::size_t&, unsigned int*&,
               typename view_type<TYPES>::pointer_type...>& allocs) {

    return make_buffer_views_impl<TYPES...>(
        capacity, size, allocs, std::index_sequence_for<TYPES...>());
}

}  // namespace vecmem::edm::details
